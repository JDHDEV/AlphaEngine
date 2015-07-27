#pragma once
//========================================================================
// MessageBox.h : Defines a modal message dialog
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

#include "../DirectX11Wrapper/DirectX11GUI.h"
/*
//
// class MessageBox - Chapter 10, page 286
//
class MessageBox : public BaseUI
{
protected:
	DXW11Dialog m_UI;             // DirectX dialog
	int m_ButtonId;

public:
	MessageBox(std::wstring msg, std::wstring title, int buttonFlags=MB_OK);
	~MessageBox();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 99; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
	static int Ask(MessageBox_Questions question);

};
*/