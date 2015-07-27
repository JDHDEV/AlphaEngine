#pragma once
//========================================================================
// SceneNodes.h - defines the base class for the 3D graphics scene graph, and
//                  derived classes like RootNode, Alpha scene nodes, Camera, Grids, etc.                  
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//=======================================================================
#include "Geometry.h"
#include "Material.h"
//#include "Shaders.h"
#include "SceneNodes.h"
//#include "../DirectX11Wrapper/DirectX11Vertex.h"
//#include "../DirectX11Wrapper/DirectX11Pixel.h"
//#include "../DirectX11Wrapper/DirectX11Renderable.h"

//#include <DirectXCollision.h>

// Forward declarations
//class SceneNode;
//class Scene;
class OctTreeNode;

//////////////////////////////////////////////////////////////
//
// OctTreeNodeArray						- Chapter 16, page 529
//
//   Every scene node has a list of its children - this 
//   is implemented as a vector since it is expected that
//   we won't add/delete nodes very often, and we'll want 
//   fast random access to each child.
//
//////////////////////////////////////////////////////////////
typedef std::array<shared_ptr<OctTreeNode>,8> OctTreeNodeArray;
typedef shared_ptr<OctTreeNode> OctTreeNodePtr;

typedef std::list<shared_ptr<ISceneNode>> LeafObjectList;
typedef std::queue<shared_ptr<ISceneNode>> PhysicalObjectQueue;
typedef shared_ptr<ISceneNode> PhysicalObjectPtr;

//////////////////////////////////////////////////////////////////////
//  class OctTreeNodeProperties
//
//   This class is contained in the OctTreeNode class
//
//////////////////////////////////////////////////////////////////////
class OctTreeNodeProperties
{
	friend class OctTreeNode;

protected:
	Mat4x4			m_ToWorld, m_FromWorld;
	RenderPass		m_RenderPass;
	bool			HasMoved; // TODO: updated when they do actully move
	bool			HasBoundingBox;
	BoundingBox		m_BoundingBox;
	BoundingSphere	m_BoundingSphere;// properly initialize the sphere

public:
    OctTreeNodeProperties(void);
	Mat4x4 const &ToWorld() const { return m_ToWorld; }
	Mat4x4 const &FromWorld() const { return m_FromWorld; }
	void Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const;

	RenderPass RenderPass() const { return m_RenderPass; }
	float Radius() const { return m_BoundingSphere.m_Radius; }
};

//////////////////////////////////////////////////////////////
//
// OctTreeNode
//
//   Implements ...
//
//////////////////////////////////////////////////////////////
class OctTreeNode : public ISceneNode
{
	//friend class Scene;
public:

	static bool m_TreeReady;       //the tree has a few objects which need to be inserted before it is complete
	static bool m_TreeBuilt;       //there is no pre-existing tree yet.

protected:
	LeafObjectList m_Objects;	
	
	// These are items which we're waiting to insert into the data structure. 
	// We want to accrue as many objects in here as possible before we inject them into the tree. This is slightly more cache friendly.
	static PhysicalObjectQueue m_PendingInsertion;

	OctTreeNodeArray		m_ChildNodes;
    OctTreeNodeProperties	m_Props;
	OctTreeNodePtr			m_pParent;

	// This is a bitmask indicating which child nodes are actively being used.
	// It adds slightly more complexity, but is faster for performance since there is only one comparison instead of 8.
	byte m_ActiveNodes;

	// The minumum size for enclosing region is a 1x1x1 cube.
	const int MIN_SIZE;

	// this is how many frames we'll wait before deleting an empty tree branch. Note that this is not a constant. The maximum lifespan doubles
	// every time a node is reused, until it hits a hard coded constant of 64
	int m_MaxLifespan;
	int m_CurLife;            //this is a countdown time showing how much time we have left to live

public:
	OctTreeNode(BoundingBox region, LeafObjectList objList);
	OctTreeNode(BoundingBox region);
	OctTreeNode();
	//OctTreeNode(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const Mat4x4 *to, const Mat4x4 *from=NULL);

	virtual ~OctTreeNode();

	virtual const SceneNodeProperties* const VGet() const { return nullptr; }
	virtual BoundingBox* VGetBox() { return &m_Props.m_BoundingBox; }
	virtual BoundingSphere* VGetSphere() { return &m_Props.m_BoundingSphere; }
	virtual bool VHasMoved() const { return false; }

	OctTreeNodePtr CreateNode(BoundingBox region, LeafObjectList objList);
	OctTreeNodePtr CreateNode(BoundingBox region, PhysicalObjectPtr Item);

	virtual void VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld=NULL);
	virtual HRESULT VOnRestore(Scene *pScene);

	virtual HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);

	virtual HRESULT VPreRender(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const;
	const Vec3 GetWorldPosition() const;

	virtual HRESULT VRender(Scene *pScene) { return S_OK; } 
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual HRESULT VPostRender(Scene *pScene);

	bool VAddChild(PhysicalObjectPtr node);
	virtual bool VRemoveChild(ActorId id);
	virtual HRESULT VOnLostDevice(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	Vec3 GetPosition() const { return m_Props.m_ToWorld.GetPosition(); }
	void SetPosition(const Vec3 &pos) { m_Props.m_ToWorld.SetPosition(pos); }

private:

	// Internal helper functions
	void ProcessesInsertions();
	void BuildTree();
	void FindEnclosingBox();
	void FindEnclosingCube();
};

////////////////////////////////////////////////////
//
// class RootOctNode
//
//    Interchanable with RootNode but will implement the octree
//
////////////////////////////////////////////////////
class RootOctNode : public SceneNode
{
public:
	RootOctNode();
	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual bool VRemoveChild(ActorId id);
	virtual bool VIsVisible(Scene *pScene) const { return true; }
};
