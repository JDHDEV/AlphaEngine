//========================================================================
// DirectX11Wrapper.h : to replace DXUT and provide further control
//
// Part of the Alpha Application
//
// http://www.directxtutorial.com/
//
// Justin Hunt
//========================================================================

#include "AlphaStd.h"
#include "DirectX11Font.h"
using namespace std;

DX11WFont::DX11WFont()
{
	m_fCharTexSizeX = 0.010526315f;// idk how to calculate stole from DXUT
	m_fGlyphSizeX = 15.0f;
	m_fGlyphSizeY = 42.0f;
	m_Font = 0;
	m_spTexture = 0;
}

DX11WFont::DX11WFont(const DX11WFont& other)
{
}

DX11WFont::~DX11WFont()
{ 
}

bool DX11WFont::Initialize(ID3D11Device* device, std::string textureFilename, char* fontFilename)
{
	bool result;

	// Load in the text file containing the font data if needed
	if(!(fontFilename == nullptr))
	{
		result = LoadFontData(fontFilename);
		if(!result)
		{
			return false;
		}
	}

	// Load the texture that has the font characters on it.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void DX11WFont::Release()
{
	// Release the font texture.
	ReleaseTexture();

	// Release the font data.
	ReleaseFontData();

	return;
}

bool DX11WFont::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	m_Font = new FontType[95];
	if(!m_Font)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for(i=0; i<95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}

void DX11WFont::ReleaseFontData()
{
	// Release the font data array.
	if(m_Font)
	{
		delete [] m_Font;
		m_Font = 0;
	}

	return;
}

bool DX11WFont::LoadTexture(ID3D11Device* device, string filename)
{
	bool result;

	Resource resource(filename);
	shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
	if(!texture)
	{
		return false;
	}

	m_spTexture = static_pointer_cast<D3DTextureResourceExtraData11>(texture->GetExtra());
	if(!m_spTexture)
	{
		return false;
	}
	return true;
}

void DX11WFont::ReleaseTexture()
{
	m_spTexture = nullptr;
	return;
}

ID3D11ShaderResourceView *const *DX11WFont::GetTexture()
{
	return m_spTexture->GetTexture();
}

void DX11WFont::BuildVertexArray(std::vector<DXUT_SCREEN_VERTEX_10>* vertices, LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight, float fBBWidth, float fBBHeight, bool bCenter)
{
	if(m_Font)
	{
		BuildVertexArrayNonStd(vertices, strText, rcScreen, vFontColor, fGlyphHeight, fBBWidth, fBBHeight, bCenter);
	}else{
		BuildVertexArrayStd(vertices, strText, rcScreen, vFontColor, fGlyphHeight , fBBWidth, fBBHeight, bCenter);
	}
}

void DX11WFont::BuildVertexArrayNonStd(std::vector<DXUT_SCREEN_VERTEX_10>* vertices, LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight, float fBBWidth, float fBBHeight, bool bCenter)
{
	// TODO implement
}

void DX11WFont::BuildVertexArrayStd(std::vector<DXUT_SCREEN_VERTEX_10>* vertices, LPCWSTR strText, const RECT& rcScreen, XMFLOAT4 vFontColor, float fGlyphHeight, float fBBWidth, float fBBHeight, bool bCenter)
{
	float fGlyphSizeX;
	float fGlyphSizeY;

	if(fGlyphHeight == 0.0f)
	{
	    fGlyphSizeX = m_fGlyphSizeX / fBBWidth;
	    fGlyphSizeY = m_fGlyphSizeY / fBBHeight;
	}else{
		fGlyphSizeX = fGlyphHeight / fBBWidth;
		fGlyphSizeY = ((m_fGlyphSizeY*fGlyphHeight)/m_fGlyphSizeX) / fBBHeight;
	}	

    float fRectLeft = rcScreen.left / fBBWidth;
    float fRectTop = 1.0f - rcScreen.top / fBBHeight;

    fRectLeft = fRectLeft * 2.0f - 1.0f;
    fRectTop = fRectTop * 2.0f - 1.0f;
	
	// Get the number of letters in the sentence.
	int NumChars = (int)wcslen(strText);

	//Center the text
    if (bCenter) {
        float fRectRight = rcScreen.right / fBBWidth;
        fRectRight = fRectRight * 2.0f - 1.0f;
        float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;
        fRectBottom = fRectBottom * 2.0f - 1.0f;
        float fcenterx = ((fRectRight - fRectLeft) - (float)NumChars*fGlyphSizeX) *0.5f;
        float fcentery = ((fRectTop - fRectBottom) - (float)1*fGlyphSizeY) *0.5f;
        fRectLeft += fcenterx ;    
        fRectTop -= fcentery;
    }
    float fOriginalLeft = fRectLeft;
    float fTexTop = 0.0f;
    float fTexBottom = 1.0f;
	float fDepth = 0.5f;

	// Draw each letter onto a quad.
	for(int i=0; i<NumChars; i++)
	{
		if(strText[i] == '\n')
        {
            fRectLeft = fOriginalLeft;
            fRectTop -= fGlyphSizeY;
            continue;
        }
        else if(strText[i] < 32 || strText[i] > 126)
        {
            continue;
        }

		DXUT_SCREEN_VERTEX_10 Vertex;
		Vertex.color = vFontColor;
		Vertex.z = fDepth;
		float fRectRight = fRectLeft + fGlyphSizeX;
        float fRectBottom = fRectTop - fGlyphSizeY;
        float fTexLeft = (strText[i] - 32) * m_fCharTexSizeX;
        float fTexRight = fTexLeft + m_fCharTexSizeX;

		// tri1
		Vertex.x = fRectLeft; 
		Vertex.y = fRectTop; 
		Vertex.tu = fTexLeft; 
		Vertex.tv = fTexTop;
		vertices->push_back(Vertex);

		Vertex.x = fRectRight; 
		Vertex.y = fRectTop; 
		Vertex.tu = fTexRight; 
		Vertex.tv = fTexTop;
		vertices->push_back(Vertex);

		Vertex.x = fRectLeft; 
		Vertex.y = fRectBottom; 
		Vertex.tu = fTexLeft; 
		Vertex.tv = fTexBottom;
		vertices->push_back(Vertex);

        // tri2
		Vertex.x = fRectRight; 
		Vertex.y = fRectTop; 
		Vertex.tu = fTexRight; 
		Vertex.tv = fTexTop;
		vertices->push_back(Vertex);
		
		Vertex.x = fRectRight; 
		Vertex.y = fRectBottom; 
		Vertex.tu = fTexRight; 
		Vertex.tv = fTexBottom;
		vertices->push_back(Vertex);

		Vertex.x = fRectLeft; 
		Vertex.y = fRectBottom; 
		Vertex.tu = fTexLeft; 
		Vertex.tv = fTexBottom;
		vertices->push_back(Vertex);

		fRectLeft += fGlyphSizeX;
	}
}
