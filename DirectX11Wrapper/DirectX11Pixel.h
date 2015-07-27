#pragma once
//========================================================================
// DirectX11Pixel.h : Defeine all DirectX11 Pixel Shaders
//
// Part of the Alpha Application
//
// http://www.braynzarsoft.net/
//
// Justin Hunt
//========================================================================

#include "DirectX.h"

class DirectX11Renderable;

class Alpha_D3D11BluePixelShader
{
public:
    Alpha_D3D11BluePixelShader();
    ~Alpha_D3D11BluePixelShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender();

protected:
	ID3D11PixelShader*      m_pPixelShader;
};

class Alpha_D3D11ColorPixelShader
{
public:
    Alpha_D3D11ColorPixelShader();
    ~Alpha_D3D11ColorPixelShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender();

protected:
	ID3D11PixelShader*      m_pPixelShader;
};

class Alpha_D3D11ColorNormalPixelShader
{
public:
    Alpha_D3D11ColorNormalPixelShader();
    ~Alpha_D3D11ColorNormalPixelShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender();

protected:
	ID3D11PixelShader*      m_pPixelShader;
};

class Alpha_D3D11NormalTexturePixelShader
{
public:
    Alpha_D3D11NormalTexturePixelShader();
    ~Alpha_D3D11NormalTexturePixelShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
	HRESULT SetTexture(const std::string& textureName);
    HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
	ID3D11PixelShader*      m_pPixelShader;
	ID3D11Buffer*			m_pcbPSLighting;
	ID3D11Buffer*           m_pcbPSMaterial;
	bool					m_enableLights;
    std::string             m_textureResource;
};

class Alpha_D3D11TexturePixelShader
{
public:
    Alpha_D3D11TexturePixelShader();
    ~Alpha_D3D11TexturePixelShader();

	HRESULT OnRestore(Scene *pScene);
    HRESULT SetupRender(SceneNode *pNode);
	HRESULT SetupRender(DirectX11Renderable *pRenderable);
	HRESULT SetTexture(const std::string& textureName);
    HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
	ID3D11PixelShader*      m_pPixelShader;
	ID3D11Buffer*           m_pcbPSMaterial;
    std::string             m_textureResource;
};