//========================================================================
// File: Scene.cpp - implements the container class for 3D Graphics scenes 
// may later be worked into a scenegraph i dont like the term scene ha
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

#include "../Alpha/Alpha.h"
#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Utilities/String.h"

#include "Geometry.h"
#include "Lights.h"

////////////////////////////////////////////////////
// Scene Implementation
////////////////////////////////////////////////////

//
// Scene::Scene						- Chapter 16, page 539
//
//    Note: The shared_ptr<IRenderer> was added to allow for both D3D9 and D3D11 renderer implementations.
//          The book only describes D3D11, so to find all the differences, just search for m_Renderer!
//
Scene::Scene(shared_ptr<IRenderer> renderer)
{
	m_Root.reset(AC_NEW RootOctNode());
	m_Renderer = renderer;
	m_LightManager = AC_NEW LightManager;

	D3DXCreateMatrixStack(0, &m_MatrixStack);

	// [mrike] - event delegates were added post-press
    IEventManager* pEventMgr = IEventManager::Get();
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);
}

//
// Scene::~Scene					- Chapter 16, page 539
//
Scene::~Scene()
{
	// [mrmike] - event delegates were added post-press!
    IEventManager* pEventMgr = IEventManager::Get();
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);

	SAFE_RELEASE(m_MatrixStack);
	SAFE_DELETE(m_LightManager);
}

//
// Scene::OnRender					- Chapter 16, page 539
//
HRESULT Scene::OnRender()
{
	// The render passes usually go like this 
	// 1. Static objects & terrain
	// 2. Actors (dynamic objects that can move)
	// 3. The Sky
	// 4. Anything with Alpha

	if (m_Root && m_Camera)
	{
		// The scene root could be anything, but it
		// is usually a SceneNode with the identity
		// matrix

		m_Camera->SetViewTransform(this);
		m_LightManager->CalcLighting(this);
		if (m_Root->VPreRender(this)==S_OK)
		{
			m_Root->VRender(this);
			m_Root->VRenderChildren(this);
			m_Root->VPostRender(this);
		}
		RenderInstances();
		RenderAlphaPass();
	}

	return S_OK;
}	

//
// Scene::OnRestore					- Chapter 16, page 540
//
HRESULT Scene::OnRestore()
{
	if (!m_Root)
		return S_OK;

	HRESULT hr;
	V_RETURN(m_Renderer->VOnRestore());// is this only needed for d3d9? could i remove?

	return m_Root->VOnRestore(this);
}

//
// Scene::OnLostDevice						- not in the book
//
//    All Scene nodes implement VOnLostDevice, which is called in the D3D9 renderer. could remove? not planning on supporting d3d9
//
HRESULT Scene::OnLostDevice()
{
	if (m_Root)
	{
		return m_Root->VOnLostDevice(this);
	}
	return S_OK;
}

//
// Scene::OnUpdate					- Chapter 16, page 540
//
HRESULT Scene::OnUpdate(const int deltaMilliseconds)
{
	if (!m_Root)
		return S_OK;

	static DWORD lastTime = timeGetTime();
	DWORD elapsedTime = 0;
	DWORD now = timeGetTime();

	elapsedTime = now - lastTime;
	lastTime = now;

	return m_Root->VOnUpdate(this, elapsedTime);
}

//
// Scene::FindActor					- Chapter 16, page 542
//
shared_ptr<ISceneNode> Scene::FindActor(ActorId id)
{
	SceneActorMap::iterator i = m_ActorMap.find(id);
	if (i==m_ActorMap.end())
	{
		return shared_ptr<ISceneNode>();
	}

	return i->second;
}

bool Scene::AddChild(ActorId id, shared_ptr<ISceneNode> kid)
{ 
	if (id != INVALID_ACTOR_ID)
	{
		// This allows us to search for this later based on actor id
		m_ActorMap[id] = kid;	
	}

	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(kid);
	if (pLight != NULL && m_LightManager->m_Lights.size()+1 < MAXIMUM_LIGHTS_SUPPORTED)
	{
		m_LightManager->m_Lights.push_back(pLight);
	}
	return m_Root->VAddChild(kid); 
}

bool Scene::RemoveChild(ActorId id)
{
	if (id == INVALID_ACTOR_ID)
		return false;
	shared_ptr<ISceneNode> kid = FindActor(id);
	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(kid);
	if (pLight != NULL)
	{
		m_LightManager->m_Lights.remove(pLight);
	}
	m_ActorMap.erase(id);
	return m_Root->VRemoveChild(id); 
}

void Scene::NewRenderComponentDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_New_Render_Component> pCastEventData = static_pointer_cast<EvtData_New_Render_Component>(pEventData);

    ActorId actorId = pCastEventData->GetActorId();
    shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());

    // FUTURE WORK: Add better error handling here.		
    if (FAILED(pSceneNode->VOnRestore(this)))
    {
		std::string error = "Failed to restore scene node to the scene for actorid " + ToStr(actorId);
        AC_ERROR(error);
        return;
    }

    AddChild(actorId, pSceneNode);
}

void Scene::ModifiedRenderComponentDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Modified_Render_Component> pCastEventData = static_pointer_cast<EvtData_Modified_Render_Component>(pEventData);

    ActorId actorId = pCastEventData->GetActorId();
	if (actorId == INVALID_ACTOR_ID)
	{
		AC_ERROR("Scene::ModifiedRenderComponentDelegate - unknown actor id!");
		return;
	}

	if (g_pApp->GetGameLogic()->GetState()==BGS_LoadingGameEnvironment)
		return;

	shared_ptr<ISceneNode> pSceneNode = FindActor(actorId);
    // FUTURE WORK: Add better error handling here.		
    if (!pSceneNode  || FAILED(pSceneNode->VOnRestore(this)))
    {
        AC_ERROR("Failed to restore scene node to the scene for actorid " + ToStr(actorId));
    }
}

void Scene::DestroyActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Destroy_Actor> pCastEventData = static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
    RemoveChild(pCastEventData->GetId());
}

void Scene::MoveActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Move_Actor> pCastEventData = static_pointer_cast<EvtData_Move_Actor>(pEventData);

    ActorId id = pCastEventData->GetId();
    Mat4x4 transform = pCastEventData->GetMatrix();

    shared_ptr<ISceneNode> pNode = FindActor(id);
    if (pNode)
    {
        pNode->VSetTransform(&transform);
    }
}

//
// Scene::PushAndSetMatrix			- Chapter 16, page 541
//
void Scene::PushAndSetMatrix(const Mat4x4 &toWorld)
{
    // Note this code carefully!!!!! It is COMPLETELY different
    // from some DirectX 9 documentation out there....
    // Scene::PushAndSetMatrix - Chapter 16, page 541

    m_MatrixStack->Push();
    m_MatrixStack->MultMatrixLocal(&toWorld);
    Mat4x4 mat = GetTopMatrix();
    m_Renderer->VSetWorldTransform(&mat);
}

//
// Scene::PopMatrix			- Chapter 16, page 541
//
void Scene::PopMatrix() 
{
    // Scene::PopMatrix - Chapter 16, page 541
    m_MatrixStack->Pop(); 
    Mat4x4 mat = GetTopMatrix();
    m_Renderer->VSetWorldTransform(&mat);
}

//
// Scene::GetTopMatrix			- Chapter 16, page 541
//
const Mat4x4 Scene::GetTopMatrix() 
{ 
    // Scene::GetTopMatrix - Chapter 16, page 541
    return static_cast<const Mat4x4>(*m_MatrixStack->GetTop()); 
}

//
// Scene::RenderInstances
//
void Scene::RenderInstances()
{
	while(!m_RenderableList.empty())
	{
		if(m_RenderableList.top()->VGetInstanceCount() > 0)
		{
			m_RenderableList.top()->VRender(this);
		}
		m_RenderableList.pop();
	}
	//for (std::list<StrongRenderablePtr>::iterator it = m_RenderableList.begin(); it != m_RenderableList.end(); it++)
	//{
	//	if(it->get()->VGetInstanceCount() > 0)
	//	{
	//		it->get()->VRender(this);
	//	}
	//}
}

//
// Scene::RenderAlphaPass			- Chapter 16, page 543
//
void Scene::RenderAlphaPass()
{
	shared_ptr<IRenderState> alphaPass = m_Renderer->VPrepareAlphaPass();

	m_AlphaSceneNodes.sort();
	while (!m_AlphaSceneNodes.empty())
	{
		AlphaSceneNodes::reverse_iterator i = m_AlphaSceneNodes.rbegin();
		PushAndSetMatrix((*i)->m_Concat);
		(*i)->m_pNode->VRender(this);
		delete (*i);
		PopMatrix();
		m_AlphaSceneNodes.pop_back();
	}
}

//********** Renderable stuff *************
//
// Scene::GetRenderable
//
StrongRenderablePtr Scene::GetRenderable(const std::string name)
{
	if(!m_RenderableMap.empty())
	{
		auto findIt = m_RenderableMap.find(name);
        if (findIt != m_RenderableMap.end())
        {
            return findIt->second;
        }
	}
	return nullptr;
}

//
// Scene::AddRenderable
//
void Scene::AddRenderable(const std::string name, StrongRenderablePtr Renderable)
{
	m_RenderableMap.insert(std::pair<std::string, StrongRenderablePtr>(name, Renderable));
}

//
// Scene::SetRenderable
//
void Scene::SetRenderable(const std::string name)
{
	StrongRenderablePtr srp = GetRenderable(name);
	if(srp != nullptr)
	{
		if(!srp->VIsRendering())
		{
			m_RenderableList.push(srp);
			srp->VSetRendering(true);
		}
	}
}