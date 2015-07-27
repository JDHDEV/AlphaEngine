//========================================================================
// EventManager.cpp : implements a multi-listener multi-sender event system
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
#include "EventManager.h"

static IEventManager* g_pEventMgr = NULL;
GenericObjectFactory<IEventData, EventType> g_eventFactory;

//GCC_MEMORY_WATCHER_DEFINITION(IEventData);

IEventManager* IEventManager::Get(void)
{
	AC_ASSERT(g_pEventMgr);
	return g_pEventMgr;
}

IEventManager::IEventManager(const char* pName, bool setAsGlobal)
{
	if (setAsGlobal)
	{
		if (g_pEventMgr)
		{
			AC_ERROR("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.");
			delete g_pEventMgr;
		}

		g_pEventMgr = this;
	}
}

IEventManager::~IEventManager(void)
{
    if (g_pEventMgr == this)
        g_pEventMgr = NULL;
}