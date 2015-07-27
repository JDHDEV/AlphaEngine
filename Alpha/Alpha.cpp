//========================================================================
// Alpha.cpp : Defines the Application layer for Alpha
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

//#include "Dbt.h"                                                // required for DBT_DEVICEREMOVECOMPLETE

#include "../MainLoop/Initialization.h"
//#include "../Debugging/MiniDump.h"
#include "../Alpha/BaseGameLogic.h"
#include "../Graphics3D/D3DRenderer.h"
#include "../EventManager/EventManagerImpl.h"
//#include "../Network/Network.h"
#include "../LUAScripting/LuaStateManager.h"
#include "../LUAScripting/ScriptExports.h"
#include "../LUAScripting/ScriptProcess.h"
#include "../ResourceCache/ResCache.h"
#include "../ResourceCache/XmlResource.h"
#include "../UserInterface/UserInterface.h"
//#include "../UserInterface/MessageBox.h"
#include "../UserInterface/HumanView.h"
//#include "../Utilities/Math.h"
#include "../Utilities/String.h"
#include "../Actors/BaseScriptComponent.h"

// All event type headers
//#include "../Physics/PhysicsEventListener.h"
#include "../EventManager/Events.h"

#define MAX_LOADSTRING 100

AlphaApp *g_pApp = NULL;

const char* SCRIPT_PREINIT_FILE = "Scripts\\PreInit.lua";

//========================================================================
//
// AlphaApp implementation
//
//========================================================================
AlphaApp::AlphaApp()
{
    g_pApp = this;
    m_pGame = NULL;
	AC_NEW DirectX11Wrapper();

    m_rcDesktop.bottom = m_rcDesktop.left = m_rcDesktop.right = m_rcDesktop.top = 0;
    m_screenSize = Point(0,0);
    m_iColorDepth = 32;

    m_bIsRunning = false;
    m_bIsEditorRunning = false;

    m_pEventManager = NULL;
    m_ResCache = NULL;

    //m_pNetworkEventForwarder = NULL; 
    //m_pBaseSocketManager = NULL;

    m_bQuitRequested = false;
    m_bQuitting = false;
    m_HasModalDialog = 0;
}

HWND AlphaApp::GetHwnd()
{ 
    return g_pDX11W->GetHWND();
}

//===================================================================
// Win32 Specific Stuff
//
// InitInstance - this checks system resources, creates your window, and launches the game
//
// preprocessor macro setting in the C++ options of the project provides the base macro
// C preprocessor string concatenation takes care of the rest.
//
// AlphaApp::InitInstance - Chapter 5, page 136
//
//===================================================================
bool AlphaApp::InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight, bool windowed)
{	
	AC_LOG("Script","Init");
	// Check for existing instance of the same window - Chapter 5, page 137
    // 
#ifndef _DEBUG
    // Note - it can be really useful to debug network code to have
    // more than one instance of the game up at one time - so
    // feel free to comment these lines in or out as you wish!
    if (!IsOnlyInstance(VGetGameTitle()))
		return false;

#endif

	// We don't need a mouse cursor by default, let the game turn it on
	SetCursor(NULL);

	// Check for adequate machine resources.
	// Check secondary storage space and memory. - Chapter 5, page 138-139
	// Calculate the CPU speed. - Chapter 5, page 140
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		const DWORDLONG physicalRAM = 512 * MEGABYTE;
		const DWORDLONG virtualRAM = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;
		if (!CheckStorage(diskSpace))
			return false;

		const DWORD minCpuSpeed = 1300;     // 1.3Ghz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			AC_ERROR("GetCPUSpeed reports CPU is too slow for this game.");
			return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

    // register all events
    RegisterEngineEvents();
    VRegisterGameEvents();

	//
	// Initialize the ResCache - Chapter 5, page 141
	//
	//    Note - this is a little different from the book. Here we have a speccial resource ZIP file class, DevelopmentResourceZipFile,
	//    that actually reads directly from the source asset files, rather than the ZIP file. This is MUCH better during development, since
	//    you don't want to have to rebuild the ZIP file every time you make a minor change to an asset.
	//
	IResourceFile *zipFile = (m_bIsEditorRunning || m_Options.m_useDevelopmentDirectories) ? 
		AC_NEW DevelopmentResourceZipFile(L"Assets.zip", DevelopmentResourceZipFile::Editor) :
		AC_NEW ResourceZipFile(L"Assets.zip");

	m_ResCache = AC_NEW ResCache(50, zipFile);

	if (!m_ResCache->Init())
	{
        AC_ERROR("Failed to initialize resource cache!  Are your paths set up correctly?");
		return false;
	}
	
	//extern shared_ptr<IResourceLoader> CreateWAVResourceLoader();
	//extern shared_ptr<IResourceLoader> CreateOGGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateDDSResourceLoader();
	extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
    extern shared_ptr<IResourceLoader> CreateXmlResourceLoader();
    //extern shared_ptr<IResourceLoader> CreateSdkMeshResourceLoader();
    extern shared_ptr<IResourceLoader> CreateScriptResourceLoader();

	// Note - register these in order from least specific to most specific! They get pushed onto a list.
	// RegisterLoader is discussed in Chapter 5, page 142
	//m_ResCache->RegisterLoader(CreateWAVResourceLoader());
	//m_ResCache->RegisterLoader(CreateOGGResourceLoader());
	m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	m_ResCache->RegisterLoader(CreateJPGResourceLoader());
    m_ResCache->RegisterLoader(CreateXmlResourceLoader());
	//m_ResCache->RegisterLoader(CreateSdkMeshResourceLoader());
    m_ResCache->RegisterLoader(CreateScriptResourceLoader());

	//// Load strings that will be presented to the player. - Chapter 5, page 142
	//if(!LoadStrings("English"))
	//{
 //       AC_ERROR("Failed to load strings");
	//	return false;
	//}

	// Create the LUA script manager. - Chapter 5, page 144
	if (!LuaStateManager::Create())
	{
		AC_ERROR("Failed to initialize Lua");
		return false;
	}

	// Load the preinit file.  This is within braces to create a scope and destroy the resource once it's loaded.  We
    // don't need to do anything with it, we just need to load it.
    {
        Resource resource(SCRIPT_PREINIT_FILE);
        shared_ptr<ResHandle> pResourceHandle = m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
    }

    // Register function exported from C++
    ScriptExports::Register();
    ScriptProcess::RegisterScriptClass();
    BaseScriptComponent::RegisterScriptFunctions();

	// Create the game's event manager. - Chapter 5, page 145
	m_pEventManager = AC_NEW EventManager("AlphaApp Event Mgr", true);
	if (!m_pEventManager)
	{
		AC_ERROR("Failed to create EventManager.");
		return false;
	}

	// Inititalize DirectX, the application's window, and the D3D device. - Chapter 5, page 145
	//DXUTInit(true, true, lpCmdLine, true);
	//Init DirectX11Wrapper class
	g_pDX11W->DX11WInit(lpCmdLine);// may need to be put in the alpha app init instance function

	if (hWnd==NULL)
	{
		//DXUTCreateWindow(VGetGameTitle(), hInstance, VGetIcon());
		g_pDX11W->DX11WCreateWindow(VGetGameTitle(), hInstance, VGetIcon());
	}
	else
	{
	    //DXUTSetWindow( hWnd, hWnd, hWnd );
	}

	if(!GetHwnd())
	{
		return false;
	}
	SetWindowText(GetHwnd(), VGetGameTitle());
	
	// initialize the directory location you can store save game files
	//_tcscpy_s(m_saveGameDirectory, GetSaveGameDirectory(GetHwnd(), VGetGameAppDirectory()));

    // DXUTCreateDevice - Chapter 5 - page 139
    m_screenSize = Point(screenWidth, screenHeight);

	//DXUTCreateDevice( D3D_FEATURE_LEVEL_10_1, true, screenWidth, screenHeight);
	g_pDX11W->DX11WCreateDevice(windowed, screenWidth, screenHeight);

	m_Renderer = shared_ptr<IRenderer>(AC_NEW D3DRenderer11());
    m_Renderer->VSetBackgroundColor(0, 50, 100, 255);
    m_Renderer->VOnRestore();

	// You usually must have an HWND to initialize your game views...
	//    VCreateGameAndView			- Chapter 5, page 145
	m_pGame = VCreateGameAndView();
	if (!m_pGame)
		return false;

	// now that all the major systems are initialized, preload resources 
	//    Preload calls are discussed in Chapter 5, page 148
	//m_ResCache->Preload("*.ogg", NULL);
	m_ResCache->Preload("*.dds", NULL);
	//m_ResCache->Preload("*.jpg", NULL);

	//if ( GameCodeApp::GetRendererImpl() == GameCodeApp::Renderer_D3D11 )
	//	m_ResCache->Preload("*.sdkmesh", NULL);

	m_bIsRunning = TRUE;

	return TRUE;
}

//----------------------------------------------------------
// Win32 Specific Message Handlers
//
// WndProc - the main message handler for the window class
//
// OnNcCreate - this is where you can set window data before it is created
// OnMove - called whenever the window moves; used to update members of g_App
// OnDeviceChange - called whenever you eject the CD-ROM.
// OnDisplayChange - called whenever the user changes the desktop settings
// OnPowerBroadcast - called whenever a power message forces a shutdown
// OnActivate - called whenever windows on the desktop change focus.
//
// Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
LRESULT CALLBACK AlphaApp::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{

    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = D3DRenderer::g_GuiResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
    if(*pbNoFurtherProcessing)
        return 0;

    LRESULT result = 0;

    switch (uMsg) 
    {
		case WM_POWERBROADCAST:
		{
			int event = (int)wParam;
			result = g_pApp->OnPowerBroadcast(event);
			break;
		}

		//case WM_EXITSIZEMOVE:
		//{
		//	int colorDepth = (int)wParam;
		//	break;
		//}

		//case :
		//{
		//	MoveWindow(g_pDX11W->DX11WGetHWND(),,,,,true);
		//}

		case WM_DISPLAYCHANGE://:WM_ENTERSIZEMOVE
		{
			int colorDepth = (int)wParam;
			int width = (int)(short) LOWORD(lParam);
			int height = (int)(short) HIWORD(lParam);

			//result = g_pApp->OnDisplayChange(colorDepth, width, height);
			break;
		}

		case WM_SYSCOMMAND: 
		{
			result = g_pApp->OnSysCommand(wParam, lParam);
			if (result)
			{
				*pbNoFurtherProcessing = true;
			}
			break;
		}

		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_RETURN)
			{
				*pbNoFurtherProcessing = true;
				return g_pApp->OnAltEnter();
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}


		case WM_CLOSE:
		{
			// DXUT apps choose ESC key as a default exit command.
			// GameCode4 doesn't like this so we disable it by checking 
			// the m_bQuitting bool, and if we're not really quitting
			// set the "no further processing" parameter to true.
			if (g_pApp->m_bQuitting)
			{
				result = g_pApp->OnClose();
			}
			else
			{
				*pbNoFurtherProcessing = true;
			}
			break;
		}


		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case MM_JOY1BUTTONDOWN:
		case MM_JOY1BUTTONUP:
		case MM_JOY1MOVE:
		case MM_JOY1ZMOVE:
		case MM_JOY2BUTTONDOWN:
		case MM_JOY2BUTTONUP:
		case MM_JOY2MOVE:
		case MM_JOY2ZMOVE:
		{
			//
			// See Chapter 10, page 278 for more explanation of this code.
			//
			if (g_pApp->m_pGame)
			{
				BaseGameLogic *pGame = g_pApp->m_pGame;
				// Note the reverse order! User input is grabbed first from the view that is on top, 
				// which is the last one in the list.
				AppMsg msg;
				msg.m_hWnd = hWnd;
				msg.m_uMsg = uMsg;
				msg.m_wParam = wParam;
				msg.m_lParam = lParam;
				for(GameViewList::reverse_iterator i=pGame->m_gameViews.rbegin(); i!=pGame->m_gameViews.rend(); ++i)
				{
					if ( (*i)->VOnMsgProc( msg ) )
					{
						result = true;
						break;                          // WARNING! This breaks out of the for loop.
					}
				}
			}
		break;
		}

        /**********************
        WARNING!!!!! You MIGHT think you need this, but if you use the DirectX
        Framework the DefWindowProc is called for you....

        default:
                return DefWindowProc(hWnd, message, wParam, lParam);

        ***********************/
	}
	return true; //result;
}

//===================================================================
// LoadStrings - Reads strings from an XML file
//
// GameCodeApp::LoadStrings - Chapter 5, page 143
//
//===================================================================
/*
bool AlphaApp::LoadStrings(std::string language)
{
	std::string languageFile = "Strings\\";
	languageFile += language;
	languageFile += ".xml";

	TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(languageFile.c_str());
	if(!pRoot)
	{
		AC_ERROR("Strings are missing.");
		return false;
	}
	// Loop through each child element and load the component
	for(TiXmlElement* pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
	{
		const char *pKey=pElem->Attribute("id");
		const char *pText=pElem->Attribute("value");
		if(pKey && pText)
		{
			wchar_t wideKey[64];
			wchar_t wideText[1024];
			AnsiToWideCch(wideKey, pKey, 64);
			AnsiToWideCch(wideText, pText, 1024);
			m_textResource[std::wstring(wideKey)] = std::wstring(wideText);
		}
	}
	return true;
}

std::wstring AlphaApp::GetString(std::wstring sID)
{
	auto localizedSting = m_textResource.find(sID);
	if(localizedString == m_textResource.end())
	{
		AC_ASSERT(0 && "String not found!");
		return L"";
	}
	return localizedString->second;
}
*/

//=========================================================
// AlphaApp::OnDisplayChange - Chapter X, page Y
//
// Handles the WM_DISPLAYCHANGE message
//
//=========================================================
LRESULT AlphaApp::OnDisplayChange(int colorDepth, int width, int height)
{
	m_rcDesktop.left = 0;
	m_rcDesktop.top = 0; 
	m_rcDesktop.right = width;
	m_rcDesktop.bottom = height;
	m_iColorDepth = colorDepth;
	return 0;
}

//=========================================================
// AlphaApp::OnPowerBroadcast - Chapter X, page Y
//
// Handles the WM_POWERBROADCAST message
//
//=========================================================
LRESULT AlphaApp::OnPowerBroadcast(int event)
{
	// Don't allow the game to go into sleep mode
	if (event == PBT_APMQUERYSUSPEND)
		return BROADCAST_QUERY_DENY;
	else if (event == PBT_APMBATTERYLOW)
	{
		AbortGame();
	}

	return true;
}
//=========================================================
// AlphaApp::OnSysCommand - Chapter X, page Y
//
// Handles the WM_SYSCOMMAND message
//
//=========================================================
LRESULT AlphaApp::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case SC_MAXIMIZE :
		{
			// If windowed and ready...
			if ( m_bWindowedMode && IsRunning() )
			{
				// Make maximize into FULLSCREEN toggle
				OnAltEnter();
			}
		}
		return 0;

		case SC_CLOSE :
		{
			// The quit dialog confirmation would appear once for
			// every SC_CLOSE we get - which happens multiple times
			// if modal dialogs are up.  This now uses the g_QuitNoPrompt
			// flag to only prompt when receiving a SC_CLOSE that isn't
			// generated by us (identified by g_QuitNoPrompt).
			
			// If closing, prompt to close if this isn't a forced quit
			if ( lParam != g_QuitNoPrompt )
			{
				// ET - 05/21/01 - Bug #1916 - Begin
				// We were receiving multiple close dialogs
				// when closing again ALT-F4 while the close
				// confirmation dialog was up.
				// Eat if already servicing a close
				if ( m_bQuitRequested )
					return true;

				// Wait for the application to be restored
				// before going any further with the new 
				// screen.  Flash until the person selects
				// that they want to restore the game, then
				// reinit the display if fullscreen.  
				// The reinit is necessary otherwise the game
				// will switch to windowed mode.
	
				// Quit requested
				m_bQuitRequested = true;
				// Prompt TODO Fix pop up message
				//if ( MessageBox::Ask(QUESTION_QUIT_GAME) == IDNO )
				//{
				//	// Bail - quit aborted
				//	
				//	// Reset quit requested flag
				//	m_bQuitRequested = false;
				//	
				//	return true;
				//}
			}
	
			m_bQuitting = true;

			// Is there a game modal dialog up?
			if ( HasModalDialog() )
			{
				// Close the modal
				// and keep posting close to the app
				//ForceModalExit(); TODO uncomment when the func is coded

				// Reissue the close to the app
				
				// Issue the new close after handling the current one,
				// but send in g_QuitNoPrompt to differentiate it from a 
				// regular CLOSE issued by the system.
				PostMessage( GetHwnd(), WM_SYSCOMMAND, SC_CLOSE, g_QuitNoPrompt );

				m_bQuitRequested = false;
				
				// Eat the close
				return true;
			}

			// Reset the quit after any other dialogs have popped up from this close
			m_bQuitRequested = false;
		}
		return 0;

		default:
			// return non-zero of we didn't process the SYSCOMMAND message
			return DefWindowProc(GetHwnd(), WM_SYSCOMMAND, wParam, lParam);
	}

	return 0;
}

//===================================================================
// OnClose - Handles the WM_CLOSE message
//
// AlphaApp::OnClose - Chapter 5, page 152
//
//===================================================================
LRESULT AlphaApp::OnClose()
{
	// Release all the game systems in reverse order from whcih they
	// were created
	// TODO make sure these are all called if they have been implemented

	SAFE_DELETE(m_pGame);
	DestroyWindow(GetHwnd());
	//VDestroyNetworkEventForwarder();
	//SAFE_DELETE(m_pBaseSocketManager);
	SAFE_DELETE(m_pEventManager);
	ScriptExports::Unregister();
	LuaStateManager::Destroy();
	SAFE_DELETE(m_ResCache);
	return 0;
}

//=========================================================
// GameCodeApp::OnAltEnter
//
// Called when the player hits Alt-Enter to flip the 
// display mode.
//
// Not discussed in the book.
//=========================================================
LRESULT AlphaApp::OnAltEnter()
{
	//DXUTToggleFullScreen(); seems to work with out this here?????
	return 0;
}

//===================================================================
// FlashWhileMinimized - Flash the application on the task bar until 
// restored.
//
// AlphaApp::FlashWhileMinimized - Chapter 5, page 149
//
//===================================================================
/*
void AlpahApp::FlashWhileMinimized()
{
	// Flash the application on the task bar until 
	// restored.
	if(!GetHwnd())
		return;

	// Blink the application if we are minimized.
	// waiting until we are no longer minimized
	if(IsIconic(GetHwnd()))
	{
		// Make sure the app is up when creating a new screen
		// this should be the case most of the time, but when
		// we close the app down, minimized, and a confirmation
		// dialog appears, we need to restore
		DWORD now = timeGetTime();
		DWORD then = now;
		MSG msg;

		FlashWindow(GetHwnd(), true);

		while(true)
		{
			if(PeekMessage(&msg, NULL, 0, 0, 0))
			{
				if(msg.message != WM_SYSCOMMAND || msg.wParam != SC_CLOSE)
				{
					TranslateMessage(&msg);
					DisapatchMessge(&msg);
				}

				// Are we done?
				if(!IsIconic(GetHwnd()))
				{
					FlashWindow(GetHwnd(), false);
					break;
				}
			}
			else
			{
				now = timeGetTime();
				DWORD timeSpan = now > then ? (now-then) : (then-now);
				if(timeSpan > 1000)
				{
					then = now;
					FlashWindow(GetHwnd(), true);
				}
			}
		}
	}
}*/

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK AlphaApp::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr = S_OK;

	ID3D11DeviceContext* pd3dImmediateContext = g_pDX11W->GetD3D11DeviceContext();
    V_RETURN(D3DRenderer::g_GuiResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));

	return hr;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK AlphaApp::OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr = S_OK;

	V_RETURN(D3DRenderer::g_GuiResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	if (g_pApp->m_pGame)
	{
		BaseGameLogic *pGame = g_pApp->m_pGame;
		for(GameViewList::iterator i=pGame->m_gameViews.begin(); i!=pGame->m_gameViews.end(); ++i)
		{
			(*i)->VOnRestore();
		}
	}

	return hr;	
}

void CALLBACK AlphaApp::OnD3D11ReleasingSwapChain( void* pUserContext )
{
	
}

void CALLBACK AlphaApp::OnD3D11DestroyDevice( void* pUserContext )
{
	if (g_pApp->m_Renderer)  // [rez] Fix for multi-monitor issue when target monitor is portrait; posted by Kl1X
	    g_pApp->m_Renderer->VShutdown();
    D3DRenderer::g_GuiResourceManager.OnD3D11DestroyDevice();
	g_pApp->m_Renderer = shared_ptr<IRenderer>(NULL);
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK AlphaApp::OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext )
{
    BaseGameLogic *pGame = g_pApp->m_pGame;

    for(GameViewList::iterator i=pGame->m_gameViews.begin(),
        end=pGame->m_gameViews.end(); i!=end; ++i)
    {
        (*i)->VOnRender(fTime, fElapsedTime);
    }

    g_pApp->m_pGame->VRenderDiagnostics();	
}

//===================================================================
// ModifyDeviceSettings - FCalled right before creating a D3D9 or D3D11 
// device, allowing the app to modify the device settings as needed.
//
//===================================================================
bool CALLBACK AlphaApp::ModifyDeviceSettings( DX11WDeviceSettings* pDeviceSettings, void* pUserContext )
{
	/* Display some tiype of warning if creating a REF Device, reenable this when you know what a REF device is
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if(pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE)
        {
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
        }

    }
	*/
    return true;
}

//===================================================================
// OnUpdateGame - This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.
//
// See Game Coding Complete - 4th Edition - Chapter X, page Y
//===================================================================
void CALLBACK AlphaApp::OnUpdateGame( double fTime, float fElapsedTime, void* pUserContext  )
{
	if (g_pApp->HasModalDialog())
	{       
		// don't update the game if a modal dialog is up.
		return;
	}

	if (g_pApp->m_bQuitting)
	{
		PostMessage(g_pApp->GetHwnd(), WM_CLOSE, 0, 0);
	}

	if (g_pApp->m_pGame)
	{
		IEventManager::Get()->VUpdate(20); // allow event queue to process for up to 20 ms
		
		// Uncomment when you write the network code
		//if (g_pApp->m_pBaseSocketManager)
			//g_pApp->m_pBaseSocketManager->DoSelect(0);      // pause 0 microseconds

		g_pApp->m_pGame->VOnUpdate(float(fTime), fElapsedTime);
	}
}

bool AlphaApp::VLoadGame(void)
{
    // Read the game options and see what the current game
    // needs to be - all of the game graphics are initialized by now, too...
	return m_pGame->VLoadGame(m_Options.m_Level.c_str());
}

// GameCodeApp::GetHumanView()					- not described in the book
//    FUTURE WORK - This function should accept a player number for split screen games
HumanView* AlphaApp::GetHumanView()
{
	HumanView *pView = NULL;
	for(GameViewList::iterator i=m_pGame->m_gameViews.begin(); i!=m_pGame->m_gameViews.end(); ++i)
	{
		if ((*i)->VGetType()==GameView_Human)
		{
			shared_ptr<IGameView> pIGameView(*i);
			pView = static_cast<HumanView *>(&*pIGameView);
			break;
		}
	}
	return pView;
}

void AlphaApp::RegisterEngineEvents(void)
{
    REGISTER_EVENT(EvtData_Environment_Loaded);
    REGISTER_EVENT(EvtData_New_Actor);
    REGISTER_EVENT(EvtData_Move_Actor);
    REGISTER_EVENT(EvtData_Destroy_Actor);
	REGISTER_EVENT(EvtData_Request_New_Actor);
	//REGISTER_EVENT(EvtData_Network_Player_Actor_Assignment);
}