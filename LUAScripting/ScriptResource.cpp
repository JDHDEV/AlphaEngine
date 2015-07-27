//========================================================================
// XmlResource.cpp : Defines a simple resource cache
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
//=======================================================================
#include "AlphaStd.h"

#include "ScriptResource.h"
#include "LuaStateManager.h"

bool ScriptResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    if (rawSize <= 0)
        return false;

	if (!g_pApp->m_pGame || g_pApp->m_pGame->CanRunLua())
		LuaStateManager::Get()->VExecuteString(rawBuffer);

    return true;
}

shared_ptr<IResourceLoader> CreateScriptResourceLoader()
{
    return shared_ptr<IResourceLoader>(AC_NEW ScriptResourceLoader());
}