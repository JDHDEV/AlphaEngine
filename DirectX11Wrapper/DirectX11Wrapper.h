#pragma once
//========================================================================
// DirectX11Wrapper.h : to replace DXUT and provide further control
//
// Part of the Alpha Application
//
// http://www.directxtutorial.com/
//
// Justin Hunt
//========================================================================

// include my classes
#include "DirectX.h"

//---------------------------------------------------------------------------------------------------------------------
// Callback registration 
//---------------------------------------------------------------------------------------------------------------------

// General callbacks
typedef void    (CALLBACK *LPCALLBACKFRAMEMOVE)( double fTime, float fElapsedTime, void* pUserContext );
typedef LRESULT (CALLBACK *LPCALLBACKMSGPROC)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
typedef bool    (CALLBACK *LPCALLBACKMODIFYDEVICESETTINGS)( DX11WDeviceSettings* pDeviceSettings, void* pUserContext );

// Direct3D 11 callbacks
//typedef bool    (CALLBACK *LPCALLBACKISD3D11DEVICEACCEPTABLE)( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
typedef HRESULT (CALLBACK *LPCALLBACKD3D11DEVICECREATED)( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef HRESULT (CALLBACK *LPCALLBACKD3D11SWAPCHAINRESIZED)( ID3D11Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef void    (CALLBACK *LPCALLBACKD3D11FRAMERENDER)( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );
typedef void    (CALLBACK *LPCALLBACKD3D11SWAPCHAINRELEASING)( void* pUserContext );
typedef void    (CALLBACK *LPCALLBACKD3D11DEVICEDESTROYED)( void* pUserContext );

// the WindowProc function prototype
LRESULT CALLBACK DX11WStaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------------------------------------------------------------
// DirectX11Wrapper class
// Custom
//---------------------------------------------------------------------------------------------------------------------
class DirectX11Wrapper
{
private:

	// the dxw11state class
	DX11WState* g_pDX11WState;
	bool DestroyingState;

	// Window vars
	HINSTANCE	m_hPrevInstance;
	LPWSTR		m_lpCmdLine;
	int			m_nCmdShow;
	DWORD		m_dwStyle;

public:
	DirectX11Wrapper();
	~DirectX11Wrapper();
	HRESULT DX11WInit(LPWSTR lpCmdLine);

	// Winodows funtion
	HRESULT WINAPI DX11WCreateWindow( const WCHAR* strWindowTitle = L"Direct3D Window", 
                          HINSTANCE hInstance = NULL, HICON hIcon = NULL, HMENU hMenu = NULL,
                          int x = 0, int y = 0 );
	//HRESULT WINAPI DX11WCreateWindow( const WCHAR* strWindowTitle = L"Direct3D Window", 
    //                      HINSTANCE hInstance = NULL, HICON hIcon = NULL, HMENU hMenu = NULL,
    //                      int x = CW_USEDEFAULT, int y = CW_USEDEFAULT );

	// D3D11 function 
	HRESULT WINAPI DX11WCreateDevice(bool bWindowed, int nSuggestedWidth, int nSuggestedHeight);// sets up and initializes Direct3D
	void DX11WApplyDefaultDeviceSettings(DX11WDeviceSettings *modifySettings);
	void DX11WUpdateDeviceSettingsWithOverrides(DX11WDeviceSettings* pDeviceSettings);
	void DX11WUpdateBackBufferDesc();
	HRESULT DX11WCreateD3D11Views(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, DX11WDeviceSettings* pDeviceSettings);
	HRESULT WINAPI DX11WSetupD3D11Views(ID3D11DeviceContext* pd3dDeviceContext);

	// Set Handles
	void SetWindow(HWND hWndFocus);

		// Set Callbacks
	void SetCallbackFrameMove(LPCALLBACKFRAMEMOVE pCallback);
	void SetCallbackMsgProc(LPCALLBACKMSGPROC pCallBack);
	void SetCallbackDeviceChanging(LPCALLBACKMODIFYDEVICESETTINGS pCallback);

	// Direct3D 11 callbacks
	//void SetCallbackD3D11DeviceAcceptable(LPDXUTCALLBACKISD3D11DEVICEACCEPTABLE pCallback);
	void SetCallbackD3D11DeviceCreated( LPCALLBACKD3D11DEVICECREATED pCallback);
	void SetCallbackD3D11SwapChainResized( LPCALLBACKD3D11SWAPCHAINRESIZED pCallback);
	void SetCallbackD3D11FrameRender( LPCALLBACKD3D11FRAMERENDER pCallback);
	void SetCallbackD3D11SwapChainReleasing( LPCALLBACKD3D11SWAPCHAINRELEASING pCallback);
	void SetCallbackD3D11DeviceDestroyed( LPCALLBACKD3D11DEVICEDESTROYED pCallback);

	// Get Callbacks
	LPCALLBACKMSGPROC GetCallbackMsgProc();

	// General
	LRESULT WINAPI MainLoop();// move out of here should be in the alpha app, i doont like my wrapper have such power
	void WINAPI DX11WRender3DEnvironment();
	void EndRender();

	void Shutdown();

	// General
	HINSTANCE WINAPI GetHINSTANCE();
	HWND      WINAPI GetHWND();

	// Utility Functions
	void DX11WUpdateFrameStats();

// External state access functions
/*BOOL WINAPI DXUTGetMSAASwapChainCreated() { 
    DXUTDeviceSettings *psettings = GetDXUTState().GetCurrentDeviceSettings();
    if (psettings->ver == DXUT_D3D11_DEVICE) {
        return psettings->d3d11.sd.SampleDesc.Count > 1;
    }else if (psettings->ver == DXUT_D3D9_DEVICE) {
        return (psettings->d3d9.pp.MultiSampleType >= D3DMULTISAMPLE_2_SAMPLES);
    }
    else return false;
}*/

	ID3D11Device* WINAPI GetD3D11Device()                    {return GetDX11WState().GetD3D11Device();}
	///D3D_FEATURE_LEVEL	 WINAPI DX11WGetD3D11DeviceFeatureLevel() {return GetDX11WState().GetD3D11FeatureLevel();} // may need to add
	ID3D11DeviceContext* WINAPI GetD3D11DeviceContext()      {return GetDX11WState().GetD3D11DeviceContext();}
	IDXGISwapChain* WINAPI GetDXGISwapChain()                {return GetDX11WState().GetDXGISwapChain();}
	ID3D11RenderTargetView* WINAPI GetD3D11RenderTargetView() { return GetDX11WState().GetD3D11RenderTargetView(); }
	ID3D11DepthStencilView* WINAPI GetD3D11DepthStencilView() { return GetDX11WState().GetD3D11DepthStencilView(); }
	const DXGI_SURFACE_DESC* WINAPI GetDXGIBackBufferSurfaceDesc() { return GetDX11WState().GetBackBufferSurfaceDescDXGI(); }
	//HINSTANCE WINAPI DXUTGetHINSTANCE()                        { return GetDX11WState().GetHInstance(); }
	//HWND WINAPI DXUTGetHWND()                                  { return DXUTIsWindowed() ? GetDXUTState().GetHWNDDeviceWindowed() : GetDXUTState().GetHWNDDeviceFullScreen(); }
	/*HWND WINAPI DXUTGetHWNDFocus()                             { return GetDXUTState().GetHWNDFocus(); }
	HWND WINAPI DXUTGetHWNDDeviceFullScreen()                  { return GetDXUTState().GetHWNDDeviceFullScreen(); }
	HWND WINAPI DXUTGetHWNDDeviceWindowed()                    { return GetDXUTState().GetHWNDDeviceWindowed(); }*/
	RECT WINAPI GetWindowClientRect()                      { RECT rc; GetClientRect(GetHWND(), &rc); return rc; }
	LONG WINAPI GetWindowWidth()                           { RECT rc = GetWindowClientRect(); return ((LONG)rc.right - rc.left); }
	LONG WINAPI GetWindowHeight()                          { RECT rc = GetWindowClientRect(); return ((LONG)rc.bottom - rc.top); }
	/*RECT WINAPI DXUTGetWindowClientRectAtModeChange()          { RECT rc = { 0, 0, GetDXUTState().GetWindowBackBufferWidthAtModeChange(), GetDXUTState().GetWindowBackBufferHeightAtModeChange() }; return rc; }
	RECT WINAPI DXUTGetFullsceenClientRectAtModeChange()       { RECT rc = { 0, 0, GetDXUTState().GetFullScreenBackBufferWidthAtModeChange(), GetDXUTState().GetFullScreenBackBufferHeightAtModeChange() }; return rc; }
	*/
	double WINAPI GetTime()                                { return GetDX11WState().GetTime(); }
	float WINAPI GetElapsedTime()                          { return GetDX11WState().GetElapsedTime(); }
	float WINAPI GetFPS()                                  { return GetDX11WState().GetFPS(); }
	LPCWSTR WINAPI GetFrameStats(bool bIncludeFPS = false);
	/*
	LPCWSTR WINAPI DXUTGetWindowTitle()                        { return GetDXUTState().GetWindowTitle(); }
	LPCWSTR WINAPI DXUTGetDeviceStats()                        { return GetDXUTState().GetDeviceStats(); }
	bool WINAPI DXUTIsRenderingPaused()                        { return GetDXUTState().GetPauseRenderingCount() > 0; }
	bool WINAPI DXUTIsTimePaused()                             { return GetDXUTState().GetPauseTimeCount() > 0; }
	bool WINAPI DXUTIsActive()                                 { return GetDXUTState().GetActive(); }
	int WINAPI DXUTGetExitCode()                               { return GetDXUTState().GetExitCode(); }
	bool WINAPI DXUTGetShowMsgBoxOnError()                     { return GetDXUTState().GetShowMsgBoxOnError(); }
	bool WINAPI DXUTGetAutomation()                            { return GetDXUTState().GetAutomation(); }
	bool WINAPI DXUTIsWindowed()                               { return DXUTGetIsWindowedFromDS( GetDXUTState().GetCurrentDeviceSettings() ); }
	bool WINAPI DXUTIsInGammaCorrectMode()                     { return GetDXUTState().GetIsInGammaCorrectMode(); }
	IDirect3D9* WINAPI DXUTGetD3D9Object()                     { DXUTDelayLoadD3D9(); return GetDXUTState().GetD3D9(); }
	IDXGIFactory1* WINAPI DXUTGetDXGIFactory()                  { DXUTDelayLoadDXGI(); return GetDXUTState().GetDXGIFactory(); }
	bool WINAPI DXUTIsD3D11Available()                         { DXUTDelayLoadDXGI(); return GetDXUTState().GetD3D11Available(); }
	bool WINAPI DXUTIsAppRenderingWithD3D9()                   { return (GetDXUTState().GetD3D9Device() != NULL); }
	bool WINAPI DXUTIsAppRenderingWithD3D11()                  { return (GetDXUTState().GetD3D11Device() != NULL); }
	*/

private:

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* pDebug;
#endif	

	// DX11WState functions
	HRESULT WINAPI DX11WCreateState();
	void WINAPI DX11WDestroyState();
	DX11WState& GetDX11WState();

	// helper functions
	HRESULT DXW11LoadDXGI();
};

extern DirectX11Wrapper *g_pDX11W;
