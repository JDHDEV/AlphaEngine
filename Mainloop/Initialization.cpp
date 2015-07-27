//========================================================================
// Initialization.cpp : Defines utility functions for game initialization
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

#include "AlphaStd.h"
#include <shlobj.h>
#include <direct.h>
#include "Initialization.h"

//========================================================================
// CheckStorage - Chapter 5, page 139
//
// Justin Hunt
//========================================================================
bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	int const drive = _getdrive();
	struct _diskfree_t diskfree;

	_getdiskfree(drive, &diskfree);

	unsigned __int64 const neededClusters = diskSpaceNeeded /
		(diskfree.sectors_per_cluster * diskfree.bytes_per_sector);

	if (diskfree.avail_clusters < neededClusters)
	{
		// If you get here you dont have room
		AC_ERROR("CheckStorage Failure: Not enough physical storage. Delete the porn! ");
		return false;
	}
	return true;
}

//========================================================================
// CheckMemory - Chapter 5, page 139
//
// Justin Hunt
//========================================================================
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);
	if(status.ullTotalPhys < physicalRAMNeeded)
	{
		// You dont have enough physical memory. Time for an upgrade noob!
		AC_ERROR("CheckMemory Failure: Not enough physical memory. NOOB.");
		return false;
	}
	// Check for enough free memory.
	if(status.ullAvailVirtual < virtualRAMNeeded)
	{
		// You dont have enough virtual memory available. Turn off the torrents!
		AC_ERROR("CheckMemory Failure: Not enough virtual memory. NOOB.");
		return false;
	}

	char *buff = AC_NEW char[virtualRAMNeeded];
	if(buff)
		delete[] buff;
	else
	{
		// Even though ther is enough memory, it isnt available in one
		// block, which can be critical for games that manage their own memory
		AC_ERROR("CheckMemory Failure: Not enough contigous memory. Contiguous?");
		return false;
	}
}

//========================================================================
// ReadCPUSpeed - Chapter 5, page 140
//
// Justin Hunt
//========================================================================
DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	// Open the key where the proc speed is hidden
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	if(lError == ERROR_SUCCESS)
	{
		// Query the key
		RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE) &dwMHz, &BufSize);
	}
	return dwMHz;
}

GameOptions::GameOptions()
{
	// set all the options to decent default valu
	m_Level = "";
	m_Renderer = "Direct3D 11";
	m_runFullSpeed = false;
	m_Windowed = false;
	m_soundEffectsVolume = 1.0f;                    
	m_musicVolume = 1.0f;                           
	m_expectedPlayers = 1;
	m_listenPort = -1;                                      
	std::string m_gameHost = "HitsABlunt";
	m_numAIs = 1;
	m_maxAIs = 4;
	m_maxPlayers = 4;
	m_ScreenSize = Point(1024,768);
	m_useDevelopmentDirectories = false;

	m_pDoc = NULL;
}

void GameOptions::Init(const char* xmlFileName, LPWSTR lpCmdLine)
{
    // read the XML file
    // if needed, override the XML file with options passed in on the command line.

    m_pDoc = new TiXmlDocument(xmlFileName);
    if (m_pDoc && m_pDoc->LoadFile())
    {
		TiXmlElement *pRoot = m_pDoc->RootElement();
		if (!pRoot)
			return;

		// Loop through each child element and load the component
		TiXmlElement* pNode = NULL;
		pNode = pRoot->FirstChildElement("Graphics"); 
		if (pNode)
		{
			std::string attribute;
			attribute = pNode->Attribute("renderer");
			if (attribute != "Direct3D 9" && attribute != "Direct3D 11")
			{
				AC_ASSERT(0 && "Bad Renderer setting in Graphics options.");
			}
			else
			{
				m_Renderer = attribute;
			}

			if (pNode->Attribute("width"))
			{
				m_ScreenSize.x = atoi(pNode->Attribute("width"));
				if (m_ScreenSize.x < 800) m_ScreenSize.x = 800;
			}

			if (pNode->Attribute("height"))
			{
				m_ScreenSize.y = atoi(pNode->Attribute("height"));
				if (m_ScreenSize.y < 600) m_ScreenSize.y = 600;
			}

			if (pNode->Attribute("runfullspeed"))
			{
				attribute = pNode->Attribute("runfullspeed");
				m_runFullSpeed = (attribute == "yes") ? true : false;
			}

			if (pNode->Attribute("windowed"))
			{
				attribute = pNode->Attribute("windowed");
				m_Windowed = (attribute == "yes") ? true : false;
			}
		}

		pNode = pRoot->FirstChildElement("Sound"); 
		if (pNode)
		{
			m_musicVolume = atoi(pNode->Attribute("musicVolume")) / 100.0f;
			m_soundEffectsVolume = atoi(pNode->Attribute("sfxVolume")) / 100.0f;
		}

		pNode = pRoot->FirstChildElement("Multiplayer"); 
		if (pNode)
		{
			m_expectedPlayers = atoi(pNode->Attribute("expectedPlayers"));
			m_numAIs = atoi(pNode->Attribute("numAIs"));
			m_maxAIs = atoi(pNode->Attribute("maxAIs"));
			m_maxPlayers = atoi(pNode->Attribute("maxPlayers"));
                        
			m_listenPort = atoi(pNode->Attribute("listenPort"));
			m_gameHost = pNode->Attribute("gameHost");
		}

		pNode = pRoot->FirstChildElement("ResCache");
		if (pNode)
		{
			std::string attribute(pNode->Attribute("useDevelopmentDirectories"));
			m_useDevelopmentDirectories = ((attribute == "yes") ? (true) : (false));
		}
    }
}

//========================================================================
// IsOnlyInstance - Chapter 5, page 137
//
// Justin Hunt
//========================================================================
bool IsOnlyInstance(LPCTSTR gameTitle)
{
	/* NEED TO FIX
	// Find the window. If active, set and return false
	// Only one gmae instance may have this mutex at a time...

	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);

	// If ERROR_SUCCESS then CreateMutex failed, Process already exists
	if(GetLastError() != ERROR_SUCCES)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if(hWnd)
		{
			// An instance of your gameTitle is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForgroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}
	*/
	return true;
}
/*
//========================================================================
// ReadCPUSpeed - Chapter 5, page 140
//
// Justin Hunt
//========================================================================
const TCHAR *GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory)
{
	HRESULT hr;
	static TCHAR m_SaveGameDirectory[MAX_PATH];
	THCAR userDataPath[MAX_PATH];
	hr = SHGetSpecialFolderPath(hWnd, userDataPath, CDIDL_APPDATA, true);

	_tcscpy_s(m_SaveGameDirectory, userDataPath);
	_tcscpy_s(m_SaveGameDirectory, _T("\\"));
	_tcscpy_s(m_SaveGameDirectorym gameAppDirectory);

	// Does our directory exist?
	if(0xffffffff == GetFileAttributes(m_SaveGameDirectory))
	{
		if(SHCreateDirectoryEX(hWnd, mSaveGameDirectory, NULL) != ERROR_SUCCESS)
			return false;
	}

	_tcscat_s(m_SaveGameDirectory, _T("||"));
	return m_SaveGameDirectory;
}
*/