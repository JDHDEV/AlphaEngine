//========================================================================
// DirectX11Wrapper.cpp - Wrap up DX11
//
// Part of the Alpha Application
//
// http://www.directxtutorial.com/
//
// Justin Hunt
//========================================================================

#include "AlphaStd.h"
#include "DirectX.h"
#include "../Utilities/AlphaTimer.h"
#include <sstream> 
DirectX11Wrapper *g_pDX11W = NULL;

//========================================================================
//
// DirectX11Wrapper implementation
//
//========================================================================
DirectX11Wrapper::DirectX11Wrapper()
{
        g_pDX11W = this;
		g_pDX11WState = NULL;
		DestroyingState = FALSE;
  			
		//Things that should load from file or be more dynamicly set
		//m_dwStyle = WS_OVERLAPPEDWINDOW;
		m_dwStyle=(WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);//  | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

		DX11WCreateState();
}

DirectX11Wrapper::~DirectX11Wrapper()
{
	DestroyingState = TRUE;
	DX11WDestroyState();

#if defined(DEBUG) || defined(_DEBUG)
if (pDebug != nullptr)
{
	pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	pDebug->Release();
	pDebug = nullptr;
}
#endif
}

/*
HRESULT WINAPI DXUTInit( bool bParseCommandLine, 
                         bool bShowMsgBoxOnError, 
                         __in_opt WCHAR* strExtraCommandLineParams, 
                         bool bThreadSafeDXUT )
*/
//
// DirectX11Wrapper::Init
//
HRESULT DirectX11Wrapper::DX11WInit(LPWSTR lpCmdLine)
{
	m_lpCmdLine = lpCmdLine;
	
	// Reset the timer
    GetGlobalAlphaTimer()->Reset();

	// Reset the timer here
	GetDX11WState().SetDX11WInited(TRUE);

    return S_OK;
}

//
// DirectX11Wrapper::CreateWindow
//
HRESULT WINAPI DirectX11Wrapper::DX11WCreateWindow(const WCHAR* strWindowTitle, HINSTANCE hInstance,
                                 HICON hIcon, HMENU hMenu, int x, int y)
{
	HRESULT hr;

	if((GetDX11WState().GetWindowCreated() == FALSE)&&(GetDX11WState().GetDX11WInited() == TRUE))
    {

		// the handle for the window, filled by a function
		HWND hWnd;
		// this struct holds information for the window class
		WNDCLASSEX wc;

		// clear out the window class for use
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		// fill in the struct with the needed information
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_DBLCLKS;//CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DX11WStaticWndProc;
		wc.cbClsExtra = 0;// idk but DXUT had em
		wc.cbWndExtra = 0;// idk but DXUT had em
		wc.hInstance = hInstance;
		wc.hIcon = hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		//DXUT has this set but that is for demos not games, i will assume Fullscreen unless set otherwise
		//wc.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );//(HBRUSH)COLOR_WINDOW; 
		wc.lpszMenuName = NULL;// idk but DXUT had em
		wc.lpszClassName = L"Direct3DWindowClass";

		// register the window class
		if(!RegisterClassEx(&wc))
		{
			DWORD ERR = GetLastError();
			// to do profvide more info to errmsg
			AC_FATAL("Failed to register Window class");
		}

		
        // Set the defualt Width and height then over ride if over riden.
        int nDefaultWidth = 640;
        int nDefaultHeight = 480;
        //if( GetDXUTState().GetOverrideWidth() != 0 )
            //nDefaultWidth = GetDXUTState().GetOverrideWidth();
        //if( GetDXUTState().GetOverrideHeight() != 0 )
            //nDefaultHeight = GetDXUTState().GetOverrideHeight();

		// adjust for borders and menu
		RECT wr = {0, 0, nDefaultWidth, nDefaultHeight};    // set the size, but not the position
		AdjustWindowRect(&wr, m_dwStyle, FALSE);    // adjust the size

		// create the window and use the result as the handle
		hWnd = CreateWindowEx(NULL,
							  L"Direct3DWindowClass",   // name of the window class
							  strWindowTitle,   // title of the window
							  m_dwStyle,    // window style
							  x,    // x-position of the window
							  y,    // y-position of the window
							  (wr.right - wr.left),    // width of the window
							  (wr.bottom - wr.top),    // height of the window
							  NULL,    // we have no parent window, NULL
							  NULL,    // we aren't using menus, NULL
							  hInstance,    // application handle
							  NULL);    // used with multiple windows, NULL

		// How DXUT creates the render window
		// HWND hWnd = CreateWindow( L"Direct3DWindowClass", strWindowTitle, WS_OVERLAPPEDWINDOW,
									 // x, y, ( rc.right - rc.left ), ( rc.bottom - rc.top ), 0,
									 // hMenu, hInstance, 0 );
		SetWindow(hWnd);
	}

	return S_OK;
}

//
// DirectX11Wrapper::DX11WCreateDevice
//
HRESULT WINAPI DirectX11Wrapper::DX11WCreateDevice(bool bWindowed, int nSuggestedWidth, int nSuggestedHeight)
{
    HRESULT hr = S_OK;

	// If DX11WCreateWindow() has not already been called, 
    // then call DX11WCreateWindow() with the default parameters.         
	if(!GetDX11WState().GetWindowCreated())
    {
		//TODO: check if this has failed before
        hr = DX11WCreateWindow();
        if(FAILED(hr))
            return hr;
    }

	/// push this device setting into the state and move thiere creation to another function
	DX11WDeviceSettings deviceSettings;
	DX11WApplyDefaultDeviceSettings(&deviceSettings);
	//deviceSettings.MinimumFeatureLevel = D3D_FEATURE_LEVEL_10_1;
    deviceSettings.scd.BufferDesc.Width = nSuggestedWidth;
    deviceSettings.scd.BufferDesc.Height = nSuggestedHeight;
    deviceSettings.scd.Windowed = bWindowed;
	//deviceSettings.ver = DXUT_D3D11_DEVICE;

	// overides from direct x tuitorial tutorial
    //deviceSettings.scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    //deviceSettings.scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    //deviceSettings.scd.SampleDesc.Count = 4;                               // how many multisamples
	
	// Load overiden properties
	DX11WUpdateDeviceSettingsWithOverrides(&deviceSettings); 

	/*
    // Change to a Direct3D device created from the new device settings.  
    // If there is an existing device, then either reset or recreated the scene
    hr = DXUTChangeDevice( &deviceSettings, NULL, NULL, false, true );
	*/

	//create the IDXGIFactory
    hr = DXW11LoadDXGI();

	// move to another funtion when you can
	//**DXUTCreate3DEnvironment11

	ID3D11Device* pd3d11Device = nullptr;
    ID3D11DeviceContext* pd3dImmediateContext = nullptr;
    D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

    IDXGISwapChain* pSwapChain = nullptr;
        
    IDXGIFactory* pDXGIFactory = GetDX11WState().GetDXGIFactory(); //DX11WGetDXGIFactory(); 
    assert( pDXGIFactory != NULL );
	hr = pDXGIFactory->MakeWindowAssociation(GetDX11WState().GetHWNDFocus(), 0);
	if(FAILED(hr))
	{
		AC_LOG("INFO","NO WINDO ASSOCIATION");
	}

	//// Try to create the device with the chosen settings
 //   IDXGIAdapter1* pAdapter = nullptr;

 //   hr = S_OK;
 //   D3D_DRIVER_TYPE ddt = pNewDeviceSettings->d3d11.DriverType;
 //   if( pNewDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_HARDWARE ) 
 //   {
 //       hr = pDXGIFactory->EnumAdapters1( pNewDeviceSettings->d3d11.AdapterOrdinal, &pAdapter );
 //       if ( FAILED( hr) ) 
 //       {
 //           return E_FAIL;
 //       }
 //       ddt = D3D_DRIVER_TYPE_UNKNOWN;    
 //   }
 //   else if (pNewDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_WARP) 
 //   {
 //       ddt = D3D_DRIVER_TYPE_WARP;  
 //       pAdapter = nullptr;
 //   }
 //   else if (pNewDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE) 
 //   {
 //       ddt = D3D_DRIVER_TYPE_REFERENCE;
 //       pAdapter = nullptr;
 //   } Look at DXUT to see how to impolemnt the adpators

	// create a device, device context and swap chain using the information in the scd struct
    /*D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &deviceSettings.scd,
                                  &swapchain,
                                  &dev,
                                  NULL,
                                  &devcon);*/

	// Create the device and device context objects
    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
#if defined(DEBUG) || defined(_DEBUG)
		D3D11_CREATE_DEVICE_DEBUG,
#else 
		0,
#endif
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &pd3d11Device,
        nullptr,
        &pd3dImmediateContext);
    
	if (FAILED(hr))
    {
        //DXUT_ERR( L"D3D11CreateDevice", hr );
        //return DXUTERR_CREATINGDEVICE;
		return hr;
    }

	// Set D3DDeviceInited
	GetDX11WState().SetDeviceCreated(TRUE);
	
	// set default render state to msaa enabled
    D3D11_RASTERIZER_DESC drd = {
        D3D11_FILL_SOLID, //D3D11_FILL_MODE FillMode;
        D3D11_CULL_BACK,//D3D11_CULL_MODE CullMode;
        FALSE, //BOOL FrontCounterClockwise;
        0, //INT DepthBias;
        0.0f,//FLOAT DepthBiasClamp;
        0.0f,//FLOAT SlopeScaledDepthBias;
        TRUE,//BOOL DepthClipEnable;
        FALSE,//BOOL ScissorEnable;
        TRUE,//BOOL MultisampleEnable;
        FALSE//BOOL AntialiasedLineEnable;        
    };
    ID3D11RasterizerState* pRS = nullptr;
    hr = pd3d11Device->CreateRasterizerState(&drd, &pRS);
    if (FAILED(hr))
    {
        //DXUT_ERR( L"CreateRasterizerState", hr );
        //return DXUTERR_CREATINGDEVICE;
    }
    //DXUT_SetDebugName(pRS, "DXUT Default");
    GetDX11WState().SetD3D11RasterizerState(pRS);
    pd3dImmediateContext->RSSetState(pRS);

	// Create the swapchain
	hr = GetDX11WState().GetDXGIFactory()->CreateSwapChain(pd3d11Device, &deviceSettings.scd, &pSwapChain);
    if(FAILED(hr))
    {
        AC_ERROR("CreateSwapChain");
        return E_FAIL;//set to more descriptive
    }

	GetDX11WState().SetD3D11Device(pd3d11Device);
    GetDX11WState().SetD3D11DeviceContext(pd3dImmediateContext);
    //GetDX11WState().SetD3D11FeatureLevel( FeatureLevel );
    GetDX11WState().SetDXGISwapChain(pSwapChain);

    // Update back buffer desc before calling app's device callbacks
    DX11WUpdateBackBufferDesc();

	// Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)

    // Update the device stats text

	// Call the app's device created callback if non-NULL
    LPCALLBACKD3D11DEVICECREATED pCallbackDeviceCreated = GetDX11WState().GetD3D11DeviceCreatedFunc();
	hr = S_OK;
    if(pCallbackDeviceCreated)
    {
        hr = pCallbackDeviceCreated(pd3d11Device, GetDX11WState().GetBackBufferSurfaceDescDXGI(), NULL);// null for void pointer whihc is d3d9 will remove later

        if(!GetD3D11Device())// may not need this check here, something to think about it is in DXUT
            return E_FAIL;
    }
	if(FAILED(hr))
    {
        //DXUT_ERR( L"DeviceCreated callback", hr );
        //return ( hr == DXUTERR_MEDIANOTFOUND ) ? DXUTERR_MEDIANOTFOUND : DXUTERR_CREATINGDEVICEOBJECTS;
		return hr;
    }

	// Create viewport and render view
	hr = DX11WCreateD3D11Views(pd3d11Device, pd3dImmediateContext, &deviceSettings);
    if(FAILED(hr))
    {
		//DXUT_ERR( L"DXUTCreateD3D11Views", hr );
		//return DXUTERR_CREATINGDEVICEOBJECTS;
		return hr;
    }

	// Call the app's swap chain reset callback if non-NULL
    LPCALLBACKD3D11SWAPCHAINRESIZED pCallbackSwapChainResized = GetDX11WState().GetD3D11SwapChainResizedFunc();
    hr = S_OK;
    if(pCallbackSwapChainResized)
	{
        hr = pCallbackSwapChainResized(pd3d11Device, pSwapChain,  GetDX11WState().GetBackBufferSurfaceDescDXGI(),
                                        NULL);// null for void pointer whihc is d3d9 will remove later
		if(!GetD3D11Device()) // Handle DXUTShutdown from inside callback
			return E_FAIL;
	}
    if(FAILED(hr))
    {
        //DXUT_ERR( L"DeviceReset callback", hr );
        //return ( hr == DXUTERR_MEDIANOTFOUND ) ? DXUTERR_MEDIANOTFOUND : DXUTERR_RESETTINGDEVICEOBJECTS;
		return hr;
    }

	// Make a window rect with a client rect that is the same size as the backbuffer
	RECT rcWindow = {0};
	rcWindow.right = nSuggestedWidth;//(long)(deviceSettings.scd.BufferDesc.Width);
	rcWindow.bottom = nSuggestedHeight;//(long)(deviceSettings.scd.BufferDesc.Height);
	AdjustWindowRect(&rcWindow, GetWindowLong(GetDX11WState().GetHWNDFocus(), GWL_STYLE ), false);// false is for no icon true for icon

	// Resize the window.  It is important to adjust the window size 
	// after resetting the device rather than beforehand to ensure 
	// that the monitor resolution is correct and does not limit the size of the new window.
	int cx = ( int )( rcWindow.right - rcWindow.left );
	int cy = ( int )( rcWindow.bottom - rcWindow.top );
	SetWindowPos(GetDX11WState().GetHWNDFocus(), 0, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);

	// if valid hWnd then display the window on the screen
	if(GetDX11WState().GetWindowCreated())
		ShowWindow(GetDX11WState().GetHWNDFocus(), 1);// figure out what second param should be

	return hr;
}

//
// DirectX11Wrapper::DX11WApplyDefaultDeviceSettings
//
void DirectX11Wrapper::DX11WApplyDefaultDeviceSettings(DX11WDeviceSettings *modifySettings)
{
    ZeroMemory( modifySettings, sizeof(DX11WDeviceSettings) );

    //modifySettings->ver = DXUT_D3D11_DEVICE;
    modifySettings->AdapterOrdinal = 0;
    modifySettings->AutoCreateDepthStencil = true;
    modifySettings->AutoDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
#if defined(DEBUG) || defined(_DEBUG)
    modifySettings->CreateFlags |= D3D10_CREATE_DEVICE_DEBUG;
#else
    modifySettings->CreateFlags = 0;
#endif
    modifySettings->DriverType = D3D_DRIVER_TYPE_HARDWARE;
    modifySettings->Output = 0;
    modifySettings->PresentFlags = 0;
    modifySettings->scd.BufferCount = 2;
    modifySettings->scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//_SRGB;
    modifySettings->scd.BufferDesc.Height = 480;
    modifySettings->scd.BufferDesc.RefreshRate.Numerator = 0;
    modifySettings->scd.BufferDesc.RefreshRate.Denominator = 0;
    modifySettings->scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    modifySettings->scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    modifySettings->scd.BufferDesc.Width = 640;
    modifySettings->scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//32;
    modifySettings->scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
    modifySettings->scd.OutputWindow = GetDX11WState().GetHWNDFocus();
    modifySettings->scd.SampleDesc.Count = 4;//1;
    modifySettings->scd.SampleDesc.Quality = 0;
    modifySettings->scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    modifySettings->scd.Windowed = 1;
    modifySettings->SyncInterval = 0;
}

//
// DirectX11Wrapper::DX11WUpdateDeviceSettingsWithOverrides
// Updates the device settings with default values..  
//
void DirectX11Wrapper::DX11WUpdateDeviceSettingsWithOverrides(DX11WDeviceSettings* pDeviceSettings)
{
    // Override with settings from the command line
}

//
// DirectX11Wrapper::DX11WUpdateBackBufferDesc
// Stores back buffer surface desc in GetDXUTState().GetBackBufferSurfaceDesc10()
//
void DirectX11Wrapper::DX11WUpdateBackBufferDesc()
{
    HRESULT hr;
    ID3D11Texture2D* pBackBuffer;
    IDXGISwapChain* pSwapChain = GetDX11WState().GetDXGISwapChain();
	assert( pSwapChain != NULL );
    hr = pSwapChain->GetBuffer( 0, __uuidof( *pBackBuffer ), ( LPVOID* )&pBackBuffer );
	auto pBBufferSurfaceDesc = GetDX11WState().GetBackBufferSurfaceDescDXGI();
    //DXGI_SURFACE_DESC* pBBufferSurfaceDesc = &GetDX11WState().GetBackBufferSurfaceDescDXGI();
    ZeroMemory( pBBufferSurfaceDesc, sizeof( DXGI_SURFACE_DESC ) );
    if( SUCCEEDED( hr ) )
    {
        D3D11_TEXTURE2D_DESC TexDesc;
        pBackBuffer->GetDesc( &TexDesc );
        pBBufferSurfaceDesc->Width = ( UINT )TexDesc.Width;
        pBBufferSurfaceDesc->Height = ( UINT )TexDesc.Height;
        pBBufferSurfaceDesc->Format = TexDesc.Format;
        pBBufferSurfaceDesc->SampleDesc = TexDesc.SampleDesc;
        SAFE_RELEASE( pBackBuffer );
    }
	//GetDX11WState().SetBackBufferSurfaceDescDXGI(pBBufferSurfaceDesc);
}

HRESULT DirectX11Wrapper::DX11WCreateD3D11Views(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, DX11WDeviceSettings* pDeviceSettings)
{
	HRESULT hr = S_OK;
	auto pSwapChain = GetDXGISwapChain();
    ID3D11DepthStencilView* pDSV = nullptr;
	ID3D11RenderTargetView* pRTV = nullptr;
	
    // Get the back buffer and desc
    ID3D11Texture2D* pBackBuffer;
    hr = pSwapChain->GetBuffer(0, __uuidof(*pBackBuffer), (LPVOID*)&pBackBuffer);
    if(FAILED(hr))
        return hr;
    D3D11_TEXTURE2D_DESC backBufferSurfaceDesc;
    pBackBuffer->GetDesc(&backBufferSurfaceDesc);

    // Use the back buffer address to create the render target view
	hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRTV);
    SAFE_RELEASE(pBackBuffer);
	if(FAILED(hr))
        return hr;
	GetDX11WState().SetD3D11RenderTargetView(pRTV);

	if(pDeviceSettings->AutoCreateDepthStencil)
    {
        // Create depth stencil texture
        ID3D11Texture2D* pDepthStencil = nullptr;
        D3D11_TEXTURE2D_DESC descDepth;
        descDepth.Width = backBufferSurfaceDesc.Width;
        descDepth.Height = backBufferSurfaceDesc.Height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = pDeviceSettings->AutoDepthStencilFormat;
        descDepth.SampleDesc.Count = pDeviceSettings->scd.SampleDesc.Count;
        descDepth.SampleDesc.Quality = pDeviceSettings->scd.SampleDesc.Quality;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        hr = pd3dDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
        if(FAILED(hr))
            return hr;
        GetDX11WState().SetD3D11DepthStencil(pDepthStencil);

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        descDSV.Format = descDepth.Format;
        descDSV.Flags = 0;
        if(descDepth.SampleDesc.Count > 1)
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        else
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = pd3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);
        if(FAILED(hr))
            return hr;
        GetDX11WState().SetD3D11DepthStencilView(pDSV);
    }

	hr = DX11WSetupD3D11Views(pd3dImmediateContext);
	if(FAILED(hr))
		return hr;

	return hr;
}

//
// DirectX11Wrapper::DX11WSetupD3D11Views
// Sets the viewport, render target view, and depth stencil view.
//
//TODO: COMPARE THIS TO DXUTCreateD3D11Views
HRESULT WINAPI DirectX11Wrapper::DX11WSetupD3D11Views(ID3D11DeviceContext* pd3dDeviceContext)
{
    HRESULT hr = S_OK;
	
	// Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
	viewport.Width = GetDXGIBackBufferSurfaceDesc()->Width;
	viewport.Height = GetDXGIBackBufferSurfaceDesc()->Height;
	viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    pd3dDeviceContext->RSSetViewports(1, &viewport);

	// set the render target as the back buffer			
    ID3D11RenderTargetView* pRTV = GetDX11WState().GetD3D11RenderTargetView();
    ID3D11DepthStencilView* pDSV = GetDX11WState().GetD3D11DepthStencilView();
    pd3dDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
    //pd3dDeviceContext->OMSetDepthStencilState( NULL, 0 ); // hm,mmmm??
    return hr;
}

//
// DirectX11Wrapper::SetWindow
//
void DirectX11Wrapper::SetWindow(HWND hWndFocus)
{
	//WCHAR* strCachedWindowTitle = GetDXUTState().GetWindowTitle();
    //GetWindowText( hWndFocus, strCachedWindowTitle, 255 );
    //strCachedWindowTitle[255] = 0;

    HINSTANCE hInstance = ( HINSTANCE )( LONG_PTR )GetWindowLongPtr( hWndFocus, GWLP_HINSTANCE );
    GetDX11WState().SetHInstance(hInstance);
    //GetDX11WState().SetWindowCreatedWithDefaultPositions( false );
    GetDX11WState().SetWindowCreated(true);
    GetDX11WState().SetHWNDFocus( hWndFocus );
    //GetDX11WState().SetHWNDDeviceFullScreen( hWndDeviceFullScreen );
    //GetDX11WState().SetHWNDDeviceWindowed( hWndDeviceWindowed );
}

//
// DirectX11Wrapper::SetCallbackFrameMove
//
void DirectX11Wrapper::SetCallbackFrameMove(LPCALLBACKFRAMEMOVE pCallback)
{
	GetDX11WState().SetCallbackFrameMove(pCallback);
}

//
// DirectX11Wrapper::SetCallbackMsgProc
//
void DirectX11Wrapper::SetCallbackMsgProc(LPCALLBACKMSGPROC pCallback)
{
	GetDX11WState().SetWindowsMsgProc(pCallback);
}

//
// DirectX11Wrapper::SetCallbackDeviceChanging
//
void DirectX11Wrapper::SetCallbackDeviceChanging(LPCALLBACKMODIFYDEVICESETTINGS pCallback)
{
	GetDX11WState().SetCallbackDeviceChanging(pCallback);
}

//
// DirectX11Wrapper::SetCallbackFrameMove
//
//void DirectX11Wrapper::SetCallbackD3D11DeviceAcceptable(LPDXUTCALLBACKISD3D11DEVICEACCEPTABLE pCallback)

//
// DirectX11Wrapper::SetCallbackFrameMove
//
void DirectX11Wrapper::SetCallbackD3D11DeviceCreated(LPCALLBACKD3D11DEVICECREATED pCallback)
{
	GetDX11WState().SetD3D11DeviceCreatedFunc(pCallback);
}

//
// DirectX11Wrapper::SetCallbackFrameMove
//
void DirectX11Wrapper::SetCallbackD3D11SwapChainResized(LPCALLBACKD3D11SWAPCHAINRESIZED pCallback)
{
	GetDX11WState().SetD3D11SwapChainResizedFunc(pCallback);
}

//
// DirectX11Wrapper::SetCallbackFrameMove
//
void DirectX11Wrapper::SetCallbackD3D11FrameRender(LPCALLBACKD3D11FRAMERENDER pCallback)
{
	GetDX11WState().SetD3D11FrameRenderFunc(pCallback);
}

//
// DirectX11Wrapper::SetCallbackFrameMove
//
void DirectX11Wrapper::SetCallbackD3D11SwapChainReleasing(LPCALLBACKD3D11SWAPCHAINRELEASING pCallback)
{
	GetDX11WState().SetD3D11SwapChainReleasingFunc(pCallback);
}

//
// DirectX11Wrapper::SetCallbackFrameMove
//
void DirectX11Wrapper::SetCallbackD3D11DeviceDestroyed(LPCALLBACKD3D11DEVICEDESTROYED pCallback)
{
	GetDX11WState().SetD3D11DeviceDestroyedFunc(pCallback);
}

//
// DirectX11Wrapper::GetCallbackMsgProc
//
LPCALLBACKMSGPROC DirectX11Wrapper::GetCallbackMsgProc()
{
	return GetDX11WState().GetWindowsMsgProc();
}

//
// DirectX11Wrapper::MainLoop
//
LRESULT DirectX11Wrapper::MainLoop()
{
    // DX11WInit() must have been called and succeeded for this function to proceed
    // DX11WCreateWindow() must have been called and succeeded for this function to proceed
    // DX11WCreateDevice() must have been called and succeeded for this function to proceed
	if(!GetDX11WState().GetWindowCreated()||!GetDX11WState().GetDX11WInited()||!GetDX11WState().GetDeviceCreated())
	{
		AC_FATAL("DXUTMainLoop Something not Inititalzed and the Loop was called");

		return E_FAIL;
	}

    // Now we're ready to receive and process Windows messages.
	// pull top msg to see if it is a WM_QUIT
    bool bGotMsg;
    MSG msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message )
    {
        // Use PeekMessage() so we can use idle time to render the scene. 
        bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

        if( bGotMsg )
        {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
        }
        else
        {
            // Render a frame during idle time (no messages are waiting)
			DX11WRender3DEnvironment();
        }
    }

	// this is where you set the state status if you ever code a state status
	//GetDXUTState().SetInsideMainloop( false );

    // return this part of the WM_QUIT message to Windows
    //return msg.wParam;
	return S_OK;
}

//
// DirectX11Wrapper::DXUTRender3DEnvironment
//
void WINAPI DirectX11Wrapper::DX11WRender3DEnvironment()
{
	HRESULT hr;

	auto pd3dDevice = GetD3D11Device();
	if(!pd3dDevice)
		return;

	auto pd3dImmediateContext = GetD3D11DeviceContext();
	if(!pd3dImmediateContext)
		return;
	/*
	if(DXUTIsRenderingPaused()||!DXUTIsActive()||GetDXUTState().GetRenderingOccluded())
	{
		// Window is minimized/paused/occluded/or not exclusive so yield CPU time to other processes
		Sleep( 50 );
	}
	*/
	// Get the app's time, in seconds. Skip rendering if no time elapsed
	double fTime, fAbsTime; 
	float fElapsedTime;
	GetGlobalAlphaTimer()->GetTimeValues(&fTime, &fAbsTime, &fElapsedTime);

	// move to timer code and put it in some defines
	//std::ostringstream strs;
	//strs << "T: ";
	//strs << fTime;
	//strs << "AT: ";
	//strs << fAbsTime;
	//strs << "ET: ";
	//strs << fElapsedTime;
	//std::string str = strs.str();

	//AC_INFO(str);
	
	// Store the time for the app
	//if( GetDXUTState().GetConstantFrameTime() )
	//{
		//fElapsedTime = GetDX11WState().GetTimePerFrame();
		//fTime = DXUTGetTime() + fElapsedTime;
	//}

	GetDX11WState().SetTime(fTime);
	GetDX11WState().SetAbsoluteTime(fAbsTime);
	GetDX11WState().SetElapsedTime(fElapsedTime);

	// Update the FPS stats
	//DXUTUpdateFrameStats();

	//DXUTHandleTimers();

	// Animate the scene by calling the app's frame move callback
	LPCALLBACKFRAMEMOVE pCallbackFrameMove = GetDX11WState().GetCallbackFrameMove();
	if(pCallbackFrameMove)
	{
		pCallbackFrameMove(fTime, fElapsedTime, GetDX11WState().GetD3D11FrameRenderFuncUserContext());

		pd3dDevice = GetD3D11Device();
		if(!pd3dDevice) // Handle DXUTShutdown from inside callback
			return;
	}

    // do 3D rendering on the back buffer here
	//if( !GetDX11WState().GetRenderingPaused() )
    //{
        // Render the scene by calling the app's render callback
        LPCALLBACKD3D11FRAMERENDER pCallbackFrameRender = GetDX11WState().GetD3D11FrameRenderFunc();
        if(pCallbackFrameRender)// && !GetDX11WState().GetRenderingOccluded() )
        {
            pCallbackFrameRender(pd3dDevice, pd3dImmediateContext, fTime, fElapsedTime,
                                  GetDX11WState().GetD3D11FrameRenderFuncUserContext());
            
            pd3dDevice = GetD3D11Device();
            if(!pd3dDevice) // Handle Shutdown from inside callback
                return;
        }
#if defined(DEBUG) || defined(_DEBUG)
        // The back buffer should always match the client rect 
        // if the Direct3D backbuffer covers the entire window
        RECT rcClient;
        GetClientRect(GetHWND(), &rcClient);
        if( !IsIconic(GetHWND()))
        {
            GetClientRect(GetHWND(), &rcClient);
            
            assert(GetDXGIBackBufferSurfaceDesc()->Width == (UINT)rcClient.right);
            assert(GetDXGIBackBufferSurfaceDesc()->Height == (UINT)rcClient.bottom);
        }
#endif
    //}
    return;
}

//
// DirectX11Wrapper::EndRender
//
void DirectX11Wrapper::EndRender()
{
	HRESULT hr;

	auto pSwapChain = GetDXGISwapChain();
	if(!pSwapChain)
		return;

	/*
	if ( GetDXUTState().GetSaveScreenShot() )
    {
		DXUTSnapD3D11Screenshot( GetDXUTState().GetScreenShotName(), false );
    }
    if ( GetDXUTState().GetExitAfterScreenShot() )
    {
        DXUTShutdown();
        return;
    }

    DWORD dwFlags = 0;
    if( GetDXUTState().GetRenderingOccluded() )
        dwFlags = DXGI_PRESENT_TEST;
    else
        dwFlags = GetDXUTState().GetCurrentDeviceSettings()->d3d11.PresentFlags;
    UINT SyncInterval = GetDXUTState().GetCurrentDeviceSettings()->d3d11.SyncInterval;
	*/
    // Show the frame on the primary surface.
    hr = pSwapChain->Present( 0, 0 );
    //hr = pSwapChain->Present( SyncInterval, dwFlags );
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        // There is a window covering our entire rendering area.
        // Don't render until we're visible again.
        //GetDXUTState().SetRenderingOccluded( true );
    }
    else if( DXGI_ERROR_DEVICE_RESET == hr )
    {
		AC_FATAL("EAGAG");
		/*
        // If a mode change happened, we must reset the device
        if( FAILED( hr = DXUTReset3DEnvironment11() ) )
        {
            if( DXUTERR_RESETTINGDEVICEOBJECTS == hr ||
                DXUTERR_MEDIANOTFOUND == hr )
            {
                DXUTDisplayErrorMessage( hr );
                DXUTShutdown();
                return;
            }
            else
            {
                // Reset failed, but the device wasn't lost so something bad happened, 
                // so recreate the device to try to recover
                DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
                if( FAILED( DXUTChangeDevice( pDeviceSettings, NULL, NULL, true, false ) ) )
                {
                    DXUTShutdown();
                    return;
                }

                // How to handle display orientation changes in full-screen mode?
            }
        }*/
    }
    else if( DXGI_ERROR_DEVICE_REMOVED == hr )
    {
        // Use a callback to ask the app if it would like to find a new device.  
        // If no device removed callback is set, then look for a new device
        //if( FAILED( DXUTHandleDeviceRemoved() ) )
       // {
            /// Perhaps get more information from pD3DDevice->GetDeviceRemovedReason()?
            ///DXUTDisplayErrorMessage( DXUTERR_DEVICEREMOVED );
            //DXUTShutdown();
            //return;
        //}
    }
    else if( SUCCEEDED( hr ) )
    {
        //if( //GetDXUTState().GetRenderingOccluded() )
        ////{
            // Now that we're no longer occluded
            //// allow us to render again
          /// GetDXUTState().SetRenderingOccluded( false );
        //}/
    }
	/*
	// Update current frame #
    int nFrame = GetDXUTState().GetCurrentFrameNumber();
    nFrame++;
    GetDXUTState().SetCurrentFrameNumber( nFrame );

    // Check to see if the app should shutdown due to cmdline
    if( GetDXUTState().GetOverrideQuitAfterFrame() != 0 )
    {
        if( nFrame > GetDXUTState().GetOverrideQuitAfterFrame() )
            DXUTShutdown();
    }
	*/
}

//
// DirectX11Wrapper::Shutdown
//
void DirectX11Wrapper::Shutdown()
{
	// call the app's Device created callback
	if(GetDX11WState().GetD3D11Device())
	{
		LPCALLBACKD3D11DEVICEDESTROYED pCallbackDeviceDestroyed = GetDX11WState().GetD3D11DeviceDestroyedFunc();
		if(pCallbackDeviceDestroyed)
			pCallbackDeviceDestroyed(NULL);//null for void pointer whihc is d3d9 will remove later
	}
	// shutting down in Fullscreen casues some threading issues
	if(GetDX11WState().GetDXGISwapChain())
	{
		GetDX11WState().GetDXGISwapChain()->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
	}
    // close and release all existing COM objects // make all safe in future
	if(GetDX11WState().GetD3D11RasterizerState())
	{
		GetDX11WState().GetD3D11RasterizerState()->Release();
		GetDX11WState().SetD3D11RasterizerState(nullptr);
	}
	if(GetDX11WState().GetD3D11DepthStencil())
	{
		GetDX11WState().GetD3D11DepthStencil()->Release();
		GetDX11WState().SetD3D11DepthStencil(nullptr);
	}
	if(GetDX11WState().GetD3D11DepthStencilView())
	{
		GetDX11WState().GetD3D11DepthStencilView()->Release();
		GetDX11WState().SetD3D11DepthStencilView(nullptr);
	}
	if(GetDX11WState().GetD3D11RenderTargetView())
	{
		GetDX11WState().GetD3D11RenderTargetView()->Release();
		GetDX11WState().SetD3D11RenderTargetView(nullptr);
	}
	if(GetDX11WState().GetDXGISwapChain())
	{
		GetDX11WState().GetDXGISwapChain()->Release();
		GetDX11WState().SetDXGISwapChain(nullptr);
	}
	if(GetDX11WState().GetDXGIFactory())
	{
		GetDX11WState().GetDXGIFactory()->Release();
		GetDX11WState().SetDXGIFactory(nullptr);
	}
	if(GetDX11WState().GetD3D11Device())
	{
#if defined(DEBUG) || defined(_DEBUG)
	GetDX11WState().GetD3D11Device()->QueryInterface(IID_PPV_ARGS(&pDebug));
#endif
		GetDX11WState().GetD3D11Device()->Release();
		GetDX11WState().SetD3D11Device(nullptr);
	}
	if(GetDX11WState().GetD3D11DeviceContext())
	{
		GetDX11WState().GetD3D11DeviceContext()->Release();
		GetDX11WState().SetD3D11DeviceContext(nullptr);
	}
}

//
// DirectX11Wrapper::DX11WGetHINSTANCE
//
HINSTANCE WINAPI DirectX11Wrapper::GetHINSTANCE()
{
	return GetDX11WState().GetHInstance(); 
}

//
// DirectX11Wrapper::DX11WGetHWND
//
HWND WINAPI DirectX11Wrapper::GetHWND()
{
	return GetDX11WState().GetHWNDFocus();
}

//
// DirectX11Wrapper::DX11WUpdateFrameStats
//
void DirectX11Wrapper::DX11WUpdateFrameStats()
{
	 //if( GetDX11WState().GetNoStats() )
  //      return;

    // Keep track of the frame count
    double fLastTime = GetDX11WState().GetLastStatsUpdateTime();
    DWORD dwFrames = GetDX11WState().GetLastStatsUpdateFrames();
    double fAbsTime = GetDX11WState().GetAbsoluteTime();
    dwFrames++;
    GetDX11WState().SetLastStatsUpdateFrames(dwFrames);

    // Update the scene stats once per second
    if(fAbsTime - fLastTime > 1.0f)
    {
        float fFPS = (float)(dwFrames / (fAbsTime - fLastTime));
        GetDX11WState().SetFPS(fFPS);
        GetDX11WState().SetLastStatsUpdateTime(fAbsTime);
        GetDX11WState().SetLastStatsUpdateFrames(0);

        auto pstrFPS = GetDX11WState().GetFPSStats();
        swprintf_s(pstrFPS, 64, L"%0.2f fps ", fFPS);
    }
}

//
// DirectX11Wrapper::GetFrameStats
//
LPCWSTR WINAPI DirectX11Wrapper::GetFrameStats(bool bIncludeFPS)
{
    //auto pstrFrameStats = GetDXUTState().GetFrameStats();
    WCHAR* pstrFPS = (bIncludeFPS) ? GetDX11WState().GetFPSStats() : L"";
    //swprintf_s(pstrFrameStats, 256, GetDXUTState().GetStaticFrameStats(), pstrFPS);
    return pstrFPS; //pstrFrameStats;
}

//
// DirectX11Wrapper::DX11WCreateState
//
HRESULT WINAPI DirectX11Wrapper::DX11WCreateState()
{
    if(g_pDX11WState == NULL)
    {
		if(DestroyingState == TRUE)
		{
			return E_FAIL;
		}
        g_pDX11WState = new DX11WState;
        if(NULL == g_pDX11WState)
            return E_OUTOFMEMORY;
    }
    return S_OK;
}

//
// DirectX11Wrapper::DX11WDestroyState
//
void WINAPI DirectX11Wrapper::DX11WDestroyState()
{
     delete g_pDX11WState;
	 g_pDX11WState = NULL;
}

//
// DirectX11Wrapper::GetDX11WState
//
DX11WState& DirectX11Wrapper::GetDX11WState()
{
    // This class will auto create the memory when its first accessed and delete it after the program exits WinMain.
    // However the application can also call DXUTCreateState() & DXUTDestroyState() independantly if its wants 
	DX11WCreateState();
    //assert( g_pDXUTState != NULL );
    return *g_pDX11WState;
}

//
// DirectX11Wrapper::DXW11LoadDXGI
//
HRESULT DirectX11Wrapper::DXW11LoadDXGI()
{
    IDXGIFactory* pDXGIFactory = GetDX11WState().GetDXGIFactory();
    if(pDXGIFactory == NULL)
    {
        HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (LPVOID*)&pDXGIFactory);
        GetDX11WState().SetDXGIFactory(pDXGIFactory);
		if(FAILED(hr))
		{
			AC_LOG("INFO","Failed to create DXGI Factory!");
			return E_FAIL;
		}
        if(pDXGIFactory == NULL)
        {
            // If still NULL, then DXGI is not availible
			return E_FAIL; // idk if that whats needed
            //GetDXUTState().SetD3D11Available( false );
            //return DXUTERR_NODIRECT3D11;
        }
        // DXGI 1.1 implies Direct3D 11
        //GetDXUTState().SetD3D11Available( true );
    }

    return S_OK;
}

// this is the main message handler for the program
LRESULT CALLBACK DX11WStaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// look in DXUT.CPP to see how Microsoft and all their glory would handle these messages....
	
	// Pass all messages to the app's MsgProc callback, and don't 
    // process further messages if the apps says not to.
	LPCALLBACKMSGPROC pCallbackMsgProc = g_pDX11W->GetCallbackMsgProc();
	if( pCallbackMsgProc )
	{
		bool bNoFurtherProcessing = false;
		LRESULT nResult = pCallbackMsgProc( hWnd, message, wParam, lParam, &bNoFurtherProcessing,
											NULL );
		if( bNoFurtherProcessing )
			return nResult;
	}


	// sort through and find what code to run for the message given
	switch(message)
	{
        case WM_POWERBROADCAST:
            switch( wParam )
            {
                case PBT_APMQUERYSUSPEND:
                    // At this point, the app should save any data for open
                    // network connections, files, etc., and prepare to go into
                    // a suspended mode.  The app can use the MsgProc callback
                    // to handle this if desired.
                    return true;

                case PBT_APMRESUMESUSPEND:
                    // At this point, the app should recover any data, network
                    // connections, files, etc., and resume running from when
                    // the app was suspended. The app can use the MsgProc callback
                    // to handle this if desired.

                    // QPC may lose consistency when suspending, so reset the timer
                    // upon resume.
                    GetGlobalAlphaTimer()->Reset();
                    //GetDXUTState().SetLastStatsUpdateTime( 0 );
                    return true;
            }
            break;

        case WM_CLOSE:
        {
            HMENU hMenu;
            hMenu = GetMenu( hWnd );
            if( hMenu )
                DestroyMenu( hMenu );
            DestroyWindow( hWnd );
            UnregisterClass( L"Direct3DWindowClass", nullptr );
            //GetDXUTState().SetHWNDFocus( nullptr );
            //GetDXUTState().SetHWNDDeviceFullScreen( nullptr );
            //GetDXUTState().SetHWNDDeviceWindowed( nullptr );
            return 0;
        }
		// this message is read when the window is closed
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		} 
		break;
	}
	
	// add more code to handle more messages later... or not you lazy man you

	/*// Stolen form DXUT try to implement into your own code if possible.
	    // Don't allow the F10 key to act as a shortcut to the menu bar
    // by not passing these messages to the DefWindowProc only when
    // there's no menu present
    if( !GetDXUTState().GetCallDefWindowProc() || GetDXUTState().GetMenu() == NULL &&
        ( uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) && wParam == VK_F10 )
        return 0;
    else
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
	*/
    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}