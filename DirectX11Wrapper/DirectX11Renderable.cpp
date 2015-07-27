//========================================================================
//  DirectX11Renderable.cpp - Defines the base DirectX11Renderable class which 
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
#include "AlphaStd.h"

#include "../Alpha/Alpha.h"
#include "../Actors/ActorComponent.h"
#include "../Actors/RenderComponent.h"
#include "../Actors/TransformComponent.h"

#include "../Graphics3d/D3DRenderer.h"
//#include "Geometry.h"
//#include "Lights.h"
//#include "Mesh.h"
//#include "Raycast.h"
#include "../Graphics3D/SceneNodes.h"
#include "DirectX11Renderable.h"
//#include "Shaders.h"
#include "tchar.h"
#include "../ResourceCache/ResCache.h"

#include "../DirectX11Wrapper/DirectX11Vertex.h"
#include "../DirectX11Wrapper/DirectX11Pixel.h"

#include "../Alpha/interfaces.h"

#include <xnamath.h>

////////////////////////////////////////////////////
// RenderableProperties Implementation
////////////////////////////////////////////////////
RenderableProperties::RenderableProperties(void)
{
    m_RenderPass = RenderPass_0;
    m_AlphaType = AlphaOpaque;
}

////////////////////////////////////////////////////
// DirectX11Renderable Implementation
////////////////////////////////////////////////////
DirectX11Renderable::DirectX11Renderable(WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass) 
{ 
	m_Props.m_Name = (renderComponent) ? renderComponent->VGetName() : "Renderable";
	m_Props.m_RenderPass = renderPass;
	m_RenderComponent = renderComponent;
	m_instanceCount = 0;
	m_IsRendering = false;
}

DirectX11Renderable::~DirectX11Renderable()
{
}

bool DirectX11Renderable::VAddInstance(RenderableInstance* instance)
{
	m_InstanceList.push(instance);
	m_instanceCount = m_InstanceList.size();
	return true;
}

////////////////////////////////////////////////////
// BrickRenderable Implementation
////////////////////////////////////////////////////
BrickRenderable::BrickRenderable(WeakBaseRenderComponentPtr renderComponent)
    : DirectX11Renderable(renderComponent, RenderPass_Actor)
{
	m_bTextureHasAlpha = false;
	m_numVerts = m_numPolys = 0;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pInstanceBuffer = nullptr;
}

BrickRenderable::~BrickRenderable()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);
}

HRESULT BrickRenderable::VOnRestore(Scene *pScene)
{
	HRESULT hr;

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);

	V_RETURN (m_D3D11VertexShader.OnRestore(pScene));
	V_RETURN (m_D3D11PixelShader.OnRestore(pScene));

	BrickRenderComponent* brc = static_cast<BrickRenderComponent*>(m_RenderComponent);

	float width = brc->GetWidth();
	float length = brc->GetLength();
	float height = brc->GetHeight();

	// Create the vertex buffer
	m_numVerts = 24;
	m_numPolys = 12;

	// create Pos vectors centered
	Vec3 PosA(-length/2.0f, height/2.0f, -width/2.0f);
	Vec3 PosB(length/2.0f, height/2.0f, -width/2.0f);
	Vec3 PosC(-length/2.0f, -height/2.0f, -width/2.0f);
	Vec3 PosD(length/2.0f, -height/2.0f, -width/2.0f);
	Vec3 PosE(length/2.0f, height/2.0f, width/2.0f);
	Vec3 PosF(-length/2.0f, height/2.0f, width/2.0f);
	Vec3 PosG(length/2.0f,-height/2.0f, width/2.0f);
	Vec3 PosH(-length/2.0f, -height/2.0f, width/2.0f);

	// create Normal vectors
	Vec3 NormalA(0.0f, 0.0f, -1.0f);
	Vec3 NormalB(0.0f, 1.0f, 0.0f);
	Vec3 NormalC(-1.0f, 0.0f, 0.0f);
	Vec3 NormalD(1.0f, 0.0f, 0.0f);
	Vec3 NormalE(0.0f, -1.0f, 0.0f);
	Vec3 NormalF(0.0f, 0.0f, 1.0f);

	// create Texture vectors
	Vec2 UvA(0.0f, 1.0f);
	Vec2 UvB(1.0f, 0.0f);
	Vec2 UvC(0.0f, 1.0f);
	Vec2 UvD(1.0f, 1.0f);

	D3D11VertexPosTexture *m_Vertices = AC_NEW D3D11VertexPosTexture[m_numVerts];

	// create Color vectors
	Vec4 Color(0.0f,1.0f,1.0f,1.0f);

	m_Vertices[0] = D3D11VertexPosTexture(PosA, UvA);
	m_Vertices[1] = D3D11VertexPosTexture(PosB, UvB);
	m_Vertices[2] = D3D11VertexPosTexture(PosC, UvC);
	m_Vertices[3] = D3D11VertexPosTexture(PosD, UvD);
	m_Vertices[4] = D3D11VertexPosTexture(PosF, UvA);
	m_Vertices[5] = D3D11VertexPosTexture(PosE, UvB);
	m_Vertices[6] = D3D11VertexPosTexture(PosA, UvC);
	m_Vertices[7] = D3D11VertexPosTexture(PosB, UvD);
	m_Vertices[8] = D3D11VertexPosTexture(PosF, UvA);
	m_Vertices[9] = D3D11VertexPosTexture(PosA, UvB);
	m_Vertices[10] = D3D11VertexPosTexture(PosH, UvC);
	m_Vertices[11] = D3D11VertexPosTexture(PosC, UvD);
	m_Vertices[12] = D3D11VertexPosTexture(PosB, UvA);
	m_Vertices[13] = D3D11VertexPosTexture(PosE, UvB);
	m_Vertices[14] = D3D11VertexPosTexture(PosD, UvC);
	m_Vertices[15] = D3D11VertexPosTexture(PosG, UvD);
	m_Vertices[16] = D3D11VertexPosTexture(PosC, UvA);
	m_Vertices[17] = D3D11VertexPosTexture(PosD, UvB);
	m_Vertices[18] = D3D11VertexPosTexture(PosH, UvC);
	m_Vertices[19] = D3D11VertexPosTexture(PosG, UvD);
	m_Vertices[20] = D3D11VertexPosTexture(PosH, UvA);
	m_Vertices[21] = D3D11VertexPosTexture(PosG, UvB);
	m_Vertices[22] = D3D11VertexPosTexture(PosF, UvC);
	m_Vertices[23] = D3D11VertexPosTexture(PosE, UvD);

	WORD *m_Indices = AC_NEW WORD[m_numPolys * 3];

	m_Indices[0] = 0;
	m_Indices[1] = 1;
	m_Indices[2] = 2;
	m_Indices[3] = 1;
	m_Indices[4] = 3;
	m_Indices[5] = 2;

	m_Indices[6] = 4;
	m_Indices[7] = 5;
	m_Indices[8] = 6;
	m_Indices[9] = 5;
	m_Indices[10] = 7;
	m_Indices[11] = 6;

	m_Indices[12] = 8;
	m_Indices[13] = 9;
	m_Indices[14] = 10;
	m_Indices[15] = 9;
	m_Indices[16] = 11;
	m_Indices[17] = 10;

	m_Indices[18] = 12;
	m_Indices[19] = 13;
	m_Indices[20] = 14;
	m_Indices[21] = 13;
	m_Indices[22] = 15;
	m_Indices[23] = 14;

	m_Indices[24] = 16;
	m_Indices[25] = 17;
	m_Indices[26] = 18;
	m_Indices[27] = 17;
	m_Indices[28] = 19;
	m_Indices[29] = 18;

	m_Indices[30] = 20;
	m_Indices[31] = 21;
	m_Indices[32] = 22;
	m_Indices[33] = 21;
	m_Indices[34] = 23;
	m_Indices[35] = 22;

	V_RETURN (m_D3D11VertexBuffer.OnRestore(m_Vertices, m_numVerts, m_Indices, m_numPolys));

	SAFE_DELETE_ARRAY(m_Vertices);
	SAFE_DELETE_ARRAY(m_Indices);

	return hr;
}

HRESULT BrickRenderable::VRender(Scene *pScene)
{
	HRESULT hr;

	//DirectX11RenderableInstance* pInstances;

	//// Create the instance array.
	//pInstances = new DirectX11RenderableInstance[m_instanceCount];
	//if(!pInstances)
	//{
	//	return false;
	//}

	//// Populate the instance array
	//for(int i=0; i<m_instanceCount; i++)	
	//{
	//	pInstances[i].m_World = m_InstanceList.top()->m_World;
	//	m_InstanceList.pop();
	//}

	GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);
	m_D3D11PixelShader.SetTexture(grc->GetTextureResource());

	V_RETURN (m_D3D11VertexShader.SetupRender(pScene, this, m_instanceCount, &m_InstanceList));

	V_RETURN (m_D3D11PixelShader.SetupRender(this));

	V_RETURN (m_D3D11VertexBuffer.SetupRender());

	// Set up the description of the instance buffer.
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(DirectX11RenderableInstance) * m_instanceCount;
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	//bd.MiscFlags = 0;
	//bd.StructureByteStride = 0;

	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = pInstances;
	//InitData.SysMemPitch = 0;
	//InitData.SysMemSlicePitch = 0;
	//hr = g_pDX11W->GetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);

	//// Create the instance buffer.
	//V_RETURN(g_pDX11W->GetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pInstanceBuffer));

	//// Release the instance array now that the instance buffer has been created and loaded.
	//SAFE_DELETE_ARRAY(pInstances);

    // Set primitive topology
    g_pDX11W->GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//g_pDX11W->GetD3D11DeviceContext()->DrawIndexed(m_numPolys * 3, 0, 0);
	g_pDX11W->GetD3D11DeviceContext()->DrawIndexedInstanced(m_numPolys * 3, m_instanceCount, 0, 0, 0 );

	m_IsRendering = false;

	return S_OK;
}