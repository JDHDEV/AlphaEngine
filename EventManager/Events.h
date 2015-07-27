#pragma once
//========================================================================
// Events.h : defines common game events
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


#include "EventManager.h"
#include "../Alpha/Alpha.h"
#include "../LUAScripting/ScriptEvent.h"

// Auxiliary data decls ...
//
// data that is passed per-event in the userData parameter
// 
// ( for some, but not all, events )

//-- new object notification

//void RegisterEngineScriptEvents(void);

//---------------------------------------------------------------------------------------------------------------------
// EvtData_Environment_Loaded - this event is sent when a new game is started
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Environment_Loaded : public BaseEventData
{
public:
	static const EventType sk_EventType;

    EvtData_Environment_Loaded(void) { }
	virtual const EventType& VGetEventType(void) const	{ return sk_EventType; }
	virtual IEventDataPtr VCopy(void) const
		{ return IEventDataPtr(AC_NEW EvtData_Environment_Loaded()); }
    virtual const char* GetName(void) const  { return "EvtData_Environment_Loaded";  }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_New_Actor - This event is sent out when an actor is *actually* created.
//---------------------------------------------------------------------------------------------------------------------
class EvtData_New_Actor : public BaseEventData
{
	ActorId m_actorId;
    GameViewId m_viewId;

public:
	static const EventType sk_EventType;

	EvtData_New_Actor(void) 
	{
		m_actorId = INVALID_ACTOR_ID;
		m_viewId = gc_InvalidGameViewId;
	}

    explicit EvtData_New_Actor(ActorId actorId, GameViewId viewId = gc_InvalidGameViewId) 
        : m_actorId(actorId),
          m_viewId(viewId)
	{
	}

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_actorId;
		in >> m_viewId;
    }

	virtual const EventType& VGetEventType(void) const
	{
		return sk_EventType;
	}

	virtual IEventDataPtr VCopy(void) const
	{
		return IEventDataPtr(AC_NEW EvtData_New_Actor(m_actorId, m_viewId));
	}

	virtual void VSerialize(std::ostrstream& out) const
	{
		out << m_actorId << " ";
		out << m_viewId << " ";
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_New_Actor";
    }

	const ActorId GetActorId(void) const
	{
		return m_actorId;
	}

    GameViewId GetViewId(void) const
    {
        return m_viewId;
    }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_Move_Actor - sent when actors are moved
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Move_Actor : public BaseEventData
{
    ActorId m_id;
    Mat4x4 m_matrix;

public:
	static const EventType sk_EventType;

	virtual const EventType& VGetEventType(void) const
	{
		return sk_EventType;
	}

    EvtData_Move_Actor(void)
    {
        m_id = INVALID_ACTOR_ID;
    }

	EvtData_Move_Actor(ActorId id, const Mat4x4& matrix)
        : m_id(id), m_matrix(matrix)
	{
        //
	}

	virtual void VSerialize(std::ostrstream &out) const
	{
		out << m_id << " ";
		for (int i=0; i<4; ++i)
		{
			for (int j=0; j<4; ++j)
			{
				out << m_matrix.m[i][j] << " ";
			}
		}
	}

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
        for (int i=0; i<4; ++i)
        {
            for (int j=0; j<4; ++j)
            {
                in >> m_matrix.m[i][j];
            }
        }
    }

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(AC_NEW EvtData_Move_Actor(m_id, m_matrix));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_Move_Actor";
    }

    ActorId GetId(void) const
    {
        return m_id;
    }

    const Mat4x4& GetMatrix(void) const
    {
        return m_matrix;
    }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_Destroy_Actor - sent when actors are destroyed	
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Destroy_Actor : public BaseEventData
{
    ActorId m_id;

public:
	static const EventType sk_EventType;

    explicit EvtData_Destroy_Actor(ActorId id = INVALID_ACTOR_ID)
        : m_id(id)
    {
        //
    }

	virtual const EventType& VGetEventType(void) const
	{
		return sk_EventType;
	}

	virtual IEventDataPtr VCopy(void) const
	{
		return IEventDataPtr (AC_NEW EvtData_Destroy_Actor(m_id));
	}

	virtual void VSerialize(std::ostrstream &out) const
	{
		out << m_id;
	}

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_Destroy_Actor";
    }

    ActorId GetId(void) const { return m_id; }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_New_Render_Component - This event is sent out when an actor is *actually* created.
//---------------------------------------------------------------------------------------------------------------------
class EvtData_New_Render_Component : public BaseEventData
{
    ActorId m_actorId;
    shared_ptr<SceneNode> m_pSceneNode;

public:
    static const EventType sk_EventType;

    EvtData_New_Render_Component(void) 
    {
        m_actorId = INVALID_ACTOR_ID;
    }

    explicit EvtData_New_Render_Component(ActorId actorId, shared_ptr<SceneNode> pSceneNode) 
        : m_actorId(actorId),
          m_pSceneNode(pSceneNode)
    {
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        AC_ERROR(GetName() + std::string(" should not be serialzied!"));
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        AC_ERROR(GetName() + std::string(" should not be serialzied!"));
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy(void) const
    {
        return IEventDataPtr(AC_NEW EvtData_New_Render_Component(m_actorId, m_pSceneNode));
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_New_Render_Component";
    }

    const ActorId GetActorId(void) const
    {
        return m_actorId;
    }

    shared_ptr<SceneNode> GetSceneNode(void) const
    {
        return m_pSceneNode;
    }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_Modified_Render_Component - This event is sent out when a render component is changed
//   NOTE: This class is not described in the book!
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Modified_Render_Component : public BaseEventData
{
    ActorId m_id;

public:
	static const EventType sk_EventType;

	virtual const EventType& VGetEventType(void) const
	{
		return sk_EventType;
	}

    EvtData_Modified_Render_Component(void)
    {
        m_id = INVALID_ACTOR_ID;
    }

	EvtData_Modified_Render_Component(ActorId id)
        : m_id(id)
	{
	}

	virtual void VSerialize(std::ostrstream &out) const
	{
		out << m_id;
	}

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
    }

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(AC_NEW EvtData_Modified_Render_Component(m_id));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_Modified_Render_Component";
    }

    ActorId GetActorId(void) const
    {
        return m_id;
    }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_Request_Start_Game - this is sent by the authoritative game logic to all views so they will load a game level.
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Request_Start_Game : public BaseEventData
{

public:
	static const EventType sk_EventType;

    EvtData_Request_Start_Game(void) { }

	virtual const EventType& VGetEventType(void) const
	{
		return sk_EventType;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(AC_NEW EvtData_Request_Start_Game());
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_Request_Start_Game";
    }
};

//---------------------------------------------------------------------------------------------------------------------
// EvtData_Update_Tick - sent by the game logic each game tick
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Update_Tick : public BaseEventData
{
    int m_DeltaMilliseconds;

public:
	static const EventType sk_EventType;

    explicit EvtData_Update_Tick( const int deltaMilliseconds )
        : m_DeltaMilliseconds( deltaMilliseconds )
    {
    }

	virtual const EventType& VGetEventType( void ) const
	{
		return sk_EventType;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (AC_NEW EvtData_Update_Tick ( m_DeltaMilliseconds ) );
	}

	virtual void VSerialize( std::ostrstream & out )
	{
		AC_ERROR("You should not be serializing update ticks!");
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_Update_Tick";
    }
};

//---------------------------------------------------------------------------------------------------------------------
// class EvtData_Request_New_Actor				
// This event is sent by a server asking Client proxy logics to create new actors from their local resources.
// It can be sent from script or via code.
// This event is also sent from the server game logic to client logics AFTER it has created a new actor. The logics will allow follow suit to stay in sync.
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Request_New_Actor : public BaseEventData
{
    std::string m_actorResource;
	bool m_hasInitialTransform;
	Mat4x4 m_initialTransform;
	ActorId m_serverActorId;
    GameViewId m_viewId;

public:
	static const EventType sk_EventType;

	EvtData_Request_New_Actor()
	{
		m_actorResource = "";
		m_hasInitialTransform = false;
		m_initialTransform = Mat4x4::g_Identity;
		m_serverActorId = -1;
		m_viewId = gc_InvalidGameViewId;
	}

    explicit EvtData_Request_New_Actor(const std::string &actorResource, const Mat4x4 *initialTransform=NULL, const ActorId serverActorId = INVALID_ACTOR_ID, const GameViewId viewId = gc_InvalidGameViewId)
    {
        m_actorResource = actorResource;
		if (initialTransform)
		{
			m_hasInitialTransform = true;
			m_initialTransform = *initialTransform;
		}
		else
			m_hasInitialTransform = false;

		m_serverActorId = serverActorId;
        m_viewId = viewId;
    }

	virtual const EventType& VGetEventType(void) const
	{
		return sk_EventType;
	}

	virtual void VDeserialize( std::istrstream & in )
	{
		in >> m_actorResource;
		in >> m_hasInitialTransform;
		if (m_hasInitialTransform)
		{
			for (int i=0; i<4; ++i)
			{
				for (int j=0; j<4; ++j)
				{
					in >> m_initialTransform.m[i][j];
				}
			}
		}
		in >> m_serverActorId;
		in >> m_viewId;
	}

	virtual IEventDataPtr VCopy() const
	{	 
		return IEventDataPtr (AC_NEW EvtData_Request_New_Actor(m_actorResource, (m_hasInitialTransform) ? &m_initialTransform : NULL, m_serverActorId, m_viewId));
	}

	virtual void VSerialize(std::ostrstream & out) const
	{
		out << m_actorResource << " ";
		out << m_hasInitialTransform << " ";
		if (m_hasInitialTransform)
		{
			for (int i=0; i<4; ++i)
			{
				for (int j=0; j<4; ++j)
				{
					out << m_initialTransform.m[i][j] << " ";
				}
			}		
		}
		out << m_serverActorId << " ";
		out << m_viewId << " ";
	}

    virtual const char* GetName(void) const { return "EvtData_Request_New_Actor";  }

    const std::string &GetActorResource(void) const { return m_actorResource;  }
	const Mat4x4 *GetInitialTransform(void) const { return (m_hasInitialTransform) ? &m_initialTransform : NULL; }
	const ActorId GetServerActorId(void) const 	{ return m_serverActorId; }
    GameViewId GetViewId(void) const { return m_viewId; }
};