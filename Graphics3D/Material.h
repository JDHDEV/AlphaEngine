#pragma once
//========================================================================
// File: Material.h - stores texture and material information for D3D9 and D3D11
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
//  Content References in Game Coding Complete 4th Edition
// 
//  class Material                              - Chapter 14, page 485
#include "Geometry.h"
#include "../ResourceCache/ResCache.h"

class Material
{
    D3DMATERIAL9 m_D3DMaterial;                             // This structure stores diffuse, ambient, specular, emissive, and power.
public:
    Material();
    void SetAmbient(const Color &color);
    const Color GetAmbient() { return m_D3DMaterial.Ambient; }

    void SetDiffuse(const Color &color);
    const Color GetDiffuse() { return m_D3DMaterial.Diffuse; }

    void SetSpecular(const Color &color, const float power);
    void GetSpecular(Color &_color, float &_power) 
            { _color = m_D3DMaterial.Specular; _power = m_D3DMaterial.Power; }

    void SetEmissive(const Color &color);
    const Color GetEmissive() { return m_D3DMaterial.Emissive; }

    void SetAlpha(const float alpha);
    bool HasAlpha() const { return GetAlpha() != fOPAQUE; }
    float GetAlpha() const { return m_D3DMaterial.Diffuse.a; }
};

// 
//  class D3DTextureResourceExtraData11                         - Chapter 14, page 492
//
class D3DTextureResourceExtraData11 : public IResourceExtraData
{
    friend class TextureResourceLoader;

public:
    D3DTextureResourceExtraData11();
    virtual ~D3DTextureResourceExtraData11() { SAFE_RELEASE(m_pTexture); SAFE_RELEASE(m_pSamplerLinear); }
    virtual std::string VToString() { return "D3DTextureResourceExtraData11"; }

    ID3D11ShaderResourceView * const *GetTexture() { return &m_pTexture; }
    ID3D11SamplerState * const *GetSampler() { return &m_pSamplerLinear; }

protected:
    ID3D11ShaderResourceView *m_pTexture;                   
    ID3D11SamplerState* m_pSamplerLinear;
};

// 
//  class TextureResourceLoader                         - Chapter 14, page 492
//
class TextureResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};
