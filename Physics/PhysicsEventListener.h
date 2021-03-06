#pragma once
//========================================================================
// PhysicsEventListener.cpp : implements the events sent FROM the phsyics system
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
#include "../EventManager/EventManager.h"
#include "../Alpha/Alpha.h"
#include "../LUAScripting/ScriptEvent.h"

//
// Physics event implementation 
//
class EvtData_PhysTrigger_Enter : public BaseEventData
{
	int m_triggerID;
    ActorId m_other;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysTrigger_Enter()
	{
		m_triggerID = -1;
		m_other = INVALID_ACTOR_ID;
	}

	explicit EvtData_PhysTrigger_Enter(int triggerID, ActorId other)
		: m_triggerID(triggerID),
		  m_other(other)
	{}

	IEventDataPtr VCopy() const 
	{
		return IEventDataPtr(AC_NEW EvtData_PhysTrigger_Enter(m_triggerID, m_other));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysTrigger_Enter";
    }

    int GetTriggerId(void) const
    {
        return m_triggerID;
    }

    ActorId GetOtherActor(void) const
    {
        return m_other;
    }
};

class EvtData_PhysTrigger_Leave : public BaseEventData
{
	int m_triggerID;
    ActorId m_other;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType(void) const
	{
		return sk_EventType;
	}

	EvtData_PhysTrigger_Leave()
	{
		m_triggerID = -1;
		m_other = INVALID_ACTOR_ID;
	}

	explicit EvtData_PhysTrigger_Leave(int triggerID, ActorId other)
		: m_triggerID(triggerID),
		  m_other(other)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (AC_NEW EvtData_PhysTrigger_Leave(m_triggerID, m_other));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysTrigger_Leave";
    }

    int GetTriggerId(void) const
    {
        return m_triggerID;
    }
    
    ActorId GetOtherActor(void) const
    {
        return m_other;
    }
};

class EvtData_PhysCollision : public ScriptEvent
{
	ActorId m_ActorA;
    ActorId m_ActorB;
	Vec3 m_SumNormalForce;
    Vec3 m_SumFrictionForce;
    Vec3List m_CollisionPoints;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysCollision()
	{
		m_ActorA = INVALID_ACTOR_ID;
		m_ActorB = INVALID_ACTOR_ID;
		m_SumNormalForce = Vec3(0.0f, 0.0f, 0.0f);
		m_SumFrictionForce = Vec3(0.0f, 0.0f, 0.0f);
	}

	explicit EvtData_PhysCollision(ActorId actorA,
								ActorId actorB,
								Vec3 sumNormalForce,
								Vec3 sumFrictionForce,
								Vec3List collisionPoints)
		: m_ActorA(actorA),
		m_ActorB(actorB),
		m_SumNormalForce(sumNormalForce),
		m_SumFrictionForce(sumFrictionForce),
		m_CollisionPoints(collisionPoints)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (AC_NEW EvtData_PhysCollision(m_ActorA, m_ActorB, m_SumNormalForce, m_SumFrictionForce, m_CollisionPoints));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysCollision";
    }

    ActorId GetActorA(void) const
    {
        return m_ActorA;
    }

    ActorId GetActorB(void) const
    {
        return m_ActorB;
    }

    const Vec3& GetSumNormalForce(void) const
    {
        return m_SumNormalForce;
    }

    const Vec3& GetSumFrictionForce(void) const
    {
        return m_SumFrictionForce;
    }

    const Vec3List& GetCollisionPoints(void) const
    {
        return m_CollisionPoints;
    }

    virtual void VBuildEventData(void);

    EXPORT_FOR_SCRIPT_EVENT(EvtData_PhysCollision);
};


class EvtData_PhysSeparation : public BaseEventData
{
	ActorId m_ActorA;
    ActorId m_ActorB;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysSeparation()
	{

		m_ActorA = INVALID_ACTOR_ID;
		m_ActorB = INVALID_ACTOR_ID;
	}

	explicit EvtData_PhysSeparation(ActorId actorA, ActorId actorB)
		: m_ActorA(actorA)
		, m_ActorB(actorB)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (AC_NEW EvtData_PhysSeparation(m_ActorA, m_ActorB));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysSeparation";
    }

    ActorId GetActorA(void) const
    {
        return m_ActorA;
    }

    ActorId GetActorB(void) const
    {
        return m_ActorB;
    }
};