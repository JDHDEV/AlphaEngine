//========================================================================
// ScriptEvent.cpp
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

#include "ScriptEvent.h"
#include "LuaStateManager.h"

ScriptEvent::CreationFunctions ScriptEvent::s_creationFunctions;

//---------------------------------------------------------------------------------------------------------------------
// Returns the event data after building it (if necessary)
//---------------------------------------------------------------------------------------------------------------------
LuaPlus::LuaObject ScriptEvent::GetEventData(void)
{
	if (!m_eventDataIsValid)
	{
		VBuildEventData();
		m_eventDataIsValid = true;
	}
	
	return m_eventData;
}

//---------------------------------------------------------------------------------------------------------------------
// This function is called when an event is sent from the script.  It sets the m_eventData member and calls 
// VBuildEventFromScript().
//---------------------------------------------------------------------------------------------------------------------
bool ScriptEvent::SetEventData(LuaPlus::LuaObject eventData)
{
	m_eventData = eventData;
	m_eventDataIsValid = VBuildEventFromScript();
	return m_eventDataIsValid;
}

//---------------------------------------------------------------------------------------------------------------------
// This function is called to register an event type with the script to link them.  The simplest way to do this is 
// with the REGISTER_SCRIPT_EVENT() macro, which calls this function.
//---------------------------------------------------------------------------------------------------------------------
void ScriptEvent::RegisterEventTypeWithScript(const char* key, EventType type)
{
	// get or create the EventType table
	LuaPlus::LuaObject eventTypeTable = LuaStateManager::Get()->GetGlobalVars().GetByName("EventType");
	if (eventTypeTable.IsNil())
		eventTypeTable = LuaStateManager::Get()->GetGlobalVars().CreateTable("EventType");

	// error checking
	AC_ASSERT(eventTypeTable.IsTable());
	AC_ASSERT(eventTypeTable[key].IsNil());
	
	// add the entry
	eventTypeTable.SetNumber(key, (double)type);
}

//---------------------------------------------------------------------------------------------------------------------
// This function is called to map an event creation function pointer with the event type.  This allows an event to be
// created by only knowing its type.  This is required to allow scripts to trigger the instantiation and queueing of 
// events.
//---------------------------------------------------------------------------------------------------------------------
void ScriptEvent::AddCreationFunction(EventType type, CreateEventForScriptFunctionType pCreationFunctionPtr)
{
	AC_ASSERT(s_creationFunctions.find(type) == s_creationFunctions.end());
	AC_ASSERT(pCreationFunctionPtr != NULL);
	s_creationFunctions.insert(std::make_pair(type, pCreationFunctionPtr));
}

//---------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------
ScriptEvent* ScriptEvent::CreateEventFromScript(EventType type)
{
	CreationFunctions::iterator findIt = s_creationFunctions.find(type);
	if (findIt != s_creationFunctions.end())
	{
		CreateEventForScriptFunctionType func = findIt->second;
		return func();
	}
	else
	{
		AC_ERROR("Couldn't find event type");
		return NULL;
	}
}

//---------------------------------------------------------------------------------------------------------------------
// Default implementation for VBuildEventData() sets the event data to nil.
//---------------------------------------------------------------------------------------------------------------------
void ScriptEvent::VBuildEventData(void)
{
	m_eventData.AssignNil(LuaStateManager::Get()->GetLuaState());
}