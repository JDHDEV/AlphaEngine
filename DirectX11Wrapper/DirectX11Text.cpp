//========================================================================
// DirectX11Text.cpp - implements the DXW11DialogResourceManager and DXW11TextHelper
//					Which are gutted version of the DXUTGUI!! Shameless
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//=======================================================================

#include "AlphaStd.h"
#include  "DirectX11Text.h"

DX11WText::DX11WText()
{
	m_pFont = nullptr;
	m_FontVertices.clear();
	m_pFontBuffer11 = nullptr;
	m_FontBufferBytes11 = 0;
	m_pInputLayout11 = nullptr;
}

DX11WText::~DX11WText()
{

}

void DX11WText::InitText(DX11WFont* pFont, ID3D11InputLayout* pInputLayout)
{
	m_pFont = pFont;
	m_pInputLayout11 = pInputLayout;
}

void DX11WText::Release()
{
	m_pFont = nullptr;
	m_FontVertices.clear();
	SAFE_RELEASE(m_pFontBuffer11);
	m_FontBufferBytes11 = 0;
	m_pInputLayout11 = nullptr;
}

void DX11WText::ClearFont11()
{
	m_pFont = nullptr;
	SAFE_RELEASE(m_pFontBuffer11);
	m_FontBufferBytes11 = 0;
}

void DX11WText::BeginText11()
{
    m_FontVertices.clear();
}

void DX11WText::DrawText11(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext,
                 LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight,
                 float fBBWidth, float fBBHeight, bool bCenter)
{

	m_pFont->BuildVertexArray(&m_FontVertices, strText, rcScreen, vFontColor, fGlyphHeight, fBBWidth, fBBHeight, bCenter);

    // We have to end text after every line so that rendering order between sprites and fonts is preserved
    EndText11(pd3dDevice, pd3d11DeviceContext);
}

void DX11WText::EndText11(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext)
{
    if (m_FontVertices.empty())
        return;

	if(!m_pInputLayout11)
		return;

    // ensure our buffer size can hold our sprites
    UINT FontDataBytes = static_cast<UINT>(m_FontVertices.size() * sizeof(DXUT_SCREEN_VERTEX_10));
    if(m_FontBufferBytes11 < FontDataBytes)
    {
        SAFE_RELEASE(m_pFontBuffer11);
        m_FontBufferBytes11 = FontDataBytes;

        D3D11_BUFFER_DESC BufferDesc;
        BufferDesc.ByteWidth = m_FontBufferBytes11;
        BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        BufferDesc.MiscFlags = 0;

        if(FAILED(pd3dDevice->CreateBuffer(&BufferDesc, nullptr, &m_pFontBuffer11)))
        {
            m_pFontBuffer11 = nullptr;
            m_FontBufferBytes11 = 0;
            return;
        }
    }

    // Copy the qauds over
    D3D11_BOX destRegion;
    destRegion.left = 0;
    destRegion.right = FontDataBytes;
    destRegion.top = 0;
    destRegion.bottom = 1;
    destRegion.front = 0;
    destRegion.back = 1;
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (S_OK == pd3d11DeviceContext->Map(m_pFontBuffer11, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource))
    { 
        memcpy(MappedResource.pData, (void*)&m_FontVertices[0], FontDataBytes);
        pd3d11DeviceContext->Unmap(m_pFontBuffer11, 0);
    }

    ID3D11ShaderResourceView* pOldTexture = nullptr;
    pd3d11DeviceContext->PSGetShaderResources(0, 1, &pOldTexture);
	pd3d11DeviceContext->PSSetShaderResources(0, 1, m_pFont->GetTexture());

    // Draw
    UINT Stride = sizeof(DXUT_SCREEN_VERTEX_10);
    UINT Offset = 0;
    pd3d11DeviceContext->IASetVertexBuffers(0, 1, &m_pFontBuffer11, &Stride, &Offset);
    pd3d11DeviceContext->IASetInputLayout(m_pInputLayout11);
    pd3d11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pd3d11DeviceContext->Draw(static_cast<UINT>(m_FontVertices.size()), 0);

    pd3d11DeviceContext->PSSetShaderResources(0, 1, &pOldTexture);
    SAFE_RELEASE(pOldTexture);

    m_FontVertices.clear();
}