#pragma once
//========================================================================
// DirectX11Renderable.h - Defines the base DirectX11Renderable class which 
// contains all the base information to draw instances of renderable objects
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
//#include "Geometry.h"
//#include "Material.h"
//#include "Shaders.h"
//#include "../DirectX11Wrapper/DirectX11Vertex.h"
//#include "../DirectX11Wrapper/DirectX11Pixel.h"


//// Forward declarations
class DirectX11Renderable;
//class SceneNode;
//class Scene;
//class RayCast;
//class MovementController;
//class IResourceExtraData;
//class ActorComponent;
class BaseRenderComponent; 

// FUTURE WORK - Smart pointers don't work right....going to use a naked pointer for now!
typedef BaseRenderComponent* WeakBaseRenderComponentPtr;

//////////////////////////////////////////////////////////////////////
//  class RenderableProperties
//
//////////////////////////////////////////////////////////////////////
class RenderableProperties
{
	friend class DirectX11Renderable;

protected:
	std::string				m_Name;
	RenderPass				m_RenderPass;
	Material				m_Material;
	AlphaType				m_AlphaType;

	void SetAlpha(const float alpha) 
		{ m_AlphaType=AlphaMaterial; m_Material.SetAlpha(alpha); } 

public:
    RenderableProperties(void);

	const char * Name() const { return m_Name.c_str(); }		

	bool HasAlpha() const { return m_Material.HasAlpha(); }						
	float Alpha() const { return m_Material.GetAlpha(); }
	AlphaType AlphaType() const { return m_AlphaType; }

	RenderPass RenderPass() const { return m_RenderPass; }

	Material GetMaterial() const { return m_Material; }
};

//////////////////////////////////////////////////////////////
//
// InstanceList					
//
//	Every update the scene will populate this list with all
//	instances of this object to be draw in one pass later.
//
//////////////////////////////////////////////////////////////
typedef std::stack<RenderableInstance*> InstanceList;

//////////////////////////////////////////////////////////////
//
// DirectX11Renderable
//
//   Implements ISceneNode. Forms the base class for any node
//   contains all the base information to draw instances of renderable objects
//
//////////////////////////////////////////////////////////////
class DirectX11Renderable : public IRenderable
{
	friend class Scene;

protected:
	RenderableProperties			m_Props;
	InstanceList					m_InstanceList;
	WeakBaseRenderComponentPtr		m_RenderComponent;
	UINT							m_instanceCount;
	bool							m_IsRendering;

public:
	DirectX11Renderable(WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass);
	virtual ~DirectX11Renderable();

	virtual bool VAddInstance(RenderableInstance* instance);

	virtual const RenderableProperties* const VGet() const { return &m_Props; }
	virtual int VGetInstanceCount() { return m_instanceCount; }
	virtual bool VIsRendering() { return m_IsRendering; }
	virtual void VSetRendering(bool rendering) { m_IsRendering = rendering; }
};

class BrickRenderable : public DirectX11Renderable
{
protected:
	DWORD							m_numVerts;
	DWORD							m_numPolys;

	ID3D11Buffer*					m_pVertexBuffer;
	ID3D11Buffer*					m_pIndexBuffer;
	ID3D11Buffer*					m_pInstanceBuffer;
	ID3D11Buffer*					m_pcbInstanceMatrices;

	Alpha_D3D11VertexPosTextureBuffer	m_D3D11VertexBuffer;
	Alpha_DX11VS_PosTextureInstance		m_D3D11VertexShader;
	Alpha_D3D11TexturePixelShader		m_D3D11PixelShader;

public:
	bool					m_bTextureHasAlpha;
	
    BrickRenderable(WeakBaseRenderComponentPtr renderComponent);
	virtual ~BrickRenderable();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);

	bool VHasAlpha() const { return m_bTextureHasAlpha; }
};
