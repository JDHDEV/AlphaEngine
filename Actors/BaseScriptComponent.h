#pragma once
//========================================================================
// BaseScriptComponent.h - Component for exposing actors to Lua
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
#include "ScriptComponentInterface.h"
#include "LuaPlus.h"

//---------------------------------------------------------------------------------------------------------------------
// BaseScriptComponent class
// Chapter 12, page 388
//---------------------------------------------------------------------------------------------------------------------
class BaseScriptComponent : public ScriptComponentInterface
{
	std::string m_scriptObjectName;
    std::string m_constructorName;
    std::string m_destructorName;

	LuaPlus::LuaObject m_scriptObject;
    LuaPlus::LuaObject m_scriptConstructor;
	LuaPlus::LuaObject m_scriptDestructor;
	
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

	BaseScriptComponent(void);
	virtual ~BaseScriptComponent(void);
	virtual bool VInit(TiXmlElement* pData);
    virtual void VPostInit(void);
    virtual TiXmlElement* VGenerateXml(void);
	
	static void RegisterScriptFunctions(void);
	static void UnregisterScriptFunctions(void);
	
private:
	void CreateScriptObject(void);
	
	// component script functions
	LuaPlus::LuaObject GetActorId(void);
	
	// physics component script functions
	LuaPlus::LuaObject GetPos(void);
    void SetPos(LuaPlus::LuaObject newPos);
    LuaPlus::LuaObject GetLookAt(void) const;
    float GetYOrientationRadians(void) const;
	//void RotateY(float angleRadians);
	//void SetPosition(float x, float y, float z);
	void LookAt(Vec3 target);
    //void Stop(void);
};