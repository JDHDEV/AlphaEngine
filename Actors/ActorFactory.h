#pragma once
//========================================================================
// ActorFactory.h : Defines the ActorFactory class
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

//---------------------------------------------------------------------------------------------------------------------
// ActorFactory class
// Chapter 6, page 161
//---------------------------------------------------------------------------------------------------------------------
class ActorFactory
{
    ActorId m_lastActorId;

protected:
    GenericObjectFactory<ActorComponent, ComponentId> m_componentFactory;

public:
    ActorFactory(void);

    StrongActorPtr CreateActor(const char* actorResource, TiXmlElement* overrides, const Mat4x4* initialTransform, const ActorId serversActorId);
	void ModifyActor(StrongActorPtr pActor, TiXmlElement* overrides);

//protected:
    // [rez] This function can be overridden by a subclass so you can create game-specific C++ components.  If you do
    // this, make sure you call the base-class version first.  If it returns NULL, you know it's not an engine component.
    virtual StrongActorComponentPtr VCreateComponent(TiXmlElement* pData);

private:
    ActorId GetNextActorId(void) { ++m_lastActorId; return m_lastActorId; }
};