//========================================================================
// D3DRenderer.cpp - implements D3D11 renderer implementations
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//
//========================================================================

#include "AlphaStd.h"
#include "D3DRenderer.h"
#include "Lights.h"
#include "../UserInterface/HumanView.h"

// You should leave this global - it does wacky things otherwise.
DX11WGuiResourceManager D3DRenderer::g_GuiResourceManager;
//CDXUTTextHelper* D3DRenderer::g_pTextHelper = NULL;

//
// class D3DRendererAlphaPass911			- Chapter 16, page 543
//
class D3DRendererAlphaPass11 : public IRenderState
{
protected:
	ID3D11BlendState* m_pOldBlendState;
	FLOAT m_OldBlendFactor[ 4 ];
    UINT m_OldSampleMask;
        
	ID3D11BlendState* m_pCurrentBlendState;

public:
	D3DRendererAlphaPass11();
	~D3DRendererAlphaPass11();
	std::string VToString() { return "D3DRendererAlphaPass11"; }
};

//
// D3DRendererAlphaPass11::D3DRendererAlphaPass11			- Chapter 16, page 544
//
D3DRendererAlphaPass11::D3DRendererAlphaPass11()
{
	g_pDX11W->GetD3D11DeviceContext()->OMGetBlendState(&m_pOldBlendState, m_OldBlendFactor, &m_OldSampleMask);
	m_pCurrentBlendState = NULL;

	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));

	BlendState.AlphaToCoverageEnable = false;
    BlendState.IndependentBlendEnable = false;
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDX11W->GetD3D11Device()->CreateBlendState(&BlendState, &m_pCurrentBlendState);
	g_pDX11W->GetD3D11DeviceContext()->OMSetBlendState(m_pCurrentBlendState, 0, 0xffffffff);
}

//
// D3DRendererAlphaPass11::~D3DRendererAlphaPass11			- Chapter 16, page 544
//
D3DRendererAlphaPass11::~D3DRendererAlphaPass11()
{
	g_pDX11W->GetD3D11DeviceContext()->OMSetBlendState(m_pOldBlendState, m_OldBlendFactor, m_OldSampleMask);
	SAFE_RELEASE(m_pCurrentBlendState);
	SAFE_RELEASE(m_pOldBlendState);
}

//
// class D3DRendererSkyBoxPass11							- Chapter 16, page 548
//
class D3DRendererSkyBoxPass11 : public IRenderState
{
protected:
	ID3D11DepthStencilState* m_pOldDepthStencilState;
	ID3D11DepthStencilState* m_pSkyboxDepthStencilState;

    ID3D11RasterizerState* m_pOldRasterState;
    ID3D11RasterizerState* m_pRasterState;

public:
	D3DRendererSkyBoxPass11();
	~D3DRendererSkyBoxPass11();
	std::string VToString() { return "D3DRendererSkyBoxPass11"; }
};

//
// D3DRendererSkyBoxPass11::D3DRendererSkyBoxPass11()		- Chapter 16, page 548
//
D3DRendererSkyBoxPass11::D3DRendererSkyBoxPass11()
{
 //   // Depth stencil state
	D3D11_DEPTH_STENCIL_DESC DSDesc;
    ZeroMemory(&DSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    DSDesc.DepthEnable = true;
    DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//;D3D11_DEPTH_WRITE_MASK_ZERO
    DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_LESS;
    //DSDesc.StencilEnable = false;
    g_pDX11W->GetD3D11Device()->CreateDepthStencilState(&DSDesc, &m_pSkyboxDepthStencilState);
    //DXUT_SetDebugName m_pSkyboxDepthStencilState, "SkyboxDepthStencil");

    D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&DSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    //rasterizerState.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME;
    rasterizerState.CullMode = D3D11_CULL_NONE; //D3D11_CULL_FRONT;
    rasterizerState.FrontCounterClockwise = false;
    //rasterizerState.DepthBias = false;
    //rasterizerState.DepthBiasClamp = 0;
    //rasterizerState.SlopeScaledDepthBias = 0;
    //rasterizerState.DepthClipEnable = true;
    //rasterizerState.ScissorEnable = false;
    //rasterizerState.MultisampleEnable = false;
    //rasterizerState.AntialiasedLineEnable = false;
    //rasterizerState.ForcedSampleCount = 0;
    //g_pDX11W->GetD3D11Device()->CreateRasterizerState(&rasterizerState, &m_pRasterState);

    UINT StencilRef;
    g_pDX11W->GetD3D11DeviceContext()->OMGetDepthStencilState(&m_pOldDepthStencilState, &StencilRef);
    g_pDX11W->GetD3D11DeviceContext()->OMSetDepthStencilState(m_pSkyboxDepthStencilState, 0);

	//g_pDX11W->GetD3D11DeviceContext()->RSGetState(&m_pOldRasterState);
	//g_pDX11W->GetD3D11DeviceContext()->RSSetState(m_pRasterState);
}

//
// D3DRendererSkyBoxPass11::~D3DRendererSkyBoxPass11()		- Chapter 16, page 544
//
D3DRendererSkyBoxPass11::~D3DRendererSkyBoxPass11()
{
	g_pDX11W->GetD3D11DeviceContext()->OMSetDepthStencilState(m_pOldDepthStencilState, 0);
	//g_pDX11W->GetD3D11DeviceContext()->RSSetState(m_pOldRasterState);
	
	SAFE_RELEASE(m_pOldDepthStencilState);
	SAFE_RELEASE(m_pSkyboxDepthStencilState);
	//SAFE_RELEASE(m_pOldRasterState);
	//SAFE_RELEASE(m_pRasterState);
}

//
// class D3D11RasterizerWireframe
//
class D3D11RasterizerWireframe : public IRenderState
{
protected:
	ID3D11RasterizerState* m_pRasterStateOld;
    ID3D11RasterizerState* m_pRasterStateWireFrame;

public:
	D3D11RasterizerWireframe();
	~D3D11RasterizerWireframe();
	std::string VToString() { return "D3D11RasterizerWireframe"; }
};

//
// D3D11RasterizerWireframe::D3D11RasterizerWireframe
//
D3D11RasterizerWireframe::D3D11RasterizerWireframe()
{
	m_pRasterStateOld = nullptr;
    m_pRasterStateWireFrame = nullptr;

	g_pDX11W->GetD3D11DeviceContext()->RSGetState(&m_pRasterStateOld);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	g_pDX11W->GetD3D11Device()->CreateRasterizerState(&rasterizerDesc, &m_pRasterStateWireFrame);

	g_pDX11W->GetD3D11DeviceContext()->RSSetState(m_pRasterStateWireFrame);
}

//
// D3D11RasterizerWireframe::~D3D11RasterizerWireframe
//
D3D11RasterizerWireframe::~D3D11RasterizerWireframe()
{
	g_pDX11W->GetD3D11DeviceContext()->RSSetState(m_pRasterStateOld);
	SAFE_RELEASE(m_pRasterStateWireFrame);
	SAFE_RELEASE(m_pRasterStateOld);
}

//========================================================================
//
// D3DLineDrawer11 implementation
//
//========================================================================

//
// D3DLineDrawer11::DrawLine
//
void D3DLineDrawer11::DrawLine(const Vec3& from, const Vec3& to, const Color& color)
{
	HRESULT hr;

	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;
	shared_ptr<IRenderer> pRenderer = pScene->GetRenderer();

	if (FAILED(m_LineDrawerShader.SetupRender(&(*pScene))))
		return;

	m_LineDrawerShader.SetDiffuse("art\\grid.dds", color);	

    // Set vertex buffer
    UINT stride = sizeof(Vec3);
    UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	Vec3 * pVerts = (Vec3*)MappedResource.pData;
	pVerts[0] = from;
	pVerts[1] = to;
	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pVertexBuffer, 0);

    g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Set primitive topology
    g_pDX11W->GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	g_pDX11W->GetD3D11DeviceContext()->Draw(2, 0);
}

//
// D3DLineDrawer11::OnRestore
//
HRESULT D3DLineDrawer11::OnRestore()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof( D3D11Vertex_PositionColored ) * 2;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = &m_Verts;
	HRESULT hr;

	V_RETURN ( g_pDX11W->GetD3D11Device()->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) );

	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;

	V_RETURN (m_LineDrawerShader.OnRestore(&(*pScene)) );

	return S_OK;
}

//========================================================================
//
// AlphaD3DLineDrawer implementation
//
//========================================================================

//
// AlphaD3DLineDrawer::DrawLine
//
void AlphaD3DLineDrawer::DrawLine(const Vec3& from, const Vec3& to, const Color& color)
{
	HRESULT hr;

	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;
	shared_ptr<IRenderer> pRenderer = pScene->GetRenderer();

	if (FAILED(m_LineShader.SetupRender(&(*pScene))))
		return;

	m_PixelShader.SetTexture("art\\grid.dds");	

    // Set vertex buffer
    UINT stride = sizeof(D3D11Vertex_UnlitTextured);
    UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	Vec3 Normal(0.0f, 1.0f, 0.0f);
	Vec2 Uv(0.0f, 1.0f);

	D3D11Vertex_UnlitTextured * pVerts = (D3D11Vertex_UnlitTextured*)MappedResource.pData;
	//pVerts[0].Normal = Normal;
	pVerts[0].Pos = from;
	///pVerts[0].Uv = Uv;
	//pVerts[1].Normal = Normal;
	pVerts[1].Pos = to;
	//pVerts[1].Uv = Uv;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pVertexBuffer, 0);

    g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Set primitive topology
    g_pDX11W->GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	g_pDX11W->GetD3D11DeviceContext()->Draw(2, 0);
}

//
// AlphaD3DLineDrawer::OnRestore
//
HRESULT AlphaD3DLineDrawer::OnRestore()
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(D3D11Vertex_UnlitTextured) * 2;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = &m_Verts;

	V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;

	V_RETURN (m_LineShader.OnRestore(&(*pScene)));

	return S_OK;
}

//========================================================================
//
// D3DRenderer11 implementation
//
//========================================================================

//
// D3DRenderer11::VPreRender
//
bool D3DRenderer11::VPreRender()
{
	//if (g_pDX11W->GetD3D11DeviceContext() && g_pDX11W->GetD3D11RenderTargetView())
	//{
		g_pDX11W->GetD3D11DeviceContext()->ClearRenderTargetView(g_pDX11W->GetD3D11RenderTargetView(), m_backgroundColor);

		// Clear the depth buffer to 1.0 (max depth)
		g_pDX11W->GetD3D11DeviceContext()->ClearDepthStencilView(g_pDX11W->GetD3D11DepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	//}
	return true;
}

//
// D3DRenderer11::VPostRender
//
bool D3DRenderer11::VPostRender()
{
	g_pDX11W->EndRender();

	return true;
}

//
// D3DRenderer11::VOnRestore
//
HRESULT D3DRenderer11::VOnRestore()
{
	HRESULT hr;
	V_RETURN (D3DRenderer::VOnRestore());

	return S_OK;
}

//virtual void VCalcLighting(Lights *lights, int maximumLights) { }

//
// D3DRenderer11::VDrawLine
//
void D3DRenderer11::VDrawLine(const Vec3& from,const Vec3& to,const Color& color)
{
	if (!m_pLineDrawer)
	{
		m_pLineDrawer = AC_NEW AlphaD3DLineDrawer();
		m_pLineDrawer->OnRestore();
	}
	m_pLineDrawer->DrawLine(from, to, color); 
}

//
// D3DRenderer11::VPrepareAlphaPass
//
shared_ptr<IRenderState> D3DRenderer11::VPrepareAlphaPass()
{
	return shared_ptr<IRenderState>(AC_NEW D3DRendererAlphaPass11());
}

//
// D3DRenderer11::VPrepareSkyBoxPass
//
shared_ptr<IRenderState> D3DRenderer11::VPrepareSkyBoxPass()
{
	return shared_ptr<IRenderState>(AC_NEW D3DRendererSkyBoxPass11());
}

//
// D3DRenderer11::VPrepareWireFramePass
//
shared_ptr<IRenderState> D3DRenderer11::VPrepareWireFramePass()
{
	return shared_ptr<IRenderState>(AC_NEW D3D11RasterizerWireframe());
}

//
// D3DRenderer11::CompileShader
//
HRESULT D3DRenderer11::CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromMemory(pSrcData, SrcDataLen, pFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
    if(FAILED(hr))
    {
        if(pErrorBlob != NULL)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        if(pErrorBlob) pErrorBlob->Release();
        return hr;
    }
    if(pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

//
// D3DRenderer11::CompileShaderFromFile
//
HRESULT D3DRenderer11::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;
	/*
    //DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	DWORD dwShaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
    if(FAILED(hr))
    {
        if(pErrorBlob != NULL)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        if(pErrorBlob) pErrorBlob->Release();
        return hr;
    }
    if(pErrorBlob) pErrorBlob->Release();
	*/
    return S_OK;
}