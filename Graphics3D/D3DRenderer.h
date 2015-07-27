#pragma once
//========================================================================
// D3DRenderer.h : Defines the D3D renderer abstraction classes of the Alpha application
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//========================================================================

//#include <DXUTGui.h>

//
// struct ConstantBuffer_Matrices				- Chapter X, page Y
//
struct ConstantBuffer_Matrices
{
    Mat4x4 m_WorldViewProj;
    Mat4x4 m_World;
};

//
// struct ConstantBuffer_Material				- Chapter X, page Y
//
struct ConstantBuffer_Material
{
    Vec4 m_vDiffuseObjectColor;
	Vec4 m_vAmbientObjectColor;
	BOOL m_bHasTexture;
	Vec3 m_vUnused;
};

#define MAXIMUM_LIGHTS_SUPPORTED (8)

//
// struct ConstantBuffer_Lighting				- Chapter X, page Y
//
struct ConstantBuffer_Lighting
{
    Vec4 m_vLightDiffuse[MAXIMUM_LIGHTS_SUPPORTED];
    Vec4 m_vLightDir[MAXIMUM_LIGHTS_SUPPORTED];
	Vec4 m_vLightAmbient;
	UINT m_nNumLights;
	Vec3 m_vUnused;
};

//
// class D3DRenderer11                                                          - Chapter 10, page 270
//
//   The D3DRenderer and D3DRenderer9 classes are not discussed in the book. The D3DRenderer class is designed to 
//   implement the IRenderer interface, which abstracts the implentation of the renderer technology, which for this
//   engine can be either D3D9 or D3D11. It also encapsulates the usefulness of CDXUTDialogResourceManager 
//   and CDXUTTextHelper for user interface tasks whether D3D9 or D3D11 is being used.
//
class D3DRenderer : public IRenderer
{
public:
    // You should leave this global - it does wacky things otherwise.
    //static CDXUTDialogResourceManager g_DialogResourceManager;
	static DX11WGuiResourceManager g_GuiResourceManager;
    //static CDXUTTextHelper* g_pTextHelper;

    virtual HRESULT VOnRestore() { return S_OK; }
	virtual void VShutdown() { }//SAFE_DELETE(g_pTextHelper); }
};

//
// class D3DLineDrawer11 - not described in the book, it is a helper class to draw lines in D3D11
//
class D3DLineDrawer11 
{
public:
    D3DLineDrawer11() { m_pVertexBuffer = NULL;}
    ~D3DLineDrawer11() { SAFE_RELEASE(m_pVertexBuffer); }

    void DrawLine(const Vec3& from,const Vec3& to,const Color& color);
    HRESULT OnRestore();

protected:
    Vec3					m_Verts[2];
    LineDraw_Hlsl_Shader    m_LineDrawerShader;
    ID3D11Buffer*			m_pVertexBuffer;
};

class AlphaD3DLineDrawer 
{
public:
    AlphaD3DLineDrawer() { m_pVertexBuffer = NULL; m_VertexShader.EnableLights(false);}
    ~AlphaD3DLineDrawer() { SAFE_RELEASE(m_pVertexBuffer); }

    void DrawLine(const Vec3& from,const Vec3& to,const Color& color);
    HRESULT OnRestore();

protected:
    D3D11Vertex_UnlitTextured	m_Verts[2];

	Alpha_Hlsl_VertexShader		m_VertexShader;
	Alpha_Hlsl_PixelShader		m_PixelShader;
	Alpha_Hlsl_LineShader       m_LineShader;
    ID3D11Buffer*				m_pVertexBuffer;
};

class D3DRenderer11 : public D3DRenderer
{
public:
    D3DRenderer11() { m_pLineDrawer = NULL; }
    virtual void VShutdown()  { D3DRenderer::VShutdown(); SAFE_DELETE(m_pLineDrawer); }

    virtual void VSetBackgroundColor(BYTE bgR, BYTE bgG, BYTE bgB, BYTE bgA) 
    { 
        m_backgroundColor[0] = float(bgR) / 255.0f;
        m_backgroundColor[1] = float(bgG) / 255.0f;
        m_backgroundColor[2] = float(bgB) / 255.0f;
        m_backgroundColor[3] = float(bgA) / 255.0f;
    }

    virtual bool VPreRender();
    virtual bool VPostRender();
    virtual HRESULT VOnRestore();
    virtual void VCalcLighting(Lights *lights, int maximumLights) { }

    // These three functions are done for each shader, not as a part of beginning the render - so they do nothing in D3D11.
    virtual void VSetWorldTransform(const Mat4x4 *m) {  }   
    virtual void VSetViewTransform(const Mat4x4 *m) {  }
    virtual void VSetProjectionTransform(const Mat4x4 *m) {  }

	virtual void VDrawLine(const Vec3& from,const Vec3& to,const Color& color);

    virtual shared_ptr<IRenderState> VPrepareAlphaPass();
    virtual shared_ptr<IRenderState> VPrepareSkyBoxPass();
	virtual shared_ptr<IRenderState> VPrepareWireFramePass();

    HRESULT CompileShader( LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );
    HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

protected:
    float m_backgroundColor[4];
	AlphaD3DLineDrawer *m_pLineDrawer;
};