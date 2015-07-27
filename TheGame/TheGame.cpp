//========================================================================
// TeapotWars.cpp : source file for the sample game
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
#include "AlphaStd.h"// late to be The GameStd.h when Alpha is DLL

#include "../Alpha/Alpha.h"
#include "../Physics/Physics.h"
//#include "../Physics/PhysicsEventListener.h"
#include "../MainLoop/Initialization.h"
//#include "../AI/Pathing.h"
#include "../EventManager/Events.h"
#include "../EventManager/EventManagerImpl.h"
#include "../Actors/Actor.h"
//#include "../Actors/PhysicsComponent.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/String.h"

#include "TheGame.h"
#include "TheGameView.h"
//#include "TeapotWarsNetwork.h"
#include "TheGameEvents.h"
//#include "TheGameResources.h"

TheGameApp g_TheGameApp;

//========================================================================
// wWinMain - Defines the entry point to your application (Chapter 5 - page 121)
//
// In the book _tWinMain goes straight into the initialization sequence. Here,
// the GameCode4() free function handles it. This allows the GameCode4 function
// to live in a library, separating the game engine from game specific code,
// in this case TeapotWars.
//
//========================================================================
INT WINAPI wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
	return Alpha(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

//========================================================================
//
// TheGameApp Implementation     - Chapter 21, page 722
//
//========================================================================

//
// TheGameApp::VCreateGameAndView
//
BaseGameLogic *TheGameApp::VCreateGameAndView()
{
	m_pGame = AC_NEW TheGameLogic();
    m_pGame->Init();

	shared_ptr<IGameView> menuView(AC_NEW MainMenuView());
	m_pGame->VAddView(menuView);

	return m_pGame;
}

HICON TheGameApp::VGetIcon()
{
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON1));
}

void TheGameApp::VRegisterGameEvents(void)
{
    //REGISTER_EVENT(EvtData_StartThrust);
    //REGISTER_EVENT(EvtData_EndThrust);
    //REGISTER_EVENT(EvtData_StartSteer);
    //REGISTER_EVENT(EvtData_EndSteer);
}

//void TheGameApp::VCreateNetworkEventForwarder(void)
//{
//	AlphaApp::VCreateNetworkEventForwarder();
//    if (m_pNetworkEventForwarder != NULL)
//    {
//	    IEventManager* pGlobalEventManager = IEventManager::Get();
//		// FUTURE WORK - Events should have a "classification" that signals if they are sent from client to server, from server to client, or both.
//		//               Then as events are created, they are automatically added to the right network forwarders.
//		//               This could also detect a 
//	    pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Fire_Weapon::sk_EventType);
//        pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_StartThrust::sk_EventType);
//        pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_EndThrust::sk_EventType);
//        pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_StartSteer::sk_EventType);
//        pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_EndSteer::sk_EventType);
//	}
//}

//void TeapotWarsApp::VDestroyNetworkEventForwarder(void)
//{
//	GameCodeApp::VDestroyNetworkEventForwarder();
//    if (m_pNetworkEventForwarder)
//    {
//        IEventManager* pGlobalEventManager = IEventManager::Get();
//        pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Fire_Weapon::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_StartThrust::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_EndThrust::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_StartSteer::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_EndSteer::sk_EventType);
//		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
//        SAFE_DELETE(m_pNetworkEventForwarder);
//    }
//}

//========================================================================
//
// TheGameLogic Implementation       - Chapter 21, page 725
//
//========================================================================

//
// TheGameLogic::TheGameLogic
//
TheGameLogic::TheGameLogic()
{
	m_pPhysics.reset(CreateGamePhysics());
    RegisterAllDelegates();
}

//
// TheGameLogic::~TheGameLogic
//
TheGameLogic::~TheGameLogic()
{
    RemoveAllDelegates();
    //DestroyAllNetworkEventForwarders();
}

//void TheGameLogic::VSetProxy()
//{
//	// FUTURE WORK: This can go in the base game logic!!!!
//	BaseGameLogic::VSetProxy();
//}

class WatchMeProcess : public Process
{
	ActorId m_me;
	ActorId m_target;
public:
	WatchMeProcess(ActorId me, ActorId target) { m_me = me; m_target = target; }
	void VOnUpdate(unsigned long deltaMs);
};

void WatchMeProcess::VOnUpdate(unsigned long deltaMs)
{
    StrongActorPtr pTarget = MakeStrongPtr(g_pApp->m_pGame->VGetActor(m_target));
	StrongActorPtr pMe = MakeStrongPtr(g_pApp->m_pGame->VGetActor(m_me));

    shared_ptr<TransformComponent> pTargetTransform = MakeStrongPtr(pTarget->GetComponent<TransformComponent>(TransformComponent::g_Name));
    shared_ptr<TransformComponent> pMyTransform = MakeStrongPtr(pMe->GetComponent<TransformComponent>(TransformComponent::g_Name));

	if (!pTarget || !pMe || !pTargetTransform || !pMyTransform)
	{
		AC_ERROR("This shouldn't happen");
		Fail();
	}

	Vec3 targetPos = pTargetTransform->GetPosition();
	Mat4x4 myTransform = pMyTransform->GetTransform();
	Vec3 myDir = myTransform.GetDirection();
	myDir = Vec3(0.0f, 0.0f, 1.0f);
	Vec3 myPos = pMyTransform->GetPosition();

	Vec3 toTarget = targetPos - myPos;
	toTarget.Normalize();

	float dotProduct = myDir.Dot(toTarget);
	Vec3 crossProduct = myDir.Cross(toTarget);

	float angleInRadians = acos(dotProduct);

	if (crossProduct.y < 0)
		angleInRadians = -angleInRadians;
	
	Mat4x4 rotation;
	rotation.BuildRotationY(angleInRadians);
	rotation.SetPosition(myPos);
	pMyTransform->SetTransform(rotation);
}

//
// TeapotWarsLogic::VChangeState
//
void TheGameLogic::VChangeState(BaseGameState newState)
{
	BaseGameLogic::VChangeState(newState);

	switch(newState)
	{
		case BGS_WaitingForPlayers:
		{
			// spawn all local players (should only be one, though we might support more in the future)
			AC_ASSERT(m_ExpectedPlayers == 1);
			for (int i = 0; i < m_ExpectedPlayers; ++i)
			{
				shared_ptr<IGameView> playersView(AC_NEW TheGameHumanView(g_pApp->m_Renderer));
				VAddView(playersView);

				if (m_bProxy)
				{
					// if we are a remote player, all we have to do is spawn our view - the server will do the rest.
					return;
				}
			}

			//// spawn all remote player's views on the game
			//for (int i = 0; i < m_ExpectedRemotePlayers; ++i)
			//{
			//	shared_ptr<IGameView> remoteGameView(AC_NEW NetworkGameView);
			//	VAddView(remoteGameView);
			//}

			// spawn all AI's views on the game
			//for (int i = 0; i < m_ExpectedAI; ++i)
			//{
			//	shared_ptr<IGameView> aiView(AC_NEW AITeapotView(m_pPathingGraph)); TOODO
			//	VAddView(aiView);
			//}
			break;
		}

		case BGS_SpawningPlayersActors:
		{
			if (m_bProxy)
			{
				// only the server needs to do this.
				return;
			}

			for (auto it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
			{
				shared_ptr<IGameView> pView = *it;
				if (pView->VGetType() == GameView_Human)
				{
					StrongActorPtr pActor = VCreateActor("actors\\player_teapot.xml", NULL);
					if (pActor)
					{
						shared_ptr<EvtData_New_Actor> pNewActorEvent(AC_NEW EvtData_New_Actor(pActor->GetId(), pView->VGetId()));
                        IEventManager::Get()->VTriggerEvent(pNewActorEvent);  // [rez] This needs to happen asap because the constructor function for Lua (which is called in through VCreateActor()) queues an event that expects this event to have been handled
					}
				}
				//else if (pView->VGetType() == GameView_Remote)
				//{
				//	shared_ptr<NetworkGameView> pNetworkGameView = static_pointer_cast<NetworkGameView, IGameView>(pView);
				//	StrongActorPtr pActor = VCreateActor("actors\\remote_teapot.xml", NULL);
				//	if (pActor)
				//	{
				//		shared_ptr<EvtData_New_Actor> pNewActorEvent(AC_NEW EvtData_New_Actor(pActor->GetId(), pNetworkGameView->VGetId()));
				//		IEventManager::Get()->VQueueEvent(pNewActorEvent);
				//	}
				//}
				//else if (pView->VGetType() == GameView_AI)
				//{
				//	shared_ptr<AITeapotView> pAiView = static_pointer_cast<AITeapotView, IGameView>(pView);
				//	StrongActorPtr pActor = VCreateActor("actors\\ai_teapot.xml", NULL);
				//	if (pActor)
				//	{
				//		shared_ptr<EvtData_New_Actor> pNewActorEvent(AC_NEW EvtData_New_Actor(pActor->GetId(), pAiView->VGetId()));
				//		IEventManager::Get()->VQueueEvent(pNewActorEvent);
				//	}
				//}
			}
			break;
		}
	}
}

//
// TeapotWarsLogic::VAddView
//
void TheGameLogic::VAddView(shared_ptr<IGameView> pView, ActorId actor)
{
	BaseGameLogic::VAddView(pView, actor);
	//This is commented out because while the view is created and waiting, the player has NOT attached yet. 
	//if (dynamic_pointer_cast<NetworkGameView>(pView))
	//{
	//	m_HumanPlayersAttached++;
	//}
	if (dynamic_pointer_cast<TheGameHumanView>(pView))
	{
		m_HumanPlayersAttached++;
	}
	//else if (dynamic_pointer_cast<AITeapotView>(pView))
	//{
	//	m_AIPlayersAttached++;
	//}
}

void TheGameLogic::VMoveActor(const ActorId id, Mat4x4 const &mat)
{
    BaseGameLogic::VMoveActor(id, mat);

    // [rez] HACK: This will be removed whenever the gameplay update stuff is in.  This is meant to model the death
    // zone under the grid.

	// FUTURE WORK - This would make a great basis for a Trigger actor that ran a LUA script when other
	//               actors entered or left it!

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(id));
    if (pActor)
    {
        shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
        if (pTransformComponent && pTransformComponent->GetPosition().y < -25)
        {
            shared_ptr<EvtData_Destroy_Actor> pDestroyActorEvent(AC_NEW EvtData_Destroy_Actor(id));
            IEventManager::Get()->VQueueEvent(pDestroyActorEvent);
        }
    }
}


// FUTURE WORK - this isn't TeapotWars specific so it can go into the game agnostic base class
//void TheGameLogic::RemoteClientDelegate(IEventDataPtr pEventData)
//{
//	// This event is always sent from clients to the game server.
//
//	shared_ptr<EvtData_Remote_Client> pCastEventData = static_pointer_cast<EvtData_Remote_Client>(pEventData);
//	const int sockID = pCastEventData->GetSocketId();
//	const int ipAddress = pCastEventData->GetIpAddress();
//
//	// go find a NetworkGameView that doesn't have a socket ID, and attach this client to that view.
//    for (auto it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
//    {
//        shared_ptr<IGameView> pView = *it;
//        if (pView->VGetType() == GameView_Remote)
//        {
//            shared_ptr<NetworkGameView> pNetworkGameView = static_pointer_cast<NetworkGameView, IGameView>(pView);
//			if (!pNetworkGameView->HasRemotePlayerAttached())
//			{				
//				pNetworkGameView->AttachRemotePlayer(sockID);
//				CreateNetworkEventForwarder(sockID);
//				m_HumanPlayersAttached++;
//
//				return;
//			}
//        }
//    }
//}

//void TheGameLogic::NetworkPlayerActorAssignmentDelegate(IEventDataPtr pEventData)
//{
//	if (!m_bProxy)
//		return;
//
//    // we're a remote client getting an actor assignment.
//    // the server assigned us a playerId when we first attached (the server's socketId, actually)
//    shared_ptr<EvtData_Network_Player_Actor_Assignment> pCastEventData = static_pointer_cast<EvtData_Network_Player_Actor_Assignment>(pEventData);
//
//	if (pCastEventData->GetActorId()==INVALID_ACTOR_ID)
//	{
//		m_remotePlayerId = pCastEventData->GetSocketId();
//		return;
//	}
//
//	for (auto it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
//    {
//        shared_ptr<IGameView> pView = *it;
//        if (pView->VGetType() == GameView_Human)
//        {
//            shared_ptr<TeapotWarsHumanView> pHumanView = static_pointer_cast<TeapotWarsHumanView, IGameView>(pView);
//			if (m_remotePlayerId == pCastEventData->GetSocketId())
//			{
//				pHumanView->VSetControlledActor(pCastEventData->GetActorId());
//			}
//			return;
//		}
//	}
//
//	AC_ERROR("Could not find HumanView to attach actor to!");
//}

//void TheGameLogic::StartThrustDelegate(IEventDataPtr pEventData)
//{
//    shared_ptr<EvtData_StartThrust> pCastEventData = static_pointer_cast<EvtData_StartThrust>(pEventData);
//    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
//    if (pActor)
//    {
//        shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
//        if (pPhysicalComponent)
//        {
//            pPhysicalComponent->ApplyAcceleration(pCastEventData->GetAcceleration());
//        }
//    }
//}

//void TheGameLogic::TestScriptDelegate(IEventDataPtr pEventData)
//{
//    shared_ptr<EvtData_ScriptEventTest_FromLua> pCastEventData = static_pointer_cast<EvtData_ScriptEventTest_FromLua>(pEventData);
//    AC_LOG("Lua", "Event received in C++ from Lua: " + ToStr(pCastEventData->GetNum()));
//}

void TheGameLogic::RegisterAllDelegates(void)
{
	// FUTURE WORK: Only these belong in TeapotWars.
    //pGlobalEventManager->VAddListener(MakeDelegate(this, &TheGameLogic::StartThrustDelegate), EvtData_StartThrust::sk_EventType);
    //pGlobalEventManager->VAddListener(MakeDelegate(this, &TheGameLogic::EndThrustDelegate), EvtData_EndThrust::sk_EventType);
    //pGlobalEventManager->VAddListener(MakeDelegate(this, &TheGameLogic::StartSteerDelegate), EvtData_StartSteer::sk_EventType);
    //pGlobalEventManager->VAddListener(MakeDelegate(this, &TheGameLogic::EndSteerDelegate), EvtData_EndSteer::sk_EventType);

    //pGlobalEventManager->VAddListener(MakeDelegate(this, &TheGameLogic::TestScriptDelegate), EvtData_ScriptEventTest_FromLua::sk_EventType);
}

void TheGameLogic::RemoveAllDelegates(void)
{
	// FUTURE WORK: These belong in teapot wars!
	//pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TheGameLogic::StartThrustDelegate), EvtData_StartThrust::sk_EventType);
	//pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TheGameLogic::EndThrustDelegate), EvtData_EndThrust::sk_EventType);
	//pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TheGameLogic::StartSteerDelegate), EvtData_StartSteer::sk_EventType);
	//pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TheGameLogic::EndSteerDelegate), EvtData_EndSteer::sk_EventType);

    //pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TheGameLogic::TestScriptDelegate), EvtData_ScriptEventTest_FromLua::sk_EventType);
}

//void TheGameLogic::CreateNetworkEventForwarder(const int socketId)
//{
//    NetworkEventForwarder* pNetworkEventForwarder = GCC_NEW NetworkEventForwarder(socketId);
//
//    IEventManager* pGlobalEventManager = IEventManager::Get();
//
//	// then add those events that need to be sent along to amy attached clients
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_PhysCollision::sk_EventType);
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Destroy_Actor::sk_EventType);
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Fire_Weapon::sk_EventType);
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_New_Actor::sk_EventType);
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Move_Actor::sk_EventType);
//	pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Request_New_Actor::sk_EventType);
//    pGlobalEventManager->VAddListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Network_Player_Actor_Assignment::sk_EventType);
//
//    m_networkEventForwarders.push_back(pNetworkEventForwarder);
//}

//void TheGameLogic::DestroyAllNetworkEventForwarders(void)
//{
//    for (auto it = m_networkEventForwarders.begin(); it != m_networkEventForwarders.end(); ++it)
//    {
//        NetworkEventForwarder* pNetworkEventForwarder = (*it);
//
//        IEventManager* pGlobalEventManager = IEventManager::Get();
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_PhysCollision::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Destroy_Actor::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Fire_Weapon::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_New_Actor::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Move_Actor::sk_EventType);
//        pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Request_New_Actor::sk_EventType);
//	    pGlobalEventManager->VRemoveListener(MakeDelegate(pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Network_Player_Actor_Assignment::sk_EventType);
//
//        delete pNetworkEventForwarder;
//    }
//
//    m_networkEventForwarders.clear();
//}

bool TheGameLogic::VLoadGameDelegate(TiXmlElement* pLevelData)
{
    RegisterGameScriptEvents();
    return true;
}