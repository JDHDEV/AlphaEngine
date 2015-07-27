#pragma once
//========================================================================
// Shaders.h - helper classes to call GameCode4_VS.hlsl and GameCode4_PS.hlsl
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

//========================================================================
// Content References in Game Coding Complete 4th Edition
// 
// class GameCode4_Hlsl_VertexShader							- Chapter 15, page 508
// class GameCode4_Hlsl_PixelShader                             - Chapter 15, page 515
//
#include <xnamath.h>

#include "Geometry.h"
#include "Material.h"

// Forward declarations
class D3DRenderer11;
class SceneNode;
class Scene;

class Alpha_Hlsl_VertexShader
{
public:
    Alpha_Hlsl_VertexShader();
    ~Alpha_Hlsl_VertexShader();

	//HRESULT virtual OnRestore(Scene *pScene) { return S_OK;}
	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
    void EnableLights(bool enableLights) { m_enableLights = enableLights; }

protected:
    ID3D11InputLayout*          m_pVertexLayout11;
    ID3D11VertexShader*         m_pVertexShader;
    ID3D11Buffer*               m_pcbVSMatrices;
    ID3D11Buffer*               m_pcbVSLighting;
    ID3D11Buffer*               m_pcbVSMaterial;
    bool                        m_enableLights;
};

class Alpha_Hlsl_PixelShader
{
public:
    Alpha_Hlsl_PixelShader();
    ~Alpha_Hlsl_PixelShader();

    HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
    HRESULT SetTexture(const std::string& textureName);
    HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
    ID3D11PixelShader*          m_pPixelShader;
    ID3D11Buffer*               m_pcbPSMaterial;
    std::string                 m_textureResource;
};

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectPass;

class LineDraw_Hlsl_Shader
{
public:
    LineDraw_Hlsl_Shader();
    ~LineDraw_Hlsl_Shader();

    HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene);
    HRESULT SetDiffuse(const std::string& textureName, const Color &color);
    HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
    ID3D11InputLayout*          m_pVertexLayout11;
    ID3D11Buffer*               m_pcbRenderTargetSize;
    ID3DX11Effect*              m_pEffect;
    ID3DX11EffectTechnique*     m_EffectTechnique; // No need to be Release()-d.
    ID3DX11EffectPass*          m_EffectPass; // No need to be Release()-d.
    ID3D11Buffer*               m_pcbChangePerFrame;
    ID3D11Buffer*               m_pcbDiffuseColor;
    std::string                 m_textureResource;

};

class Alpha_Hlsl_LineShader
{
public:
    Alpha_Hlsl_LineShader();
    ~Alpha_Hlsl_LineShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene);
    HRESULT SetTexture(const std::string& textureName);
    HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
    ID3D11InputLayout*          m_pVertexLayout11;
    ID3D11VertexShader*         m_pVertexShader;
    ID3D11Buffer*               m_pcbVSMatrices;
    ID3D11Buffer*               m_pcbVSLighting;
    ID3D11Buffer*               m_pcbVSMaterial;
	ID3D11PixelShader*          m_pPixelShader;
    ID3D11Buffer*               m_pcbPSMaterial;	
    std::string					m_textureResource;
};