//========================================================================
// TheGameEvents.cpp : defines game-specific events for TeapotWars
#include "AlphaStd.h" // TheGameStd.h when Alpha is Dll
#include "TheGameEvents.h"
#include "../LUAScripting/LuaStateManager.h"


const EventType EvtData_Spawn_Brick::sk_EventType(0x53fbab61);

void EvtData_Spawn_Brick::VBuildEventData(void)
{
	m_eventData.AssignNewTable(LuaStateManager::Get()->GetLuaState());
	Vec3 pos = m_pos.GetPosition();
	m_eventData.SetNumber("X", pos.x);
	m_eventData.SetNumber("Y", pos.y);
	m_eventData.SetNumber("Z", pos.z);
	m_eventData.SetInteger("roofus", 0);
}


void RegisterGameScriptEvents(void)
{
    REGISTER_SCRIPT_EVENT(EvtData_Spawn_Brick, EvtData_Spawn_Brick::sk_EventType);
    //REGISTER_SCRIPT_EVENT(EvtData_Gameplay_UI_Update, EvtData_Gameplay_UI_Update::sk_EventType);
    //REGISTER_SCRIPT_EVENT(EvtData_Set_Controlled_Actor, EvtData_Set_Controlled_Actor::sk_EventType);

    //REGISTER_SCRIPT_EVENT(EvtData_StartThrust, EvtData_StartThrust::sk_EventType);
    //REGISTER_SCRIPT_EVENT(EvtData_EndThrust, EvtData_EndThrust::sk_EventType);
    //REGISTER_SCRIPT_EVENT(EvtData_StartSteer, EvtData_StartSteer::sk_EventType);
    //REGISTER_SCRIPT_EVENT(EvtData_EndSteer, EvtData_EndSteer::sk_EventType);

    //// tests
    //REGISTER_SCRIPT_EVENT(EvtData_ScriptEventTest_ToLua, EvtData_ScriptEventTest_ToLua::sk_EventType);
    //REGISTER_SCRIPT_EVENT(EvtData_ScriptEventTest_FromLua, EvtData_ScriptEventTest_FromLua::sk_EventType);
}


//// Note: these are game specific events, which really should be in TeapotEvents.cpp.
//// They aren't because of an annoying dependancy in Network.cpp, which creates events
//// from a bitstream. Once we get "real" streaming code in we can move these back to TeapotEvents.cpp.
//const EventType EvtData_Fire_Weapon::sk_EventType(0x1b15b6a7);
//const EventType EvtData_StartThrust::sk_EventType(0x1d62d48c);
//const EventType EvtData_EndThrust::sk_EventType(0xe60f88a4);
//const EventType EvtData_StartSteer::sk_EventType(0xf0b5b4fd);
//const EventType EvtData_EndSteer::sk_EventType(0x176645ef);
////const EventType EvtData_PlaySound::sk_EventType(0x3d8118ee);
//const EventType EvtData_Gameplay_UI_Update::sk_EventType(0x1002ded2);
//const EventType EvtData_Set_Controlled_Actor::sk_EventType(0xbe5e3388);
//const EventType EvtData_ScriptEventTest_ToLua::sk_EventType(0x56cff2bc);
//const EventType EvtData_ScriptEventTest_FromLua::sk_EventType(0x53fbab61);

//bool EvtData_Gameplay_UI_Update::VBuildEventFromScript(void)
//{
//    if (m_eventData.IsString())
//    {
//        m_gameplayUiString = m_eventData.GetString();
//        return true;
//    }
//
//    return false;
//}

//bool EvtData_Set_Controlled_Actor::VBuildEventFromScript(void)
//{
//    if (m_eventData.IsInteger())
//    {
//        m_id = m_eventData.GetInteger();
//        return true;
//    }
//
//    return false;
//}
//
//void EvtData_ScriptEventTest_ToLua::VBuildEventData(void)
//{
//    m_eventData.AssignNumber(LuaStateManager::Get()->GetLuaState(), m_num);
//}
//
//bool EvtData_ScriptEventTest_FromLua::VBuildEventFromScript(void)
//{
//    if (m_eventData.IsInteger())
//    {
//        m_num = m_eventData.GetInteger();
//        return true;
//    }
//
//    return false;
//}
