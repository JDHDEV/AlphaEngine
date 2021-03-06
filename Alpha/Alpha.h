#pragma once
//========================================================================
// Alpha.h : Defines the entry point for the application.
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

#include "../Mainloop/Initialization.h"
#include "../Alpha/BaseGameLogic.h"
#include "../Graphics3D/SceneNodes.h"
#include "../Graphics3D/OctTree.h"
#include "../UserInterface/UserInterface.h"

//class FontHandler;
//class BaseUI;
class EventManager;
//class LuaStateManager;
//class BaseSocketManager;
//class NetworkEventForwarder;
class HumanView;

//
// class AlphaApp                                                    - Chapter X, page Y
//
class AlphaApp
{
protected:
	//DirectX11Wrapper m_DirectX11Wrapper;					// Create the DirectXW11Wrapper
    HINSTANCE m_hInstance;                                  // the module instance
    bool m_bWindowedMode;                                   // true if the app is windowed, false if fullscreen
    bool m_bIsRunning;                                      // true if everything is initialized and the game is in the main loop
    bool m_bQuitRequested;                                  // true if the app should run the exit sequence
    bool m_bQuitting;                                       // true if the app is running the exit sequence
    Rect m_rcDesktop;                                       // current desktop size - not necessarilly the client window size
    Point m_screenSize;                                     // game screen size
    int m_iColorDepth;                                      // current color depth (16 or 32)
    bool m_bIsEditorRunning;                                // true if the game editor is running

public:
    const Point &GetScreenSize()  { return m_screenSize; }

protected:
    std::map<std::wstring,std::wstring> m_textResource;
    std::map<std::wstring,UINT> m_hotkeys;

    int m_HasModalDialog;                                   // determines if a modal dialog is up
    int PumpUntilMessage (UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam);
    int EatSpecificMessages( UINT msgType, optional<LPARAM> lParam, optional<WPARAM> wParam);
    void FlashWhileMinimized();

public:

	AlphaApp();

	// Game Application Data
	// You must define these in an inherited
	// class - see TeapotWarsApp for an example
	virtual TCHAR *VGetGameTitle()=0;
	virtual TCHAR *VGetGameAppDirectory()=0;
	virtual HICON VGetIcon()=0;

	// Win32 Specific Stuff
	HWND GetHwnd();
	HINSTANCE GetInstance() { return m_hInstance; }
	virtual bool InitInstance(HINSTANCE hInstance, LPWSTR nCmdShow, HWND hWnd = NULL, int screenWidth = SCREEN_WIDTH, int screenHeight = SCREEN_HEIGHT, bool windowed = false);   

	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void *pUserContext );
	bool HasModalDialog() { return m_HasModalDialog!=0; }
	//void ForceModalExit() { PostMessage(GetHwnd(), g_MsgEndModal, 0, g_QuitNoPrompt);       }

	LRESULT OnDisplayChange(int colorDepth, int width, int height);
	LRESULT OnPowerBroadcast(int event);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnClose();

	// Game Application actions
	LRESULT OnAltEnter();
	LRESULT OnNcCreate(LPCREATESTRUCT cs);

	bool LoadStrings(std::string language);
	std::wstring GetString(std::wstring sID);                               
	int GetHotKeyForString(std::wstring sID);
	UINT MapCharToKeycode(const char pHotkey);

	//int Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);

	// FUTURE WORK - Seems like this class could be refactored, especailly the renderer !

	shared_ptr<IRenderer> m_Renderer;

	// DirectX 11 Specific Stuff
	//static bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
	static HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
	static HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
	static void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
	static void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
	static void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );

	static bool CALLBACK ModifyDeviceSettings(DX11WDeviceSettings* pDeviceSettings, void* pUserContext);
	static void CALLBACK OnUpdateGame(double fTime, float fElapsedTime, void *pUserContext);

	// GameCode Specific Stuff
	BaseGameLogic *m_pGame;
	struct GameOptions m_Options;

	// You must define these functions to initialize your game.
	// Look for them in TeapotWars.h and TeapotWars.cpp
	virtual BaseGameLogic *VCreateGameAndView()=0;
	virtual bool VLoadGame(void);
	HumanView* GetHumanView();                              // Added post press - it was convenient to grab the HumanView attached to the game.

	// File and Resource System
	class ResCache *m_ResCache;
	TCHAR m_saveGameDirectory[MAX_PATH];

	//bool IsEditorRunning() { return m_bIsEditorRunning; }

	// Event manager
	EventManager *m_pEventManager;

	//// Socket manager - could be server or client
	//BaseSocketManager *m_pBaseSocketManager;
	//NetworkEventForwarder* m_pNetworkEventForwarder;
	//bool AttachAsClient();

protected:
    //virtual void VCreateNetworkEventForwarder(void);
    //virtual void VDestroyNetworkEventForwarder(void);

public:

	// Main loop processing
	void AbortGame() { m_bQuitting = true; }
	// int GetExitCode() { return DXUTGetExitCode(); } TODO
	bool IsRunning() { return m_bIsRunning; }
	void SetQuitting(bool quitting) { m_bQuitting = quitting; }

	BaseGameLogic* GetGameLogic(void) const { return m_pGame; }

protected:
    virtual void VRegisterGameEvents(void) {}

private:
    void RegisterEngineEvents(void);
};

extern AlphaApp *g_pApp;
