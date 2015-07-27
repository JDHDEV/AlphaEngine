#pragma once
//========================================================================
// DirectX11Vertex.h : Defeine all DirectX11 Vetex Structs, Layouts and Shaders
//
// Part of the Alpha Application
//
// http://www.braynzarsoft.net/
//
// Justin Hunt
//========================================================================

class DirectX11Renderable;
struct DirectX11RenderableInstance;

//
// D3D11VertexPos
//
struct D3D11VertexPos
{
	D3D11VertexPos(){}
	D3D11VertexPos(float x, float y, float z)
		: pos(x,y,z){}

	XMFLOAT3 pos;
};

const D3D11_INPUT_ELEMENT_DESC D3D11VertexPosLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
};

const UINT D3D11VertexPosLayoutSize = ARRAYSIZE(D3D11VertexPosLayout);

//
// Alpha_D3D11VertexPosBuffer
//
class Alpha_D3D11VertexPosBuffer
{
public:
    Alpha_D3D11VertexPosBuffer();
    ~Alpha_D3D11VertexPosBuffer();

	HRESULT OnRestore(D3D11VertexPos *pVertices, DWORD numVerts, WORD *pIndices = nullptr, DWORD numPolys = 0);
    HRESULT SetupRender();

protected:
	ID3D11Buffer*           m_pIndexBuffer;
	ID3D11Buffer*           m_pVertexBuffer;
	bool					m_bIndex;
};

//
// Alpha_D3D11VertexPosScreenShader
//
class Alpha_D3D11VertexPosScreenShader
{
public:
    Alpha_D3D11VertexPosScreenShader();
    ~Alpha_D3D11VertexPosScreenShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender();

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
};

//
// D3D11VertexPosColor
//
struct D3D11VertexPosColor
{
	D3D11VertexPosColor(){}
	D3D11VertexPosColor(float x, float y, float z,
		float cr, float cg, float cb, float ca)
		: pos(x,y,z), color(cr, cg, cb, ca){}

	XMFLOAT3 pos;
	XMFLOAT4 color;
};

const D3D11_INPUT_ELEMENT_DESC D3D11VertexPosColorLayout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const UINT D3D11VertexPosColorLayoutSize = ARRAYSIZE(D3D11VertexPosColorLayout);

//
// Alpha_D3D11VertexPosColorBuffer
//
class Alpha_D3D11VertexPosColorBuffer
{
public:
    Alpha_D3D11VertexPosColorBuffer();
    ~Alpha_D3D11VertexPosColorBuffer();

	HRESULT OnRestore(D3D11VertexPosColor *pVertices, DWORD numVerts, WORD *pIndices = nullptr, DWORD numPolys = 0);
    HRESULT SetupRender();

protected:
	ID3D11Buffer*           m_pIndexBuffer;
	ID3D11Buffer*           m_pVertexBuffer;
	bool					m_bIndex;
};

//
// Alpha_D3D11VertexPosColorScreenShader
//
class Alpha_D3D11VertexPosColorScreenShader
{
public:
    Alpha_D3D11VertexPosColorScreenShader();
    ~Alpha_D3D11VertexPosColorScreenShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender();

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
};

//
// Alpha_D3D11VertexPosColorShader
//
class Alpha_D3D11VertexPosColorShader
{
public:
    Alpha_D3D11VertexPosColorShader();
    ~Alpha_D3D11VertexPosColorShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene);

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11Buffer*           m_pcbVSMatrices;
};

//
// D3D11VertexPosColorNormal
//
struct D3D11VertexPosColorNormal
{
	D3D11VertexPosColorNormal(){}
	D3D11VertexPosColorNormal(float x, float y, float z,
		float cr, float cg, float cb, float ca,
		float nx, float ny, float nz)
		: pos(x,y,z), color(cr, cg, cb, ca), normal(nx, ny, nz){}
	
	D3D11VertexPosColorNormal(Vec3 Pos, Vec4 Color, Vec3 Normal)
		: pos(Pos.x,Pos.y,Pos.z), color(Color.x, Color.y, Color.z, Color.w), normal(Normal.x, Normal.y, Normal.z){}

	XMFLOAT3 pos;
	XMFLOAT4 color;
	XMFLOAT3 normal;
};

const D3D11_INPUT_ELEMENT_DESC D3D11VertexPosColorNormalLayout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const UINT D3D11VertexPosColorNormalLayoutSize = ARRAYSIZE(D3D11VertexPosColorNormalLayout);

//
// Alpha_D3D11VertexPosColorNormalBuffer
//
class Alpha_D3D11VertexPosColorNormalBuffer
{
public:
    Alpha_D3D11VertexPosColorNormalBuffer();
    ~Alpha_D3D11VertexPosColorNormalBuffer();

	HRESULT OnRestore(D3D11VertexPosColorNormal *pVertices, DWORD numVerts, WORD *pIndices = nullptr, DWORD numPolys = 0);
    HRESULT SetupRender();

protected:
	ID3D11Buffer*           m_pIndexBuffer;
	ID3D11Buffer*           m_pVertexBuffer;
	bool					m_bIndex;
};

//
// Alpha_D3D11VertexPosColorNormalShader
//
class Alpha_D3D11VertexPosColorNormalShader
{
public:
    Alpha_D3D11VertexPosColorNormalShader();
    ~Alpha_D3D11VertexPosColorNormalShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11Buffer*           m_pcbVSMatrices;
	ID3D11Buffer*			m_pcbVSLighting;
	ID3D11Buffer*           m_pcbVSMaterial;
	bool					m_enableLights;
};

//
// D3D11VertexPosNormalTexture
//
struct D3D11VertexPosNormalTexture
{
	D3D11VertexPosNormalTexture(){}
	D3D11VertexPosNormalTexture(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
		: pos(x,y,z), normal(nx, ny, nz), texCoord(u, v){}
	
	D3D11VertexPosNormalTexture(Vec3 Pos, Vec3 Normal, Vec2 TexCoord)
		: pos(Pos.x,Pos.y,Pos.z), normal(Normal.x, Normal.y, Normal.z), texCoord(TexCoord.x, TexCoord.y) {}

	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 texCoord;
};

const D3D11_INPUT_ELEMENT_DESC D3D11VertexPosNormalTextureLayout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const UINT D3D11VertexPosNormalTextureLayoutSize = ARRAYSIZE(D3D11VertexPosNormalTextureLayout);

//
// Alpha_D3D11VertexPosNormalTextureBuffer
//
class Alpha_D3D11VertexPosNormalTextureBuffer
{
public:
    Alpha_D3D11VertexPosNormalTextureBuffer();
    ~Alpha_D3D11VertexPosNormalTextureBuffer();

	HRESULT OnRestore(D3D11VertexPosNormalTexture *pVertices, DWORD numVerts, WORD *pIndices = nullptr, DWORD numPolys = 0);
    HRESULT SetupRender();

protected:
	ID3D11Buffer*           m_pIndexBuffer;
	ID3D11Buffer*           m_pVertexBuffer;
	bool					m_bIndex;
};

//
// Alpha_D3D11VertexPosNormalTextureShader
//
class Alpha_D3D11VertexPosNormalTextureShader
{
public:
    Alpha_D3D11VertexPosNormalTextureShader();
    ~Alpha_D3D11VertexPosNormalTextureShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11Buffer*           m_pcbVSMatrices;
};

//
// D3D11VertexPosTexture
//
struct D3D11VertexPosTexture
{
	D3D11VertexPosTexture(){}
	D3D11VertexPosTexture(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}
	
	D3D11VertexPosTexture(Vec3 Pos, Vec2 TexCoord)
		: pos(Pos.x,Pos.y,Pos.z), texCoord(TexCoord.x, TexCoord.y){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

const D3D11_INPUT_ELEMENT_DESC D3D11VertexPosTextureLayout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const UINT D3D11VertexPosTextureLayoutSize = ARRAYSIZE(D3D11VertexPosTextureLayout);

//
// Alpha_D3D11VertexPosTextureBuffer
//
class Alpha_D3D11VertexPosTextureBuffer
{
public:
    Alpha_D3D11VertexPosTextureBuffer();
    ~Alpha_D3D11VertexPosTextureBuffer();

	HRESULT OnRestore(D3D11VertexPosTexture *pVertices, DWORD numVerts, WORD *pIndices = nullptr, DWORD numPolys = 0);
    HRESULT SetupRender();
	HRESULT SetupRenderInstance(UINT instanceStride, ID3D11Buffer* pInstanceBuffer);

protected:
	ID3D11Buffer*           m_pIndexBuffer;
	ID3D11Buffer*           m_pVertexBuffer;
	bool					m_bIndex;
};

//
// Alpha_D3D11VertexPosTextureShader
//
class Alpha_D3D11VertexPosTextureShader
{
public:
    Alpha_D3D11VertexPosTextureShader();
    ~Alpha_D3D11VertexPosTextureShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
	HRESULT SetupRenderInstance(Scene *pScene, SceneNode *pNode);

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11Buffer*           m_pcbVSMatrices;
	ID3D11Buffer*			m_pcbVSLighting;
	ID3D11Buffer*           m_pcbVSMaterial;
	bool					m_enableLights;
};

//
// Alpha_D3D11PosTextureSkyVShader
//
class Alpha_D3D11PosTextureSkyVShader
{
public:
    Alpha_D3D11PosTextureSkyVShader();
    ~Alpha_D3D11PosTextureSkyVShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11Buffer*           m_pcbVSMatrices;
	ID3D11Buffer*           m_pcbVSMaterial;
	bool					m_enableLights;
};

struct ConstantBuffer_InstanceMatrices
{
    Mat4x4 m_WorldViewProj[100];
};

//
// Alpha_DX11VS_PosTextureInstance
//
class Alpha_DX11VS_PosTextureInstance
{
public:
    Alpha_DX11VS_PosTextureInstance();
    ~Alpha_DX11VS_PosTextureInstance();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
	HRESULT SetupRender(Scene *pScene, DirectX11Renderable *pRenderable, int instanceCount, std::stack<RenderableInstance*>* instanceList);

protected:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11Buffer*           m_pcbVSMatrices;
	ID3D11Buffer*           m_pcbVSMaterial;
};