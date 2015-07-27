#pragma once
//========================================================================
// Initialization.h : Defines utility functions for game initialization
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

extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern DWORD ReadCPUSpeed();
extern bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern bool IsOnlyInstance(LPCTSTR gameTitle);
extern const TCHAR *GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory);
extern bool CheckForJoystick(HWND hWnd);

//extern DWORD GetFreeVRAM();           // deprecated!!!!

struct GameOptions
{
	// Level option
	std::string m_Level;

	// Rendering options
	std::string m_Renderer;
	bool m_runFullSpeed;
	bool m_Windowed;
	Point m_ScreenSize;

	// Sound options
	float m_soundEffectsVolume;                     
	float m_musicVolume;                            

	// Multiplayer options
	int m_expectedPlayers;                          
	int m_listenPort;                                       
	std::string m_gameHost;                         
	int m_numAIs;
	int m_maxAIs;
	int m_maxPlayers;

	// resource cache options
	bool m_useDevelopmentDirectories;

	// TiXmlElement - look at this to find other options added by the developer
	TiXmlDocument *m_pDoc;

	GameOptions();
	~GameOptions() { SAFE_DELETE(m_pDoc); }

	void Init(const char* xmlFilePath, LPWSTR lpCmdLine);
};