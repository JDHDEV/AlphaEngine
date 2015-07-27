//========================================================================
// File: Sky.cpp - implements a sky box in D3D9 or D3D11
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
#include "D3DRenderer.h"
#include "Geometry.h"
#include "SceneNodes.h"
#include "Shaders.h"
#include "Sky.h"

////////////////////////////////////////////////////
// SkyNode Implementation
////////////////////////////////////////////////////

//
// SkyNode::SkyNode						- Chapter 16, page 554
//
SkyNode::SkyNode(const char *pTextureBaseName)
: SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_Sky, &Mat4x4::g_Identity)
, m_bActive(true)
{
	m_textureBaseName = pTextureBaseName;
}

//
// SkyNode::VPreRender				- Chapter 14, page 502
//
HRESULT SkyNode::VPreRender(Scene *pScene)
{
	Vec3 cameraPos = m_camera->VGet()->ToWorld().GetPosition();
	Mat4x4 mat = m_Props.ToWorld();
	mat.SetPosition(cameraPos);
	VSetTransform(&mat);

	return SceneNode::VPreRender(pScene);
}

////
//// D3DSkyNode9::D3DSkyNode9				- 3rd Edition, Chapter 14, page 499
////
//D3DSkyNode9::D3DSkyNode9(const char *pTextureBaseName)
//: SkyNode(pTextureBaseName)
//{
//	for (int i=0; i<5; ++i)
//	{
//		m_pTexture[i] = NULL;
//	}
//	m_pVerts = NULL;
//}
//
////
//// D3DSkyNode9::~D3DSkyNode9			- 3rd Edition, Chapter 14, page 499
////
//D3DSkyNode9::~D3DSkyNode9()
//{
//	for (int i=0; i<5; ++i)
//	{
//		SAFE_RELEASE(m_pTexture[i]);
//	}
//	SAFE_RELEASE(m_pVerts);
//}
//
////
//// D3DSkyNode9::VOnRestore				- 3rd Edition, Chapter 14, page 500
////
//HRESULT D3DSkyNode9::VOnRestore(Scene *pScene)
//{
//	// Call the base class's restore
//	SceneNode::VOnRestore(pScene);
//
//	m_camera = pScene->GetCamera();					// added post press!
//
//	m_numVerts = 20;
//
//	SAFE_RELEASE(m_pVerts);
//    if(FAILED(DXUTGetD3D9Device()->CreateVertexBuffer( 
//		m_numVerts*sizeof(D3D9Vertex_ColoredTextured),
//		D3DUSAGE_WRITEONLY, D3D9Vertex_ColoredTextured::FVF,
//        D3DPOOL_MANAGED, &m_pVerts, NULL)))
//    {
//        return E_FAIL;
//    }
//
//    // Fill the vertex buffer. We are setting the tu and tv texture
//    // coordinates, which range from 0.0 to 1.0
//    D3D9Vertex_ColoredTextured* pVertices;
//    if( FAILED( m_pVerts->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
//        return E_FAIL;
//
//	// Loop through the grid squares and calc the values
//	// of each index. Each grid square has two triangles:
//	//
//	//		A - B
//	//		| / |
//	//		C - D
//
//	D3D9Vertex_ColoredTextured skyVerts[4];
//	D3DCOLOR skyVertColor = 0xffffffff;
//	float dim = 50.0f;
//
//	skyVerts[0].position = Vec3( dim, dim, dim ); skyVerts[0].color=skyVertColor; skyVerts[0].tu=1; skyVerts[0].tv=0; 
//	skyVerts[1].position = Vec3(-dim, dim, dim ); skyVerts[1].color=skyVertColor; skyVerts[1].tu=0; skyVerts[1].tv=0; 
//	skyVerts[2].position = Vec3( dim,-dim, dim ); skyVerts[2].color=skyVertColor; skyVerts[2].tu=1; skyVerts[2].tv=1; 
//	skyVerts[3].position = Vec3(-dim,-dim, dim ); skyVerts[3].color=skyVertColor; skyVerts[3].tu=0; skyVerts[3].tv=1; 
//
//	Vec3 triangle[3];
//	triangle[0] = Vec3(0.f,0.f,0.f);
//	triangle[1] = Vec3(5.f,0.f,0.f);
//	triangle[2] = Vec3(5.f,5.f,0.f);
//
//	Vec3 edge1 = triangle[1]-triangle[0];
//	Vec3 edge2 = triangle[2]-triangle[0];
//
//	Vec3 normal;
//	normal = edge1.Cross(edge2);
//	normal.Normalize();
//
//	Mat4x4 rotY;
//	rotY.BuildRotationY(GCC_PI/2.0f);
//	Mat4x4 rotX;
//	rotX.BuildRotationX(-GCC_PI/2.0f);
//	
//	m_sides = 5;
//
//	for (DWORD side = 0; side < m_sides; side++)
//	{
//		for (DWORD v = 0; v < 4; v++)
//		{
//			Vec4 temp;
//			if (side < m_sides-1)
//			{
//				temp = rotY.Xform(Vec3(skyVerts[v].position));
//			}
//			else
//			{
//				skyVerts[0].tu=1; skyVerts[0].tv=1; 
//				skyVerts[1].tu=1; skyVerts[1].tv=0; 
//				skyVerts[2].tu=0; skyVerts[2].tv=1; 
//				skyVerts[3].tu=0; skyVerts[3].tv=0; 
//
//				temp = rotX.Xform(Vec3(skyVerts[v].position));
//			}
//			skyVerts[v].position = Vec3(temp.x, temp.y, temp.z);
//		}
//		memcpy(&pVertices[side*4], skyVerts, sizeof(skyVerts));
//	}
//
//	m_pVerts->Unlock();
//	return S_OK;
//}
//
////
//// D3DSkyNode9::VRender				- Chapter 14, page 502
////
//HRESULT D3DSkyNode9::VRender(Scene *pScene)
//{
//
//	// Setup our texture. Using textures introduces the texture stage states,
//    // which govern how textures get blended together (in the case of multiple
//    // textures) and lighting information. In this case, we are modulating
//    // (blending) our texture with the diffuse color of the vertices.
//
//    DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//    DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//    DXUTGetD3D9Device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//
//	DXUTGetD3D9Device()->SetStreamSource( 0, m_pVerts, 0, sizeof(D3D9Vertex_ColoredTextured) );
//	DXUTGetD3D9Device()->SetFVF( D3D9Vertex_ColoredTextured::FVF );
//
//	for (DWORD side = 0; side < m_sides; side++)
//	{
//		// FUTURE WORK: A good optimization would be to transform the camera's
//		// world look vector into local space, and do a dot product. If the
//		// result is positive, we shouldn't draw the side since it has to be
//		// behind the camera!
//
//		// Sky boxes aren't culled by the normal mechanism
//
//		std::string name = GetTextureName(side);
//
//		/***
//		// [mrmike] - This was slightly changed post press, look at the lines below this commented out code
//		const char *suffix[] = { "_n.jpg", "_e.jpg",  "_s.jpg",  "_w.jpg",  "_u.jpg" };
//		name += suffix[side];
//		****/
//		
//		Resource resource(name);
//		shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
//		shared_ptr<D3DTextureResourceExtraData9> extra = static_pointer_cast<D3DTextureResourceExtraData9>(texture->GetExtra());
//
//		DXUTGetD3D9Device()->SetTexture( 0, extra->GetTexture() );
//		DXUTGetD3D9Device()->DrawPrimitive( D3DPT_TRIANGLESTRIP , 4 * side, 2);
//	}
//
//	DXUTGetD3D9Device()->SetTexture (0, NULL);
//	return S_OK;
//}

//
// D3DSkyNode11::D3DSkyNode11					- Chapter 16, page 555
//
D3DSkyNode11::D3DSkyNode11(const char *pTextureBaseName) 
	: SkyNode(pTextureBaseName)
{
}

//
// D3DSkyNode11::~D3DSkyNode11					- Chapter 16, page 555
//
D3DSkyNode11::~D3DSkyNode11()
{
}

//
// D3DSkyNode11::VOnRestore						- Chapter 16, page 556
//
HRESULT D3DSkyNode11::VOnRestore(Scene *pScene)
{
	HRESULT hr;

	V_RETURN(SceneNode::VOnRestore(pScene));

	m_camera = pScene->GetCamera();

	V_RETURN (m_D3D11VertexShader.OnRestore(pScene));
	V_RETURN (m_D3D11PixelShader.OnRestore(pScene));

	// Create the vertex buffer
	m_numVerts = 24;
	m_sides = 5;

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    D3D11VertexPosTexture *m_Vertices = AC_NEW D3D11VertexPosTexture[m_numVerts];
	AC_ASSERT(m_Vertices && "Out of memory in D3DSkyNode11::VOnRestore()");
	if (!m_Vertices)
		return E_FAIL;

	float dim = 50.0f;

	// create Pos vectors centered
	Vec3 PosA(-dim/2.0f, dim/2.0f, -dim/2.0f);
	Vec3 PosB(dim/2.0f, dim/2.0f, -dim/2.0f);
	Vec3 PosC(-dim/2.0f, -dim/2.0f, -dim/2.0f);
	Vec3 PosD(dim/2.0f, -dim/2.0f, -dim/2.0f);
	Vec3 PosE(dim/2.0f, dim/2.0f, dim/2.0f);
	Vec3 PosF(-dim/2.0f, dim/2.0f, dim/2.0f);
	Vec3 PosG(dim/2.0f,-dim/2.0f, dim/2.0f);
	Vec3 PosH(-dim/2.0f, -dim/2.0f, dim/2.0f);

	// create Texture vectors
	Vec2 UvA(0.0f, 1.0f);
	Vec2 UvB(1.0f, 1.0f);
	Vec2 UvC(0.0f, 0.0f);
	Vec2 UvD(1.0f, 0.0f);

	// N
	m_Vertices[0] = D3D11VertexPosTexture(PosH, UvA);
	m_Vertices[1] = D3D11VertexPosTexture(PosG, UvB);
	m_Vertices[2] = D3D11VertexPosTexture(PosF, UvC);
	m_Vertices[3] = D3D11VertexPosTexture(PosE, UvD);
	// E
	m_Vertices[4] = D3D11VertexPosTexture(PosE, UvC);
	m_Vertices[5] = D3D11VertexPosTexture(PosG, UvA);
	m_Vertices[6] = D3D11VertexPosTexture(PosB, UvD);
	m_Vertices[7] = D3D11VertexPosTexture(PosD, UvB);
	// S
	m_Vertices[8] = D3D11VertexPosTexture(PosA, UvD);
	m_Vertices[9] = D3D11VertexPosTexture(PosB, UvC);
	m_Vertices[10] = D3D11VertexPosTexture(PosC, UvB);
	m_Vertices[11] = D3D11VertexPosTexture(PosD, UvA);
	// W
	m_Vertices[12] = D3D11VertexPosTexture(PosH, UvB);
	m_Vertices[13] = D3D11VertexPosTexture(PosF, UvD);
	m_Vertices[14] = D3D11VertexPosTexture(PosC, UvA);
	m_Vertices[15] = D3D11VertexPosTexture(PosA, UvC);
	// U
	m_Vertices[16] = D3D11VertexPosTexture(PosC, UvA);
	m_Vertices[17] = D3D11VertexPosTexture(PosD, UvB);
	m_Vertices[18] = D3D11VertexPosTexture(PosH, UvC);
	m_Vertices[19] = D3D11VertexPosTexture(PosG, UvD);
	// D
	m_Vertices[20] = D3D11VertexPosTexture(PosF, UvA);
	m_Vertices[21] = D3D11VertexPosTexture(PosE, UvB);
	m_Vertices[22] = D3D11VertexPosTexture(PosA, UvC);
	m_Vertices[23] = D3D11VertexPosTexture(PosB, UvD);

	WORD *m_Indices = AC_NEW WORD[m_sides * 6];

	m_Indices[0] = 2;
	m_Indices[1] = 1;
	m_Indices[2] = 0;
	m_Indices[3] = 2;
	m_Indices[4] = 3;
	m_Indices[5] = 1;

	m_Indices[6] = 6;
	m_Indices[7] = 5;
	m_Indices[8] = 4;
	m_Indices[9] = 6;
	m_Indices[10] = 7;
	m_Indices[11] = 5;

	m_Indices[12] = 10;
	m_Indices[13] = 9;
	m_Indices[14] = 8;
	m_Indices[15] = 10;
	m_Indices[16] = 11;
	m_Indices[17] = 9;

	m_Indices[18] = 14;
	m_Indices[19] = 13;
	m_Indices[20] = 12;
	m_Indices[21] = 14;
	m_Indices[22] = 15;
	m_Indices[23] = 13;

	m_Indices[24] = 22;
	m_Indices[25] = 21;
	m_Indices[26] = 20;
	m_Indices[27] = 22;
	m_Indices[28] = 23;
	m_Indices[29] = 21;

	//m_Indices[30] = 18;
	//m_Indices[31] = 17;
	//m_Indices[32] = 16;
	//m_Indices[33] = 18;
	//m_Indices[34] = 19;
	//m_Indices[35] = 17;

	V_RETURN (m_D3D11VertexBuffer.OnRestore(m_Vertices, m_numVerts, m_Indices, m_sides * 2));

	SAFE_DELETE_ARRAY(m_Vertices);
	SAFE_DELETE_ARRAY(m_Indices);

	return S_OK;
}

//
// D3DSkyNode11::VRender						- Chapter 16, page 559
//
HRESULT D3DSkyNode11::VRender(Scene *pScene)
{
	HRESULT hr;

	V_RETURN (m_D3D11VertexShader.SetupRender(pScene, this));
	V_RETURN (m_D3D11PixelShader.SetupRender(this));
	V_RETURN (m_D3D11VertexBuffer.SetupRender());

    // Set primitive topology
    g_pDX11W->GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (DWORD side = 0; side < m_sides; side++)
	{
		// FUTURTE WORK: A good optimization would be to transform the camera's
		// world look vector into local space, and do a dot product. If the
		// result is positive, we shouldn't draw the side since it has to be
		// behind the camera!

		// Sky boxes aren't culled by the normal mechanism

		/***
		// [mrmike] - This was slightly changed post press, look at the lines below this commented out code
		const char *suffix[] = { "_n.jpg", "_e.jpg",  "_s.jpg",  "_w.jpg",  "_u.jpg" };
		name += suffix[side];
		****/

		std::string name = GetTextureName(side);
		m_D3D11PixelShader.SetTexture(name.c_str());

		g_pDX11W->GetD3D11DeviceContext()->DrawIndexed(6, side * 6, 0);
	}
	return S_OK;
}

std::string SkyNode::GetTextureName(const int side)
{
	std::string name = m_textureBaseName;
	char *letters[] = { "n", "e", "s", "w", "u" };
	unsigned int index = name.find_first_of("_");
	AC_ASSERT(index >= 0 && index < name.length()-1);
	if (index >= 0 && index < name.length()-1)
	{
		name[index+1] = *letters[side];
	}
	return name;
}