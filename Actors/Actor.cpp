//========================================================================
// Actor.cpp - Implements the Actor class
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

#include "Actor.h"
#include "ActorComponent.h"
#include "../Utilities/String.h"

//---------------------------------------------------------------------------------------------------------------------
// Actor
// Chapter 6, page 165
//---------------------------------------------------------------------------------------------------------------------
Actor::Actor(ActorId id)
{
    m_id = id;
    m_type = "Unknown";

	// [mrmike] added post press - this is an editor helper
	m_resource = "Unknown";
}

Actor::~Actor(void)
{
    AC_LOG("Actor", std::string("Destroying Actor ") + ToStr(m_id));
    AC_ASSERT(m_components.empty());  // [rez] if this assert fires, the actor was destroyed without calling Actor::Destroy()
}

bool Actor::Init(TiXmlElement* pData)
{
    AC_LOG("Actor", std::string("Initializing Actor ") + ToStr(m_id));

	m_type = pData->Attribute("type");
	m_resource = pData->Attribute("resource");
    return true;
}

void Actor::PostInit(void)
{
    for (ActorComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
    {
        it->second->VPostInit();
    }
}

void Actor::Destroy(void)
{
    m_components.clear();
}

void Actor::Update(int deltaMs)
{
    for (ActorComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
    {
        it->second->VUpdate(deltaMs);
    }
}

std::string Actor::ToXML()
{
    TiXmlDocument outDoc;

    // Actor element
    TiXmlElement* pActorElement = AC_NEW TiXmlElement("Actor");
    pActorElement->SetAttribute("type", m_type.c_str());
	pActorElement->SetAttribute("resource", m_resource.c_str());

    // components
    for (auto it = m_components.begin(); it != m_components.end(); ++it)
    {
        StrongActorComponentPtr pComponent = it->second;
        TiXmlElement* pComponentElement = pComponent->VGenerateXml();
        pActorElement->LinkEndChild(pComponentElement);
    }

    outDoc.LinkEndChild(pActorElement);
	TiXmlPrinter printer;
	outDoc.Accept(&printer);

	return printer.CStr();
}

void Actor::AddComponent(StrongActorComponentPtr pComponent)
{
    std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
    AC_ASSERT(success.second);
}