#pragma once
//========================================================================
// EventMangerImpl.h : implementation side of the event system
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

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager
{
    typedef std::list<EventListenerDelegate> EventListenerList;
    typedef std::map<EventType, EventListenerList> EventListenerMap;
    typedef std::list<IEventDataPtr> EventQueue;

    EventListenerMap m_eventListeners;
    EventQueue m_queues[EVENTMANAGER_NUM_QUEUES];
    int m_activeQueue;  // index of actively processing queue; events enque to the opposing queue

    ThreadSafeEventQueue m_realtimeEventQueue;

public:
	explicit EventManager(const char* pName, bool setAsGlobal);
	virtual ~EventManager(void);

	virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
	virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

	virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const;
	virtual bool VQueueEvent(const IEventDataPtr& pEvent);
	virtual bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent);
	virtual bool VAbortEvent(const EventType& type, bool allOfType = false);

	virtual bool VUpdate(unsigned long maxMillis = kINFINITE);
};