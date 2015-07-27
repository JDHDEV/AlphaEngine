#pragma once
//========================================================================
// DirectX11Font.h : to replace DXUT and provide further control
//
// Part of the Alpha Application
//
// http://www.rastertek.com/dx11tut12.html
//
// Justin Hunt
//========================================================================

#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#include "DirectX.h"
#include <fstream>

class D3DTextureResourceExtraData11;

class DX11WFont
{
public:
	DX11WFont();
	DX11WFont(const DX11WFont&);
	~DX11WFont();

	bool Initialize(ID3D11Device* device, std::string textureFilename, char* fontFilename = nullptr);
	void Release();

	ID3D11ShaderResourceView *const *GetTexture();

	void BuildVertexArray(std::vector<DXUT_SCREEN_VERTEX_10>* vertices, LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight, float fBBWidth, float fBBHeight, bool bCenter);

private:
	void BuildVertexArrayNonStd(std::vector<DXUT_SCREEN_VERTEX_10>* vertices, LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight, float fBBWidth, float fBBHeight, bool bCenter);
	void BuildVertexArrayStd(std::vector<DXUT_SCREEN_VERTEX_10>* vertices, LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight, float fBBWidth, float fBBHeight, bool bCenter);

	bool LoadFontData(char* fontFilename); // TODO make it load from raw bits
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, std::string);
	void ReleaseTexture();

private:
	float m_fCharTexSizeX; // Width of each char if texture is standardized 
    float m_fGlyphSizeX;
    float m_fGlyphSizeY;

	FontType* m_Font;
	std::shared_ptr<D3DTextureResourceExtraData11>	m_spTexture;
};

#endif