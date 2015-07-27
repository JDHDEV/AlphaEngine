#pragma once
//========================================================================
// File: Scene.h - implements the container class for 3D Graphics scenes
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
#include "Geometry.h"
#include "SceneNodes.h"

// Forward declarations
////////////////////////////////////////////////////
//
// SceneActorMap Description                            - Chapter 16, page Y
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////
typedef std::map<ActorId, shared_ptr<ISceneNode> > SceneActorMap;

////////////////////////////////////////////////////
//
// Scene Description                                            - Chapter 16, page 536
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////
class CameraNode;
class SkyNode;
class LightNode;
class LightManager;

class Scene
{
protected:
    shared_ptr<SceneNode>   m_Root;
    shared_ptr<CameraNode>  m_Camera;
    shared_ptr<IRenderer>   m_Renderer;

    ID3DXMatrixStack        *m_MatrixStack;
    AlphaSceneNodes         m_AlphaSceneNodes;
    SceneActorMap           m_ActorMap;

    LightManager            *m_LightManager;

	std::stack<StrongRenderablePtr> m_RenderableList;

	void RenderInstances();
    void RenderAlphaPass();

//********** Renderable stuff *************
	std::map <std::string, StrongRenderablePtr> m_RenderableMap;
public:
	StrongRenderablePtr GetRenderable(std::string name);
	void AddRenderable(const std::string name, StrongRenderablePtr Renderable);
	void SetRenderable(const std::string name);
	//void ModifyActor(StrongActorPtr pActor, TiXmlElement* overrides);

//protected:
    // [rez] This function can be overridden by a subclass so you can create game-specific C++ components.  If you do
    // this, make sure you call the base-class version first.  If it returns NULL, you know it's not an engine component.
    //virtual StrongActorComponentPtr VCreateComponent(TiXmlElement* pData);

//private:
    //ActorId GetNextActorId(void) { ++m_lastActorId; return m_lastActorId; }

public:
	Scene(shared_ptr<IRenderer> renderer);
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate(const int deltaMilliseconds);
	shared_ptr<ISceneNode> FindActor(ActorId id);
	bool AddChild(ActorId id, shared_ptr<ISceneNode> kid);
	bool RemoveChild(ActorId id);

    // event delegates
    void NewRenderComponentDelegate(IEventDataPtr pEventData);
    void ModifiedRenderComponentDelegate(IEventDataPtr pEventData); // added post-press!
    void DestroyActorDelegate(IEventDataPtr pEventData);
    void MoveActorDelegate(IEventDataPtr pEventData);

    void SetCamera(shared_ptr<CameraNode> camera) { m_Camera = camera; }
    const shared_ptr<CameraNode> GetCamera() const { return m_Camera; }

    void PushAndSetMatrix(const Mat4x4 &toWorld);
    void PopMatrix();
    const Mat4x4 GetTopMatrix();

    LightManager *GetLightManager() { return m_LightManager; }

    void AddAlphaSceneNode(AlphaSceneNode *asn) { m_AlphaSceneNodes.push_back(asn); }

    HRESULT Pick(RayCast *pRayCast) { return m_Root->VPick(this, pRayCast); }

    shared_ptr<IRenderer> GetRenderer() { return m_Renderer; }
};