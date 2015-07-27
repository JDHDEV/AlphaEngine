//========================================================================
// DirectX11Gui.cpp - implements the DXW11DialogResourceManager and DXW11TextHelper
//					Which are gutted version of the DXUTGUI!! Shameless
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

#include "AlphaStd.h" //unsure if this is correct place for include

#include "../Alpha/Alpha.h" //unsure if this is correct place for include

#include "DirectX.h"
#include "DirectX11Gui.h"

//#include "../3rdParty/DXUT11/Core/DDSTextureLoader.h"

double			DXW11UIFrame::s_fTimeRefresh = 0.0f;
DX11WUIControl* DXW11UIFrame::s_pControlFocus = nullptr;        // The control which has focus
DX11WUIControl* DXW11UIFrame::s_pControlPressed = nullptr;      // The control currently pressed

// todo put this in a file
CHAR g_strUIEffectFile[] = \
    "Texture2D g_Texture;"\
    ""\
    "SamplerState Sampler"\
    "{"\
    "    Filter = MIN_MAG_MIP_LINEAR;"\
    "    AddressU = Wrap;"\
    "    AddressV = Wrap;"\
    "};"\
    ""\
    "BlendState UIBlend"\
    "{"\
    "    AlphaToCoverageEnable = FALSE;"\
    "    BlendEnable[0] = TRUE;"\
    "    SrcBlend = SRC_ALPHA;"\
    "    DestBlend = INV_SRC_ALPHA;"\
    "    BlendOp = ADD;"\
    "    SrcBlendAlpha = ONE;"\
    "    DestBlendAlpha = ZERO;"\
    "    BlendOpAlpha = ADD;"\
    "    RenderTargetWriteMask[0] = 0x0F;"\
    "};"\
    ""\
    "BlendState NoBlending"\
    "{"\
    "    BlendEnable[0] = FALSE;"\
    "    RenderTargetWriteMask[0] = 0x0F;"\
    "};"\
    ""\
    "DepthStencilState DisableDepth"\
    "{"\
    "    DepthEnable = false;"\
    "};"\
    "DepthStencilState EnableDepth"\
    "{"\
    "    DepthEnable = true;"\
    "};"\
    "struct VS_OUTPUT"\
    "{"\
    "    float4 Pos : POSITION;"\
    "    float4 Dif : COLOR;"\
    "    float2 Tex : TEXCOORD;"\
    "};"\
    ""\
    "VS_OUTPUT VS( float3 vPos : POSITION,"\
    "              float4 Dif : COLOR,"\
    "              float2 vTexCoord0 : TEXCOORD )"\
    "{"\
    "    VS_OUTPUT Output;"\
    ""\
    "    Output.Pos = float4( vPos, 1.0f );"\
    "    Output.Dif = Dif;"\
    "    Output.Tex = vTexCoord0;"\
    ""\
    "    return Output;"\
    "}"\
    ""\
    "float4 PS( VS_OUTPUT In ) : SV_Target"\
    "{"\
    "    return g_Texture.Sample( Sampler, In.Tex ) * In.Dif;"\
    "}"\
    ""\
    "float4 PSUntex( VS_OUTPUT In ) : SV_Target"\
    "{"\
    "    return In.Dif;"\
    "}"\
    ""\
    "technique10 RenderUI"\
    "{"\
    "    pass P0"\
    "    {"\
    "        SetVertexShader( CompileShader( vs_4_0, VS() ) );"\
    "        SetGeometryShader( NULL );"\
    "        SetPixelShader( CompileShader( ps_4_0, PS() ) );"\
    "        SetDepthStencilState( DisableDepth, 0 );"\
    "        SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );"\
    "    }"\
    "}"\
    "technique10 RenderUIUntex"\
    "{"\
    "    pass P0"\
    "    {"\
    "        SetVertexShader( CompileShader( vs_4_0, VS() ) );"\
    "        SetGeometryShader( NULL );"\
    "        SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );"\
    "        SetDepthStencilState( DisableDepth, 0 );"\
    "        SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );"\
    "    }"\
    "}"\
    "technique10 RestoreState"\
    "{"\
    "    pass P0"\
    "    {"\
    "        SetDepthStencilState( EnableDepth, 0 );"\
    "        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );"\
    "    }"\
    "}";
const UINT g_uUIEffectFileSize = sizeof(g_strUIEffectFile);

//======================================================================================
// DX11WBlendColor
//======================================================================================
void DX11WBlendColor::Init(DWORD defaultColor, DWORD disabledColor, DWORD hiddenColor)
{
    for(int i = 0; i < MAX_CONTROL_STATES; i++)
    {
        States[i] = defaultColor;
    }

    States[DX11W_STATE_DISABLED] = disabledColor;
    States[DX11W_STATE_HIDDEN] = hiddenColor;
    SetCurrent(defaultColor);// usaully hidden color
}

void DX11WBlendColor::Blend(UINT iState, float fElapsedTime, float fRate)
{
    XMFLOAT4 destColor = D3DCOLOR_TO_D3DCOLORVALUE(States[ iState ]);
    XMVECTOR clr1 = XMLoadFloat4(&destColor);
    XMVECTOR clr = XMLoadFloat4(&Current);
    clr = XMVectorLerp(clr, clr1, 1.0f - powf(fRate, 30 * fElapsedTime));
    XMStoreFloat4(&Current, clr);
}

void DX11WBlendColor::SetCurrent(DWORD color)
{
    Current = D3DCOLOR_TO_D3DCOLORVALUE(color);
}

//======================================================================================
// DX11WElement
//======================================================================================
void DX11WElement::SetTexture(UINT iTexture, RECT* prcTexture, DWORD defaultTextureColor)
{
    this->iTexture = iTexture;

    if(prcTexture)
        rcTexture = *prcTexture;
    else
        SetRectEmpty(&rcTexture);

    TextureColor.Init(defaultTextureColor);
}

void DX11WElement::SetFont(UINT iFont, DWORD defaultFontColor, DWORD dwTextFormat)
{
    this->iFont = iFont;
    this->dwTextFormat = dwTextFormat;

    FontColor.Init(defaultFontColor);
}

void DX11WElement::Refresh()
{
    TextureColor.SetCurrent(TextureColor.States[DX11W_STATE_HIDDEN]);
    FontColor.SetCurrent(FontColor.States[DX11W_STATE_HIDDEN]);
}

//======================================================================================
// DX11WUIBackground
//======================================================================================
DX11WUIBackground::DX11WUIBackground()
{
	m_pManager = nullptr;
	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_borderWidth = 2;
	m_bChanged = false;
	m_bColorChanged = false;
	m_bHasBackground = true;
	m_bHasBorder = true;
	m_bDrawBackground = true;
	m_bDrawBorder = true;
	m_backgroundColor = m_curBackgroundColor = D3DCOLOR_ARGB(50,0,0,0);
    m_borderColor = m_curBorderColor = D3DCOLOR_ARGB(200, 128, 128, 128);
}

void DX11WUIBackground::Init(DX11WGuiResourceManager* pManager)
{
	m_pManager = pManager;
	m_bChanged = true;
}

// Background helpers
HRESULT DX11WUIBackground::Draw()
{
	if(m_pManager == nullptr)
		return E_FAIL;

	auto pd3dDevice = m_pManager->GetD3D11Device();
    auto pd3dDeviceContext = m_pManager->GetD3D11DeviceContext();

	if(m_bHasBorder && m_bDrawBorder)
	{
		// copy the vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		pd3dDeviceContext->Map(m_pManager->m_pVBScreen4Quad11, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // map the buffer
		memcpy(ms.pData, m_BorderVertices, sizeof(m_BorderVertices)); // copy the data
		pd3dDeviceContext->Unmap(m_pManager->m_pVBScreen4Quad11, NULL); // unmap the buffer
	
		// select which vertex buffer to display
		UINT stride = sizeof(DXUT_SCREEN_VERTEX_10);
		UINT offset = 0;
		pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pManager->m_pVBScreen4Quad11, &stride, &offset);

		// select which Layout we are using
		pd3dDeviceContext->IASetInputLayout(m_pManager->m_pInputLayout11);

		// select which primtive type we are using
		pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// draw the vertex buffer to the back buffer
		m_pManager->ApplyRenderUIUntex11(pd3dDeviceContext);
		pd3dDeviceContext->Draw(24, 0);
	}

	if(m_bHasBackground && m_bDrawBackground)
	{
		// copy the background vertices into the buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		pd3dDeviceContext->Map(m_pManager->m_pVBScreenQuad11, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // map the buffer
		memcpy(ms.pData, m_BackGroundVertices, sizeof(m_BackGroundVertices)); // copy the data
		pd3dDeviceContext->Unmap(m_pManager->m_pVBScreenQuad11, NULL); // unmap the buffer

		// Draw the background
		// select which vertex buffer to display
		UINT stride = sizeof(DXUT_SCREEN_VERTEX_10);
		UINT offset = 0;
		pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pManager->m_pVBScreenQuad11, &stride, &offset);

		// select which Layout we are using
		pd3dDeviceContext->IASetInputLayout(m_pManager->m_pInputLayout11);

		// select which primtive type we are using
		pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// draw the vertex buffer to the back buffer
		m_pManager->ApplyRenderUIUntex11(pd3dDeviceContext);
		pd3dDeviceContext->Draw(4, 0);
	}

	return S_OK;
}

void DX11WUIBackground::Update()
{
	if(m_bChanged)
	{
		UpdateBackground();
	}

	if(m_bColorChanged)
	{
		UpdateColor();
	}
}

void DX11WUIBackground::UpdateBackground()
{
	if(m_pManager == nullptr)
		return;
	
	if(m_bHasBackground || m_bHasBorder)
	{
		float top, left, right, bot;
		top = 1.0f - m_y * 2.0f / m_pManager->m_nBackBufferHeight; 
		left = m_x * 2.0f / m_pManager->m_nBackBufferWidth - 1.0f; 
		right = (m_x + m_width) * 2.0f / m_pManager->m_nBackBufferWidth - 1.0f; 
		bot = 1.0f - (m_y + m_height) * 2.0f / m_pManager->m_nBackBufferHeight;

		XMFLOAT4 borderColor = D3DCOLOR_TO_D3DCOLORVALUE(m_curBorderColor);
		DXUT_SCREEN_VERTEX_10 a = {left, top, 0.0f, borderColor, 0.0f, 0.0f};
		DXUT_SCREEN_VERTEX_10 b = {right, top, 0.0f, borderColor, 0.0f, 0.0f};
		DXUT_SCREEN_VERTEX_10 c = {left, bot, 0.0f, borderColor, 0.0f, 0.0f};
		DXUT_SCREEN_VERTEX_10 d = {right, bot, 0.0f, borderColor, 0.0f, 0.0f};

		if(m_bHasBorder)
		{
			float itop, ileft, iright, ibot;
			itop = 1.0f - (m_y + m_borderWidth) * 2.0f / m_pManager->m_nBackBufferHeight; 
			ileft = (m_x + m_borderWidth) * 2.0f / m_pManager->m_nBackBufferWidth - 1.0f;		
			iright = (m_x + m_width - m_borderWidth) * 2.0f / m_pManager->m_nBackBufferWidth - 1.0f; 
			ibot = 1.0f - (m_y + m_height - m_borderWidth) * 2.0f / m_pManager->m_nBackBufferHeight;

	
			DXUT_SCREEN_VERTEX_10 ia = {ileft, itop, 0.0f, borderColor, 0.0f, 0.0f};
			DXUT_SCREEN_VERTEX_10 ib = {iright, itop, 0.0f, borderColor, 0.0f, 0.0f};
			DXUT_SCREEN_VERTEX_10 ic = {ileft, ibot, 0.0f, borderColor, 0.0f, 0.0f};
			DXUT_SCREEN_VERTEX_10 id = {iright, ibot, 0.0f, borderColor, 0.0f, 0.0f};
			m_BorderVertices[0] = a;
			m_BorderVertices[1] = b;
			m_BorderVertices[2] = ia;
			m_BorderVertices[3] = ia;
			m_BorderVertices[4] = b;
			m_BorderVertices[5] = ib;
			m_BorderVertices[6] = b;
			m_BorderVertices[7] = d;
			m_BorderVertices[8] = ib;
			m_BorderVertices[9] = ib;
			m_BorderVertices[10] = d;
			m_BorderVertices[11] = id;
			m_BorderVertices[12] = d;
			m_BorderVertices[13] = c;
			m_BorderVertices[14] = id;
			m_BorderVertices[15] = id;
			m_BorderVertices[16] = c;
			m_BorderVertices[17] = ic;
			m_BorderVertices[18] = c;
			m_BorderVertices[19] = a;
			m_BorderVertices[20] = ic;
			m_BorderVertices[21] = ic;
			m_BorderVertices[22] = a;
			m_BorderVertices[23] = ia;

			if(m_bHasBackground)
			{
				m_BackGroundVertices[0] = ia;
				m_BackGroundVertices[1] = ib;
				m_BackGroundVertices[2] = ic;
				m_BackGroundVertices[3] = id;
			}
		}
		else
		{	
			if(m_bHasBackground)
			{
				m_BackGroundVertices[0] = a;
				m_BackGroundVertices[1] = b;
				m_BackGroundVertices[2] = c;
				m_BackGroundVertices[3] = d;
			}
		}

		if(m_bHasBackground)
		{
			XMFLOAT4 backgroundColor = D3DCOLOR_TO_D3DCOLORVALUE(m_curBackgroundColor);
			m_BackGroundVertices[0].color = backgroundColor;
			m_BackGroundVertices[1].color = backgroundColor;
			m_BackGroundVertices[2].color = backgroundColor;
			m_BackGroundVertices[3].color = backgroundColor;
		}
	}
	else
	{
		// TODO set verts to blank or release the arrays or something
	}
	m_bChanged = false;
}

void DX11WUIBackground::UpdateColor()
{
	XMFLOAT4 backgroundColor = D3DCOLOR_TO_D3DCOLORVALUE(m_curBackgroundColor);
	int aryLen = sizeof(m_BackGroundVertices)/sizeof(DXUT_SCREEN_VERTEX_10);
	for(int i = 0; i < aryLen; ++i)
	{
		m_BackGroundVertices[i].color = backgroundColor;
	}
	XMFLOAT4 borderColor = D3DCOLOR_TO_D3DCOLORVALUE(m_curBorderColor);
	aryLen = sizeof(m_BorderVertices)/sizeof(DXUT_SCREEN_VERTEX_10);
	for(int i = 0; i < aryLen; ++i)
	{
		m_BorderVertices[i].color = borderColor;
	}
	m_bColorChanged = false;
}

//======================================================================================
// DXW11UIFrame
//======================================================================================
DXW11UIFrame::DXW11UIFrame() :
	m_bNonUserEvents(false),
    m_bKeyboardInput(false),
    m_bMouseInput(true),
	m_pControlMouseOver(nullptr),
	m_bVisible(true),
	m_x(0),
    m_y(0),
    m_width(0),
    m_height(0),
    m_pManager(nullptr),
	m_pCallbackEvent(nullptr)
{
	m_Background.SetHasBorder(false);
	m_highlightColor = D3DCOLOR_ARGB(255,202,176,0);
}

DXW11UIFrame::~DXW11UIFrame()
{
    RemoveAllControls();

    m_Fonts.clear();
    m_Textures.clear();

    for(auto it = m_DefaultElements.begin(); it != m_DefaultElements.end(); ++it)
    {
        SAFE_DELETE(*it);
    }

    m_DefaultElements.clear();
}

void DXW11UIFrame::InitDefaultElements()
{
    //SetFont(0, L"Arial", 14, FW_NORMAL); causees mem leaks

    DX11WElement Element;
	RECT rcTexture;

    ////-------------------------------------
    //// Element for the caption
    ////-------------------------------------
    //m_CapElement.SetFont( 0 );
    //SetRect( &rcTexture, 17, 269, 241, 287 );
    //m_CapElement.SetTexture( 0, &rcTexture );
    //m_CapElement.TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    //m_CapElement.FontColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    //m_CapElement.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
    //// Pre-blend as we don't need to transition the state
    //m_CapElement.TextureColor.Blend( DXUT_STATE_NORMAL, 10.0f );
    //m_CapElement.FontColor.Blend( DXUT_STATE_NORMAL, 10.0f );

    //-------------------------------------
    // CDXUTStatic
    //-------------------------------------
    Element.SetFont(0);
	//Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[DX11W_STATE_DISABLED] = D3DCOLOR_ARGB(200, 200, 200, 200);
	Element.FontColor.States[DX11W_STATE_NORMAL] = D3DCOLOR_ARGB(200, 128, 128, 128);

    // Assign the Element
    SetDefaultElement(DX11W_CONTROL_STATIC, 0, &Element);

    //-------------------------------------
    // CDX11WButton - Button
    //-------------------------------------
    //SetRect( &rcTexture, 0, 0, 136, 54 );
    //Element.SetTexture( 0, &rcTexture );
    Element.SetFont(0);
    //Element.TextureColor.States[ DX11W_STATE_NORMAL ] = D3DCOLOR_ARGB( 150, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_PRESSED ] = D3DCOLOR_ARGB( 200, 255, 255, 255 );
	Element.FontColor.States[DX11W_STATE_NORMAL] = D3DCOLOR_ARGB(200, 128, 128, 128);
	Element.FontColor.States[DX11W_STATE_MOUSEOVER] = m_highlightColor;
    Element.FontColor.States[DX11W_STATE_PRESSED] = D3DCOLOR_ARGB(200, 255, 255, 255);

    // Assign the Element
    SetDefaultElement(DX11W_CONTROL_BUTTON, 0, &Element);

    ////-------------------------------------
    //// CDX11WButton - Fill layer
    ////-------------------------------------
    //SetRect( &rcTexture, 136, 0, 252, 54 );
    //Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB( 0, 255, 255, 255 ) );
    //Element.TextureColor.States[ DX11W_STATE_MOUSEOVER ] = D3DCOLOR_ARGB( 160, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_PRESSED ] = D3DCOLOR_ARGB( 60, 0, 0, 0 );
    //Element.TextureColor.States[ DX11W_STATE_FOCUS ] = D3DCOLOR_ARGB( 30, 255, 255, 255 );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_BUTTON, 1, &Element );

    ////-------------------------------------
    //// CDX11WCheckBox - Box
    ////-------------------------------------
    //SetRect( &rcTexture, 0, 54, 27, 81 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
    //Element.FontColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    //Element.TextureColor.States[ DX11W_STATE_NORMAL ] = D3DCOLOR_ARGB( 150, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_FOCUS ] = D3DCOLOR_ARGB( 200, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_PRESSED ] = D3DCOLOR_ARGB( 255, 255, 255, 255 );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_CHECKBOX, 0, &Element );


    ////-------------------------------------
    //// CDX11WCheckBox - Check
    ////-------------------------------------
    //SetRect( &rcTexture, 27, 54, 54, 81 );
    //Element.SetTexture( 0, &rcTexture );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_CHECKBOX, 1, &Element );


    ////-------------------------------------
    //// CDX11WRadioButton - Box
    ////-------------------------------------
    //SetRect( &rcTexture, 54, 54, 81, 81 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
    //Element.FontColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
    //Element.TextureColor.States[ DX11W_STATE_NORMAL ] = D3DCOLOR_ARGB( 150, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_FOCUS ] = D3DCOLOR_ARGB( 200, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_PRESSED ] = D3DCOLOR_ARGB( 255, 255, 255, 255 );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_RADIOBUTTON, 0, &Element );


    ////-------------------------------------
    //// CDX11WRadioButton - Check
    ////-------------------------------------
    //SetRect( &rcTexture, 81, 54, 108, 81 );
    //Element.SetTexture( 0, &rcTexture );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_RADIOBUTTON, 1, &Element );


    ////-------------------------------------
    //// CDX11WComboBox - Main
    ////-------------------------------------
    //SetRect( &rcTexture, 7, 81, 247, 123 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0 );
    //Element.TextureColor.States[ DX11W_STATE_NORMAL ] = D3DCOLOR_ARGB( 150, 200, 200, 200 );
    //Element.TextureColor.States[ DX11W_STATE_FOCUS ] = D3DCOLOR_ARGB( 170, 230, 230, 230 );
    //Element.TextureColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 70, 200, 200, 200 );
    //Element.FontColor.States[ DX11W_STATE_MOUSEOVER ] = D3DCOLOR_ARGB( 255, 0, 0, 0 );
    //Element.FontColor.States[ DX11W_STATE_PRESSED ] = D3DCOLOR_ARGB( 255, 0, 0, 0 );
    //Element.FontColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );


    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_COMBOBOX, 0, &Element );


    ////-------------------------------------
    //// CDX11WComboBox - Button
    ////-------------------------------------
    //SetRect( &rcTexture, 98, 189, 151, 238 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.TextureColor.States[ DX11W_STATE_NORMAL ] = D3DCOLOR_ARGB( 150, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_PRESSED ] = D3DCOLOR_ARGB( 255, 150, 150, 150 );
    //Element.TextureColor.States[ DX11W_STATE_FOCUS ] = D3DCOLOR_ARGB( 200, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 70, 255, 255, 255 );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_COMBOBOX, 1, &Element );


    ////-------------------------------------
    //// CDX11WComboBox - Dropdown
    ////-------------------------------------
    //SetRect( &rcTexture, 13, 123, 241, 160 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_COMBOBOX, 2, &Element );


    ////-------------------------------------
    //// CDX11WComboBox - Selection
    ////-------------------------------------
    //SetRect( &rcTexture, 12, 163, 239, 183 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_TOP );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_COMBOBOX, 3, &Element );


    ////-------------------------------------
    //// CDX11WSlider - Track
    ////-------------------------------------
    //SetRect( &rcTexture, 1, 187, 93, 228 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.TextureColor.States[ DX11W_STATE_NORMAL ] = D3DCOLOR_ARGB( 150, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_FOCUS ] = D3DCOLOR_ARGB( 200, 255, 255, 255 );
    //Element.TextureColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 70, 255, 255, 255 );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_SLIDER, 0, &Element );

    ////-------------------------------------
    //// CDX11WSlider - Button
    ////-------------------------------------
    //SetRect( &rcTexture, 151, 193, 192, 234 );
    //Element.SetTexture( 0, &rcTexture );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_SLIDER, 1, &Element );

    ////-------------------------------------
    //// CDX11WScrollBar - Track
    ////-------------------------------------
    //int nScrollBarStartX = 196;
    //int nScrollBarStartY = 191;
    //SetRect( &rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 21, nScrollBarStartX + 22, nScrollBarStartY + 32 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.TextureColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 255, 200, 200, 200 );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_SCROLLBAR, 0, &Element );

    ////-------------------------------------
    //// CDX11WScrollBar - Up Arrow
    ////-------------------------------------
    //SetRect( &rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 1, nScrollBarStartX + 22, nScrollBarStartY + 21 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.TextureColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 255, 200, 200, 200 );


    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_SCROLLBAR, 1, &Element );

    ////-------------------------------------
    //// CDX11WScrollBar - Down Arrow
    ////-------------------------------------
    //SetRect( &rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 32, nScrollBarStartX + 22, nScrollBarStartY + 53 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.TextureColor.States[ DX11W_STATE_DISABLED ] = D3DCOLOR_ARGB( 255, 200, 200, 200 );


    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_SCROLLBAR, 2, &Element );

    ////-------------------------------------
    //// CDX11WScrollBar - Button
    ////-------------------------------------
    //SetRect( &rcTexture, 220, 192, 238, 234 );
    //Element.SetTexture( 0, &rcTexture );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_SCROLLBAR, 3, &Element );


    ////-------------------------------------
    //// CDX11WEditBox
    ////-------------------------------------
    //// Element assignment:
    ////   0 - text area
    ////   1 - top left border
    ////   2 - top border
    ////   3 - top right border
    ////   4 - left border
    ////   5 - right border
    ////   6 - lower left border
    ////   7 - lower border
    ////   8 - lower right border

    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

    //// Assign the style
    //SetRect( &rcTexture, 14, 90, 241, 113 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 0, &Element );
    //SetRect( &rcTexture, 8, 82, 14, 90 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 1, &Element );
    //SetRect( &rcTexture, 14, 82, 241, 90 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 2, &Element );
    //SetRect( &rcTexture, 241, 82, 246, 90 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 3, &Element );
    //SetRect( &rcTexture, 8, 90, 14, 113 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 4, &Element );
    //SetRect( &rcTexture, 241, 90, 246, 113 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 5, &Element );
    //SetRect( &rcTexture, 8, 113, 14, 121 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 6, &Element );
    //SetRect( &rcTexture, 14, 113, 241, 121 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 7, &Element );
    //SetRect( &rcTexture, 241, 113, 246, 121 );
    //Element.SetTexture( 0, &rcTexture );
    //SetDefaultElement( DX11W_CONTROL_EDITBOX, 8, &Element );

    ////-------------------------------------
    //// CDX11WListBox - Main
    ////-------------------------------------
    //SetRect( &rcTexture, 13, 123, 241, 160 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_LISTBOX, 0, &Element );

    ////-------------------------------------
    //// CDX11WListBox - Selection
    ////-------------------------------------

    //SetRect( &rcTexture, 16, 166, 240, 183 );
    //Element.SetTexture( 0, &rcTexture );
    //Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_TOP );

    //// Assign the Element
    //SetDefaultElement( DX11W_CONTROL_LISTBOX, 1, &Element );
}

void DXW11UIFrame::OnMouseMove(const POINT& pt)
{
    // Figure out which control the mouse is over now
    auto pControl = GetControlAtPoint(pt);

    // If the mouse is still over the same control, nothing needs to be done
    if(pControl == m_pControlMouseOver)
        return;

    // Handle mouse leaving the old control
    if(m_pControlMouseOver)
        m_pControlMouseOver->OnMouseLeave();

    // Handle mouse entering the new control
    m_pControlMouseOver = pControl;
    if(pControl)
        m_pControlMouseOver->OnMouseEnter();
}

void DXW11UIFrame::OnMouseUp(const POINT& pt)
{
    UNREFERENCED_PARAMETER(pt);
    s_pControlPressed = nullptr;
    m_pControlMouseOver = nullptr;
}

void DXW11UIFrame::Init(DX11WGuiResourceManager* pManager, bool bRegisterDialog)
{
    m_pManager = pManager;
	m_Background.Init(pManager);
    m_Background.Update();
	//if( bRegisterDialog )
    //    pManager->RegisterDialog( this );

    //SetTexture( 0, MAKEINTRESOURCE( 0xFFFF ), ( HMODULE )0xFFFF );
    InitDefaultElements();
}

HRESULT DXW11UIFrame::AddStatic(int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault,
                                DX11WStatic** ppCreated)
{
    HRESULT hr = S_OK;

    auto pStatic = new (std::nothrow) DX11WStatic(this);

    if(ppCreated)
        *ppCreated = pStatic;

    if(!pStatic)
        return E_OUTOFMEMORY;

    hr = AddControl(pStatic);
    if(FAILED(hr))
        return hr;

    // Set the ID and list index
    pStatic->SetID(ID);
    pStatic->SetText(strText);
    pStatic->SetLocation(x, y);
    pStatic->SetSize(width, height);
    pStatic->m_bIsDefault = bIsDefault;

    return S_OK;
}

HRESULT DXW11UIFrame::AddButton(int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey,
                                bool bIsDefault, DX11WButton** ppCreated)
{
    HRESULT hr = S_OK;

    auto pButton = new (std::nothrow) DX11WButton(this);

    if( ppCreated )
        *ppCreated = pButton;

    if( !pButton )
        return E_OUTOFMEMORY;

    hr = AddControl( pButton );
    if( FAILED( hr ) )
        return hr;

    // Set the ID and list index
    pButton->SetID( ID );
    pButton->SetText( strText );
    pButton->SetLocation( x, y );
    pButton->SetSize( width, height );
    pButton->SetHotkey( nHotkey );
    pButton->m_bIsDefault = bIsDefault;

    return S_OK;
}

HRESULT DXW11UIFrame::AddControl(_In_ DX11WUIControl* pControl)
{
	HRESULT hr = S_OK;

    hr = InitControl(pControl);
    if(FAILED(hr))
		return E_FAIL;
        //return DXTRACE_ERR( L"CDXUTDialog::InitControl", hr );TODO log failure here

    // Add to the list
    m_Controls.push_back(pControl);

    return S_OK;
}

HRESULT DXW11UIFrame::InitControl(_In_ DX11WUIControl* pControl)
{
	HRESULT hr;

    if(!pControl)
        return E_INVALIDARG;

    pControl->m_Index = static_cast<UINT>(m_Controls.size());

    // Look for a default Element entries
    for(auto it = m_DefaultElements.begin(); it != m_DefaultElements.end(); ++it)
    {
        if((*it)->nControlType == pControl->GetType())
            pControl->SetElement((*it)->iElement, &(*it)->Element);
    }

    V_RETURN(pControl->OnInit());

    return S_OK;
}

DX11WUIControl* DXW11UIFrame::GetControl(int ID) const
{
    // Try to find the control with the given ID
    for(auto it = m_Controls.cbegin(); it != m_Controls.cend(); ++it)
    {
        if((*it)->GetID() == ID)
        {
            return *it;
        }
    }

    // Not found
    return nullptr;
}

DX11WUIControl* DXW11UIFrame::GetControlAtPoint(_In_ const POINT& pt) const
{
	 // Search through all child controls for the first one which
    // contains the mouse point
    for( auto it = m_Controls.cbegin(); it != m_Controls.cend(); ++it )
    {
        if( !*it )
        {
            continue;
        }

        // We only return the current control if it is visible
        // and enabled.  Because GetControlAtPoint() is used to do mouse
        // hittest, it makes sense to perform this filtering.
        if( (*it)->ContainsPoint( pt ) && (*it)->GetEnabled() && (*it)->GetVisible() )
        {
            return *it;
        }
    }

    return nullptr;
}

HRESULT DXW11UIFrame::SetDefaultElement(UINT nControlType, UINT iElement, DX11WElement* pElement)
{
    // If this Element type already exist in the list, simply update the stored Element
    for(auto it = m_DefaultElements.begin(); it != m_DefaultElements.end(); ++it)
    {
        if((*it)->nControlType == nControlType &&
            (*it)->iElement == iElement)
        {
            (*it)->Element = *pElement;
            return S_OK;
        }
    }

    // Otherwise, add a new entry
    DX11WElementHolder* pNewHolder;
    pNewHolder = new (std::nothrow) DX11WElementHolder;
    if(!pNewHolder)
        return E_OUTOFMEMORY;

    pNewHolder->nControlType = nControlType;
    pNewHolder->iElement = iElement;
    pNewHolder->Element = *pElement;

    m_DefaultElements.push_back(pNewHolder);

    return S_OK;
}

DX11WElement* DXW11UIFrame::GetDefaultElement(UINT nControlType, UINT iElement) const
{
    for(auto it = m_DefaultElements.cbegin(); it != m_DefaultElements.cend(); ++it)
    {
        if((*it)->nControlType == nControlType &&
            (*it)->iElement == iElement)
        {
            return &(*it)->Element;
        }
    }

    return nullptr;
}

void DXW11UIFrame::SendEvent(UINT nEvent, bool bTriggeredByUser, DX11WUIControl* pControl)
{
    // If no callback has been registered there's nowhere to send the event to
    if(!m_pCallbackEvent)
        return;

    // Discard events triggered programatically if these types of events haven't been
    // enabled
    if(!bTriggeredByUser && !m_bNonUserEvents)
        return;

    m_pCallbackEvent(nEvent, pControl->GetID(), pControl, m_pCallbackEventUserContext);
}

void DXW11UIFrame::RequestFocus(DX11WUIControl* pControl)
{
    if(s_pControlFocus == pControl)
        return;

    if(!pControl->CanHaveFocus())
        return;

    if(s_pControlFocus)
        s_pControlFocus->OnFocusOut();

    pControl->OnFocusIn();
    s_pControlFocus = pControl;
}

void DXW11UIFrame::RemoveControl(int ID)
{
	for(auto it = m_Controls.begin(); it != m_Controls.end(); ++it)
	{
		if((*it)->GetID() == ID)
		{
			// Clean focus first
			//ClearFocus(); TODO code this function 

			// Clear references to this control
			if(s_pControlFocus == (*it))
				s_pControlFocus = nullptr;
			if(s_pControlPressed == (*it))
				s_pControlPressed = nullptr;
			if(m_pControlMouseOver == (*it))
				m_pControlMouseOver = nullptr;

			SAFE_DELETE((*it));
			m_Controls.erase(it);

			return;
		}
	}
}

void DXW11UIFrame::RemoveAllControls()
{
    if(s_pControlFocus && s_pControlFocus->m_pFrame == this)
        s_pControlFocus = nullptr;
    if(s_pControlPressed && s_pControlPressed->m_pFrame == this)
        s_pControlPressed = nullptr;
    m_pControlMouseOver = nullptr;

    for(auto it = m_Controls.begin(); it != m_Controls.end(); ++it)
    {
        SAFE_DELETE(*it);
    }

    m_Controls.clear();
}

void DXW11UIFrame::SetCallback(PCALLBACKDXUTGUIEVENT pCallback, void* pUserContext)
{
    // If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
    // was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
    // creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
    // of the application if it wishes to use DXUT's GUI.
    AC_ASSERT(m_pManager && L"To fix call CDXUTDialog::Init() first.  See comments for details.");

    m_pCallbackEvent = pCallback;
    m_pCallbackEventUserContext = pUserContext;
}

HRESULT DXW11UIFrame::DrawRect(const DX11WRECT* pRect, XMFLOAT4 color)
{
	auto pd3dDevice = m_pManager->GetD3D11Device();
    auto pd3dDeviceContext = m_pManager->GetD3D11DeviceContext();

	// create a triangle using the VERTEX struct
	DXUT_SCREEN_VERTEX_10 OurVertices[] =
	{       
		{pRect->left, pRect->top, 0.0f, color, 0.0f, 0.0f},
		{pRect->right, pRect->top, 0.0f, color, 0.0f, 0.0f},
		{pRect->left, pRect->bottom, 0.0f, color, 0.0f, 0.0f},
		{pRect->right, pRect->bottom, 0.0f, color, 0.0f, 0.0f}
	};

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	pd3dDeviceContext->Map(m_pManager->m_pVBScreenQuad11, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices)); // copy the data
	pd3dDeviceContext->Unmap(m_pManager->m_pVBScreenQuad11, NULL); // unmap the buffer
	
	// select which vertex buffer to display
	UINT stride = sizeof(DXUT_SCREEN_VERTEX_10);
	UINT offset = 0;
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pManager->m_pVBScreenQuad11, &stride, &offset);

	// select which Layout we are using
	pd3dDeviceContext->IASetInputLayout(m_pManager->m_pInputLayout11);

	// select which primtive type we are using
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// draw the vertex buffer to the back buffer
	m_pManager->ApplyRenderUIUntex11(pd3dDeviceContext);
	pd3dDeviceContext->Draw(4, 0);

	return S_OK;
}

HRESULT DXW11UIFrame::CalcTextRect(LPCWSTR strText, DX11WElement* pElement, const RECT* prcDest, int nCount)
{
    //auto pFontNode = GetFont(pElement->iFont);
    //if(!pFontNode)
    //    return E_FAIL;
	UNREFERENCED_PARAMETER(pElement);
    UNREFERENCED_PARAMETER(strText);
    UNREFERENCED_PARAMETER(prcDest);
    UNREFERENCED_PARAMETER(nCount);
    // TODO -

    return S_OK;
}

HRESULT DXW11UIFrame::DrawTextDX11W(LPCWSTR strText, DX11WElement* pElement, const RECT* prcDest, bool bShadow,
					bool bCenter, float fGlyphHeight)
{
    // No need to draw fully transparent layers
    if(pElement->FontColor.Current.w == 0)
        return S_OK;

    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, m_x, m_y);

    //// If caption is enabled, offset the Y position by its height.
    //if(m_bCaption)
    //    OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    float fBBWidth = (float)m_pManager->m_nBackBufferWidth;
    float fBBHeight = (float)m_pManager->m_nBackBufferHeight;

    auto pd3dDevice = m_pManager->GetD3D11Device();
    auto pd3d11DeviceContext =  m_pManager->GetD3D11DeviceContext();

	m_pManager->ApplyRenderUI11(pd3d11DeviceContext);

    if(bShadow)
    {
        RECT rcShadow = rcScreen;
        OffsetRect(&rcShadow, 1, 1);

        XMFLOAT4 vShadowColor(0,0,0, 1.0f);
        m_pManager->m_pText->DrawText11(pd3dDevice, pd3d11DeviceContext,
                 strText, rcShadow, vShadowColor, fGlyphHeight,
                 fBBWidth, fBBHeight, bCenter);
    }
	
    XMFLOAT4 vFontColor(pElement->FontColor.Current.x, pElement->FontColor.Current.y, pElement->FontColor.Current.z, 1.0f);

	m_pManager->m_pText->DrawText11(pd3dDevice, pd3d11DeviceContext,
             strText, rcScreen, vFontColor, fGlyphHeight,
             fBBWidth, fBBHeight, bCenter);

    return S_OK;
}

void DXW11UIFrame::Refresh()
{
	//if(s_pControlFocus)
 //       s_pControlFocus->OnFocusOut();

 //   if(m_pControlMouseOver)
 //       m_pControlMouseOver->OnMouseLeave();

 //   s_pControlFocus = nullptr;
 //   s_pControlPressed = nullptr;
 //   m_pControlMouseOver = nullptr;

 //   for( auto it = m_Controls.begin(); it != m_Controls.end(); ++it )
 //   {
 //       (*it)->Refresh();
 //   }

    //if(m_bKeyboardInput)
    //    FocusDefaultControl();
}

HRESULT DXW11UIFrame::VOnRestore()
{
	return S_OK;
}

HRESULT DXW11UIFrame::VOnLostDevice()
{
	return S_OK;
}

HRESULT DXW11UIFrame::VOnRender(double fTime, float fElapsedTime)
{

	VOnUpdate(fElapsedTime);

	// For invisible dialog, out now.
    if(!m_bVisible)/* || ( m_bMinimized && !m_bCaption ) )*/
        return S_OK;

    auto pd3dDevice = m_pManager->GetD3D11Device();
    auto pd3dDeviceContext = m_pManager->GetD3D11DeviceContext();

	// Set up a state block here and restore it when finished drawing all the controls
    m_pManager->StoreD3D11State(pd3dDeviceContext);

	m_Background.Draw();

	//auto pTextureNode = GetTexture( 0 );
    //pd3dDeviceContext->PSSetShaderResources( 0, 1, &pTextureNode->pTexResView11 );

    // Sort depth back to front
    //m_pManager->BeginSprites11();
    m_pManager->m_pText->BeginText11();

    //m_pManager->ApplyRenderUI11(pd3dDeviceContext);

    //// Render the caption if it's enabled.
    //if( m_bCaption )
    //{
    //    // DrawSprite will offset the rect down by
    //    // m_nCaptionHeight, so adjust the rect higher
    //    // here to negate the effect.
    //    RECT rc = { 0, -m_nCaptionHeight, m_width, 0 };
    //    DrawSprite( &m_CapElement, &rc, 0.99f );
    //    rc.left += 5; // Make a left margin
    //    WCHAR wszOutput[256];
    //    wcscpy_s( wszOutput, 256, m_wszCaption );
    //    if( m_bMinimized )
    //        wcscat_s( wszOutput, 256, L" (Minimized)" );
    //    DrawText( wszOutput, &m_CapElement, &rc, true );
    //}

	// Draw the Controls for this Frame
    for(auto it = m_Controls.cbegin(); it != m_Controls.cend(); ++it)
    {
        // Focused control is drawn last
        if(*it == s_pControlFocus)
            continue;

        (*it)->VOnRender(fTime, fElapsedTime);
    }

    if(s_pControlFocus && s_pControlFocus->m_pFrame == this )
        s_pControlFocus->VOnRender(fTime, fElapsedTime);

	// End sprites
    //if( m_bCaption )
    //{
    //    m_pManager->EndSprites11( pd3dDevice, pd3dDeviceContext );
    //    EndText11( pd3dDevice, pd3dDeviceContext );
    //}


    RECT rcScreen;
	SetRect(&rcScreen, 0, 0, 136, 54 );
   

    float fBBWidth = (float)m_pManager->m_nBackBufferWidth;
    float fBBHeight = (float)m_pManager->m_nBackBufferHeight;

	m_pManager->ApplyRenderUI11(pd3dDeviceContext);

	XMFLOAT4 vColor(0,0.00f,1.0, 1.0f);
	m_pManager->m_pText->DrawText11(pd3dDevice, pd3dDeviceContext,
                 g_pDX11W->GetFrameStats(true), rcScreen, vColor, 50.0f,
                 fBBWidth, fBBHeight, false);

	// Restore stored states
	m_pManager->RestoreD3D11State(pd3dDeviceContext);

	return S_OK;
}

//TODO make sure this function is getting called
void DXW11UIFrame::VOnUpdate(int deltaMilliseconds)
{
	//// See if the dialog needs to be refreshed
 //   if(m_fTimeLastRefresh > s_fTimeRefresh)
 //   {
 //       return;
 //   }
	//m_fTimeLastRefresh = g_pDX11W->GetTime();

	if(m_bChanged)// breakmark hear to see how often updates happen
	{
		m_Background.Update();
		m_bChanged = false;
	}
}

HRESULT DXW11UIFrame::SetFont(UINT index, LPCWSTR strFaceName, LONG height, LONG weight)
{
    // If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
    // was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
    // creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
    // of the application if it wishes to use DXUT's GUI.
    assert( m_pManager && L"To fix call CDXUTDialog::Init() first.  See comments for details." );
    //_Analysis_assume_( m_pManager );

    // Make sure the list is at least as large as the index being set
    for(size_t i = m_Fonts.size(); i <= index; i++)
    {
        m_Fonts.push_back(-1);
    }

    int iFont = m_pManager->AddFont(strFaceName, height, weight);
    m_Fonts[index] = iFont;

    return S_OK;
}

LRESULT CALLBACK DXW11UIFrame::VOnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

    // For invisible dialog, do not handle anything.
    if(!m_bVisible)
        return false;

    // If a control is in focus, it belongs to this dialog, and it's enabled, then give
    // it the first chance at handling the message.
    if(s_pControlFocus &&
       s_pControlFocus->m_pFrame == this &&
       s_pControlFocus->GetEnabled())
    {
        // If the control MsgProc handles it, then we don't.
		result = s_pControlFocus->MsgProc(uMsg, wParam, lParam);
        if(result)
            return true;
    }

	switch( uMsg )
    {
        case WM_SIZE:
        case WM_MOVE:
        {
            // Handle sizing and moving messages so that in case the mouse cursor is moved out
            // of an UI control because of the window adjustment, we can properly
            // unhighlight the highlighted control.
            POINT pt =
            {
                -1, -1
            };
            OnMouseMove(pt);
            break;
        }

    //    case WM_ACTIVATEAPP:
    //    // Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
    //    // as the application is activated/deactivated.  This matches the Windows
    //    // behavior.
    //    if(s_pControlFocus &&
    //       s_pControlFocus->m_pFrame == this &&
    //       s_pControlFocus->GetEnabled() )
    //    {
    //        if(wParam)
    //            s_pControlFocus->OnFocusIn();
    //        else
    //            s_pControlFocus->OnFocusOut();
    //    }
    //    break;

    //    // Keyboard messages
    //    case WM_KEYDOWN:
    //    case WM_SYSKEYDOWN:
    //    case WM_KEYUP:
    //    case WM_SYSKEYUP:
    //    {
    //            // If a control is in focus, it belongs to this dialog, and it's enabled, then give
    //            // it the first chance at handling the message.
    //            if( s_pControlFocus &&
    //                s_pControlFocus->m_pDialog == this &&
    //                s_pControlFocus->GetEnabled() )
    //for( auto it = m_Controls.cbegin(); it != m_Controls.cend(); ++it )
    //            {
    //                if( s_pControlFocus->HandleKeyboard( uMsg, wParam, lParam ) )
    //                    return true;
    //            }

    //            // Not yet handled, see if this matches a control's hotkey
    //            // Activate the hotkey if the focus doesn't belong to an
    //            // edit box.
    //            if( uMsg == WM_KEYDOWN && ( !s_pControlFocus ||
    //                                        ( s_pControlFocus->GetType() != DXUT_CONTROL_EDITBOX
    //                                          && s_pControlFocus->GetType() != DXUT_CONTROL_IMEEDITBOX ) ) )
    //            {
    //                for( auto it = m_Controls.begin(); it != m_Controls.end(); ++it )
    //                {
    //                    if( (*it)->GetHotkey() == wParam )
    //                    {
    //                        (*it)->OnHotkey();
    //                        return true;
    //                    }
    //                }
    //            }

    //            // Not yet handled, check for focus messages
    //            if( uMsg == WM_KEYDOWN )
    //            {
    //                // If keyboard input is not enabled, this message should be ignored
    //                if( !m_bKeyboardInput )
    //                    return false;

    //                switch( wParam )
    //                {
    //                    case VK_RIGHT:
    //                    case VK_DOWN:
    //                        if( s_pControlFocus )
    //                        {
    //                            return OnCycleFocus( true );
    //                        }
    //                        break;

    //                    case VK_LEFT:
    //                    case VK_UP:
    //                        if( s_pControlFocus )
    //                        {
    //                            return OnCycleFocus( false );
    //                        }
    //                        break;

    //                    case VK_TAB:
    //                    {
    //                        bool bShiftDown = ( ( GetKeyState( VK_SHIFT ) & 0x8000 ) != 0 );
    //                        return OnCycleFocus( !bShiftDown );
    //                    }
    //                }
    //            }

    //            break;
    //        }


        // Mouse messages
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_XBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        {
            // If not accepting mouse input, return false to indicate the message should still 
            // be handled by the application (usually to move the camera).
            if(!m_bMouseInput)
                return false;

            POINT mousePoint =
            {
                short(LOWORD(lParam)), short(HIWORD(lParam))
            };
            mousePoint.x -= m_x;
            mousePoint.y -= m_y;

            // If a control is in focus, it belongs to this dialog, and it's enabled, then give
            // it the first chance at handling the message.
            //if( s_pControlFocus &&
            //    s_pControlFocus->m_pFrame == this &&
            //    s_pControlFocus->GetEnabled() )
            //{
            //    if( s_pControlFocus->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
            //        return true;
            //}

            // Not yet handled, see if the mouse is over any controls
            auto pControl = GetControlAtPoint(mousePoint);
            if(pControl && pControl->GetEnabled())
            {
                result = pControl->HandleMouse( uMsg, mousePoint, wParam, lParam );
                if(result)
                    return result;
            }
            else
            {
                // Mouse not over any controls in this dialog, if there was a control
                // which had focus it just lost it
                if(uMsg == WM_LBUTTONDOWN &&
                    s_pControlFocus &&
                    s_pControlFocus->m_pFrame == this)
                {
                    s_pControlFocus->OnFocusOut();
                    s_pControlFocus = nullptr;
                }
            }

            // Still not handled, hand this off to the dialog. Return false to indicate the
            // message should still be handled by the application (usually to move the camera).
            switch(uMsg)
            {
                case WM_MOUSEMOVE:
                    OnMouseMove(mousePoint);
                    return false;
            }

            break;
        }
    }
	
	return false;
}

DX11WGuiResourceManager::DX11WGuiResourceManager():
    m_pVSRenderUI11(nullptr),
	m_pPSRenderUI11(nullptr),
    m_pPSRenderUIUntex11(nullptr),
    m_pInputLayout11(nullptr),
    m_pVBScreenQuad11(nullptr),
	m_pVBScreen4Quad11(nullptr)
{
    // D3D11 specific
    m_pd3d11Device = nullptr;
	m_pd3d11DeviceContext = nullptr;

	m_nBaseWidth = 2560;
	m_nBaseHeight = 1440;
	m_pFont = AC_NEW DX11WFont();
	m_pText = AC_NEW DX11WText();

    // States
    m_pDepthStencilStateUI11 = nullptr;
    m_pRasterizerStateUI11 = nullptr;
    m_pBlendStateUI11 = nullptr;
    m_pSamplerStateUI11 = nullptr;

	// Stored states
    m_pDepthStencilStateStored11 = nullptr;
    m_pRasterizerStateStored11 = nullptr;
    m_pBlendStateStored11 = nullptr;
    m_pSamplerStateStored11 = nullptr;
}

DX11WGuiResourceManager::~DX11WGuiResourceManager()
{
	delete m_pFont;
	delete m_pText;
}

bool DX11WGuiResourceManager::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Empty in DXUT
	return false;
}

HRESULT DX11WGuiResourceManager::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext)
{
	m_pd3d11Device = pd3dDevice;
    m_pd3d11DeviceContext = pd3d11DeviceContext;

    HRESULT hr = S_OK;

    // Load and compile the two shaders
    ID3DBlob *pVSBlob = nullptr;
	ID3DBlob* pPSBlob = nullptr;
	ID3DBlob *pPSUntexBlob = nullptr;

	// TODO use D3DCompile and our alpha resource loader
	V_RETURN(D3DCompile(g_strUIEffectFile, g_uUIEffectFileSize, "none", nullptr, nullptr, "VS", "vs_4_0_level_9_1", 
         D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &pVSBlob, nullptr));

	V_RETURN(D3DCompile(g_strUIEffectFile, g_uUIEffectFileSize, "none", nullptr, nullptr, "PS", "ps_4_0_level_9_1", 
         D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &pPSBlob, nullptr));

	V_RETURN(D3DCompile(g_strUIEffectFile, g_uUIEffectFileSize, "none", nullptr, nullptr, "PSUntex", "ps_4_0_level_9_1", 
         D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &pPSUntexBlob, nullptr));

    // Encapsulate both shaders into shader objects
    V_RETURN(pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVSRenderUI11));
	// DXUT_SetDebugName( m_pVSRenderUI11, "CDXUTDialogResourceManager" ); TODO profileing
	V_RETURN(pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPSRenderUI11));
	// DXUT_SetDebugName( m_pPSRenderUI11, "CDXUTDialogResourceManager" ); TODO profileing
    V_RETURN(pd3dDevice->CreatePixelShader(pPSUntexBlob->GetBufferPointer(), pPSUntexBlob->GetBufferSize(), NULL, &m_pPSRenderUIUntex11));
	// DXUT_SetDebugName( m_pPSRenderUIUntex11, "CDXUTDialogResourceManager" ); TODO profileing

	// Depth Stencil State
    D3D11_DEPTH_STENCIL_DESC DSDesc;
    ZeroMemory(&DSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    DSDesc.DepthEnable = FALSE;
    DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DSDesc.DepthFunc = D3D11_COMPARISON_LESS;
    DSDesc.StencilEnable = FALSE;
    V_RETURN(pd3dDevice->CreateDepthStencilState(&DSDesc, &m_pDepthStencilStateUI11));
    // DXUT_SetDebugName( m_pDepthStencilStateUI11, "CDXUTDialogResourceManager" );

	// Rasterizer State
    D3D11_RASTERIZER_DESC RSDesc;
    RSDesc.AntialiasedLineEnable = FALSE;
    RSDesc.CullMode = D3D11_CULL_BACK;
    RSDesc.DepthBias = 0;
    RSDesc.DepthBiasClamp = 0.0f;
    RSDesc.DepthClipEnable = TRUE;
    RSDesc.FillMode = D3D11_FILL_SOLID;
    RSDesc.FrontCounterClockwise = FALSE;
    RSDesc.MultisampleEnable = TRUE;
    RSDesc.ScissorEnable = FALSE;
    RSDesc.SlopeScaledDepthBias = 0.0f;
    V_RETURN(pd3dDevice->CreateRasterizerState(&RSDesc, &m_pRasterizerStateUI11));
    // DXUT_SetDebugName( m_pRasterizerStateUI11, "CDXUTDialogResourceManager" );

	// Blend State
    D3D11_BLEND_DESC BSDesc;
    ZeroMemory( &BSDesc, sizeof( D3D11_BLEND_DESC ) );
    BSDesc.RenderTarget[0].BlendEnable = TRUE;
    BSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BSDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

    V_RETURN( pd3dDevice->CreateBlendState( &BSDesc, &m_pBlendStateUI11 ) );
    // DXUT_SetDebugName( m_pBlendStateUI11, "CDXUTDialogResourceManager" );

	// Sampler State
    D3D11_SAMPLER_DESC SSDesc;
    ZeroMemory( &SSDesc, sizeof( D3D11_SAMPLER_DESC ) );
    SSDesc.Filter = D3D11_FILTER_ANISOTROPIC   ;
    SSDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SSDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SSDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SSDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SSDesc.MaxAnisotropy = 16;
    SSDesc.MinLOD = 0;
    SSDesc.MaxLOD = D3D11_FLOAT32_MAX;
    if ( pd3dDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_9_3 )
    {
        SSDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        SSDesc.MaxAnisotropy = 0;
    }
    V_RETURN( pd3dDevice->CreateSamplerState( &SSDesc, &m_pSamplerStateUI11 ) );
    // DXUT_SetDebugName( m_pSamplerStateUI11, "CDXUTDialogResourceManager" );

    // Create the input layout object
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    V_RETURN(pd3dDevice->CreateInputLayout(ied, 3, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout11));

    // Release the blobs .. oh fuck run away they Blob is gonna get you!!!
    SAFE_RELEASE(pVSBlob);
	SAFE_RELEASE(pPSBlob);
    SAFE_RELEASE(pPSUntexBlob);

	// create the vertex buffer
    D3D11_BUFFER_DESC BufDesc;
    ZeroMemory(&BufDesc, sizeof(BufDesc));
	BufDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    BufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 4;             // size is the VERTEX struct * x
    BufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    BufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
	BufDesc.MiscFlags = 0;
    V_RETURN(pd3dDevice->CreateBuffer(&BufDesc, NULL, &m_pVBScreenQuad11));// create the buffer

    ZeroMemory(&BufDesc, sizeof(BufDesc));
	BufDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    BufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 24;             // size is the VERTEX struct * x
    BufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    BufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
	BufDesc.MiscFlags = 0;
    V_RETURN(pd3dDevice->CreateBuffer(&BufDesc, NULL, &m_pVBScreen4Quad11));// create the buffer

	// Init the D3D11Font\text classes
	m_pFont->Initialize(pd3dDevice, "Art\\Font.dds");
	m_pText->InitText(m_pFont, m_pInputLayout11);

    return S_OK;
}

HRESULT DX11WGuiResourceManager::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)// TODO MAJKE THIS GET CALLL AT ALL APPROAPRAITE PLACES
{
    //UNREFERENCED_PARAMETER(pd3dDevice);

    HRESULT hr = S_OK;

    m_nBackBufferWidth = pBackBufferSurfaceDesc->Width;
    m_nBackBufferHeight = pBackBufferSurfaceDesc->Height;

    return hr;
}

void DX11WGuiResourceManager::OnD3D11ReleasingSwapChain()
{
	// Empty in DXUT
}

void DX11WGuiResourceManager::OnD3D11DestroyDevice()
{
	// Font
	m_pText->Release();
	m_pFont->Release();

    // D3D11
	SAFE_RELEASE(m_pVBScreen4Quad11);
    SAFE_RELEASE(m_pVBScreenQuad11);
    SAFE_RELEASE(m_pInputLayout11);

	// Sprited TODO implement if needed
    //SAFE_RELEASE(m_pSpriteBuffer11);
    //m_SpriteBufferBytes11 = 0;

    // Shaders
    SAFE_RELEASE(m_pVSRenderUI11);
	SAFE_RELEASE(m_pPSRenderUI11);
    SAFE_RELEASE(m_pPSRenderUIUntex11);

	//for(auto it = m_TextureCache.begin(); it != m_TextureCache.end(); ++it)
 //   {
 //       SAFE_RELEASE((*it)->pTexResView11);
 //       SAFE_RELEASE((*it)->pTexture11);
 //   }

    // States
    SAFE_RELEASE(m_pDepthStencilStateUI11);
    SAFE_RELEASE(m_pRasterizerStateUI11);
    SAFE_RELEASE(m_pBlendStateUI11);
    SAFE_RELEASE(m_pSamplerStateUI11);

    SAFE_RELEASE(m_pDepthStencilStateStored11);
    SAFE_RELEASE(m_pRasterizerStateStored11);
    SAFE_RELEASE(m_pBlendStateStored11);
    SAFE_RELEASE(m_pSamplerStateStored11);

    //EndFont11();
}

void DX11WGuiResourceManager::StoreD3D11State(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->OMGetDepthStencilState(&m_pDepthStencilStateStored11, &m_StencilRefStored11);
	pd3dImmediateContext->RSGetState(&m_pRasterizerStateStored11);
	pd3dImmediateContext->OMGetBlendState(&m_pBlendStateStored11, m_BlendFactorStored11, &m_SampleMaskStored11);
	pd3dImmediateContext->PSGetSamplers(0, 1, &m_pSamplerStateStored11);
}

void DX11WGuiResourceManager::RestoreD3D11State(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->OMSetDepthStencilState(m_pDepthStencilStateStored11, m_StencilRefStored11);
	pd3dImmediateContext->RSSetState(m_pRasterizerStateStored11);
	pd3dImmediateContext->OMSetBlendState(m_pBlendStateStored11, m_BlendFactorStored11, m_SampleMaskStored11);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerStateStored11);

	SAFE_RELEASE(m_pDepthStencilStateStored11);
	SAFE_RELEASE(m_pRasterizerStateStored11);
	SAFE_RELEASE(m_pBlendStateStored11);
	SAFE_RELEASE(m_pSamplerStateStored11);
}

void DX11WGuiResourceManager::ApplyRenderUI11(ID3D11DeviceContext* pd3dImmediateContext)
{
    // Shaders
    pd3dImmediateContext->VSSetShader(m_pVSRenderUI11, nullptr, 0);
    pd3dImmediateContext->HSSetShader(nullptr, nullptr, 0);
    pd3dImmediateContext->DSSetShader(nullptr, nullptr, 0);
    pd3dImmediateContext->GSSetShader(nullptr, nullptr, 0);
    pd3dImmediateContext->PSSetShader(m_pPSRenderUI11, nullptr, 0);

    // States
    pd3dImmediateContext->OMSetDepthStencilState(m_pDepthStencilStateUI11, 0);
    pd3dImmediateContext->RSSetState(m_pRasterizerStateUI11);
    float BlendFactor[4] = { 0, 0, 0, 0 };
    pd3dImmediateContext->OMSetBlendState(m_pBlendStateUI11, BlendFactor, 0xFFFFFFFF);
    pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerStateUI11);
}

void DX11WGuiResourceManager::ApplyRenderUIUntex11(ID3D11DeviceContext* pd3dImmediateContext)
{
	// Shaders
    pd3dImmediateContext->VSSetShader(m_pVSRenderUI11, nullptr, 0);
    pd3dImmediateContext->HSSetShader(nullptr, nullptr, 0);
    pd3dImmediateContext->DSSetShader(nullptr, nullptr, 0);
    pd3dImmediateContext->GSSetShader(nullptr, nullptr, 0);
    pd3dImmediateContext->PSSetShader(m_pPSRenderUIUntex11, nullptr, 0);

	// States
    pd3dImmediateContext->OMSetDepthStencilState( m_pDepthStencilStateUI11, 0 );
    pd3dImmediateContext->RSSetState(m_pRasterizerStateUI11);
    float BlendFactor[4] = { 0, 0, 0, 0 };
    pd3dImmediateContext->OMSetBlendState( m_pBlendStateUI11, BlendFactor, 0xFFFFFFFF );
    pd3dImmediateContext->PSSetSamplers( 0, 1, &m_pSamplerStateUI11 );
}

int DX11WGuiResourceManager::AddFont(LPCWSTR strFaceName, LONG height, LONG weight) 
{
    // See if this font already exists
    for(size_t i = 0; i < m_FontCache.size(); ++i)
    {
        auto pFontNode = m_FontCache[i];
        size_t nLen = 0;
        nLen = wcsnlen(strFaceName, MAX_PATH);
        if(0 == _wcsnicmp(pFontNode->strFace, strFaceName, nLen) &&
            pFontNode->nHeight == height &&
            pFontNode->nWeight == weight)
        {
            return static_cast<int>(i);
        }
    }

    // Add a new font and try to create it
    auto pNewFontNode = new (std::nothrow) DX11WFontNode;
    if(!pNewFontNode)
        return -1;

    ZeroMemory(pNewFontNode, sizeof(DX11WFontNode));
    wcscpy_s(pNewFontNode->strFace, MAX_PATH, strFaceName);
    pNewFontNode->nHeight = height;
    pNewFontNode->nWeight = weight;
    m_FontCache.push_back(pNewFontNode);

    int iFont = (int)m_FontCache.size() - 1;

    // If a device is available, try to create immediately
    return iFont;
}

//======================================================================================
// DX11WUIControl class
//======================================================================================
DX11WUIControl::DX11WUIControl(DXW11UIFrame* pFrame)
{
    m_Type = DX11W_CONTROL_BUTTON;
    m_pFrame = pFrame;
    m_ID = 0;
    m_nHotkey = 0; 
    m_Index = 0;
    m_pUserData = nullptr;

    m_bEnabled = true;
    m_bVisible = true;
    m_bMouseOver = false;
    m_bHasFocus = false;
    m_bIsDefault = false;

    m_pFrame = nullptr;// idk why this is here but its in DXUT

    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;

    ZeroMemory(&m_rcBoundingBox, sizeof(m_rcBoundingBox));
}

DX11WUIControl::~DX11WUIControl()
{
	for(auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
	{
		auto pElement = *it;
		delete pElement;
	}
	m_Elements.clear();
}

HRESULT DX11WUIControl::OnInit() 
{
	m_Background.Init(m_pFrame->GetManager());
	m_Background.Update();

	//HighLightBlendColor.Init(m_borderColor);
	//HighLightBlendColor.States[DX11W_STATE_MOUSEOVER] = m_colorHighLight;
	//m_Elements[0].
	return S_OK; 
}

void DX11WUIControl::Refresh()
{
    m_bMouseOver = false;// not sure why this is set here, must incvestigaste 
    m_bHasFocus = false;

    for(auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
    {
        (*it)->Refresh();
    }
}

HRESULT DX11WUIControl::VOnRestore()
{
	return S_OK;
}

HRESULT DX11WUIControl::VOnLostDevice()
{
	return S_OK;
}

HRESULT DX11WUIControl::VOnRender(double fTime, float fElapsedTime)
{
	VOnUpdate(fElapsedTime);
	m_Background.Draw();
	return S_OK;
}

void DX11WUIControl::VOnUpdate(float fElapsedTime)
{
	if(m_bChanged)
	{
		UpdateRects();

		POINT Pt;
		m_pFrame->GetLocation(Pt);

		m_Background.SetX(Pt.x + m_x);
		m_Background.SetY(Pt.y + m_y);
		m_Background.Update();
		//HighLightBlendColor.States[DX11W_STATE_MOUSEOVER] = m_colorHighLight;// this cant be the right place can it?
		m_bChanged = false;
	} 
	//if((iState == DX11W_STATE_PRESSED))// && //m_bColorBlending)
	//{
	//	XMFLOAT4 pressedColor = D3DCOLOR_TO_D3DCOLORVALUE(HighLightBlendColor.States[DX11W_STATE_PRESSED]);
	//	int aryLen = sizeof(m_HighLightVertices)/sizeof(DXUT_SCREEN_VERTEX_10);
	//	for(int i = 0; i < aryLen; ++i)
	//	{
	//		m_HighLightVertices[i].color = pressedColor;
	//	}
	//	//m_bColorBlending = false;
	//}
	//else if((m_bMouseOver || m_bHasFocus) && m_bColorBlending)
	//{
	//	HighLightBlendColor.Blend(DX11W_STATE_MOUSEOVER, fElapsedTime);//, 0.7f);
	//	int aryLen = sizeof(m_HighLightVertices)/sizeof(DXUT_SCREEN_VERTEX_10);
	//	for(int i = 0; i < aryLen; ++i)
	//	{
	//		m_HighLightVertices[i].color = HighLightBlendColor.Current;
	//	}
	//	DWORD compareColor = D3DCOLORVALUE_TO_D3DCOLOR(m_HighLightVertices[0].color);
	//	if(compareColor == HighLightBlendColor.States[DX11W_STATE_MOUSEOVER])
	//		m_bColorBlending = false;

	//}
	//else if(m_bColorBlending)
	//{
	//	HighLightBlendColor.Blend(0, fElapsedTime);//, 0.7f);
	//	int aryLen = sizeof(m_HighLightVertices)/sizeof(DXUT_SCREEN_VERTEX_10);
	//	for(int i = 0; i < aryLen; ++i)
	//	{
	//		m_HighLightVertices[i].color = HighLightBlendColor.Current;
	//	}
	//	DWORD compareColor = D3DCOLORVALUE_TO_D3DCOLOR(m_HighLightVertices[0].color);
	//	if(compareColor == HighLightBlendColor.States[0])
	//		m_bColorBlending = false;
	//}
}    

void DX11WUIControl::SetLocation(int x, int y)
{
	POINT Pt;
	m_pFrame->GetLocation(Pt);
	m_x = x; 
	m_y = y;
	m_Background.SetX(Pt.x + m_x);
	m_Background.SetY(Pt.y + m_y);
	m_bChanged = true;
}

void DX11WUIControl::UpdateRects()
{
    SetRect(&m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height);
}

void DX11WUIControl::SetTextColor(DWORD Color)
{
	auto pElement = m_Elements[0];

    if(pElement)
        pElement->FontColor.States[DX11W_STATE_NORMAL] = Color;
}

HRESULT DX11WUIControl::SetElement(UINT iElement, DX11WElement* pElement)
{
    if(!pElement)
        return E_INVALIDARG;

    // Make certain the array is this large
    for(size_t i = m_Elements.size(); i <= iElement; i++)
    {
        auto pNewElement = new (std::nothrow) DX11WElement();
        if(!pNewElement)
            return E_OUTOFMEMORY;

        m_Elements.push_back(pNewElement);
    }

    // Update the data
    auto pCurElement = m_Elements[iElement];
    *pCurElement = *pElement;

    return S_OK;
}

//======================================================================================
// CDXUTStatic class
//======================================================================================
DX11WStatic::DX11WStatic(DXW11UIFrame* pFrame)
{
    m_Type = DX11W_CONTROL_STATIC;
    m_pFrame = pFrame;

    ZeroMemory(&m_strText, sizeof(m_strText));

    for(auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
    {
        auto pElement = *it;
        SAFE_DELETE(pElement);
    }

    m_Elements.clear();
	m_Background.SetHasBackground(false);
	m_Background.SetHasBorder(false);
}

HRESULT DX11WStatic::VOnRender(double fTime, float fElapsedTime)
{
	VOnUpdate(fElapsedTime);

	if(m_bVisible == false)
        return S_OK;

	m_Background.Draw();

    DX11W_CONTROL_STATE iState = DX11W_STATE_NORMAL;

    if(m_bEnabled == false)
        iState = DX11W_STATE_DISABLED;

	auto pElement = m_Elements[0];// asuuiming there will be elemetnst mmayber check for it later if it crashes

	pElement->FontColor.Blend(iState, fElapsedTime); // se if you can only call when needed

	m_pFrame->DrawTextDX11W(m_strText, pElement, &m_rcBoundingBox, false, true, (0.45f*m_height));// make a variable for centered

	return S_OK;
}

HRESULT DX11WStatic::GetTextCopy(LPWSTR strDest, UINT bufferCount) const
{
    // Validate incoming parameters
    if(!strDest || bufferCount == 0)
    {
        return E_INVALIDARG;
    }

    // Copy the window text
    wcscpy_s(strDest, bufferCount, m_strText);

    return S_OK;
}

HRESULT DX11WStatic::SetText(LPCWSTR strText)
{
    if(!strText)
    {
        m_strText[0] = 0;
        return S_OK;
    }

    wcscpy_s(m_strText, MAX_PATH, strText);
    return S_OK;
}

//======================================================================================
// CDXUTButton class
//======================================================================================
DX11WButton::DX11WButton(DXW11UIFrame* pFrame)
{
    m_Type = DX11W_CONTROL_BUTTON;
    m_pFrame = pFrame;

	m_Background.SetHasBackground(true);
	m_Background.SetHasBorder(true);
    m_bPressed = false;
    m_nHotkey = 0;
}

//bool DX11WButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//    UNREFERENCED_PARAMETER(lParam);
//
//    if( !m_bEnabled || !m_bVisible )
//        return false;
//
//    switch( uMsg )
//    {
//        case WM_KEYDOWN:
//        {
//            switch( wParam )
//            {
//                case VK_SPACE:
//                    m_bPressed = true;
//                    return true;
//            }
//        }
//
//        case WM_KEYUP:
//        {
//            switch( wParam )
//            {
//                case VK_SPACE:
//                    if( m_bPressed == true )
//                    {
//                        m_bPressed = false;
//                        m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this );
//                    }
//                    return true;
//            }
//        }
//    }
//    return false;
//}

bool DX11WButton::HandleMouse(UINT uMsg, const POINT& pt, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    if(!m_bEnabled || !m_bVisible)
        return false;

    switch(uMsg)
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            {
                if(ContainsPoint(pt))
                {
                    // Pressed while inside the control
                    m_bPressed = true;
                    SetCapture(g_pDX11W->GetHWND());

                    if(!m_bHasFocus)
                        //m_pFrame->RequestFocus(this);

                    return true;
                }
                break;
            }
        case WM_LBUTTONUP:
        {
            if(m_bPressed)
            {
                m_bPressed = false;
                ReleaseCapture();

                //if(!m_pDialog->m_bKeyboardInput)
                //    m_pDialog->ClearFocus();

                // Button click
                if(ContainsPoint(pt))
                    m_pFrame->SendEvent(EVENT_BUTTON_CLICKED, true, this);

                return true;
            }
            break;
        }
    };

    return false;
}

HRESULT DX11WButton::VOnRender(double fTime, float fElapsedTime)
{
	VOnUpdate(fElapsedTime);

	if(m_bVisible == false)
        return S_OK;

    int nOffsetX = 0;
    int nOffsetY = 0;

    DX11W_CONTROL_STATE iState = DX11W_STATE_NORMAL;

    if(m_bVisible == false)
    {
        iState = DX11W_STATE_HIDDEN;
    }
    else if(m_bEnabled == false)
    {
        iState = DX11W_STATE_DISABLED;
    }
    else if(m_bPressed)
    {
        iState = DX11W_STATE_PRESSED;

        //nOffsetX = 1;
        //nOffsetY = 2;
    }
    else if(m_bMouseOver)
    {
        iState = DX11W_STATE_MOUSEOVER;

        //nOffsetX = -1;
        //nOffsetY = -2;
    }
    else if(m_bHasFocus)
    {
        iState = DX11W_STATE_FOCUS;
    }

    // Background fill layer
    auto pElement = m_Elements[0];

    float fBlendRate = ( iState == DX11W_STATE_PRESSED ) ? 0.0f : 0.8f;

    RECT rcWindow = m_rcBoundingBox;
    OffsetRect( &rcWindow, nOffsetX, nOffsetY );

    // Blend current color
   // pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

	m_Background.SetCurrentBorderColor(D3DCOLORVALUE_TO_D3DCOLOR(pElement->FontColor.Current));
	m_Background.Update();
	m_Background.Draw();

    //m_pDialog->DrawSprite( pElement, &rcWindow, DX11W_FAR_BUTTON_DEPTH );
    m_pFrame->DrawTextDX11W(m_strText, pElement, &rcWindow, false, true, (0.45f*m_height));

   // // Main button
   // pElement = m_Elements[ 1 ];

   // // Blend current color
   //// pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
   // pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

   // // m_pDialog->DrawSprite( pElement, &rcWindow, DX11W_NEAR_BUTTON_DEPTH );
   // m_pFrame->DrawTextDX11W( m_strText, pElement, &rcWindow, false, true );

	return S_OK;
}

