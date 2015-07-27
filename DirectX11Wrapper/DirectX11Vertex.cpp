//========================================================================
// DirectX11Vertex.cpp: Defeine all DirectX11 Vetex Structs, Layouts and Shaders
//
// Part of the Alpha Application
//
// http://www.braynzarsoft.net/
//
// Justin Hunt
//========================================================================

#include "AlphaStd.h"
#include "DirectX.h"
#include "../Graphics3D/D3DRenderer.h"
#include "../Graphics3D/Geometry.h"
#include "../Graphics3D/Lights.h"
#include "../Graphics3D/SceneNodes.h"
#include "DirectX11Renderable.h"
#include "../ResourceCache/ResCache.h"

#include "DirectX11Vertex.h"


//
// Alpha_D3D11VertexPosBuffer::Alpha_D3D11VertexPosBuffer()
//
Alpha_D3D11VertexPosBuffer::Alpha_D3D11VertexPosBuffer()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_bIndex = false;
}

//
// Alpha_D3D11VertexPosBuffer::~Alpha_D3D11VertexPosBuffer()
//
Alpha_D3D11VertexPosBuffer::~Alpha_D3D11VertexPosBuffer()
{
    SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

//
// Alpha_D3D11VertexPosBuffer::OnRestore()
//
HRESULT Alpha_D3D11VertexPosBuffer::OnRestore(D3D11VertexPos *pVertices, DWORD numVerts, WORD *pIndices, DWORD numPolys)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(D3D11VertexPos)*numVerts;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bufferData; 
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = pVertices;

	hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pVertexBuffer);
    if(SUCCEEDED(hr)&&(pIndices != nullptr))
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(WORD)*numPolys*3;// 3 verts for each poly
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferData.pSysMem = pIndices;
		hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pIndexBuffer);
		m_bIndex = true;
	}

	return hr;
}

//
// Alpha_D3D11VertexPosBuffer::SetupRender()
//
HRESULT Alpha_D3D11VertexPosBuffer::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex buffer
    UINT stride = sizeof(D3D11VertexPos);
    UINT offset = 0;
	g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index buffer if thier is one
	if(m_bIndex)
	{
		g_pDX11W->GetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}

	return hr;
}

//
// Alpha_D3D11VertexPosScreenShader::Alpha_D3D11VertexPosScreenShader()
//
Alpha_D3D11VertexPosScreenShader::Alpha_D3D11VertexPosScreenShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
}

//
// Alpha_D3D11VertexPosScreenShader::~Alpha_D3D11VertexPosScreenShader()
//
Alpha_D3D11VertexPosScreenShader::~Alpha_D3D11VertexPosScreenShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
}

//
// Alpha_D3D11VertexPosScreenShader::OnRestore()
//
HRESULT Alpha_D3D11VertexPosScreenShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPos.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VS", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	//Create the Input Layout
	if (SUCCEEDED(g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosLayout, D3D11VertexPosLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout)))
	{

	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11VertexPosScreenShader::SetupRender()
//
HRESULT Alpha_D3D11VertexPosScreenShader::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	return hr;
}

//
// Alpha_D3D11VertexPosColorBuffer::Alpha_D3D11VertexPosColorBuffer()
//
Alpha_D3D11VertexPosColorBuffer::Alpha_D3D11VertexPosColorBuffer()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_bIndex = false;
}

//
// Alpha_D3D11VertexPosColorBuffer::~Alpha_D3D11VertexPosColorBuffer()
//
Alpha_D3D11VertexPosColorBuffer::~Alpha_D3D11VertexPosColorBuffer()
{
    SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

//
// Alpha_D3D11VertexPosColorBuffer::OnRestore()
//
HRESULT Alpha_D3D11VertexPosColorBuffer::OnRestore(D3D11VertexPosColor *pVertices, DWORD numVerts, WORD *pIndices, DWORD numPolys)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(D3D11VertexPosColor)*numVerts;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bufferData; 
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = pVertices;

	hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pVertexBuffer);
    if(SUCCEEDED(hr)&&(pIndices != nullptr))
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(WORD)*numPolys*3;// 3 verts for each poly
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferData.pSysMem = pIndices;
		hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pIndexBuffer);
		m_bIndex = true;
	}

	return hr;
}

//
// Alpha_D3D11VertexPosColorBuffer::SetupRender()
//
HRESULT Alpha_D3D11VertexPosColorBuffer::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex buffer
    UINT stride = sizeof(D3D11VertexPosColor);
    UINT offset = 0;
	g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index buffer if thier is one
	if(m_bIndex)
	{
		g_pDX11W->GetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}

	return hr;
}

//
// Alpha_D3D11VertexPosColorScreenShader::Alpha_D3D11VertexPosColorScreenShader()
//
Alpha_D3D11VertexPosColorScreenShader::Alpha_D3D11VertexPosColorScreenShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
}

//
// Alpha_D3D11VertexPosColorScreenShader::~Alpha_D3D11VertexPosColorScreenShader()
//
Alpha_D3D11VertexPosColorScreenShader::~Alpha_D3D11VertexPosColorScreenShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
}

//
// Alpha_D3D11VertexPosColorScreenShader::OnRestore()
//
HRESULT Alpha_D3D11VertexPosColorScreenShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosColor.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VSScreen", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	//Create the Input Layout
	V(g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosColorLayout, D3D11VertexPosColorLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout));

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11VertexPosColorScreenShader::SetupRender()
//
HRESULT Alpha_D3D11VertexPosColorScreenShader::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	return hr;
}

//
// Alpha_D3D11VertexPosColorShader::Alpha_D3D11VertexPosColorShader()
//
Alpha_D3D11VertexPosColorShader::Alpha_D3D11VertexPosColorShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
}

//
// Alpha_D3D11VertexPosColorShader::~Alpha_D3D11VertexPosColorShader()
//
Alpha_D3D11VertexPosColorShader::~Alpha_D3D11VertexPosColorShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
}

//
// Alpha_D3D11VertexPosColorShader::OnRestore()
//
HRESULT Alpha_D3D11VertexPosColorShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosColor.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VS", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	//Create the Input Layout
	if (SUCCEEDED(g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosColorLayout, D3D11VertexPosColorLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout)))
	{
		// Setup constant buffers
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;

		// Create matrix constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Matrices);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMatrices));
	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11VertexPosColorShader::SetupRender()
//
HRESULT Alpha_D3D11VertexPosColorShader::SetupRender(Scene *pScene)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	// Get the projection & view matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 mWorld = pScene->GetTopMatrix();

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)); // this is where is crashes right now

	ConstantBuffer_Matrices* pVSMatricies = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatricies->m_World, &mWorld);

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	return hr;
}

//
// Alpha_D3D11VertexPosColorNormalBuffer::Alpha_D3D11VertexPosColorNormalBuffer()
//
Alpha_D3D11VertexPosColorNormalBuffer::Alpha_D3D11VertexPosColorNormalBuffer()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_bIndex = false;
}

//
// Alpha_D3D11VertexPosColorNormalBuffer::~Alpha_D3D11VertexPosColorNormalBuffer()
//
Alpha_D3D11VertexPosColorNormalBuffer::~Alpha_D3D11VertexPosColorNormalBuffer()
{
    SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

//
// Alpha_D3D11VertexPosColorNormalBuffer::OnRestore()
//
HRESULT Alpha_D3D11VertexPosColorNormalBuffer::OnRestore(D3D11VertexPosColorNormal *pVertices, DWORD numVerts, WORD *pIndices, DWORD numPolys)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(D3D11VertexPosColorNormal)*numVerts;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bufferData; 
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = pVertices;

	hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pVertexBuffer);
    if(SUCCEEDED(hr)&&(pIndices != nullptr))
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(WORD)*numPolys*3;// 3 verts for each poly
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferData.pSysMem = pIndices;
		hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pIndexBuffer);
		m_bIndex = true;
	}

	return hr;
}

//
// Alpha_D3D11VertexPosColorNormalBuffer::SetupRender()
//
HRESULT Alpha_D3D11VertexPosColorNormalBuffer::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex buffer
    UINT stride = sizeof(D3D11VertexPosColorNormal);
    UINT offset = 0;
	g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index buffer if thier is one
	if(m_bIndex)
	{
		g_pDX11W->GetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}

	return hr;
}

//
// Alpha_D3D11VertexPosColorNormalShader::Alpha_D3D11VertexPosColorNormalShader()
//
Alpha_D3D11VertexPosColorNormalShader::Alpha_D3D11VertexPosColorNormalShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
	m_pcbVSLighting = nullptr;
	m_pcbVSMaterial = nullptr;
	m_enableLights = true;
}

//
// Alpha_D3D11VertexPosColorNormalShader::~Alpha_D3D11VertexPosColorNormalShader()
//
Alpha_D3D11VertexPosColorNormalShader::~Alpha_D3D11VertexPosColorNormalShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSLighting);
	SAFE_RELEASE(m_pcbVSMaterial);
}

//
// Alpha_D3D11VertexPosColorNormalShader::OnRestore()
//
HRESULT Alpha_D3D11VertexPosColorNormalShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSLighting);
	SAFE_RELEASE(m_pcbVSMaterial);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosColorNormal.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VSMaterial", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	//Create the Input Layout
	if (SUCCEEDED(g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosColorNormalLayout, D3D11VertexPosColorNormalLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout)))
	{
		// Setup constant buffers
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;

		// Create matrix constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Matrices);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMatrices));

		// Create lighting constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Lighting);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSLighting));

		// Create material constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Material);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMaterial));
	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11VertexPosColorNormalShader::SetupRender()
//
HRESULT Alpha_D3D11VertexPosColorNormalShader::SetupRender(Scene *pScene, SceneNode *pNode)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	// Get the projection & view matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 mWorld = pScene->GetTopMatrix();

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	ConstantBuffer_Matrices* pVSMatricies = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatricies->m_World, &mWorld);

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	// ----- Lighting ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSLighting, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
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

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSLighting, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(1, 1, &m_pcbVSLighting);

	// ----- Material ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;

	Color color = pNode->VGet()->GetMaterial().GetDiffuse();
	pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	color = (m_enableLights) ? pNode->VGet()->GetMaterial().GetAmbient() : Color(1.0f, 1.0f, 1.0f, 1.0f);
	pPSMaterial->m_vAmbientObjectColor = Vec4(color.r, color.g, color.b, color.a);
	// Note - the vertex shader doesn't care about the texture one way or another so we'll just set it to false
	pPSMaterial->m_bHasTexture = false;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMaterial, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(2, 1, &m_pcbVSMaterial);

	return hr;
}

//
// Alpha_D3D11VertexPosNormalTextureBuffer::Alpha_D3D11VertexPosNormalTextureBuffer()
//
Alpha_D3D11VertexPosNormalTextureBuffer::Alpha_D3D11VertexPosNormalTextureBuffer()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_bIndex = false;
}

//
// Alpha_D3D11VertexPosNormalTextureBuffer::~Alpha_D3D11VertexPosNormalTextureBuffer()
//
Alpha_D3D11VertexPosNormalTextureBuffer::~Alpha_D3D11VertexPosNormalTextureBuffer()
{
    SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

//
// Alpha_D3D11VertexPosNormalTextureBuffer::OnRestore()
//
HRESULT Alpha_D3D11VertexPosNormalTextureBuffer::OnRestore(D3D11VertexPosNormalTexture *pVertices, DWORD numVerts, WORD *pIndices, DWORD numPolys)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(D3D11VertexPosNormalTexture)*numVerts;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bufferData; 
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = pVertices;

	hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pVertexBuffer);
    if(SUCCEEDED(hr)&&(pIndices != nullptr))
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(WORD)*numPolys*3;// 3 verts for each poly
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferData.pSysMem = pIndices;
		hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pIndexBuffer);
		m_bIndex = true;
	}

	return hr;
}

//
// Alpha_D3D11VertexPosNormalTextureBuffer::SetupRender()
//
HRESULT Alpha_D3D11VertexPosNormalTextureBuffer::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex buffer
    UINT stride = sizeof(D3D11VertexPosNormalTexture);
    UINT offset = 0;
	g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index buffer if thier is one
	if(m_bIndex)
	{
		g_pDX11W->GetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}

	return hr;
}

//
// Alpha_D3D11VertexPosNormalTextureShader::Alpha_D3D11VertexPosNormalTextureShader()
//
Alpha_D3D11VertexPosNormalTextureShader::Alpha_D3D11VertexPosNormalTextureShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
}

//
// Alpha_D3D11VertexPosNormalTextureShader::~Alpha_D3D11VertexPosNormalTextureShader()
//
Alpha_D3D11VertexPosNormalTextureShader::~Alpha_D3D11VertexPosNormalTextureShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
}

//
// Alpha_D3D11VertexPosNormalTextureShader::OnRestore()
//
HRESULT Alpha_D3D11VertexPosNormalTextureShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosNormalTexture.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VS", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	//Create the Input Layout
	if (SUCCEEDED(g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosNormalTextureLayout, D3D11VertexPosNormalTextureLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout)))
	{
		// Setup constant buffers
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;

		// Create matrix constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Matrices);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMatrices));
	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11VertexPosNormalTextureShader::SetupRender()
//
HRESULT Alpha_D3D11VertexPosNormalTextureShader::SetupRender(Scene *pScene, SceneNode *pNode)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	// Get the projection & view matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 mWorld = pScene->GetTopMatrix();

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	ConstantBuffer_Matrices* pVSMatricies = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatricies->m_World, &mWorld);

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	return hr;
}

//
// Alpha_D3D11VertexPosTextureBuffer::Alpha_D3D11VertexPosTextureBuffer()
//
Alpha_D3D11VertexPosTextureBuffer::Alpha_D3D11VertexPosTextureBuffer()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_bIndex = false;
}

//
// Alpha_D3D11VertexPosTextureBuffer::~Alpha_D3D11VertexPosTextureBuffer()
//
Alpha_D3D11VertexPosTextureBuffer::~Alpha_D3D11VertexPosTextureBuffer()
{
    SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

//
// Alpha_D3D11VertexPosTextureBuffer::OnRestore()
//
HRESULT Alpha_D3D11VertexPosTextureBuffer::OnRestore(D3D11VertexPosTexture *pVertices, DWORD numVerts, WORD *pIndices, DWORD numPolys)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(D3D11VertexPosTexture)*numVerts;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bufferData; 
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = pVertices;

	hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pVertexBuffer);
    if(SUCCEEDED(hr)&&(pIndices != nullptr))
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(WORD)*numPolys*3;// 3 verts for each poly
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferData.pSysMem = pIndices;
		hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bufferDesc, &bufferData, &m_pIndexBuffer);
		m_bIndex = true;
	}

	return hr;
}

//
// Alpha_D3D11VertexPosTextureBuffer::SetupRender()
//
HRESULT Alpha_D3D11VertexPosTextureBuffer::SetupRender()
{
	HRESULT hr = S_OK;

	// Set the vertex buffer
    UINT stride = sizeof(D3D11VertexPosTexture);
    UINT offset = 0;
	g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index buffer if thier is one
	if(m_bIndex)
	{
		g_pDX11W->GetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}

	return hr;
}

//
// Alpha_D3D11VertexPosTextureBuffer::SetupRender()
//
HRESULT Alpha_D3D11VertexPosTextureBuffer::SetupRenderInstance(UINT instanceStride, ID3D11Buffer* pInstanceBuffer)
{
	HRESULT hr = S_OK;

	UINT strides[2];
	UINT offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(D3D11VertexPosTexture); 
	strides[1] = instanceStride; 

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_pVertexBuffer;	
	bufferPointers[1] = pInstanceBuffer;

	g_pDX11W->GetD3D11DeviceContext()->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the index buffer if thier is one
	if(m_bIndex)
	{
		g_pDX11W->GetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}

	return hr;
}

//
// Alpha_D3D11VertexPosTextureShader::Alpha_D3D11VertexPosTextureShader()
//
Alpha_D3D11VertexPosTextureShader::Alpha_D3D11VertexPosTextureShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
	m_pcbVSLighting = nullptr;
	m_pcbVSMaterial = nullptr;
	m_enableLights = true;
}

//
// Alpha_D3D11VertexPosTextureShader::~Alpha_D3D11VertexPosTextureShader()
//
Alpha_D3D11VertexPosTextureShader::~Alpha_D3D11VertexPosTextureShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSLighting);
	SAFE_RELEASE(m_pcbVSMaterial);
}

//
// Alpha_D3D11VertexPosTextureShader::OnRestore()
//
HRESULT Alpha_D3D11VertexPosTextureShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSLighting);
	SAFE_RELEASE(m_pcbVSMaterial);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosTexture.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VS", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	hr = g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosTextureLayout, D3D11VertexPosTextureLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout);
	//Create the Input Layout
	if (SUCCEEDED(hr))
	{
		// Setup constant buffers
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;

		// Create matrix constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Matrices);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMatrices));

		// Create material constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Material);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMaterial));
	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11VertexPosTextureShader::SetupRender()
//
HRESULT Alpha_D3D11VertexPosTextureShader::SetupRender(Scene *pScene, SceneNode *pNode)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	// Get the projection & view matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 mWorld = pScene->GetTopMatrix();

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	ConstantBuffer_Matrices* pVSMatricies = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatricies->m_World, &mWorld);

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	// ----- Material ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;

	Color color = pNode->VGet()->GetMaterial().GetDiffuse();
	pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	color = (m_enableLights) ? pNode->VGet()->GetMaterial().GetAmbient() : Color(1.0f, 1.0f, 1.0f, 1.0f);
	pPSMaterial->m_vAmbientObjectColor = Vec4(color.r, color.g, color.b, color.a);
	// Note - the vertex shader doesn't care about the texture one way or another so we'll just set it to false
	pPSMaterial->m_bHasTexture = false;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMaterial, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(1, 1, &m_pcbVSMaterial);

	return hr;
}

//
// Alpha_D3D11VertexPosTextureShader::SetupRenderInstance()
//
HRESULT Alpha_D3D11VertexPosTextureShader::SetupRenderInstance(Scene *pScene, SceneNode *pNode)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	// Get the projection & view matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 mWorld = pScene->GetTopMatrix();

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	ConstantBuffer_Matrices* pVSMatricies = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatricies->m_World, &mWorld);

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	// ----- Material ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;

	Color color = pNode->VGet()->GetMaterial().GetDiffuse();
	pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	color = (m_enableLights) ? pNode->VGet()->GetMaterial().GetAmbient() : Color(1.0f, 1.0f, 1.0f, 1.0f);
	pPSMaterial->m_vAmbientObjectColor = Vec4(color.r, color.g, color.b, color.a);
	// Note - the vertex shader doesn't care about the texture one way or another so we'll just set it to false
	pPSMaterial->m_bHasTexture = false;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMaterial, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(1, 1, &m_pcbVSMaterial);

	return hr;
}

//
// Alpha_D3D11PosTextureSkyVShader::Alpha_D3D11PosTextureSkyVShader()
//
Alpha_D3D11PosTextureSkyVShader::Alpha_D3D11PosTextureSkyVShader()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
	m_pcbVSMaterial = nullptr;
	m_enableLights = true;
}

//
// Alpha_D3D11PosTextureSkyVShader::~Alpha_D3D11PosTextureSkyVShader()
//
Alpha_D3D11PosTextureSkyVShader::~Alpha_D3D11PosTextureSkyVShader()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSMaterial);
}

//
// Alpha_D3D11PosTextureSkyVShader::OnRestore()
//
HRESULT Alpha_D3D11PosTextureSkyVShader::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSMaterial);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_D3D11VertexPosTexture.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_Sky_VS", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	hr = g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosTextureLayout, D3D11VertexPosTextureLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout);
	//Create the Input Layout
	if (SUCCEEDED(hr))
	{
		// Setup constant buffers
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;

		// Create matrix constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Matrices);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMatrices));

		// Create material constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Material);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMaterial));
	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_D3D11PosTextureSkyVShader::SetupRender()
//
HRESULT Alpha_D3D11PosTextureSkyVShader::SetupRender(Scene *pScene, SceneNode *pNode)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	// Get the projection & view matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 mWorld = pScene->GetTopMatrix();

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	ConstantBuffer_Matrices* pVSMatricies = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatricies->m_World, &mWorld);

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	// ----- Material ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;

	Color color = pNode->VGet()->GetMaterial().GetDiffuse();
	pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	color = (m_enableLights) ? pNode->VGet()->GetMaterial().GetAmbient() : Color(1.0f, 1.0f, 1.0f, 1.0f);
	pPSMaterial->m_vAmbientObjectColor = Vec4(color.r, color.g, color.b, color.a);
	// Note - the vertex shader doesn't care about the texture one way or another so we'll just set it to false
	pPSMaterial->m_bHasTexture = false;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMaterial, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(1, 1, &m_pcbVSMaterial);

	return hr;
}

//
// Alpha_DX11VS_PosTextureInstance::Alpha_DX11VS_PosTextureInstance()
//
Alpha_DX11VS_PosTextureInstance::Alpha_DX11VS_PosTextureInstance()
{
    m_pVertexLayout = nullptr;
    m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
	m_pcbVSMaterial = nullptr;

}

//
// Alpha_DX11VS_PosTextureInstance::~Alpha_DX11VS_PosTextureInstance()
//
Alpha_DX11VS_PosTextureInstance::~Alpha_DX11VS_PosTextureInstance()
{
    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSMaterial);
}

//
// Alpha_DX11VS_PosTextureInstance::OnRestore()
//
HRESULT Alpha_DX11VS_PosTextureInstance::OnRestore(Scene *pScene)
{
	HRESULT hr = S_OK;

    SAFE_RELEASE(m_pVertexLayout);
    SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pcbVSMatrices);
	SAFE_RELEASE(m_pcbVSMaterial);

    shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

    //========================================================
    // Set up the vertex shader and related constant buffers
    ID3DBlob* pVertexShaderBuffer = NULL;

	// Load the file via the Resource controller
	std::string hlslFileName = "Effects\\Alpha_DX11VS_PosTextureInstance.fx";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	// Compile the shader
	if(FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), hlslFileName.c_str(), "Alpha_VS", "vs_5_0", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	// Create the shader
	if(FAILED(g_pDX11W->GetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE (pVertexShaderBuffer);
		return hr;
	}

	hr = g_pDX11W->GetD3D11Device()->CreateInputLayout(D3D11VertexPosTextureLayout, D3D11VertexPosTextureLayoutSize, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout);
	//Create the Input Layout
	if (SUCCEEDED(hr))
	{
		// Setup constant buffers
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;

		// Create matrix constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_InstanceMatrices);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMatrices));

		// Create material constant buffer 
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer_Material);
		V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&constantBufferDesc, NULL, &m_pcbVSMaterial));
	}

	SAFE_RELEASE (pVertexShaderBuffer);
	return hr;
}

//
// Alpha_DX11VS_PosTextureInstance::SetupRender()
//
HRESULT Alpha_DX11VS_PosTextureInstance::SetupRender(Scene *pScene, DirectX11Renderable *pRenderable, int instanceCount, std::stack<RenderableInstance*>* instanceList)
{
	HRESULT hr = S_OK;

	// Set the vertex shader and the vertex layout
	g_pDX11W->GetD3D11DeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	g_pDX11W->GetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout);

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// ----- Transform Matricies ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_InstanceMatrices* pVSMatricies = (ConstantBuffer_InstanceMatrices*)MappedResource.pData;

	// Populate the instance array
	for(int i=0; i<instanceCount; i++)
	{
		D3DXMatrixTranspose(&pVSMatricies->m_WorldViewProj[i], &(instanceList->top()->m_World));
		instanceList->pop();
	}

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);

	// ----- Material ------
	V(g_pDX11W->GetD3D11DeviceContext()->Map(m_pcbVSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pPSMaterial = (ConstantBuffer_Material*)MappedResource.pData;

	Color color = pRenderable->VGet()->GetMaterial().GetDiffuse();
	pPSMaterial->m_vDiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	Color(1.0f, 1.0f, 1.0f, 1.0f);
	pPSMaterial->m_vAmbientObjectColor = Vec4(color.r, color.g, color.b, color.a);
	// Note - the vertex shader doesn't care about the texture one way or another so we'll just set it to false
	pPSMaterial->m_bHasTexture = false;

	g_pDX11W->GetD3D11DeviceContext()->Unmap(m_pcbVSMaterial, 0);
	g_pDX11W->GetD3D11DeviceContext()->VSSetConstantBuffers(1, 1, &m_pcbVSMaterial);

	return hr;
}