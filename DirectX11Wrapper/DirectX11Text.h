#pragma once
//========================================================================
// DirectX11Font.h : to replace DXUT and provide further control
//
// Part of the Alpha Application
//
// http://www.rastertek.com/dx11tut12.html
//
// Justin Hunt
//========================================================================

#include "DirectX.h"
#include "DirectX11Font.h"
//#include "fontshaderclass.h"


//class CDXUTTextHelper
//{
//public:
//    CDXUTTextHelper( _In_ ID3D11Device* pd3d11Device, _In_ ID3D11DeviceContext* pd3dDeviceContext, _In_ CDXUTDialogResourceManager* pManager, _In_ int nLineHeight );
//    ~CDXUTTextHelper();
//
//    void Init( _In_ int nLineHeight = 15 );
//
//    void SetInsertionPos( _In_ int x, _In_ int y )
//    {
//        m_pt.x = x;
//        m_pt.y = y;
//    }
//    void SetForegroundColor( _In_ DirectX::XMFLOAT4 clr ) { m_clr = clr; }
//    void SetForegroundColor( _In_ DirectX::FXMVECTOR clr ) { XMStoreFloat4( &m_clr, clr ); }
//
//    void    Begin();
//    HRESULT DrawFormattedTextLine( _In_z_ const WCHAR* strMsg, ... );
//    HRESULT DrawTextLine( _In_z_ const WCHAR* strMsg );
//    HRESULT DrawFormattedTextLine( _In_ const RECT& rc, _In_z_ const WCHAR* strMsg, ... );
//    HRESULT DrawTextLine( _In_ const RECT& rc, _In_z_ const WCHAR* strMsg );
//    void    End();
//
//protected:
//    DirectX::XMFLOAT4 m_clr;
//    POINT m_pt;
//    int m_nLineHeight;
//
//    // D3D11 font 
//    ID3D11Device* m_pd3d11Device;
//    ID3D11DeviceContext* m_pd3d11DeviceContext;
//    CDXUTDialogResourceManager* m_pManager;
//};

class DX11WText
{
public:
	DX11WFont* m_pFont;
	std::vector<DXUT_SCREEN_VERTEX_10> m_FontVertices;
	ID3D11Buffer* m_pFontBuffer11;// could make these staic and merge with font class
	UINT m_FontBufferBytes11;// could make these staic and merge with font class
	ID3D11InputLayout* m_pInputLayout11;
	
public:
	DX11WText();
	~DX11WText();

	void InitText(DX11WFont* pFont, ID3D11InputLayout* pInputLayout);
	void Release();

	void ClearFont11();
	void BeginText11();
	void DrawText11(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext,
                    LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight,
                    float fBBWidth, float fBBHeight, bool bCenter);
	void EndText11(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext);
};

////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////class TextClass
//{
//private:
//
//	struct SentenceType
//	{
//		ID3D11Buffer *vertexBuffer, *indexBuffer;
//		int vertexCount, indexCount, maxLength;
//		float red, green, blue;
//	};
//
//	struct DXUT_SCREEN_VERTEX_10
//	{
//		D3DXVECTOR3 position;
//		D3DXVECTOR2 texture;
//	};
//
//public:
//	TextClass();
//	TextClass(const TextClass&);
//	~TextClass();
//
//	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
//	void Shutdown();
//	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);
//
//private:
//	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
//	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
//	void ReleaseSentence(SentenceType**);
//	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX);
//
//private:
//	FontClass* m_Font;
//	FontShaderClass* m_FontShader;
//	int m_screenWidth, m_screenHeight;
//	D3DXMATRIX m_baseViewMatrix;
//We will use two sentences in this tutorial.
//
//	SentenceType* m_sentence1;
//	SentenceType* m_sentence2;
//};
//
