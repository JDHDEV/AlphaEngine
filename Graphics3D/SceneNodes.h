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
#include "Shaders.h"
#include "../DirectX11Wrapper/DirectX11Vertex.h"
#include "../DirectX11Wrapper/DirectX11Pixel.h"
//#include "../DirectX11Wrapper/DirectX11Renderable.h"

// Forward declarations
class SceneNode;
class Scene;
class OctTreeNode;
class RayCast;
class MovementController;
class IResourceExtraData;
class ActorComponent;
class BaseRenderComponent;
class BoundingBox;
class BoundingSphere;

// FUTURE WORK - Smart pointers don't work right....going to use a naked pointer for now!
typedef BaseRenderComponent* WeakBaseRenderComponentPtr;

////////////////////////////////////////////////////
//
// RenderableInstance
//
//	A list of scene nodes that need to be drawn as an instance;
//
////////////////////////////////////////////////////
struct RenderableInstance
{
    Mat4x4 m_World;
};

////////////////////////////////////////////////////
//
// AlphaType					- Chapter X, page Y
//
//   This enum defines the different types of alpha blending
//   types that can be set on a scene node.
//
//	
////////////////////////////////////////////////////
enum AlphaType // not to be confused with ALphaEngine!!
{
	AlphaOpaque,
	AlphaTexture,
	AlphaMaterial,
	AlphaVertex
};

//////////////////////////////////////////////////////////////
//
// BoundingBox
//
//   Implements ...
//
//////////////////////////////////////////////////////////////
enum ContainmentType
{
	Alpha_Contains,	//Indicates that one bounding volume completely contains the other.
	Alpha_Disjoint,	//Indicates there is no overlap between the bounding volumes.
	Alpha_Intersects	//Indicates that the bounding volumes partially overlap.
};

class BoundingBox
{
public:
	Vec3 m_Center; // Center of a cubic bounding volume
    float m_Radius; // Radius of a cubic bounding volume

public:
	BoundingBox();
	BoundingBox(Vec3 min, Vec3 max);

	void Set(Vec3 min, Vec3 max);

	ContainmentType Contains(BoundingBox* pBoundingBox);
	ContainmentType Contains(BoundingBox boundingBox);
	ContainmentType Contains(BoundingSphere* pBoundingSphere);
	ContainmentType Contains(BoundingSphere boundingSphere);

	Vec3 MaxVec3();
	Vec3 MinVec3();
	Vec3 Dimensions();
};

class BoundingSphere
{
public:
	float m_Radius;
	Vec3 m_Center;

public:
	BoundingSphere();
	BoundingSphere(Vec3 min, Vec3 max);
};

//////////////////////////////////////////////////////////////////////
//  class SceneNodeProperties	- Chapter 16, page 527
//
//   This class is contained in the SceneNode class, and gains
//   easy access to common scene node properties such as its ActorId
//   or render pass, with a single ISceneNode::VGet() method.
//
//////////////////////////////////////////////////////////////////////
class SceneNodeProperties
{
	friend class SceneNode;
	friend class OctTreeNode;

protected:
	ActorId                 m_ActorId;
	std::string				m_Name;
	Mat4x4					m_ToWorld, m_FromWorld;
	float					m_Radius;//remove and use shphere
	RenderPass				m_RenderPass;
	Material				m_Material;
	AlphaType				m_AlphaType;

	void SetAlpha(const float alpha) 
		{ m_AlphaType=AlphaMaterial; m_Material.SetAlpha(alpha); } 

	// from the OctTree
	bool HasMoved; // TODO: updated when they do actully move
	bool HasBoundingBox;
	BoundingBox m_BoundingBox;
	BoundingSphere m_BoundingSphere;

public:
    SceneNodeProperties(void);
	const ActorId &ActorId() const { return m_ActorId; }
	Mat4x4 const &ToWorld() const { return m_ToWorld; }
	Mat4x4 const &FromWorld() const { return m_FromWorld; }
	void Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const;

	const char * Name() const { return m_Name.c_str(); }		

	bool HasAlpha() const { return m_Material.HasAlpha(); }						
	float Alpha() const { return m_Material.GetAlpha(); }
	AlphaType AlphaType() const { return m_AlphaType; }

	RenderPass RenderPass() const { return m_RenderPass; }
	float Radius() const { return m_Radius; }

	Material GetMaterial() const { return m_Material; }
};

//////////////////////////////////////////////////////////////
//
// SceneNodeList						- Chapter 16, page 529
//
//   Every scene node has a list of its children - this 
//   is implemented as a vector since it is expected that
//   we won't add/delete nodes very often, and we'll want 
//   fast random access to each child.
//
//////////////////////////////////////////////////////////////
typedef std::vector<shared_ptr<ISceneNode>> SceneNodeList;

//////////////////////////////////////////////////////////////
//
// SceneNode							- Chapter 16, page 529
//
//   Implements ISceneNode. Forms the base class for any node
//   that can exist in the 3D scene graph managed by class Scene.
//
//////////////////////////////////////////////////////////////
class SceneNode : public ISceneNode
{
	friend class Scene;

protected:

	SceneNodeList			m_Children;
	SceneNode				*m_pParent;
	SceneNodeProperties		m_Props;
	WeakBaseRenderComponentPtr	m_RenderComponent;

public:
	SceneNode(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const Mat4x4 *to, const Mat4x4 *from=NULL);

	virtual ~SceneNode();

	virtual const SceneNodeProperties* const VGet() const { return &m_Props; }
	virtual BoundingBox* VGetBox() { return nullptr; }
	virtual BoundingSphere* VGetSphere() { return &m_Props.m_BoundingSphere; }
	virtual bool VHasMoved() const { return m_Props.HasMoved; }

	virtual void VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld=NULL);
	virtual HRESULT VOnRestore(Scene *pScene);

	virtual HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);

	virtual HRESULT VPreRender(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const;
	virtual HRESULT VRender(Scene *pScene) { return S_OK; } 
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual HRESULT VPostRender(Scene *pScene);

	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual bool VRemoveChild(ActorId id);
	virtual HRESULT VOnLostDevice(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	void SetAlpha(float alpha);
	float GetAlpha() const { return m_Props.Alpha(); }

	Vec3 GetPosition() const { return m_Props.m_ToWorld.GetPosition(); }
	void SetPosition(const Vec3 &pos) { m_Props.m_ToWorld.SetPosition(pos); }

	const Vec3 GetWorldPosition() const;					// [mrmike] added post-press to respect ancestor's position 

	Vec3 GetDirection() const { return m_Props.m_ToWorld.GetDirection(); }

	void SetRadius(const float radius) { m_Props.m_Radius = radius; }
	void SetMaterial(const Material &mat) { m_Props.m_Material = mat; }
};

//
// class D3DSceneNode9						- not described in the book // remove later?
// class D3DSceneNode11						- not described in the book
//
// The book only describes a D3D11 render implementations. The engine code implements D3D9 as well as 
//    D3D11, by creating different renderers and scene nodes based on the settings of PlayerOptions.xml.  
//
class D3DSceneNode9 : public SceneNode
{
public:
	D3DSceneNode9(const ActorId actorId, 
	WeakBaseRenderComponentPtr renderComponent,
	RenderPass renderPass, 
	const Mat4x4 *t)
	 : SceneNode(actorId, renderComponent, renderPass, t) { }

	virtual HRESULT VRender(Scene *pScene);
};

class D3DSceneNode11 : public SceneNode
{
public:
	virtual HRESULT VRender(Scene *pScene) { return S_OK; }
};


////////////////////////////////////////////////////
//
// AlphaSceneNode Description						- Chapter 16, page 535
// AlphaSceneNodes Description						- Chapter 16, page 535
//
// A list of scene nodes that need to be drawn in the alpha pass;
// the list is defined as an STL list
////////////////////////////////////////////////////
struct AlphaSceneNode
{
	shared_ptr<ISceneNode> m_pNode;
	Mat4x4 m_Concat;
	float m_ScreenZ;

	// For the STL sort...
	bool const operator <(AlphaSceneNode const &other) { return m_ScreenZ < other.m_ScreenZ; }
};

typedef std::list<AlphaSceneNode *> AlphaSceneNodes;

////////////////////////////////////////////////////
//
// SceneActorMap Description
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////
typedef std::map<ActorId, shared_ptr<ISceneNode>> SceneActorMap;

////////////////////////////////////////////////////
//
// Scene Description
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////
class CameraNode;
class SkyNode;

////////////////////////////////////////////////////
//
// class RootNode					- Chapter 16, page 545
//
//    This is the root node of the scene graph.
//
////////////////////////////////////////////////////
class RootNode : public SceneNode
{
public:
	RootNode();
	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual bool VRemoveChild(ActorId id);
	virtual bool VIsVisible(Scene *pScene) const { return true; }
};

////////////////////////////////////////////////////
//
// class CameraNode					- Chapter 16, page 548
//
//    A camera node controls the D3D view transform and holds the view
//    frustum definition
//
////////////////////////////////////////////////////
class CameraNode : public SceneNode
{
public:
	CameraNode(Mat4x4 const *t, Frustum const &frustum) 
	 : SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_0, t),
	   m_Frustum(frustum),
	   m_bActive(true),
	   m_DebugCamera(false),
	   m_pTarget(shared_ptr<SceneNode>()),
	   m_CamOffsetVector( 0.0f, 1.0f, -10.0f, 0.0f )
	{ 
	}

	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const { return m_bActive; }

	const Frustum &GetFrustum() { return m_Frustum; }
	void SetTarget(shared_ptr<SceneNode> pTarget)
		{ m_pTarget = pTarget; }
	void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }
	shared_ptr<SceneNode> GetTarget() { return m_pTarget; }

	Mat4x4 GetWorldViewProjection(Scene *pScene);
	HRESULT SetViewTransform(Scene *pScene);

	Mat4x4 GetProjection() { return m_Projection; }
	Mat4x4 GetView() { return m_View; }

	void SetCameraOffset( const Vec4 & cameraOffset )
	{
		m_CamOffsetVector = cameraOffset;
	}

protected:

	Frustum			m_Frustum;
    Mat4x4			m_Projection;
	Mat4x4			m_View;
	bool			m_bActive;
	bool			m_DebugCamera;
	shared_ptr<SceneNode> m_pTarget;
	Vec4			m_CamOffsetVector;	//Direction of camera relative to target.
};
/*
////////////////////////////////////////////////////
//
// D3DGrid9 Description
//
//    A slightly modified version of Grid from 
//       the beginning 3D chapter
//
////////////////////////////////////////////////////

class D3DGrid9 : public SceneNode
{
protected:
	shared_ptr<ResHandle>	m_handle;			// the resource handle for the grid texture for the axes planes
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			// the grid verts
	LPDIRECT3DINDEXBUFFER9	m_pIndices;			// the grid index
	DWORD					m_numVerts;
	DWORD					m_numPolys;
//    std::string             m_textureResource;
//    int                     m_squares;

public:
	bool					m_bTextureHasAlpha;

    D3DGrid9(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, /* const std::string& name, const char* textureResource, int squares, const Color &diffuseColor, *//* const Mat4x4* pMatrix);
	virtual ~D3DGrid9();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	bool VHasAlpha() const { return m_bTextureHasAlpha; }
};
*/
class D3DGrid11 : public SceneNode
{
protected:
	DWORD					m_numVerts;
	DWORD					m_numPolys;

	//int                     m_squares;

	ID3D11Buffer*               m_pIndexBuffer;
	ID3D11Buffer*               m_pVertexBuffer;

	Alpha_Hlsl_VertexShader		m_VertexShader;
	Alpha_Hlsl_PixelShader		m_PixelShader;

public:
	bool					m_bTextureHasAlpha;
	
    D3DGrid11(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, /* const std::string& name, const char* textureResource, int squares, const Color &diffuseColor, */ const Mat4x4* pMatrix);
	virtual ~D3DGrid11();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnUpdate(Scene *pScene, DWORD const elapsedMs) { return S_OK; }
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast) { return E_FAIL; }

	bool VHasAlpha() const { return m_bTextureHasAlpha; }
};

class BrickNode : public SceneNode
{
protected:
	DWORD							m_numVerts;
	DWORD							m_numPolys;

	ID3D11Buffer*					m_pIndexBuffer;
	ID3D11Buffer*					m_pVertexBuffer;

	//Alpha_Hlsl_VertexShader			m_VertexShader;
	//Alpha_Hlsl_PixelShader			m_PixelShader;
	//Alpha_Hlsl_LineShader			m_LineShader;

	//Alpha_D3D11VertexPosBuffer		m_D3D11VertexBuffer;
	//Alpha_D3D11VertexPosShader		m_D3D11VertexShader;
	//Alpha_D3D11BluePixelShader		m_D3D11PixelShader;

	Alpha_D3D11VertexPosTextureBuffer	m_D3D11VertexBuffer;
	Alpha_D3D11VertexPosTextureShader	m_D3D11VertexShader;
	Alpha_D3D11TexturePixelShader		m_D3D11PixelShader;

	//Alpha_D3D11VertexPosTextureBuffer	m_D3D11VertexBuffer;
	//Alpha_D3D11PosTextureSkyVShader		m_D3D11VertexShader;
	//Alpha_D3D11TexturePixelShader		m_D3D11PixelShader;

	//Alpha_D3D11VertexPosNormalTextureBuffer	m_D3D11VertexBuffer;
	//Alpha_D3D11VertexPosNormalTextureShader	m_D3D11VertexShader;
	//Alpha_D3D11NormalTexturePixelShader		m_D3D11PixelShader;

public:
	bool					m_bTextureHasAlpha;
	
    BrickNode(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const Mat4x4* pMatrix);
	virtual ~BrickNode();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnUpdate(Scene *pScene, DWORD const elapsedMs) { return S_OK; }
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast) { return E_FAIL; }

	bool VHasAlpha() const { return m_bTextureHasAlpha; }
};

class BrickNodeInstance : public SceneNode
{
protected:
	StrongRenderablePtr      	m_pRenderable;
	RenderableInstance			m_RenderableInstance;

public:
	bool						m_bTextureHasAlpha;
	
    BrickNodeInstance(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const Mat4x4* pMatrix);
	virtual ~BrickNodeInstance();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnUpdate(Scene *pScene, DWORD const elapsedMs) { return S_OK; }
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast) { return E_FAIL; }

	bool VHasAlpha() const { return m_bTextureHasAlpha; }
};
/*
////////////////////////////////////////////////////
//
// ArrowNode Description
//
//   Graphical object that looks like an arrow - they
//     aren't discussed in the book per se but you see them
//     in a lot of the figures!
//
////////////////////////////////////////////////////

class ArrowNode : public SceneNode
{
protected:
	ID3DXMesh *m_shaft;
	ID3DXMesh *m_cone;
	Mat4x4 m_coneTrans;
	Mat4x4 m_shaftTrans;
	
public:
	ArrowNode(std::string name, WeakBaseRenderComponentPtr renderComponent, const float length, const Mat4x4 *t, const Color &color);

	virtual ~ArrowNode() { SAFE_RELEASE(m_shaft); SAFE_RELEASE(m_cone); }
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
};


////////////////////////////////////////////////////
//
// TestObject Description
//
//    Implements a test object that has no symmetry on X, Y, and Z axes.
//    Really useful to make sure you haven't flipped left-handed versus
//    right handed coordinate systems or something like that
//
////////////////////////////////////////////////////

//class TriangleIterator;
//struct TestObjectParams;


class TestObject : public SceneNode
{
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			
	DWORD					m_numVerts;
	DWORD					m_numPolys;

	DWORD					m_color;
	bool					m_squashed;

public:
	//TestObject(TestObjectParams const &p, std::string name);
    TestObject(std::string name);
	virtual ~TestObject();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);

//	TriangleIterator *CreateTriangleIterator();

	static WORD g_TestObjectIndices[][3];
	static Vec3 g_CubeVerts[];
	static Vec3 g_SquashedCubeVerts[];
};
*/