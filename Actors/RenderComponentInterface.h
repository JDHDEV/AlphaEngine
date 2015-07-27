#pragma once
//========================================================================
// RenderComponentInterface.h - Interface for render components
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

class RenderComponentInterface : public ActorComponent
{
public:
//    virtual ComponentId VGetComponentId(void) const override { return COMPONENT_ID; }

    // RenderComponentInterface
    virtual shared_ptr<SceneNode> VGetSceneNode(void) = 0;
};
