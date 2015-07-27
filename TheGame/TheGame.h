#pragma once
//========================================================================
// TheGame.h : source file for the sample game
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

//temp--------------------------------------------------------------------
#define IDI_ICON1                       101
#define IDI_MAIN_ICON                   101
//temp--------------------------------------------------------------------

#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....

class IEventManager;

//---------------------------------------------------------------------------------------------------------------------
// TeapotWarsApp class.                          
// Chapter 21, page 722
// Note: Don't put anything in this class that needs to be destructed
//---------------------------------------------------------------------------------------------------------------------
class TheGameApp : public AlphaApp
{
protected:
	virtual BaseGameLogic *VCreateGameAndView();

public:
	virtual TCHAR *VGetGameTitle() { return _T("Alpha Engine 1.0"); }
	virtual TCHAR *VGetGameAppDirectory()
	{
		return _T("Alpha\\TeapotWars\\4.0");
	}
	virtual HICON VGetIcon();

protected:
    virtual void VRegisterGameEvents(void);
    //virtual void VCreateNetworkEventForwarder(void);
    //virtual void VDestroyNetworkEventForwarder(void);
};

//---------------------------------------------------------------------------------------------------------------------
// TeapotWarsLogic class                        
// Chapter 21, page 723
//---------------------------------------------------------------------------------------------------------------------
class TheGameLogic : public BaseGameLogic
{
protected:
	//std::list<NetworkEventForwarder* m_networkEventForwarders;

public:
	TheGameLogic();
	virtual ~TheGameLogic();

	// Update
	//virtual void VSetProxy();
	virtual void VMoveActor(const ActorId id, Mat4x4 const &mat);

	// Overloads
	virtual void VChangeState(BaseGameState newState);
	virtual void VAddView(shared_ptr<IGameView> pView,
			ActorId actorId=INVALID_ACTOR_ID);
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) {return m_pPhysics;}

	// set/clear render diagnostics
	void ToggleRenderDiagnostics() {m_RenderDiagnostics = !m_RenderDiagnostics;}

	// event delegates
	//void GameStateDelegate(IEventDataPtr pEventData); not sure why this is here
    //void RemoteClientDelegate(IEventDataPtr pEventData);
    //void NetworkPlayerActorAssignmentDelegate(IEventDataPtr pEventData);
	//void NewGameDelegate(IEventDataPtr pEventData); not sure why thisis here

	//void TestScriptDelegate(IEventDataPtr pEventData);

protected:
    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData);

private:
    void RegisterAllDelegates(void);
    void RemoveAllDelegates(void);
    //void CreateNetworkEventForwarder(const int socketId);
    //void DestroyAllNetworkEventForwarders(void);
};
