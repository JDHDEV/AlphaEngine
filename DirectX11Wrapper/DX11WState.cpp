//========================================================================
// DX11WState.cpp - Class that controls access to the State Struct
//
// Part of the Alpha Application
//
// http://www.directxtutorial.com/
//
// Justin Hunt
//========================================================================

#include "AlphaStd.h"
#include "DirectX.h"

//========================================================================
//
// DX11WState implementation
//
//========================================================================

//
// DX11WState::CreateWindow
//
void DX11WState::Create()
{
		// Clear mememory for the state object
        ZeroMemory( &m_state, sizeof(STATE));
}

//
// DX11WState::CreateWindow
//
void DX11WState::Destroy()
{
	g_pDX11W->Shutdown();
}

//
// DX11WState::DXGIFactory
//
void DX11WState::SetDXGIFactory(IDXGIFactory* factory)
{
	m_state.m_DXGIFactory = factory;
}
IDXGIFactory* DX11WState::GetDXGIFactory()
{
	return m_state.m_DXGIFactory;
}

//
// DX11WState::DXGISwapChain
//
void DX11WState::SetDXGISwapChain(IDXGISwapChain* swapchain)
{
	m_state.m_DXGISwapChain = swapchain;
}
IDXGISwapChain* DX11WState::GetDXGISwapChain()
{
	return m_state.m_DXGISwapChain;
}

//
// DX11WState::BackBufferSurfaceDescDXGI
//
void DX11WState::SetBackBufferSurfaceDescDXGI(DXGI_SURFACE_DESC *surfacedesc)
{
	m_state.m_BackBufferSurfaceDescDXGI = *surfacedesc;
}
DXGI_SURFACE_DESC* DX11WState::GetBackBufferSurfaceDescDXGI()
{
	return &m_state.m_BackBufferSurfaceDescDXGI;
}

//
// DX11WState::D3D11Device
//
void DX11WState::SetD3D11Device(ID3D11Device* D3D11Device)
{
	m_state.m_D3D11Device = D3D11Device;
}
ID3D11Device* DX11WState::GetD3D11Device()
{
	return m_state.m_D3D11Device;
}

//
// DX11WState::D3D11DeviceContext
//
void DX11WState::SetD3D11DeviceContext(ID3D11DeviceContext*	D3D11DeviceContext)
{
	m_state.m_D3D11DeviceContext = D3D11DeviceContext;
}
ID3D11DeviceContext* DX11WState::GetD3D11DeviceContext()
{
	return m_state.m_D3D11DeviceContext;
}

//
// DX11WState::D3D11DepthStencil
//
void DX11WState::SetD3D11DepthStencil(ID3D11Texture2D*	D3D11DepthStencil)
{
	m_state.m_D3D11DepthStencil = D3D11DepthStencil;
}
ID3D11Texture2D* DX11WState::GetD3D11DepthStencil()
{
	return m_state.m_D3D11DepthStencil;
}

//
// DX11WState::SetD3D11DepthStencilView
//
void DX11WState::SetD3D11DepthStencilView(ID3D11DepthStencilView* D3D11DepthStencilView)
{
	m_state.m_D3D11DepthStencilView = D3D11DepthStencilView;
}
ID3D11DepthStencilView* DX11WState::GetD3D11DepthStencilView()
{
	return m_state.m_D3D11DepthStencilView;
}

//
// DX11WState::D3D11RenderTargetView
//
void DX11WState::SetD3D11RenderTargetView(ID3D11RenderTargetView* D3D11RenderTargetView)
{
	m_state.m_D3D11RenderTargetView = D3D11RenderTargetView;
}
ID3D11RenderTargetView* DX11WState::GetD3D11RenderTargetView()
{
	return m_state.m_D3D11RenderTargetView;
}

//
// DX11WState::D3D11RasterizerState
//
void DX11WState::SetD3D11RasterizerState(ID3D11RasterizerState* D3D11RasterizerState)
{
	m_state.m_D3D11RasterizerState = D3D11RasterizerState;
}
ID3D11RasterizerState* DX11WState::GetD3D11RasterizerState()
{
	return m_state.m_D3D11RasterizerState;
}

//
// DX11WState::HWNDFocus
//
void DX11WState::SetHWNDFocus(HWND hwnd)
{
	m_state.m_HWNDFocus = hwnd;
}
HWND DX11WState::GetHWNDFocus()
{
	return m_state.m_HWNDFocus;
}

//
// DX11WState::Time
//
void DX11WState::SetTime(double Time)
{
	m_state.m_Time = Time;
}
double DX11WState::GetTime()
{
	return m_state.m_Time;
}

//
// DX11WState::AbsoluteTime
//
void DX11WState::SetAbsoluteTime(double AbsoluteTime)
{
	m_state.m_AbsoluteTime = AbsoluteTime;
}
double DX11WState::GetAbsoluteTime()
{
	return m_state.m_AbsoluteTime;
}

//
// DX11WState::ElapsedTime
//
void DX11WState::SetElapsedTime(float ElapsedTime)
{
	m_state.m_ElapsedTime = ElapsedTime;
}
float DX11WState::GetElapsedTime()
{
	return m_state.m_ElapsedTime;
}

//
// DX11WState::HInstance
//
void DX11WState::SetHInstance(HINSTANCE HInstance)
{
	m_state.m_HInstance = HInstance;
}
HINSTANCE DX11WState::GetHInstance()
{
	return m_state.m_HInstance;
}

//
// DX11WState::LastStatsUpdateTime
//
void  DX11WState::SetLastStatsUpdateTime(double LastStatsUpdateTime)
{
	m_state.m_LastStatsUpdateTime = LastStatsUpdateTime;
}
double  DX11WState::GetLastStatsUpdateTime()
{
	return m_state.m_LastStatsUpdateTime;
}

//
// DX11WState::LastStatsUpdateFrames
//
void  DX11WState::SetLastStatsUpdateFrames(DWORD LastStatsUpdateFrames)
{
	m_state.m_LastStatsUpdateFrames = LastStatsUpdateFrames;
}	
DWORD  DX11WState::GetLastStatsUpdateFrames()
{
	return m_state.m_LastStatsUpdateFrames;
}

//
// DX11WState::FPS
//
void  DX11WState::SetFPS(float FPS)
{
	m_state.m_FPS = FPS;
}
float  DX11WState::GetFPS()
{
	return m_state.m_FPS;
}

//
// DX11WState::CurrentFrameNumber
//
void  DX11WState::SetCurrentFrameNumber(int CurrentFrameNumber)
{
	m_state.m_CurrentFrameNumber = CurrentFrameNumber;
}
int  DX11WState::GetCurrentFrameNumber()
{
	return m_state.m_CurrentFrameNumber;
}

//
// DX11WState::DX11WInited
//
void DX11WState::SetDX11WInited(bool DX11WInited)
{
	m_state.m_DX11WInited = DX11WInited;
}
bool DX11WState::GetDX11WInited()
{
	return m_state.m_DX11WInited;
}

//
// DX11WState::WindowCreated
//
void DX11WState::SetWindowCreated(bool WindowCreated)
{
	m_state.m_WindowCreated = WindowCreated;
}
bool DX11WState::GetWindowCreated()
{
	return m_state.m_WindowCreated;
}

//
// DX11WState::DeviceCreated
//
void DX11WState::SetDeviceCreated(bool DeviceCreated)
{
	m_state.m_DeviceCreated = DeviceCreated;
}
bool DX11WState::GetDeviceCreated()
{
	return m_state.m_DeviceCreated;
}

//
// DX11WState::CallbackDeviceChanging
//
void DX11WState::SetCallbackDeviceChanging(LPCALLBACKMODIFYDEVICESETTINGS ModifyDeviceSettingsFunc)
{
	m_state.m_ModifyDeviceSettingsFunc = ModifyDeviceSettingsFunc;
}
LPCALLBACKMODIFYDEVICESETTINGS DX11WState::GetCallbackDeviceChanging()
{
	return m_state.m_ModifyDeviceSettingsFunc;
}

//
// DX11WState::CallbackFrameMove
//
void DX11WState::SetCallbackFrameMove(LPCALLBACKFRAMEMOVE FrameMoveFunc)
{
	m_state.m_FrameMoveFunc = FrameMoveFunc;
}
LPCALLBACKFRAMEMOVE DX11WState::GetCallbackFrameMove()
{
	return m_state.m_FrameMoveFunc;
}

//
// DX11WState::WindowsMsgProc
//
void DX11WState::SetWindowsMsgProc(LPCALLBACKMSGPROC WindowsMsgProc)
{
	m_state.m_WindowsMsgProc = WindowsMsgProc;
}
LPCALLBACKMSGPROC DX11WState::GetWindowsMsgProc()
{
	return m_state.m_WindowsMsgProc;
}

/*
void DX11WState::SetIsD3D11DeviceAcceptableFunc(LPCALLBACKISD3D11DEVICEACCEPTABLE IsD3D11DeviceAcceptableFunc)
{
	m_state.m_IsD3D11DeviceAcceptableFunc = IsD3D11DeviceAcceptableFunc;
}
*/

//
// DX11WState::D3D11DeviceCreatedFunc
//
void DX11WState::SetD3D11DeviceCreatedFunc(LPCALLBACKD3D11DEVICECREATED D3D11DeviceCreatedFunc)
{
	m_state.m_D3D11DeviceCreatedFunc = D3D11DeviceCreatedFunc;
}
LPCALLBACKD3D11DEVICECREATED DX11WState::GetD3D11DeviceCreatedFunc()
{
	return m_state.m_D3D11DeviceCreatedFunc;
}

//
// DX11WState::D3D11SwapChainResizedFunc
//
void DX11WState::SetD3D11SwapChainResizedFunc(LPCALLBACKD3D11SWAPCHAINRESIZED D3D11SwapChainResizedFunc)
{
	m_state.m_D3D11SwapChainResizedFunc = D3D11SwapChainResizedFunc;
}
LPCALLBACKD3D11SWAPCHAINRESIZED DX11WState::GetD3D11SwapChainResizedFunc()
{
	return m_state.m_D3D11SwapChainResizedFunc;
}

//
// DX11WState::D3D11SwapChainReleasingFunc
//
void DX11WState::SetD3D11SwapChainReleasingFunc(LPCALLBACKD3D11SWAPCHAINRELEASING D3D11SwapChainReleasingFunc)
{
	m_state.m_D3D11SwapChainReleasingFunc = D3D11SwapChainReleasingFunc;
}
LPCALLBACKD3D11SWAPCHAINRELEASING DX11WState::GetD3D11SwapChainReleasingFunc()
{
	return m_state.m_D3D11SwapChainReleasingFunc;
}

//
// DX11WState::D3D11DeviceDestroyedFunc
//
void DX11WState::SetD3D11DeviceDestroyedFunc(LPCALLBACKD3D11DEVICEDESTROYED D3D11DeviceDestroyedFunc)
{
	m_state.m_D3D11DeviceDestroyedFunc = D3D11DeviceDestroyedFunc;
}
LPCALLBACKD3D11DEVICEDESTROYED DX11WState::GetD3D11DeviceDestroyedFunc()
{
	return m_state.m_D3D11DeviceDestroyedFunc;
}

//
// DX11WState::D3D11FrameRenderFunc
//
void DX11WState::SetD3D11FrameRenderFunc(LPCALLBACKD3D11FRAMERENDER D3D11FrameRenderFunc)
{
	m_state.m_D3D11FrameRenderFunc = D3D11FrameRenderFunc;
}
LPCALLBACKD3D11FRAMERENDER DX11WState::GetD3D11FrameRenderFunc()
{
	return m_state.m_D3D11FrameRenderFunc;
}

//
// DX11WState::D3D11FrameRenderFuncUserContext
//
void DX11WState::SetD3D11FrameRenderFuncUserContext(void* D3D11FrameRenderFuncUserContext)
{
	m_state.m_D3D11FrameRenderFuncUserContext = D3D11FrameRenderFuncUserContext;
}
void* DX11WState::GetD3D11FrameRenderFuncUserContext()
{
	return m_state.m_D3D11FrameRenderFuncUserContext;
}






