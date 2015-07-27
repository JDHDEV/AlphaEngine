#pragma once
//========================================================================
// DirectX.h : All the directx Headers
//
// Part of the Alpha Application
//
// http://www.directxtutorial.com/
//
// Justin Hunt
//========================================================================

// include Windows Library files
//#pragma comment(lib, "windowscodecs.lib")

#ifndef STRICT
#define STRICT
#endif

// If app hasn't choosen, set to work with Windows XP and beyond
#ifndef WINVER
#define WINVER         0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600
#endif

#pragma comment(lib, "dxerr.lib")
//#pragma comment(lib, "dxguid.lib") // i think this is for DX9
#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d11.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "d3dx10d.lib")
#pragma comment(lib, "d3dx11d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#endif
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comctl32.lib")

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// Standard Windows includes
#include <windows.h>
#include <initguid.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>      // for placement new
#include <shlobj.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

// Direct3D9 includes
//#include <d3d9.h>
#include <d3dx9.h> //included for d3d materail and color conversions

// Direct3D11 includes
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include <d3dx9tex.h>
//#include <SDKmisc.h>
#include <D3D9Types.h>

// XInput includes
//#include <xinput.h>

// HRESULT translation for Direct3D and other APIs 
#include <dxerr.h>

#include <xnamath.h>

// STL includes
#include <algorithm>
#include <memory>
#include <vector>

//#if defined(DEBUG) || defined(_DEBUG)
//#ifndef V
//#define V(x)           { hr = (x); if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
//#endif
//#ifndef V_RETURN
//#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
//#endif
//#else
#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
//#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = nullptr; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p) = nullptr; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

#ifndef D3DCOLOR_ARGB
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#endif

// TODO move struct to better place

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct DX11WDeviceSettings
{
	UINT AdapterOrdinal;
	D3D_DRIVER_TYPE DriverType;
	UINT Output;
	DXGI_SWAP_CHAIN_DESC scd;
	UINT32 CreateFlags;
	UINT32 SyncInterval;
	DWORD PresentFlags;
	bool AutoCreateDepthStencil; // DXUT will create the depth stencil resource and view if true
	DXGI_FORMAT AutoDepthStencilFormat;
	D3D_FEATURE_LEVEL DeviceFeatureLevel;
};

struct FontType/// replace
{
	float left, right;
	int size;
};

struct DXUT_SCREEN_VERTEX_10
{
    float x, y, z;
    XMFLOAT4 color;
    float tu, tv;
};

typedef struct MyRECT
{
    float left, top, right, bottom;
} DX11WRECT;

//--------------------------------------------------------------------------------------
// Structs for shared resources
//--------------------------------------------------------------------------------------
//struct DX11WTextureNode
//{
//    bool bFileSource;  // True if this texture is loaded from a file. False if from resource.
//    HMODULE hResourceModule;
//    int nResourceID;   // Resource ID. If 0, string-based ID is used and stored in strFilename.
//    WCHAR strFilename[MAX_PATH];
//    DWORD dwWidth;
//    DWORD dwHeight;
//    ID3D11Texture2D* pTexture11;
//    ID3D11ShaderResourceView* pTexResView11;
//};

// bring in all vertex structs into one place.... HERE TODO

struct DX11WFontNode// i think this can be replaced with res handles or resources
{
    WCHAR strFace[MAX_PATH];
    LONG nHeight;
    LONG nWeight;
};

struct DX11WSpriteVertex
{
    XMFLOAT3 vPos;
    XMFLOAT4 vColor;
    XMFLOAT2 vTex;
};

inline XMFLOAT4 D3DCOLOR_TO_D3DCOLORVALUE(DWORD c)
{
    return XMFLOAT4 ( ( ( c >> 16 ) & 0xFF ) / 255.0f,
                      ( ( c >> 8 ) & 0xFF ) / 255.0f,
                      ( c & 0xFF ) / 255.0f,
                      ( ( c >> 24 ) & 0xFF ) / 255.0f );
}

inline DWORD D3DCOLORVALUE_TO_D3DCOLOR(XMFLOAT4 c)
{
	return D3DCOLOR_ARGB(int(c.w*255), int(c.x*255), int(c.y*255), int(c.z*255));
}

#include "DX11WState.h"
#include "DirectX11Wrapper.h"