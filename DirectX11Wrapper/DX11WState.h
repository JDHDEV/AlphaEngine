#pragma once
//========================================================================
// DX11WState.h - Class that controls access to the State Struct
//
// Part of the Alpha Application
//
// http://www.directxtutorial.com/
//
// Justin Hunt
//========================================================================

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

//---------------------------------------------------------------------------------------------------------------------
// DirectX11WrapperState class
// Stores DXUT state TODO: and data access is done with thread safety (if g_bThreadSafe==true)
//--------------------------------------------------------------------------------------
class DX11WState
{
protected:
	struct STATE
    {
        // D3D11 specific
        IDXGIFactory*           m_DXGIFactory;            // DXGI Factory object
        IDXGIAdapter*           m_DXGIAdapter;            // The DXGI adapter object for the D3D11 device
        IDXGIOutput**           m_DXGIOutputArray;        // The array of output obj for the D3D11 adapter obj
        UINT                    m_DXGIOutputArraySize;    // Number of elements in m_D3D11OutputArray
        IDXGISwapChain*         m_DXGISwapChain;          // the D3D11 swapchain
        DXGI_SURFACE_DESC       m_BackBufferSurfaceDescDXGI; // D3D11 back buffer surface description
        bool                    m_RenderingOccluded;       // Rendering is occluded by another window
        bool                    m_DoNotStoreBufferSize;    // Do not store the buffer size on WM_SIZE messages

        // D3D11 specific
        bool                    m_D3D11Available;          // if true, then D3D11 is available 
        ID3D11Device*			m_D3D11Device;             // the D3D11 rendering device
        ID3D11DeviceContext*	m_D3D11DeviceContext;	   // the D3D11 immediate device context
        D3D_FEATURE_LEVEL		m_D3D11FeatureLevel;	   // the D3D11 feature level that this device supports
        ID3D11Texture2D*        m_D3D11DepthStencil;       // the D3D11 depth stencil texture (optional)
        ID3D11DepthStencilView* m_D3D11DepthStencilView;   // the D3D11 depth stencil view (optional)
        ID3D11RenderTargetView* m_D3D11RenderTargetView;   // the D3D11 render target view
        ID3D11RasterizerState*  m_D3D11RasterizerState;    // the D3D11 Rasterizer state

        // General
        HWND  m_HWNDFocus;                  // the main app focus window
        //HWND  m_HWNDDeviceFullScreen;       // the main app device window in fullscreen mode
        //HWND  m_HWNDDeviceWindowed;         // the main app device window in windowed mode
        //HMONITOR m_AdapterMonitor;          // the monitor of the adapter 
        //HMENU m_Menu;                       // handle to menu

        //UINT m_FullScreenBackBufferWidthAtModeChange;  // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
        //UINT m_FullScreenBackBufferHeightAtModeChange; // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
        //UINT m_WindowBackBufferWidthAtModeChange;  // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
        //UINT m_WindowBackBufferHeightAtModeChange; // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
        //DWORD m_WindowedStyleAtModeChange;  // window style
        //WINDOWPLACEMENT m_WindowedPlacement;// record of windowed HWND position/show state/etc
        //bool  m_TopmostWhileWindowed;       // if true, the windowed HWND is topmost 
        //bool  m_Minimized;                  // if true, the HWND is minimized
        //bool  m_Maximized;                  // if true, the HWND is maximized
        //bool  m_MinimizedWhileFullscreen;   // if true, the HWND is minimized due to a focus switch away when fullscreen mode
        //bool  m_IgnoreSizeChange;           // if true, DXUT won't reset the device upon HWND size change

        double m_Time;                      // current time in seconds
        double m_AbsoluteTime;              // absolute time in seconds
        float m_ElapsedTime;                // time elapsed since last frame

	    HINSTANCE m_HInstance;              // handle to the app instance
		double m_LastStatsUpdateTime;       // last time the stats were updated
        DWORD m_LastStatsUpdateFrames;      // frames count since last time the stats were updated
        float m_FPS;                        // frames per second
        int   m_CurrentFrameNumber;         // the current frame number

        bool  m_DX11WInited;                 // if true, then DXUTInit() has succeeded
        bool  m_WindowCreated;              // if true, then DXUTCreateWindow() or DXUTSetWindow() has succeeded
        bool  m_DeviceCreated;              // if true, then DXUTCreateDevice() or DXUTSetD3D*Device() has succeeded

		LPCALLBACKMODIFYDEVICESETTINGS      m_ModifyDeviceSettingsFunc;     // modify Direct3D device settings callback
        LPCALLBACKFRAMEMOVE                 m_FrameMoveFunc;                // frame move callback
        LPCALLBACKMSGPROC                   m_WindowsMsgProc;                // window messages callbac

        //LPCALLBACKISD3D11DEVICEACCEPTABLE   m_IsD3D11DeviceAcceptableFunc;  // D3D11 is device acceptable callback
        LPCALLBACKD3D11DEVICECREATED        m_D3D11DeviceCreatedFunc;       // D3D11 device created callback
        LPCALLBACKD3D11SWAPCHAINRESIZED     m_D3D11SwapChainResizedFunc;    // D3D11 SwapChain reset callback
        LPCALLBACKD3D11SWAPCHAINRELEASING   m_D3D11SwapChainReleasingFunc;  // D3D11 SwapChain lost callback
        LPCALLBACKD3D11DEVICEDESTROYED      m_D3D11DeviceDestroyedFunc;     // D3D11 device destroyed callback
        LPCALLBACKD3D11FRAMERENDER          m_D3D11FrameRenderFunc;         // D3D11 frame render callback

		void* m_ModifyDeviceSettingsFuncUserContext;     // user context for modify Direct3D device settings callback
        void* m_DeviceRemovedFuncUserContext;            // user context for Direct3D device removed callback
        void* m_FrameMoveFuncUserContext;                // user context for frame move callback
        void* m_KeyboardFuncUserContext;                 // user context for keyboard callback
        void* m_MouseFuncUserContext;                    // user context for mouse callback
        void* m_WindowMsgFuncUserContext;                // user context for window messages callback

        void* m_IsD3D11DeviceAcceptableFuncUserContext;  // user context for is D3D11 device acceptable callback
        void* m_D3D11DeviceCreatedFuncUserContext;       // user context for D3D11 device created callback
        void* m_D3D11SwapChainResizedFuncUserContext;    // user context for D3D11 SwapChain resized callback
        void* m_D3D11SwapChainReleasingFuncUserContext;  // user context for D3D11 SwapChain releasing callback
        void* m_D3D11DeviceDestroyedFuncUserContext;     // user context for D3D11 device destroyed callback
        void* m_D3D11FrameRenderFuncUserContext;         // user context for D3D11 frame render callback

        bool m_Keys[256];                                // array of key state
        bool m_LastKeys[256];                            // array of last key state
        bool m_MouseButtons[5];                          // array of mouse states

        //std::vector<DXUT_TIMER>*  m_TimerList;           // list of DXUT_TIMER structs
        WCHAR m_StaticFrameStats[256];                   // static part of frames stats 
        WCHAR m_FPSStats[64];                            // fps stats
        WCHAR m_FrameStats[256];                         // frame stats (fps, width, etc)
        WCHAR m_DeviceStats[256];                        // device stats (description, device type, etc)
        WCHAR m_WindowTitle[256];                        // window title
    };

	STATE m_state;

public:
    DX11WState()  { Create(); }
    ~DX11WState() { Destroy(); }

	void Create();
	void Destroy();

	// Setters and Getters
	// D3D11 specific
	void SetDXGIFactory(IDXGIFactory* factory);
	IDXGIFactory* GetDXGIFactory();
	void SetDXGISwapChain(IDXGISwapChain* swapchain);
	IDXGISwapChain* GetDXGISwapChain();
	void SetBackBufferSurfaceDescDXGI(DXGI_SURFACE_DESC* surfacedesc);
	DXGI_SURFACE_DESC* GetBackBufferSurfaceDescDXGI();
	void SetD3D11Device(ID3D11Device* D3D11Device);
	ID3D11Device* GetD3D11Device();
	void SetD3D11DeviceContext(ID3D11DeviceContext*	D3D11DeviceContext);
	ID3D11DeviceContext* GetD3D11DeviceContext();
	void SetD3D11DepthStencil(ID3D11Texture2D*	D3D11DepthStencil);
	ID3D11Texture2D* GetD3D11DepthStencil();
	void SetD3D11DepthStencilView(ID3D11DepthStencilView* D3D11DepthStencilView);
	ID3D11DepthStencilView* GetD3D11DepthStencilView();
	void SetD3D11RenderTargetView(ID3D11RenderTargetView* D3D11RenderTargetView);
	ID3D11RenderTargetView* GetD3D11RenderTargetView();
	void SetD3D11RasterizerState(ID3D11RasterizerState* D3D11RasterizerState);
	ID3D11RasterizerState* GetD3D11RasterizerState();

	// General
	void SetHWNDFocus(HWND hwnd);
	HWND GetHWNDFocus();
	void SetTime(double Time);
	double GetTime();
	void SetAbsoluteTime(double AbsoluteTim);
	double GetAbsoluteTime();
	void SetElapsedTime(float ElapsedTime);
	float GetElapsedTime();
	void SetHInstance(HINSTANCE HInstance);
	HINSTANCE GetHInstance();
	void SetLastStatsUpdateTime(double LastStatsUpdateTime);
	double GetLastStatsUpdateTime();
	void SetLastStatsUpdateFrames(DWORD LastStatsUpdateFrames);	
    DWORD GetLastStatsUpdateFrames();
	void SetFPS(float FPS);
    float GetFPS();
	void SetCurrentFrameNumber(int CurrentFrameNumber);
    int GetCurrentFrameNumber();
	void SetDX11WInited(bool DX11WInited);
  	bool GetDX11WInited();             
    void SetWindowCreated(bool WindowCreated); 
    bool GetWindowCreated();               
    void SetDeviceCreated(bool DeviceCreated);   
    bool GetDeviceCreated(); 
	void SetCallbackDeviceChanging(LPCALLBACKMODIFYDEVICESETTINGS ModifyDeviceSettingsFunc);
	LPCALLBACKMODIFYDEVICESETTINGS GetCallbackDeviceChanging();
	void SetCallbackFrameMove(LPCALLBACKFRAMEMOVE FrameMoveFunc);
	LPCALLBACKFRAMEMOVE GetCallbackFrameMove();
	void SetWindowsMsgProc(LPCALLBACKMSGPROC WindowsMsgProc);
	LPCALLBACKMSGPROC GetWindowsMsgProc();
	//void SetIsD3D11DeviceAcceptableFunc(LPCALLBACKISD3D11DEVICEACCEPTABLE IsD3D11DeviceAcceptableFunc);
	//LPCALLBACKISD3D11DEVICEACCEPTABLE GetIsD3D11DeviceAcceptableFunc()
	void SetD3D11DeviceCreatedFunc(LPCALLBACKD3D11DEVICECREATED D3D11DeviceCreatedFunc);
	LPCALLBACKD3D11DEVICECREATED GetD3D11DeviceCreatedFunc();
	void SetD3D11SwapChainResizedFunc(LPCALLBACKD3D11SWAPCHAINRESIZED D3D11SwapChainResizedFunc);
	LPCALLBACKD3D11SWAPCHAINRESIZED GetD3D11SwapChainResizedFunc();
	void SetD3D11SwapChainReleasingFunc(LPCALLBACKD3D11SWAPCHAINRELEASING D3D11SwapChainReleasingFunc);
	LPCALLBACKD3D11SWAPCHAINRELEASING GetD3D11SwapChainReleasingFunc();
	void SetD3D11DeviceDestroyedFunc(LPCALLBACKD3D11DEVICEDESTROYED D3D11DeviceDestroyedFunc);
	LPCALLBACKD3D11DEVICEDESTROYED GetD3D11DeviceDestroyedFunc();
	void SetD3D11FrameRenderFunc(LPCALLBACKD3D11FRAMERENDER D3D11FrameRenderFunc);
	LPCALLBACKD3D11FRAMERENDER GetD3D11FrameRenderFunc();
	void SetD3D11FrameRenderFuncUserContext(void* D3D11FrameRenderFuncUserContext);
	void* GetD3D11FrameRenderFuncUserContext(); 

	//returns pointers
	WCHAR* GetFPSStats() { return m_state.m_FPSStats; } 

};
