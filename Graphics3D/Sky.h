#pragma once
//========================================================================
// Sky.h - implements a sky box in either D3D9 or D3D11
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
#include "Material.h"
#include "Shaders.h"

// Forward declarations
class SceneNode;
class Scene;

////////////////////////////////////////////////////
//
// class SkyNode					- Chapter 16, page 554
//
//    Implements a believable sky that follows
//	  the camera around - this is a base class that the D3D9 and D3D11 classes 
//    inherit from
//
////////////////////////////////////////////////////
class SkyNode : public SceneNode
{
protected:
	DWORD					m_numVerts;
	DWORD					m_sides;
	const char *			m_textureBaseName;
	shared_ptr<CameraNode>	m_camera;
	bool					m_bActive;

	std::string GetTextureName(const int side);

public:
	SkyNode(const char *textureFile);
	virtual ~SkyNode() { }
	HRESULT VPreRender(Scene *pScene);
	bool VIsVisible(Scene *pScene) const { return m_bActive; }
};

//class D3DSkyNode9 : public SkyNode
//{
//protected:
//	LPDIRECT3DTEXTURE9		m_pTexture[5];		// the sky textures
//	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			// the sky verts
//
//public:
//	D3DSkyNode9(const char *pTextureBaseName );
//	virtual ~D3DSkyNode9();
//	HRESULT VOnRestore(Scene *pScene);
//	HRESULT VRender(Scene *pScene);
//};

class D3DSkyNode11 : public SkyNode
{
public:
	D3DSkyNode11(const char *pTextureBaseName);
	virtual ~D3DSkyNode11();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);

protected:
	Alpha_D3D11VertexPosTextureBuffer	m_D3D11VertexBuffer;
	Alpha_D3D11PosTextureSkyVShader		m_D3D11VertexShader;
	Alpha_D3D11TexturePixelShader		m_D3D11PixelShader;
};

