//========================================================================
// DirectX11Pixel.cpp : Defeine all DirectX11 Pixel Shaders
//
// Part of the Alpha Application
//
// http://www.braynzarsoft.net/
//
// Justin Hunt
//========================================================================

#include "AlphaStd.h"
#include "../Graphics3D/D3DRenderer.h"
#include "../Graphics3D/Geometry.h"
#include "../Graphics3D/Lights.h"
#include "../Graphics3D/SceneNodes.h"
#include "DirectX11Renderable.h"
#include "../ResourceCache/ResCache.h"
#include "DirectX11Pixel.h"

//
// Alpha_D3D11BluePixelShader::Alpha_D3D11BluePixelShader()
//
Alpha_D3D11BluePixelShader::Alpha_D3D11BluePixelShader()
{
    m_pPixelShader = nullptr;
}

//
// Alpha_D3D11BluePixelShader::~Alpha_D3D11BluePixelShader()
//
Alpha_D3D11BluePixelShader::~Alpha_D3D11BluePixelShader()
{
    SAFE_RELEASE(m_pPixelShader);
}

//
// Alpha_D3D11BluePixelShader::OnRestore()
//
HRESULT Alpha_D3D11BluePixelShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pPixelShader);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

	//========================================================
    // Set up the pixel shader and related constant buffers
    ID3DBlob* pPixelShaderBuffer = NULL;

	// Load the file via the Resource controller
    std::string hlslFileName = "Effects\\Alpha_D3D11VertexPos.fx";
    Resource resource(hlslFileName.c_str());
    shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
    if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_PSBlue", "ps_5_0", &pPixelShaderBuffer)))
    {
        SAFE_RELEASE (pPixelShaderBuffer);
        return hr;
    }

	// Create the shader
    if(SUCCEEDED(g_pDX11W->GetD3D11Device()->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
        pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
    {

    }

    SAFE_RELEASE(pPixelShaderBuffer);
    return hr;
}

//
// Alpha_D3D11BluePixelShader::SetupRender()
//
HRESULT Alpha_D3D11BluePixelShader::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

	return S_OK;
}

//
// Alpha_D3D11ColorPixelShader::Alpha_D3D11ColorPixelShader()
//
Alpha_D3D11ColorPixelShader::Alpha_D3D11ColorPixelShader()
{
    m_pPixelShader = nullptr;
}

//
// Alpha_D3D11ColorPixelShader::~Alpha_D3D11ColorPixelShader()
//
Alpha_D3D11ColorPixelShader::~Alpha_D3D11ColorPixelShader()
{
    SAFE_RELEASE(m_pPixelShader);
}

//
// Alpha_D3D11ColorPixelShader::OnRestore()
//
HRESULT Alpha_D3D11ColorPixelShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pPixelShader);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

	//========================================================
    // Set up the pixel shader and related constant buffers
    ID3DBlob* pPixelShaderBuffer = NULL;

	// Load the file via the Resource controller
    std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosColor.fx";
    Resource resource(hlslFileName.c_str());
    shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
    if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_PS", "ps_5_0", &pPixelShaderBuffer)))
    {
        SAFE_RELEASE (pPixelShaderBuffer);
        return hr;
    }

	// Create the shader
    if(SUCCEEDED(g_pDX11W->GetD3D11Device()->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
        pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
    {

    }

    SAFE_RELEASE(pPixelShaderBuffer);
    return hr;
}

//
// Alpha_D3D11ColorPixelShader::SetupRender()
//
HRESULT Alpha_D3D11ColorPixelShader::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

	return S_OK;
}

//
// Alpha_D3D11ColorNormalPixelShader::Alpha_D3D11ColorNormalPixelShader()
//
Alpha_D3D11ColorNormalPixelShader::Alpha_D3D11ColorNormalPixelShader()
{
    m_pPixelShader = nullptr;
}

//
// Alpha_D3D11ColorNormalPixelShader::~Alpha_D3D11ColorNormalPixelShader()
//
Alpha_D3D11ColorNormalPixelShader::~Alpha_D3D11ColorNormalPixelShader()
{
    SAFE_RELEASE(m_pPixelShader);
}

//
// Alpha_D3D11ColorNormalPixelShader::OnRestore()
//
HRESULT Alpha_D3D11ColorNormalPixelShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pPixelShader);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

	//========================================================
    // Set up the pixel shader and related constant buffers
    ID3DBlob* pPixelShaderBuffer = NULL;

	// Load the file via the Resource controller
    std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosColorNormal.fx";
    Resource resource(hlslFileName.c_str());
    shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
    if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_PS", "ps_5_0", &pPixelShaderBuffer)))
    {
        SAFE_RELEASE (pPixelShaderBuffer);
        return hr;
    }

	// Create the shader
    if(SUCCEEDED(g_pDX11W->GetD3D11Device()->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
        pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
    {

    }

    SAFE_RELEASE(pPixelShaderBuffer);
    return hr;
}

//
// Alpha_D3D11ColorNormalPixelShader::SetupRender()
//
HRESULT Alpha_D3D11ColorNormalPixelShader::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

	return S_OK;
}

//
// Alpha_D3D11NormalTexturePixelShader::Alpha_D3D11NormalTexturePixelShader()
//
Alpha_D3D11NormalTexturePixelShader::Alpha_D3D11NormalTexturePixelShader()
{
    m_pPixelShader = nullptr;
	m_pcbPSLighting = nullptr;
	m_pcbPSMaterial = nullptr;
	m_enableLights = true;
}

//
// Alpha_D3D11NormalTexturePixelShader::~Alpha_D3D11NormalTexturePixelShader()
//
Alpha_D3D11NormalTexturePixelShader::~Alpha_D3D11NormalTexturePixelShader()
{
    SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pcbPSLighting);
	SAFE_RELEASE(m_pcbPSMaterial);
}

//
// Alpha_D3D11NormalTexturePixelShader::OnRestore()
//
HRESULT Alpha_D3D11NormalTexturePixelShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pcbPSLighting);
	SAFE_RELEASE(m_pcbPSMaterial);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

	//========================================================
    // Set up the pixel shader and related constant buffers
    ID3DBlob* pPixelShaderBuffer = NULL;

	// Load the file via the Resource controller
    std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosNormalTexture.fx";
    Resource resource(hlslFileName.c_str());
    shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
    if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_PS", "ps_5_0", &pPixelShaderBuffer)))
    {
        SAFE_RELEASE (pPixelShaderBuffer);
        return hr;
    }

	// Create the shader
    if(SUCCEEDED(g_pDX11W->GetD3D11Device()->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
        pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
    {
        // Setup constant buffers
        D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
        constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constantBufferDesc.MiscFlags = 0;

		// Create lighting constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Lighting);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbPSLighting));

		// Create material constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Material);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbPSMaterial));
    }

    SAFE_RELEASE(pPixelShaderBuffer);
    return hr;
}

//
// Alpha_D3D11NormalTexturePixelShader::SetupRender()
//
HRESULT Alpha_D3D11NormalTexturePixelShader::SetupRender(Scene *pScene, SceneNode *pNode)
{
	HRESULT hr = S_OK;

    g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

    D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Lighting ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbPSLighting, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Lighting* pLighting = (ConstantBuffer_Lighting*)MappedResource.pData;

	if (m_enableLights)
	{
		pScene->GetLightManager()->CalcLighting(pLighting, pNode);
	}
	else
	{
		pLighting->m_nNumLights = 0;
		pLighting->m_vLightAmbient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbPSLighting, 0);
	g_pDX11W->GetD3D11DeviceContext()->PSSetConstantBuffers(1, 1, &m_pcbPSLighting);

	// ----- Material ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbPSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;

	Color color = pNode->VGet()->GetMaterial().GetDiffuse();
	pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	color = (m_enableLights) ? pNode->VGet()->GetMaterial().GetAmbient() : Color(1.0f, 1.0f, 1.0f, 1.0f);
	pPSMaterial->m_vAmbientObjectColor = Vec4(color.r, color.g, color.b, color.a);
	pPSMaterial->m_bHasTexture = false;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbPSMaterial, 0);
	g_pDX11W->GetD3D11DeviceContext()->PSSetConstantBuffers(2, 1, &m_pcbPSMaterial);

	return S_OK;
}

//
// Alpha_D3D11NormalTexturePixelShader::SetTexture()
//
HRESULT Alpha_D3D11NormalTexturePixelShader::SetTexture(const std::string& textureName)
{
    m_textureResource = textureName;
    if (m_textureResource.length() > 0)
    {
        Resource resource(m_textureResource);
        shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
        if (texture)
        {
            shared_ptr<D3DTextureResourceExtraData11> extra = static_pointer_cast<D3DTextureResourceExtraData11>(texture->GetExtra());
            SetTexture(extra->GetTexture(), extra->GetSampler());
        }
    }
    return S_OK;
}

//
// Alpha_D3D11NormalTexturePixelShader::SetTexture()
//
HRESULT Alpha_D3D11NormalTexturePixelShader::SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers)
{
	g_pDX11W->GetD3D11DeviceContext()->PSSetShaderResources(0, 1, pDiffuseRV);
    g_pDX11W->GetD3D11DeviceContext()->PSSetSamplers(0, 1, ppSamplers);
    return S_OK;
}

//
// Alpha_D3D11TexturePixelShader::Alpha_D3D11TexturePixelShader()
//
Alpha_D3D11TexturePixelShader::Alpha_D3D11TexturePixelShader()
{
    m_pPixelShader = nullptr;
	m_pcbPSMaterial = nullptr;
}

//
// Alpha_D3D11TexturePixelShader::~Alpha_D3D11TexturePixelShader()
//
Alpha_D3D11TexturePixelShader::~Alpha_D3D11TexturePixelShader()
{
    SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pcbPSMaterial);
}

//
// Alpha_D3D11TexturePixelShader::OnRestore()
//
HRESULT Alpha_D3D11TexturePixelShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pPixelShader);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

	//========================================================
    // Set up the pixel shader and related constant buffers
    ID3DBlob* pPixelShaderBuffer = NULL;

	// Load the file via the Resource controller
    std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosTexture.fx";
    Resource resource(hlslFileName.c_str());
    shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
    if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_PS", "ps_5_0", &pPixelShaderBuffer)))
    {
        SAFE_RELEASE (pPixelShaderBuffer);
        return hr;
    }

	// Create the shader
    if(SUCCEEDED(g_pDX11W->GetD3D11Device()->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
        pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
    {
        // Setup constant buffers
        D3D11_BUFFER_DESC Desc;
        Desc.Usage = D3D11_USAGE_DYNAMIC;
        Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        Desc.MiscFlags = 0;

        Desc.ByteWidth = sizeof( ConstantBuffer_Material );
        hr = g_pDX11W->GetD3D11Device()->CreateBuffer( &Desc, NULL, &m_pcbPSMaterial );
    }

    SAFE_RELEASE(pPixelShaderBuffer);
    return hr;
}

//
// Alpha_D3D11TexturePixelShader::SetupRender()
//
HRESULT Alpha_D3D11TexturePixelShader::SetupRender(SceneNode *pNode)
{
	HRESULT hr = S_OK;

    g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbPSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;
    Color color = pNode->VGet()->GetMaterial().GetDiffuse();
    pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	g_pDX11W->GetD3D11DeviceContext()->Unmap( m_pcbPSMaterial, 0 );
	g_pDX11W->GetD3D11DeviceContext()->PSSetConstantBuffers( 1, 1, &m_pcbPSMaterial );

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

	return S_OK;
}

HRESULT Alpha_D3D11TexturePixelShader::SetupRender(DirectX11Renderable *pRenderable)
{
	HRESULT hr = S_OK;

    g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbPSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;
    Color color = pRenderable->VGet()->GetMaterial().GetDiffuse();
    pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	g_pDX11W->GetD3D11DeviceContext()->Unmap( m_pcbPSMaterial, 0 );
	g_pDX11W->GetD3D11DeviceContext()->PSSetConstantBuffers( 1, 1, &m_pcbPSMaterial );

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

	return S_OK;
}

//
// Alpha_D3D11TexturePixelShader::SetTexture()
//
HRESULT Alpha_D3D11TexturePixelShader::SetTexture(const std::string& textureName)
{
    m_textureResource = textureName;
    if (m_textureResource.length() > 0)
    {
        Resource resource(m_textureResource);
        shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
        if (texture)
        {
            shared_ptr<D3DTextureResourceExtraData11> extra = static_pointer_cast<D3DTextureResourceExtraData11>(texture->GetExtra());
            SetTexture(extra->GetTexture(), extra->GetSampler());
        }
    }
    return S_OK;
}

//
// Alpha_D3D11TexturePixelShader::SetTexture()
//
HRESULT Alpha_D3D11TexturePixelShader::SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers)
{
	g_pDX11W->GetD3D11DeviceContext()->PSSetShaderResources(0, 1, pDiffuseRV);
    g_pDX11W->GetD3D11DeviceContext()->PSSetSamplers(0, 1, ppSamplers);
    return S_OK;
}