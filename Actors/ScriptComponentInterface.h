#pragma once
//========================================================================
// ScriptComponentInterface.h - Interface for script components
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
#include "ActorComponent.h"

//---------------------------------------------------------------------------------------------------------------------
// This component is essentially a placeholder for the script representation of the entity.  The engine doesn't do 
// much (if anything) with it, hence the lack of interface.
//---------------------------------------------------------------------------------------------------------------------
class ScriptComponentInterface : public ActorComponent
{
public:
//    static ComponentId COMPONENT_ID;
//    virtual ComponentId VGetComponentId(void) const { return COMPONENT_ID; }
};