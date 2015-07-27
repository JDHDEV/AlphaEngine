#pragma once
//========================================================================
// DirectX11Gui.h - defines the DXW11DialogResourceManager and DXW11TextHelper
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

#include "DirectX11Font.h"
#include "DirectX11Text.h"

//--------------------------------------------------------------------------------------
// Defines and macros 
//--------------------------------------------------------------------------------------
#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_SLIDER_VALUE_CHANGED_UP       0x0502

#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class DX11WElement;
class DXW11UIFrame;
class DX11WGuiResourceManager;
class DX11WUIControl;
class DX11WStatic;
class DX11WButton;
struct DX11WElementHolder;
struct DX11WFontNode;
typedef void (CALLBACK*PCALLBACKDXUTGUIEVENT)(UINT nEvent, int nControlID, DX11WUIControl* pControl,
                                                 void* pUserContext);

//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum DX11W_CONTROL_TYPE
{
    DX11W_CONTROL_BUTTON,
    DX11W_CONTROL_STATIC,
    DX11W_CONTROL_CHECKBOX,
    DX11W_CONTROL_RADIOBUTTON,
    DX11W_CONTROL_COMBOBOX,
    DX11W_CONTROL_SLIDER,
    DX11W_CONTROL_EDITBOX,
    DX11W_CONTROL_IMEEDITBOX,
    DX11W_CONTROL_LISTBOX,
    DX11W_CONTROL_SCROLLBAR,
};

enum DX11W_CONTROL_STATE
{
    DX11W_STATE_NORMAL = 0,
    DX11W_STATE_DISABLED,
    DX11W_STATE_HIDDEN,
    DX11W_STATE_FOCUS,
    DX11W_STATE_MOUSEOVER,
    DX11W_STATE_PRESSED,
};

#define MAX_CONTROL_STATES 6

struct DX11WBlendColor
{
    void Init(DWORD defaultColor, DWORD disabledColor = D3DCOLOR_ARGB(255,60,60,60), DWORD hiddenColor = 0);
    void Blend(UINT iState, float fElapsedTime, float fRate = 0.7f);

    DWORD States[MAX_CONTROL_STATES]; // Modulate colors for all possible control states
    XMFLOAT4 Current;

    void SetCurrent(DWORD color);
};

//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------
class DX11WElement
{
public:
    void SetTexture(UINT iTexture, RECT* prcTexture, DWORD defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255));
    void SetFont(UINT iFont, DWORD defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER);

    void Refresh();

    UINT iTexture;          // Index of the texture for this Element 
    UINT iFont;             // Index of the font for this Element
    DWORD dwTextFormat;     // The format argument to DrawText 

    RECT rcTexture;         // Bounding rect of this element on the composite texture

    DX11WBlendColor TextureColor;
    DX11WBlendColor FontColor;
};

class DX11WUIBackground
{
public:
	DX11WUIBackground();

	void Init(DX11WGuiResourceManager* pManager);

	HRESULT Draw();
	void Update();

	int GetX() const { return m_x; };
	void SetX(int x) { m_x = x; m_bChanged = true; };
	int GetY() const { return m_y; };
	void SetY(int y) { m_y = y; m_bChanged = true; };
	int GetWidth() const { return m_width; };
	void SetWidth(int width) { m_width = width; m_bChanged = true; };
	int GetHeight() const { return m_height; };
	void SetHeight(int height) { m_height = height; m_bChanged = true; };
	int GetBorderWidth() const { return m_borderWidth; };
	void SetBorderWidth(int borderWidth) { m_borderWidth = borderWidth; m_bChanged = true; };
	bool GetHasBackground() const { return m_bHasBackground; };
	void SetHasBackground(bool bHasBackground) { m_bHasBackground = bHasBackground; m_bChanged = true; };
	bool GetHasBorder() const { return m_bHasBorder; };
	void SetHasBorder(bool bHasBorder) { m_bHasBorder = bHasBorder; m_bChanged = true; };
	bool GetDrawBackground() const { return m_bDrawBackground; };
	void SetDrawBackground(bool bDrawBackground) { m_bDrawBackground = bDrawBackground; };
	bool GetDrawBorder() const { return m_bDrawBorder; };
	void SetDrawBorder(bool bDrawBorder) { m_bDrawBorder = bDrawBorder; };
	DWORD GetBackgroundColor() const { return m_backgroundColor; };
	void SetBackgroundColor(DWORD backgroundColor) { m_backgroundColor = backgroundColor; m_bColorChanged = true; };
	DWORD GetBorderColor() const { return m_borderColor; };
	void SetBorderColor(DWORD borderColor) { m_borderColor = borderColor; m_bColorChanged = true; };
	DWORD GetCurrentBackgroundColor() const { return m_curBackgroundColor; };
	void SetCurrentBackgroundColor(DWORD backgroundColor) { m_curBackgroundColor = backgroundColor; m_bColorChanged = true; };
	DWORD GetCurrentBorderColor() const { return m_curBorderColor; };
	void SetCurrentBorderColor(DWORD borderColor) { m_curBorderColor = borderColor; m_bColorChanged = true; };

protected:
	void UpdateBackground();
	void UpdateColor();

	DX11WGuiResourceManager* m_pManager;

	int m_x, m_y;
    int m_width, m_height;
	int m_borderWidth;

	bool m_bChanged;
	bool m_bColorChanged;
	bool m_bHasBackground;
	bool m_bHasBorder;
	bool m_bDrawBackground;
	bool m_bDrawBorder;

	DWORD m_backgroundColor;
	DWORD m_borderColor; 
	//m_colorHighLight = D3DCOLOR_ARGB(255,202,176,0);//XMFLOAT4(202.0f/255.0f,176.0f/255.0f,0.0f/255.0f,1.0f);// needs to be set from file

	DWORD m_curBackgroundColor;
    DWORD m_curBorderColor;

	DXUT_SCREEN_VERTEX_10 m_BorderVertices[24];
	DXUT_SCREEN_VERTEX_10 m_BackGroundVertices[4];
};

// my own roll of DXUTDialog when finished if they dont directly make DXW!11 calls move them to userinterface.h
class DXW11UIFrame
{
public:
	bool m_bNonUserEvents;
    bool m_bKeyboardInput;
    bool m_bMouseInput;

private:

	static double s_fTimeRefresh;
    double m_fTimeLastRefresh;

	static DX11WUIControl* s_pControlFocus;        // The control which has focus
    static DX11WUIControl* s_pControlPressed;      // The control currently pressed

    DX11WUIControl* m_pControlMouseOver;           // The control which is hovered over

	bool m_bVisible;
	bool m_bChanged;

    int m_x;
    int m_y;
    int m_width;
    int m_height;
	
	DX11WGuiResourceManager* m_pManager;
	PCALLBACKDXUTGUIEVENT m_pCallbackEvent;
    void* m_pCallbackEventUserContext;

	std::vector<int> m_Textures;   // Index into m_TextureCache;
    std::vector<int> m_Fonts;      // Index into m_FontCache;

	std::vector<DX11WUIControl*> m_Controls;
    std::vector<DX11WElementHolder*> m_DefaultElements;

    //DX11WElement m_CapElement;  // Element for the caption

    DX11WUIControl* m_pNextDialog;
    DX11WUIControl* m_pPrevDialog;

	DWORD m_highlightColor;

	DX11WUIBackground m_Background;

	// Initialize default Elements
    void InitDefaultElements();

	// Windows message handlers
    void OnMouseMove(const POINT& pt);
    void OnMouseUp(const POINT& pt);

public:
	DXW11UIFrame();
    ~DXW11UIFrame();

	// Need to call this now
    void Init(_In_ DX11WGuiResourceManager* pManager, _In_ bool bRegisterDialog = true);

	// Control creation
    HRESULT AddStatic(_In_ int ID, _In_z_ LPCWSTR strText, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ bool bIsDefault=false,
						_Out_opt_ DX11WStatic** ppCreated = nullptr);
	HRESULT AddButton(int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey=0,
                       bool bIsDefault=false, DX11WButton** ppCreated = nullptr );
	HRESULT AddControl(_In_ DX11WUIControl* pControl);
    HRESULT InitControl(_In_ DX11WUIControl* pControl);

	// Control retrieval
    DX11WStatic* GetStatic(_In_ int ID) const
    {
        return reinterpret_cast<DX11WStatic*>(GetControl(ID));//, DX11W_CONTROL_STATIC));
    }

    DX11WUIControl* GetControl(_In_ int ID) const;
    DX11WUIControl* GetControlAtPoint(_In_ const POINT& pt) const;

    //bool GetControlEnabled( _In_ int ID ) const; //TODO this means the controls doent actully need the assocrs funcs
    //void SetControlEnabled( _In_ int ID, _In_ bool bEnabled );

	// Access the default display Elements used when adding new controls
    HRESULT SetDefaultElement(UINT nControlType, UINT iElement, DX11WElement* pElement);
    DX11WElement* GetDefaultElement(UINT nControlType, UINT iElement) const;

	// Methods called by controls
    void SendEvent(UINT nEvent, bool bTriggeredByUser, DX11WUIControl* pControl);
    void RequestFocus(DX11WUIControl* pControl);

    static void WINAPI SetRefreshTime( _In_ float fTime ) { s_fTimeRefresh = fTime; }

    //static CDXUTControl* WINAPI GetNextControl( _In_ CDXUTControl* pControl );
    //static CDXUTControl* WINAPI GetPrevControl( _In_ CDXUTControl* pControl );

    void RemoveControl(int ID);
    void RemoveAllControls();

	// Sets the callback used to notify the app of control events
    void SetCallback(PCALLBACKDXUTGUIEVENT pCallback, void* pUserContext = nullptr);
    void EnableNonUserEvents(bool bEnable) { m_bNonUserEvents = bEnable; }
    void EnableKeyboardInput(bool bEnable) { m_bKeyboardInput = bEnable; }
    void EnableMouseInput(bool bEnable) { m_bMouseInput = bEnable; }
    bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

    // Render helpers
    HRESULT DrawRect(const DX11WRECT* pRect, XMFLOAT4 color);
	HRESULT CalcTextRect(LPCWSTR strText, DX11WElement* pElement, const RECT* prcDest, int nCount = -1);
    HRESULT DrawTextDX11W(LPCWSTR strText, DX11WElement* pElement, const RECT* prcDest, bool bShadow = false,
                      bool bCenter = false,  float fGlyphHeight = 0.0f);
	
	// Attributes
    bool GetVisible() const { return m_bVisible; }
    void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	void GetLocation(POINT& Pt) const { Pt.x = m_x; Pt.y = m_y; }
    void SetLocation(int x, int y) { m_x = x; m_y = y; m_Background.SetX(x); m_Background.SetY(y); m_bChanged = true; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    void SetSize(int width, int height) { m_width = width; m_height = height; m_Background.SetWidth(m_width); m_Background.SetHeight(height); m_bChanged = true; }

	DX11WUIBackground* GetBackground() { return &m_Background; }

	//static void WINAPI SetRefreshTime(float fTime) { s_fTimeRefresh = fTime; }

	void Refresh(); // may be able to mege with on restore need to further explore the differences
    virtual HRESULT VOnRestore();
    virtual HRESULT VOnLostDevice();// if code shows up here make sure it is actually called
    virtual HRESULT VOnRender(double fTime, float fElapsedTime);
    virtual void VOnUpdate(int deltaMilliseconds);// make sure this gets called, its refresh in DXUT

	// Shared resource access. Indexed fonts and textures are shared among
    // all the controls.
    HRESULT SetFont(UINT index, LPCWSTR strFaceName, LONG height, LONG weight);
    //DXUTFontNode* GetFont(UINT index) const;

    virtual LRESULT CALLBACK VOnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	DX11WGuiResourceManager* GetManager() const { return m_pManager; }
};

// my own roll of CDXUTDialogResourceManager 
class DX11WGuiResourceManager
{
public:
    DX11WGuiResourceManager();
    ~DX11WGuiResourceManager();

    bool MsgProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );

    // D3D11 specific
    HRESULT OnD3D11CreateDevice( _In_ ID3D11Device* pd3dDevice, _In_ ID3D11DeviceContext* pd3d11DeviceContext );
    HRESULT OnD3D11ResizedSwapChain( _In_ ID3D11Device* pd3dDevice, _In_ const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );
    void OnD3D11ReleasingSwapChain();
    void OnD3D11DestroyDevice();
    void StoreD3D11State(ID3D11DeviceContext* pd3dImmediateContext);
    void RestoreD3D11State(ID3D11DeviceContext* pd3dImmediateContext);
	void ApplyRenderUI11(ID3D11DeviceContext* pd3dImmediateContext);
	void ApplyRenderUIUntex11(ID3D11DeviceContext* pd3dImmediateContext);

    ID3D11Device* GetD3D11Device() const { return m_pd3d11Device; }
    ID3D11DeviceContext* GetD3D11DeviceContext() const { return m_pd3d11DeviceContext; }

	int AddFont(LPCWSTR strFaceName, LONG height, LONG weight);
    //int AddTexture( _In_z_ LPCWSTR strFilename );
    //int AddTexture( _In_z_ LPCWSTR strResourceName, _In_ HMODULE hResourceModule );

    //bool RegisterDialog( _In_ CDXUTDialog* pDialog );
    //void UnregisterDialog( _In_ CDXUTDialog* pDialog );
    //void EnableKeyboardInputForAllDialogs();

    // Shared between all dialogs

    // Shaders
    ID3D11VertexShader* m_pVSRenderUI11;
	ID3D11PixelShader* m_pPSRenderUI11;
    ID3D11PixelShader* m_pPSRenderUIUntex11;

    // States
    ID3D11DepthStencilState* m_pDepthStencilStateUI11;
    ID3D11RasterizerState* m_pRasterizerStateUI11;
    ID3D11BlendState* m_pBlendStateUI11;
    ID3D11SamplerState* m_pSamplerStateUI11;

	// Stored states
    ID3D11DepthStencilState* m_pDepthStencilStateStored11;
    UINT m_StencilRefStored11;
    ID3D11RasterizerState* m_pRasterizerStateStored11;
    ID3D11BlendState* m_pBlendStateStored11;
    float m_BlendFactorStored11[4];
    UINT m_SampleMaskStored11;
    ID3D11SamplerState* m_pSamplerStateStored11;

	// D3D11
    ID3D11InputLayout* m_pInputLayout11;
    ID3D11Buffer* m_pVBScreenQuad11;
	ID3D11Buffer* m_pVBScreen4Quad11;

    UINT m_nBackBufferWidth;
    UINT m_nBackBufferHeight;

	// Base for all calculations, i belive some refer to this as Metaplane?
	UINT m_nBaseWidth;
	UINT m_nBaseHeight;

    std::vector<DXW11UIFrame*> m_Frames; // Dialogs registered

	// Global text/font functions from DXUT
	DX11WText* m_pText;

protected:
    // D3D11 specific
    ID3D11Device* m_pd3d11Device;
    ID3D11DeviceContext* m_pd3d11DeviceContext;
	//HRESULT CreateTexture11(UINT index);

	DX11WFont* m_pFont;

    //std::vector<DXUTTextureNode*> m_TextureCache;   // Shared textures
    std::vector<DX11WFontNode*> m_FontCache;         // Shared fonts
};

// my own roll of DXUTControl
class DX11WUIControl
{
public: 
	bool m_bVisible;                // Shown/hidden flag
    bool m_bMouseOver;              // Mouse pointer is above control
    bool m_bHasFocus;               // Control has input focus
    bool m_bIsDefault;              // Is the default control

    // Size, scale, and positioning members
    int m_x, m_y;
    int m_width, m_height;

    // These members are set by the container
    DXW11UIFrame* m_pFrame;    // Parent container
    UINT m_Index;              // Index within the control list

    std::vector<DX11WElement*> m_Elements;  // All display elements

protected:
    virtual void UpdateRects();

    int m_ID;					// ID number
    DX11W_CONTROL_TYPE m_Type;	// Control type, set once in constructor  
    UINT m_nHotkey;				// Virtual key code for this control's hotkey
    void* m_pUserData;			// Data associated with this control that is set by user.

    bool m_bEnabled;			// Enabled/disabled flag

    RECT m_rcBoundingBox;		// Rectangle defining the active region of the control

	bool m_bChanged;

	DX11WUIBackground m_Background;

public:
	DX11WUIControl(_In_opt_ DXW11UIFrame* pFrame = nullptr);
    virtual ~DX11WUIControl();

    virtual HRESULT OnInit();

	virtual void Refresh();
	virtual HRESULT VOnRestore(); // if code shows up here make sure it is actually called
    virtual HRESULT VOnLostDevice(); // if code shows up here make sure it is actually called
    virtual HRESULT VOnRender(double fTime, float fElapsedTime);// { UNREFERENCED_PARAMETER(fElapsedTime); UNREFERENCED_PARAMETER(fTime); return S_OK; };
    virtual void VOnUpdate(float fElapsedTime);

	// Windows message handler
    virtual bool MsgProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(uMsg);
        UNREFERENCED_PARAMETER(wParam);
        UNREFERENCED_PARAMETER(lParam);
        return false;
    }

    virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(uMsg);
        UNREFERENCED_PARAMETER(wParam);
        UNREFERENCED_PARAMETER(lParam);
        return false;
    }
    virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT& pt, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(uMsg);
        UNREFERENCED_PARAMETER(pt);
        UNREFERENCED_PARAMETER(wParam);
        UNREFERENCED_PARAMETER(lParam);
        return false;
    }

    virtual bool CanHaveFocus() { return false; }
    virtual void OnFocusIn() { m_bHasFocus = true; }
    virtual void OnFocusOut() { m_bHasFocus = false; }
	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }
    virtual void OnHotkey() { }

    virtual bool ContainsPoint( _In_ const POINT& pt ) { return PtInRect( &m_rcBoundingBox, pt ) != 0; }

    virtual void SetEnabled( _In_ bool bEnabled ) { m_bEnabled = bEnabled; }
    virtual bool GetEnabled() const { return m_bEnabled; }
    virtual void SetVisible( _In_ bool bVisible ) { m_bVisible = bVisible; }
    virtual bool GetVisible() const { return m_bVisible; }

    UINT GetType() const { return m_Type; }

    int GetID() const { return m_ID; }
    void SetID( _In_ int ID ) { m_ID = ID; }

    void SetLocation(int x, int y);
    void SetSize(int width, int height) { m_width = width; m_height = height; m_Background.SetWidth(width); m_Background.SetHeight(height); m_bChanged = true; }

    void SetHotkey( _In_ UINT nHotkey ) { m_nHotkey = nHotkey; }
    UINT GetHotkey() const { return m_nHotkey; }

    void SetUserData( _In_opt_ void* pUserData ) { m_pUserData = pUserData; }
    void* GetUserData() const { return m_pUserData; }
	
	DX11WUIBackground* GetBackground() { return &m_Background; }

    virtual void SetTextColor(DWORD Color);
    DX11WElement* GetElement(UINT iElement) const { return m_Elements[iElement]; }
    HRESULT SetElement(UINT iElement, DX11WElement* pElement);

};

//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct DX11WElementHolder
{
    UINT nControlType;
    UINT iElement;

    DX11WElement Element;
};

//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
class DX11WStatic : public DX11WUIControl
{
public:
    DX11WStatic(DXW11UIFrame* pFrame = nullptr);

    virtual HRESULT VOnRender(double fTime, float fElapsedTime) override;
    virtual bool ContainsPoint(const POINT& pt) override
    {
        UNREFERENCED_PARAMETER(pt);
        return false;
    }

    HRESULT GetTextCopy(LPWSTR strDest, UINT bufferCount) const;
    LPCWSTR GetText() const { return m_strText; }
    HRESULT SetText(LPCWSTR strText);

protected:
    WCHAR m_strText[MAX_PATH]; // Window text  
};

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class DX11WButton : public DX11WStatic
{
public:
    DX11WButton(DXW11UIFrame* pFrame = nullptr);

    //virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual bool HandleMouse(UINT uMsg, const POINT& pt, WPARAM wParam, LPARAM lParam) override;
	//virtual void OnHotkey() override
	//{
	//	if(m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this );
	//	m_pDialog->SendEvent(EVENT_BUTTON_CLICKED, true, this);
	//}

    virtual bool ContainsPoint(const POINT& pt) override
    {
        return PtInRect(&m_rcBoundingBox, pt) != 0;
    }
    virtual bool CanHaveFocus() override
    {
        return (m_bVisible && m_bEnabled);
    }

    virtual HRESULT VOnRender(double fTime, float fElapsedTime) override;

protected:
    bool m_bPressed;
};
