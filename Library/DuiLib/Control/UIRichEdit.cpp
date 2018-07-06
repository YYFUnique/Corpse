#include "stdafx.h"
#include <wchar.h>
#include <oledlg.h>
#include <ctype.h>
#pragma comment(lib,"OleDlg.lib")
// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0

namespace DuiLib {

class COleInPlaceFrame : public IOleInPlaceFrame
{
private:
	ULONG m_cRef;	  			// Object reference count
	HWND  m_hwndFrame;			// Application frame window
	BOOL  m_fHelpMode;			// Context-Sensitive help mode flag

public:
	//
	// Constructors and Destructors
	//
	COleInPlaceFrame(HWND hwndFrame = NULL)
		: m_hwndFrame(hwndFrame), m_cRef(0), m_fHelpMode(FALSE) {}
		~COleInPlaceFrame();

		//
		// IUnknown Interface members
		//
		STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		//
		// IOleWindow Interface members
		//
		STDMETHODIMP GetWindow(HWND* pHwnd);
		STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);

		//
		// IOleInPlaceUIWindow Interface members
		//
		STDMETHODIMP GetBorder(LPRECT prcBorder);
		STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS pbw);
		STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS pbw);
		STDMETHODIMP SetActiveObject(LPOLEINPLACEACTIVEOBJECT pInPlaceActiveObject,
			LPCOLESTR szObjName);

		//
		// IOleInPlaceFrame Interface members
		//
		STDMETHODIMP InsertMenus(HMENU hmenuShared,
			LPOLEMENUGROUPWIDTHS pMenuWidths);
		STDMETHODIMP SetMenu(HMENU hmenuShared, HOLEMENU holemenu,
			HWND hwndActiveObject);
		STDMETHODIMP RemoveMenus(HMENU hmenuShared);
		STDMETHODIMP SetStatusText(LPCOLESTR szStatusText);
		STDMETHODIMP EnableModeless(BOOL fEnable);
		STDMETHODIMP TranslateAccelerator(LPMSG pMsg, WORD wID);

		//
		// UI Helpers
		//
		void ShowUIAndTools(BOOL fShow, BOOL fMenu);
		void ReinstateUI(void);
public:
	static LPOLEINPLACEACTIVEOBJECT g_pActiveObject;
};



COleInPlaceFrame::~COleInPlaceFrame()
{
	// if (m_cRef != 0) DebugMsg(TEXT("WARNING!!! Object destroyed with m_cRef = %d!\r\n"),m_cRef);
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown Interface members
//

HRESULT COleInPlaceFrame::QueryInterface(REFIID riid, LPVOID* ppv)
{
	*ppv = NULL;

	//DebugTrace(TEXT("COleInPlaceFrame::QueryInterface() returns "));

	if (IID_IUnknown == riid)
	{
//		DebugTrace(TEXT("IUnknown.\r\n"));
		*ppv = (LPUNKNOWN) this;
	}
	else if (IID_IOleWindow == riid)
	{
//		DebugTrace(TEXT("IOleWindow.\r\n"));
		*ppv = (LPOLEWINDOW) this;
	}
	else if (IID_IOleInPlaceUIWindow == riid)
	{
//		DebugTrace(TEXT("IOleInPlaceUIWindow.\r\n"));
		*ppv = (LPOLEINPLACEUIWINDOW) this;
	}
	else if (IID_IOleInPlaceFrame == riid)
	{
//		DebugTrace(TEXT("IOleInPlaceFrame.\r\n"));
		*ppv = (LPOLEINPLACEFRAME) this;
	}
	else
	{
//		DebugTrace(TEXT(" E_NOINTERFACE\r\n"));
	}



	if (NULL == *ppv)
		return ResultFromScode(E_NOINTERFACE);

	((LPUNKNOWN) *ppv)->AddRef();
	return NOERROR;
}

ULONG COleInPlaceFrame::AddRef()
{
	m_cRef++;
	//DebugCount(TEXT("COleInPlaceFrame::AddRef().  Count = %d.\r\n"),m_cRef);
	return m_cRef;
}

ULONG COleInPlaceFrame::Release()
{
	m_cRef--;
	//DebugCount(TEXT("COleInPlaceFrame::Release().  Count = %d.\r\n"),m_cRef);

	if (0 == m_cRef)
		delete this;

	return m_cRef;
}



/////////////////////////////////////////////////////////////////////////////
// IOleWindow Interface members
//

//
//  FUNCTION:   COleInPlaceFrame::GetWindow
//
//  PURPOSE:    Returns the window handle of the top level application window.
//
//  PARAMETERS:
//      (out) pHwnd - Pointer where we return the handle of the frame window.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::GetWindow(HWND* pHwnd)
{
//	DebugTrace(TEXT("COleInPlaceFrame::GetWindow.\r\n"));
	if (!pHwnd)
		return E_INVALIDARG;

	*pHwnd = m_hwndFrame;
	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::ContextSensitiveHelp
//
//  PURPOSE:    Notifies the frame that the object has entered context help
//				mode.
//
//  PARAMETERS:
//      (in)  fEnterMode - TRUE if the object is entering help mode, FALSE if
//					 	   leaving help mode.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::ContextSensitiveHelp(BOOL fEnterMode)
{
//	DebugTrace(TEXT("COleInPlaceFrame::ContextSensitiveHelp.\r\n"));
	if (m_fHelpMode != fEnterMode)
	{
		m_fHelpMode = fEnterMode;

		//
		// We're just going to forward the context sensitive help stuff
		// to the inplace active object since we don't support context
		// help ourselves.
		//
		if (g_pActiveObject)
		{
			LPOLEINPLACEOBJECT pInPlaceObject;
			g_pActiveObject->QueryInterface(IID_IOleInPlaceObject,
				(LPVOID*) &pInPlaceObject);
			if (pInPlaceObject)
			{
				pInPlaceObject->ContextSensitiveHelp(fEnterMode);
				pInPlaceObject->Release();
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IOleInPlaceUIWindow Interface members
//

//
//  FUNCTION:   COleInPlaceFrame::GetBorder
//
//  PURPOSE:    The object is asking for a rectangle in which it can put
//				toolbars and similar controls while active in place.
//
//  PARAMETERS:
//      (out) prcBorder - Pointer to a rectangle containing the area that the
//						  object can use.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::GetBorder(LPRECT prcBorder)
{
//	DebugTrace(TEXT("COleInPlaceFrame::GetBorder.\r\n"));
	if (!prcBorder)
		return E_INVALIDARG;

	//
	// Get the client area of the frame window.  We need to subtract the
	// area occupied by any toolbars or status bars we want to keep while
	// the object is active.
	//

	GetClientRect(m_hwndFrame, prcBorder);
	//prcBorder->bottom -= g_pStatusBar->GetHeight();

	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::RequestBorderSpace
//
//  PURPOSE:    Determines whether tools can be installed around the objects
//				window frame while the object is active in place.
//
//  PARAMETERS:
//      (in) pbw - Requested border space
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::RequestBorderSpace(LPCBORDERWIDTHS pbw)
{
//	DebugTrace(TEXT("COleInPlaceFrame::RequestBorderSpace.\r\n"));
	// Accept all requests
	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::SetBorderSpace
//
//  PURPOSE:    Allocates space for the border requested by the active
//				in place object.
//
//  PARAMETERS:
//      (in) pbw - Pointer to the structure containing the requested widths
//				   in pixels of the tools.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//
//  COMMENTS:
//		If the pbw is NULL, then the object is saying it doesn't have any of
//		it's own tools and we can leave our tools visible.  If pbw = 
//		{0, 0, 0, 0), then the object has no tools, but it doesn't want our
//		tools visible either.
//

HRESULT COleInPlaceFrame::SetBorderSpace(LPCBORDERWIDTHS pbw)
{
	RECT rcClient;			// Rectangle of the current frame client area
	RECT rcOldEdit;			// Old rectangle for the edit control
//	DebugTrace(TEXT("COleInPlaceFrame::SetBorderSpace.\r\n"));

	//
	// Remember the previous position of the client window in coordinates
	// relative to the frame window.
	//
	//GetWindowRect(GetDlgItem(m_hwndFrame, IDC_EDIT), &rcOldEdit);
	MapWindowPoints(NULL, m_hwndFrame, (LPPOINT) &rcOldEdit, 2);

	//
	// Get the client window rect.  We also need to subtract the area of any
	// status bars or tools that shouldn't disappear when the object is
	// active.
	//
	GetClientRect(m_hwndFrame, &rcClient); 
	//rcClient.bottom -= g_pStatusBar->GetHeight();

	if (pbw)
	{
		//
		// Set the space for the object tools.
		//
		rcClient.left 	+= pbw->left;
		rcClient.top 	+= pbw->top;
		rcClient.right  -= pbw->right;
		rcClient.bottom -= pbw->bottom;

		//
		// Save the new border widths.  We also need to add space for any
		// status bars or tools that are still visible.
		//
		//CopyRect(&g_rcBorderSpace, pbw);
	}
	else
	{
		//
		// No tools, reset the border space.  If we have any of our own tools
		// such as a status bar we need to add it to this rect.
		//
		//SetRect(&g_rcBorderSpace, 0, 0, 0, 0);
	}

	//
	// Only move the window if the rectangles have changed.
	//
	if (memcmp(&rcClient, &rcOldEdit, sizeof(RECT)))
	{ 
		//::SetWindowPos(m_hwndEdit, 0, rcClient.left,  rcClient.top,  rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_NOZORDER | SWP_NOACTIVATE);

		//DebugTrace(TEXT("Move Window in SetBorderSpace.\r\n"));
	}

	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::SetActiveObject
//
//  PURPOSE:	Called by the object to provide the frame window a direct
// 				channel of communication with the active in-place object.
//
//  PARAMETERS:
//      (in) pInPlaceActiveObject - New active object, or NULL if none.
//		(in) szObjName            - Name of the new active object.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::SetActiveObject(LPOLEINPLACEACTIVEOBJECT pInPlaceActiveObject,
										  LPCOLESTR szObjName)
{

//	DebugTrace(TEXT("COleInPlaceFrame::SetActiveObject.\r\n"));

	//
	// If object we have is same as the object passed then return S_OK.
	//
	if (pInPlaceActiveObject == g_pActiveObject)
		return S_OK;

	//
	// If we already have an active object, free that first.
	//
	if (g_pActiveObject)
		g_pActiveObject->Release();

	//
	// If we're given an object, AddRef it, and also update our global
	// pointer as well.
	//
	if (pInPlaceActiveObject)
		pInPlaceActiveObject->AddRef();

	g_pActiveObject = pInPlaceActiveObject;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IOleInPlaceFrame Interface members
//

//
//  FUNCTION:   COleInPlaceFrame::InsertMenus
//
//  PURPOSE:    Called by the object server to allow the container to insert
//				its menu groups in the composite menu that will be used
//				during the in-place session.
//
//  PARAMETERS:
//      (in) hmenuShared      - Specifies a handle to an empty menu.
//		(in, out) pMenuWidths - Pointers to an array of six LONG values.  The
//								container fills elemts 0, 2, and 4 to reflect
//								the number of menu elements it provies in the
//								File, View, and Window menu groups.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::InsertMenus(HMENU hmenuShared,
									  LPOLEMENUGROUPWIDTHS pMenuWidths)
{
	//DebugTrace(TEXT("COleInPlaceFrame::InsertMenus.\r\n"));
	if (!hmenuShared || !pMenuWidths)
		return E_INVALIDARG;

	//TCHAR szMenuString[64];

	//
	// Add the file menu
	//
	//GetMenuString(g_hmenuMain, 0, szMenuString, sizeof(szMenuString), MF_BYPOSITION);
	//AppendMenu(hmenuShared, MF_POPUP, (UINT) GetSubMenu(g_hmenuMain, 0),szMenuString);
	pMenuWidths->width[0] = 1;

	//
	// Don't want to add the view menu. 
	//
	pMenuWidths->width[2] = 0;

	//
	// Don't have a window menu either.
	//
	pMenuWidths->width[4] = 0;

	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::SetMenu
//
//  PURPOSE:    Installs the composite menu into the window frame containing
//				the object that is being activated in place.
//
//  PARAMETERS:
//      (in) hmenuShared 	  - Handle to the composite menu.
//		(in) holemenu 		  - Handle to the menu descriptor.
//		(in) hwndActiveObject - Handle to the object's active window.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::SetMenu(HMENU hmenuShared, HOLEMENU holemenu,
								  HWND hwndActiveObject)
{
	//::SetMenu(m_hwndFrame, (holemenu ? hmenuShared : g_hmenuMain));
	DrawMenuBar(m_hwndFrame);

	//
	// Pass the menu descriptor to OLE.
	//
	return OleSetMenuDescriptor(holemenu, m_hwndFrame, hwndActiveObject,
		(LPOLEINPLACEFRAME) this, g_pActiveObject);
}


//
//  FUNCTION:   COleInPlaceFrame::RemoveMenus
//
//  PURPOSE:    Called by the object server to give the container a chance
//				to remove it's menu elements from the in-place composite menu.
//
//  PARAMETERS:
//      (in) hmenuShared - Handle to the composite menu.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::RemoveMenus(HMENU hmenuShared)
{
//	DebugTrace(TEXT("COleInPlaceFrame::RemoveMenus.\r\n"));
	if (!hmenuShared)
		return E_INVALIDARG;

	int nResult;

	while ((nResult = GetMenuItemCount(hmenuShared)) && (nResult != -1))
		RemoveMenu(hmenuShared, 0, MF_BYPOSITION);

	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::SetStatusText
//
//  PURPOSE:	Sets and displays status text about the in-place object in
//				the containers frame window status bar.
//
//  PARAMETERS:
//      (in) szStatusText -	String containing the message to display.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::SetStatusText(LPCOLESTR szStatusText)
{
//	DebugTrace(TEXT("COleInPlaceFrame::SetStatusText.\r\n"));

	//
	// This string is sent to us in Unicode.  We need to decide if we should
	// translate it first.
	//
#ifndef UNICODE
	UINT cch = lstrlenW(szStatusText) + 1;
	LPSTR pszMessage = new char[cch];

	ZeroMemory(pszMessage, cch);

	WideCharToMultiByte(CP_ACP, 0, szStatusText, -1, pszMessage, cch, NULL, NULL);
//	g_pStatusBar->ShowMessage(pszMessage);

	delete [] pszMessage;
#else
	//g_pStatusBar->ShowMessage(szStatusText);
#endif

	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::EnableModeless
//
//  PURPOSE:    Enables or disables modeless dialogs that are owned by the
//				frame window.
//
//  PARAMETERS:
//      fEnable - TRUE if the dialogs should be enabled, or FALSE to disable.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::EnableModeless(BOOL fEnable)
{
//	DebugTrace(TEXT("COleInPlaceFrame::EnableModeless.\r\n"));

	//
	// We don't have any modeless dialogs.
	//
	return S_OK;
}


//
//  FUNCTION:   COleInPlaceFrame::TranslateAccelerator
//
//  PURPOSE:    Translates keystrokes intended for the container frame while
//				an object is active in place.
//
//  PARAMETERS:
//      (in) pMsg - Pointer to the message to translate.
//		(in) wID  - Command ID value corresponding to the keystroke in the
//					container provided accelerator table.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

HRESULT COleInPlaceFrame::TranslateAccelerator(LPMSG pMsg, WORD wID)
{
//	DebugTrace(TEXT("COleInPlaceFrame::TranslateAccelerator.\r\n"));
	if (!pMsg)
		return E_INVALIDARG;

	HACCEL m_hAccelInPlace=NULL;

	if (!::TranslateAccelerator(m_hwndFrame, m_hAccelInPlace, pMsg))
		return (S_FALSE);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// UI Helpers
//


//
//  FUNCTION:   COleInPlaceFrame::ShowUIAndTools
//
//  PURPOSE:    Shows or hides the application's menu and/or tools.  This
//				is called as in-place objects are activated and deactivated.
//
//  PARAMETERS:
//      (in) fShow - TRUE if the app should show it's own UI and tools
//		(in) fMenu - TRUE if the app should show it's own menu
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//
//  COMMENTS:
//		fShow will be FALSE if we should hide our own tools.  We don't 
//		currently have any tools so for now we just ignore this.
//

void COleInPlaceFrame::ShowUIAndTools(BOOL fShow, BOOL fMenu)
{
//	DebugTrace(TEXT("COleInPlaceFrame::ShowUIAndTools.\r\n"));
	if (fShow)
	{
		// 
		// First reset the menu if necessary.
		//
		if (fMenu)
			this->SetMenu(NULL, NULL, NULL);

		//
		// Also redraw any tools we had hidden.
		//
		//g_pToolbar->Show(TRUE);
	}
	else
	{
		//g_pToolbar->Show(FALSE);
	}
}


//
//  FUNCTION:   COleInPlaceFrame::ReinstateUI
//
//  PURPOSE:    Restores the applications tools after an object has
//				been deactivated.
//

void COleInPlaceFrame::ReinstateUI(void)
{
	RECT rc;
//	DebugTrace(TEXT("COleInPlaceFrame::ReinstateUI.\r\n"));

	//
	// Set the default border widths
	//
	//SetRect(&rc, 0, g_pToolbar->GetHeight(), 0, 0);
	SetBorderSpace(&rc);

	//
	// Show our menu and tools
	//
	ShowUIAndTools(TRUE, TRUE);
};



/////////////////////////////////////////////////////////////////////////////////////
//
//

CRichEditUI::CRichEditUI() :m_pCallback(NULL), m_pTwh(NULL),m_pRichEditOle(NULL), m_bVScrollBarFixing(false), m_bWantTab(true), m_bWantReturn(true), 
    m_bWantCtrlReturn(true), m_bRich(true), m_bReadOnly(false), m_bWordWrap(false), m_dwTextColor(0), m_iFont(-1), 
    m_iLimitText(cInitTextMax), m_lTwhStyle(ES_MULTILINE), m_bInited(false), m_chLeadByte(0),m_bNum(false),
	m_uButtonState(0),m_dwTipValueColor(0xFFBAC0C5),m_bShowLineNum(FALSE),
	m_bShowCaret(FALSE)
{

#ifndef _UNICODE
	m_fAccumulateDBC =true;
#else
	m_fAccumulateDBC= false;
#endif

	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
	ZeroMemory(&m_rcPos,sizeof(RECT));
}

CRichEditUI::~CRichEditUI()
{
    if( m_pTwh ) {
        m_pTwh->Release();
        m_pManager->RemoveMessageFilter(this);
    }
	if(m_pRichEditOle)
	{
        m_pRichEditOle->Release();
	}

	if(m_pCallback)
	{
        m_pCallback->Release();
	}
}

LPCTSTR CRichEditUI::GetClass() const
{
    return _T("RichEditUI");
}

LPVOID CRichEditUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_RICHEDIT) == 0 ) return static_cast<CRichEditUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

UINT CRichEditUI::GetControlFlags() const
{
    if( !IsEnabled() ) return CControlUI::GetControlFlags();

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

bool CRichEditUI::IsWantTab()
{
    return m_bWantTab;
}

void CRichEditUI::SetWantTab(bool bWantTab)
{
    m_bWantTab = bWantTab;
}

bool CRichEditUI::IsWantReturn()
{
    return m_bWantReturn;
}

void CRichEditUI::SetWantReturn(bool bWantReturn)
{
    m_bWantReturn = bWantReturn;
}

bool CRichEditUI::IsWantCtrlReturn()
{
    return m_bWantCtrlReturn;
}

void CRichEditUI::SetWantCtrlReturn(bool bWantCtrlReturn)
{
    m_bWantCtrlReturn = bWantCtrlReturn;
}

bool CRichEditUI::IsRich()
{
    return m_bRich;
}

void CRichEditUI::SetRich(bool bRich)
{
    m_bRich = bRich;
    if( m_pTwh ) m_pTwh->SetRichTextFlag(bRich);
}

bool CRichEditUI::IsReadOnly()
{
    return m_bReadOnly;
}

void CRichEditUI::SetReadOnly(bool bReadOnly)
{
    m_bReadOnly = bReadOnly;
    if( m_pTwh ) m_pTwh->SetReadOnly(bReadOnly);
}

bool CRichEditUI::GetWordWrap()
{
    return m_bWordWrap;
}

void CRichEditUI::SetWordWrap(bool bWordWrap)
{
    m_bWordWrap = bWordWrap;
    if( m_pTwh ) m_pTwh->SetWordWrap(bWordWrap);
}

int CRichEditUI::GetFont()
{
    return m_iFont;
}

void CRichEditUI::SetFont(int index)
{
    m_iFont = index;
    if( m_pTwh ) {
        m_pTwh->SetFont(GetManager()->GetFont(m_iFont));
    }
}

void CRichEditUI::SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    if( m_pTwh ) {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        _tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -nSize;
        if( bBold ) lf.lfWeight += FW_BOLD;
        if( bUnderline ) lf.lfUnderline = TRUE;
        if( bItalic ) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        if( hFont == NULL ) return;
        m_pTwh->SetFont(hFont);
        ::DeleteObject(hFont);
    }
}

LONG CRichEditUI::GetWinStyle()
{
    return m_lTwhStyle;
}

void CRichEditUI::SetWinStyle(LONG lStyle)
{
    m_lTwhStyle = lStyle;
}

DWORD CRichEditUI::GetTextColor()
{
    return m_dwTextColor;
}

void CRichEditUI::SetTextColor(DWORD dwTextColor)
{
    m_dwTextColor = dwTextColor;
    if( m_pTwh ) {
        m_pTwh->SetColor(dwTextColor);
    }
}

int CRichEditUI::GetLimitText()
{
    return m_iLimitText;
}

void CRichEditUI::SetLimitText(int iChars)
{
    m_iLimitText = iChars;
    if( m_pTwh ) {
        m_pTwh->LimitText(m_iLimitText);
    }
}

long CRichEditUI::GetTextLength(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
#ifdef _UNICODE
    textLenEx.codepage = 1200;
#else
    textLenEx.codepage = CP_ACP;
#endif
    LRESULT lResult;
    TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
    return (long)lResult;
}

CDuiString CRichEditUI::GetText() const
{
    long lLen = GetTextLength(GTL_DEFAULT);
    LPTSTR lpText = NULL;
    GETTEXTEX gt;
    gt.flags = GT_DEFAULT;
#ifdef _UNICODE
    gt.cb = sizeof(TCHAR) * (lLen + 1) ;
    gt.codepage = 1200;
    lpText = new TCHAR[lLen + 1];
    ::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));
#else
    gt.cb = sizeof(TCHAR) * lLen * 2 + 1;
    gt.codepage = CP_ACP;
    lpText = new TCHAR[lLen * 2 + 1];
    ::ZeroMemory(lpText, (lLen * 2 + 1) * sizeof(TCHAR));
#endif
    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
    CDuiString sText(lpText);
    delete[] lpText;
    return sText;
}

void CRichEditUI::SetText(LPCTSTR pstrText)
{
	if (pstrText == NULL || m_sText == pstrText)
		return;
	m_sText = pstrText;
	if( !m_pTwh ) return;
	m_pTwh->SetColor(m_dwTextColor);
	SetSel(0, -1);
	ReplaceSel(pstrText, FALSE);
}

bool CRichEditUI::GetModify() const
{ 
    if( !m_pTwh ) return false;
    LRESULT lResult;
    TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

void CRichEditUI::SetModify(bool bModified) const
{ 
    TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

bool CRichEditUI::GetSel(CHARRANGE &cr) const
{ 
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
	return true;
}

bool CRichEditUI::GetSel(long& nStartChar, long& nEndChar) const
{
    CHARRANGE cr;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
	return true;
}

int CRichEditUI::SetSel(CHARRANGE &cr)
{ 
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

int CRichEditUI::SetSel(long nStartChar, long nEndChar)
{
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

void CRichEditUI::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
#ifdef _UNICODE		
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
#else
    int iLen = _tcslen(lpszNewText);
    LPWSTR lpText = new WCHAR[iLen + 1];
    ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
    ::MultiByteToWideChar(CP_ACP, 0, lpszNewText, -1, (LPWSTR)lpText, iLen) ;
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpText, 0); 
    delete[] lpText;
#endif
}

void CRichEditUI::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

CDuiString CRichEditUI::GetSelText() const
{
    if( !m_pTwh ) return CDuiString();
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    LPWSTR lpText = NULL;
    lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
    ::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
    TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
    CDuiString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int CRichEditUI::SetSelAll()
{
    return SetSel(0, -1);
}

int CRichEditUI::SetSelNone()
{
    return SetSel(-1, 0);
}

bool CRichEditUI::GetZoom(int& nNum, int& nDen) const
{
    LRESULT lResult;
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) return false;
    if (nDen < 0 || nDen > 64) return false;
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetZoomOff()
{
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

WORD CRichEditUI::GetSelectionType() const
{
    LRESULT lResult;
    TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
    return (WORD)lResult;
}

bool CRichEditUI::GetAutoURLDetect() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetAutoURLDetect(bool bAutoDetect)
{
    LRESULT lResult;
    TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
    return (BOOL)lResult == FALSE;
}

DWORD CRichEditUI::GetEventMask() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD CRichEditUI::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult;
    TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
    return (DWORD)lResult;
}

CDuiString CRichEditUI::GetTextRange(long nStartChar, long nEndChar) const
{
    TEXTRANGEW tr = { 0 };
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nEndChar - nStartChar + 1];
    ::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(WCHAR));
    tr.lpstrText = lpText;
    TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
    CDuiString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

void CRichEditUI::HideSelection(bool bHide, bool bChangeStyle)
{
    TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
}

void CRichEditUI::ScrollCaret()
{
    TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

LONG CRichEditUI::GetFirstVisibleLine() const
{
	LRESULT lRet = S_OK;
	TxSendMessage(EM_GETFIRSTVISIBLELINE,0, 0, &lRet);

	return (LONG)lRet;
}

void CRichEditUI::SetMargin(DWORD dwPixels)
{
	LRESULT lRet;
	LRESULT lret = TxSendMessage(EM_SETMARGINS, EC_LEFTMARGIN|EC_USEFONTINFO , dwPixels, &lRet);
	return ;
}

int CRichEditUI::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

int CRichEditUI::AppendText(LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(-1, -1);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

DWORD CRichEditUI::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool CRichEditUI::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTwh ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        CHARFORMAT2W cfw;
        cfw.cbSize = sizeof(CHARFORMAT2W);
        TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
        m_pTwh->SetCharFormat(cfw);
        return true;
    }
    return false;
}

DWORD CRichEditUI::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool CRichEditUI::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	cf.crTextColor = RGB(GetBValue(cf.crTextColor), GetGValue(cf.crTextColor), GetRValue(cf.crTextColor));
	cf.crBackColor = RGB(GetBValue(cf.crBackColor), GetGValue(cf.crBackColor), GetRValue(cf.crBackColor));

	if(m_pManager->IsLayered())
	{
		CRenderEngine::CheckAlphaColor(cf.crTextColor);
		CRenderEngine::CheckAlphaColor(cf.crBackColor);
	}

    if( !m_pTwh ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetWordCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTwh ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
    return (BOOL)lResult == TRUE;
}

DWORD CRichEditUI::GetParaFormat(PARAFORMAT2 &pf) const
{
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    return (DWORD)lResult;
}

bool CRichEditUI::SetParaFormat(PARAFORMAT2 &pf)
{
    if( !m_pTwh ) return false;
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        m_pTwh->SetParaFormat(pf);
        return true;
    }
    return false;
}

bool CRichEditUI::Redo()
{ 
    if( !m_pTwh ) return false;
    LRESULT lResult;
    TxSendMessage(EM_REDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

bool CRichEditUI::Undo()
{ 
    if( !m_pTwh ) return false;
    LRESULT lResult;
    TxSendMessage(EM_UNDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

void CRichEditUI::Clear()
{ 
    TxSendMessage(WM_CLEAR, 0, 0, 0); 
}

void CRichEditUI::Copy()
{ 
    TxSendMessage(WM_COPY, 0, 0, 0); 
}

void CRichEditUI::Cut()
{ 
    TxSendMessage(WM_CUT, 0, 0, 0); 
}

void CRichEditUI::Paste()
{ 
    TxSendMessage(WM_PASTE, 0, 0, 0); 
}

int CRichEditUI::GetLineCount() const
{ 
    if( !m_pTwh ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
    return (int)lResult; 
}

CDuiString CRichEditUI::GetLine(int nIndex, int nMaxLength) const
{
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nMaxLength + 1];
    ::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(WCHAR));
    *(LPWORD)lpText = (WORD)nMaxLength;
    TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
    CDuiString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int CRichEditUI::GetLineIndex(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
    return (int)lResult;
}

int CRichEditUI::GetLineLength(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
    return (int)lResult;
}

bool CRichEditUI::LineScroll(int nLines, int nChars)
{
    LRESULT lResult;
    TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
    return (BOOL)lResult == TRUE;
}

CDuiPoint CRichEditUI::GetCharPos(long lChar) const
{ 
    CDuiPoint pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0); 
    return pt;
}

long CRichEditUI::GetLineFromChar(long nIndex) const
{ 
    if( !m_pTwh ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
    return (long)lResult;
}

CDuiPoint CRichEditUI::GetPosFromChar(UINT nChar) const
{ 
    POINTL pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0); 
    return CDuiPoint(pt.x, pt.y); 
}

int CRichEditUI::GetCharFromPos(CDuiPoint pt) const
{ 
    POINTL ptl = {pt.x, pt.y}; 
    if( !m_pTwh ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
    return (int)lResult; 
}

void  CRichEditUI::GetVisibleCharRange(CHARRANGE& chr)
{
	int nFirstVisibleLine = GetFirstVisibleLine();
	chr.cpMin = GetLineIndex(nFirstVisibleLine);

	//CRect  rcClient;
	RECT* prcClient = m_pTwh->GetClientRect();
	POINT  ptRightBottom = {prcClient->right, prcClient->bottom};
	chr.cpMax = GetCharFromPos(ptRightBottom);
}

void CRichEditUI::EmptyUndoBuffer()
{ 
    TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
}

UINT CRichEditUI::SetUndoLimit(UINT nLimit)
{ 
    if( !m_pTwh ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
    return (UINT)lResult; 
}

long CRichEditUI::StreamIn(int nFormat, EDITSTREAM &es)
{ 
    if( !m_pTwh ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

long CRichEditUI::StreamOut(int nFormat, EDITSTREAM &es)
{ 
    if( !m_pTwh ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult; 
}

bool CRichEditUI::SetOLECallback( IRichEditOleCallback *pCallback )
{
	LRESULT lResult;
	TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback,&lResult);
	if( (BOOL)lResult == TRUE ) { 
		pCallback->AddRef();
		m_pCallback = pCallback;
		return true;
	}
	return false;
}


IRichEditOleCallback *CRichEditUI::GetOLECallback()
{
    return m_pCallback;
}

LPRICHEDITOLE CRichEditUI::GetRichEditOle()
{ 
	LRESULT lResult;
	TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM) (LPVOID*) &m_pRichEditOle,&lResult);
	if( (BOOL)lResult == TRUE ) { 
		m_pRichEditOle->AddRef();
		return m_pRichEditOle;
	}
	return NULL;
}
void CRichEditUI::DoInit()
{
	if(m_bInited)
		return ;

    CREATESTRUCT cs;
    cs.style = m_lTwhStyle;
    cs.x = 0;
    cs.y = 0;
    cs.cy = 0;
    cs.cx = 0;
    cs.lpszName = m_sText.GetData();
    CreateHost(this, &cs, &m_pTwh);
    if (m_pTwh) {
        m_pTwh->SetTransparent(TRUE);
        LRESULT lResult;
        m_pTwh->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
		m_pTwh->GetTextServices()->TxSendMessage(EM_SETEVENTMASK, 0, ENM_DROPFILES|ENM_LINK|ENM_CHANGE, &lResult);
        m_pTwh->OnTxInPlaceActivate(NULL);
        m_pManager->AddMessageFilter(this);
		if (IsEnabled() == false)
			m_pTwh->SetColor(GetManager()->GetDefaultDisabledColor());

		//// 由于SetAttribute 函数在DoInit之前，存在某些属性不能初始化
		//SetRich(m_bRich);
		//SetReadOnly(m_bReadOnly);
		//SetWordWrap(m_bWordWrap);
		//SetTextColor(m_dwTextColor);
		//SetLimitText(m_iLimitText);
    }

	m_bInited= true;
}

HRESULT CRichEditUI::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
    if( m_pTwh ) {
        if( msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN ) {
            if( !m_bWantReturn || (::GetKeyState(VK_CONTROL) < 0 && !m_bWantCtrlReturn) ) {
                if( m_pManager != NULL ) m_pManager->SendNotify((CControlUI*)this, DUI_MSGTYPE_RETURN);
                return S_OK;
            }
        }
		if (msg == WM_KEYDOWN){
			CControlUI* pFocus = m_pManager->GetFocus();
			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wparam;
			event.wKeyState = CPaintManagerUI::MapKeyState();
			event.pSender = pFocus;
			event.dwTimestamp = ::GetTickCount();
			pFocus->Event(event);
		}

        return m_pTwh->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
    }
    return S_FALSE;
}

IDropTarget* CRichEditUI::GetTxDropTarget()
{
    IDropTarget *pdt = NULL;
    if( m_pTwh->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR ) return pdt;
    return NULL;
}

bool CRichEditUI::OnTxViewChanged()
{
    return true;
}

bool CRichEditUI::SetDropAcceptFile(bool bAccept) 
{
	LRESULT lResult;
	DWORD dwMask = GetEventMask();
	if (bAccept)
		dwMask |= ENM_DROPFILES;
	else
		dwMask &= ~ENM_DROPFILES;

	TxSendMessage(EM_SETEVENTMASK, 0, dwMask, // ENM_CHANGE| ENM_CORRECTTEXT | ENM_DRAGDROPDONE | ENM_DROPFILES | ENM_IMECHANGE | ENM_LINK | ENM_OBJECTPOSITIONS | ENM_PROTECTED | ENM_REQUESTRESIZE | ENM_SCROLL | ENM_SELCHANGE | ENM_UPDATE,
		&lResult);

	return (BOOL)lResult == FALSE;
}

void CRichEditUI::OnTxNotify(DWORD iNotify, void *pv)
{
	switch(iNotify)
	{ 
	case EN_CHANGE:
			GetManager()->SendNotify(this, DUI_MSGTYPE_TEXTCHANGED);
		break;
	case EN_DROPFILES:   
	case EN_MSGFILTER:   
	case EN_OLEOPFAILED:   
	case EN_PROTECTED:   
	case EN_SAVECLIPBOARD:   
	case EN_SELCHANGE:   
	case EN_STOPNOUNDO:   
	case EN_LINK:   
	case EN_OBJECTPOSITIONS:   
	case EN_DRAGDROPDONE:   
		{
			if(pv)                        // Fill out NMHDR portion of pv   
			{   
				LONG nId =  GetWindowLong(this->GetManager()->GetPaintWindow(), GWL_ID);   
				NMHDR  *phdr = (NMHDR *)pv;   
				phdr->hwndFrom = this->GetManager()->GetPaintWindow();   
				phdr->idFrom = nId;   
				phdr->code = iNotify;  

				if(SendMessage(this->GetManager()->GetPaintWindow(), WM_NOTIFY, (WPARAM) nId, (LPARAM) pv))   
				{   
					//hr = S_FALSE;   
				}   
			}    
		}
		break;
	}
}

BOOL CRichEditUI::ShowCaret(BOOL fShow)
{
	if (m_bShowCaret == fShow)
		return TRUE;
	m_bShowCaret = fShow;
	InvalidateRect(m_pManager->GetPaintWindow(),&m_rcPos,FALSE);
	return TRUE;
}

void CRichEditUI::SetCaretPos(int x,int y)
{
	m_rcPos.left = x;
	m_rcPos.right += m_rcPos.left;

	m_rcPos.top = y;
	m_rcPos.bottom += m_rcPos.top;
}

bool CRichEditUI::CreateCaret(int xWidth,int yHeight)
{
	m_rcPos.right = xWidth;
	m_rcPos.bottom = yHeight;

	return true;
}

// 多行非rich格式的richedit有一个滚动条bug，在最后一行是空行时，LineDown和SetScrollPos无法滚动到最后
// 引入iPos就是为了修正这个bug
void CRichEditUI::SetScrollPos(SIZE szPos)
{
    int cx = 0;
    int cy = 0;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
		m_pVerticalScrollBar->SetScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
		m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;    
    }
    if( cy != 0 ) {
        int iPos = 0;
        if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
            iPos = m_pVerticalScrollBar->GetScrollPos();
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pVerticalScrollBar->GetScrollPos());
        TxSendMessage(WM_VSCROLL, wParam, 0L, 0);
        if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
            if( cy > 0 && m_pVerticalScrollBar->GetScrollPos() <= iPos )
                m_pVerticalScrollBar->SetScrollPos(iPos);
        }
    }
    if( cx != 0 ) {
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pHorizontalScrollBar->GetScrollPos());
        TxSendMessage(WM_HSCROLL, wParam, 0L, 0);
    }
}

void CRichEditUI::LineUp()
{
    TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
}

void CRichEditUI::LineDown()
{
    int iPos = 0;
    if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
        iPos = m_pVerticalScrollBar->GetScrollPos();
    TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
    if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        if( m_pVerticalScrollBar->GetScrollPos() <= iPos )
            m_pVerticalScrollBar->SetScrollPos(m_pVerticalScrollBar->GetScrollRange());
    }
}

void CRichEditUI::PageUp()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
}

void CRichEditUI::PageDown()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
}

void CRichEditUI::HomeUp()
{
    TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
}

void CRichEditUI::EndDown()
{
    TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void CRichEditUI::LineLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
}

void CRichEditUI::LineRight()
{
    TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
}

void CRichEditUI::PageLeft()
{
    TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
}

void CRichEditUI::PageRight()
{
    TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
}

void CRichEditUI::HomeLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
}

void CRichEditUI::EndRight()
{
    TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
}

void CRichEditUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CControlUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
    {
        if( m_pTwh && m_pTwh->DoSetCursor(NULL, &event.ptMouse) ) {
            return;
        }
    }
	if( event.Type == UIEVENT_SETFOCUS ) {
		if (GetManager()->IsLayered())
			GetManager()->SetTimer(this,IME_RICHEDIT_BLINK_TIMER_ID,GetCaretBlinkTime());
		if( m_pTwh ) {
			m_pTwh->OnTxInPlaceActivate(NULL);
			m_pTwh->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);

			m_bFocused = true;
			Invalidate();
			return;
		}
	}
	if( event.Type == UIEVENT_KILLFOCUS )  {
		if (GetManager()->IsLayered())
			GetManager()->KillTimer(this);
		if( m_pTwh ) {
			m_pTwh->OnTxInPlaceActivate(NULL);
			m_pTwh->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
		}
		m_bFocused = false;
		Invalidate();
		return;
	}
	if( event.Type == UIEVENT_TIMER ) {
		if( m_pTwh ) {
			m_pTwh->GetTextServices()->TxSendMessage(WM_TIMER, event.wParam, event.lParam, 0);
		} 
		if (GetManager()->IsLayered() && event.wParam == IME_RICHEDIT_BLINK_TIMER_ID)
		{
			m_bShowCaret = !m_bShowCaret;
			InvalidateRect(GetManager()->GetPaintWindow(),&m_rcPos,FALSE);
		}
	}
	else if( event.Type == UIEVENT_SCROLLWHEEL ) {
		if( (event.wKeyState & MK_CONTROL) != 0  ) {
			return;
		}
	}
    else if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) 
    {
        return;
    }

    else if( event.Type == UIEVENT_MOUSEMOVE ) 
    {
	    return;
    }
    else if( event.Type == UIEVENT_BUTTONUP ) 
    {
        return;
    }
	else if( event.Type == UIEVENT_MOUSEENTER )
    {
		if( IsEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}
        return;
    }
	else if( event.Type == UIEVENT_MOUSELEAVE )
    {
		if( IsEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
        return;
    }
    if( event.Type > UIEVENT__KEYBEGIN && event.Type < UIEVENT__KEYEND )
    {
        return;
    }
    CContainerUI::DoEvent(event);
}

LPCTSTR CRichEditUI::GetNormalImage()
{
	return m_sNormalImage;
}

void CRichEditUI::SetNormalImage(LPCTSTR pStrImage)
{
	m_sNormalImage = pStrImage;
	Invalidate();
}

LPCTSTR CRichEditUI::GetHotImage()
{
	return m_sHotImage;
}

void CRichEditUI::SetHotImage(LPCTSTR pStrImage)
{
	m_sHotImage = pStrImage;
	Invalidate();
}

LPCTSTR CRichEditUI::GetFocusedImage()
{
	return m_sFocusedImage;
}

void CRichEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
	m_sFocusedImage = pStrImage;
	Invalidate();
}

LPCTSTR CRichEditUI::GetDisabledImage()
{
	return m_sDisabledImage;
}

void CRichEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
	m_sDisabledImage = pStrImage;
	Invalidate();
}

RECT CRichEditUI::GetTextPadding() const
{
	return m_rcTextPadding;
}

void CRichEditUI::SetTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	Invalidate();
}

void CRichEditUI::SetTipValue( LPCTSTR pStrTipValue )
{
	m_sTipValue	= pStrTipValue;
}

LPCTSTR CRichEditUI::GetTipValue()
{
	return m_sTipValue.GetData();
}

void CRichEditUI::SetTipValueColor( LPCTSTR pStrColor )
{
	if( *pStrColor == _T('#')) pStrColor = ::CharNext(pStrColor);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(pStrColor, &pstr, 16);

	m_dwTipValueColor = clrColor;
}

DWORD CRichEditUI::GetTipValueColor()
{
	return m_dwTipValueColor;
}

void CRichEditUI::SetShowLineNum(BOOL bShowLineNumber)
{
	if (m_bShowLineNum == bShowLineNumber)
		return;

	m_bShowLineNum = bShowLineNumber;
	Invalidate();
}

BOOL CRichEditUI::IsShowLineNum()
{
	return m_bShowLineNum;
}

void CRichEditUI::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		if( !m_sDisabledImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		if( !m_sFocusedImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT ) != 0 ) {
		if( !m_sHotImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
			else return;
		}
	}

	if( !m_sNormalImage.IsEmpty() ) {
		if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
		else return;
	}
}

SIZE CRichEditUI::EstimateSize(SIZE szAvailable)
{
	//return CSize(m_rcItem); // 这种方式在第一次设置大小之后就大小不变了
	return CContainerUI::EstimateSize(szAvailable);
}

void CRichEditUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);
    rc = m_rcItem;

    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;
    bool bVScrollBarVisiable = false;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        bVScrollBarVisiable = true;
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    }
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    if( m_pTwh ) {
		RECT rcRich = rc;
		rcRich.left += m_rcTextPadding.left;
		rcRich.right -= m_rcTextPadding.right;
		rcRich.top += m_rcTextPadding.top;
		rcRich.bottom -= m_rcTextPadding.bottom;
		m_pTwh->SetClientRect(&rcRich);
		if( bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing) ) {
			LONG lWidth = rcRich.right - rcRich.left + m_pVerticalScrollBar->GetFixedWidth();
			LONG lHeight = 0;
			SIZEL szExtent = { -1, -1 };
			m_pTwh->GetTextServices()->TxGetNaturalSize(
				DVASPECT_CONTENT, 
				GetManager()->GetPaintDC(), 
				NULL,
				NULL,
				TXTNS_FITTOCONTENT,
				&szExtent,
				&lWidth,
				&lHeight);
			if( lHeight > rcRich.bottom - rcRich.top ) {
                m_pVerticalScrollBar->SetVisible(true);
                m_pVerticalScrollBar->SetScrollPos(0);
                m_bVScrollBarFixing = true;
            }
            else {
                if( m_bVScrollBarFixing ) {
                    m_pVerticalScrollBar->SetVisible(false);
                    m_bVScrollBarFixing = false;
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom};
        m_pVerticalScrollBar->SetPos(rcScrollBarPos);
    }
    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
        m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
    }

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) {
            SetFloatPos(it);
        }
        else {
            pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
        }
    }
}

void CRichEditUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    CControlUI::DoPaint(hDC, rcPaint);

    if( m_pTwh ) {
        RECT rc;
        m_pTwh->GetControlRect(&rc);
        // Remember wparam is actually the hdc and lparam is the update
        // rect because this message has been preprocessed by the window.
        m_pTwh->GetTextServices()->TxDraw(
            DVASPECT_CONTENT,  		// Draw Aspect
            /*-1*/0,				// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            hDC,			        // Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rc,			// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rcPaint,		// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object
        if( m_bVScrollBarFixing ) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTwh->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetManager()->GetPaintDC(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight <= rc.bottom - rc.top ) {
                NeedUpdate();
            }
        }
    }

    if( m_items.GetSize() > 0 ) {
        RECT rc = m_rcItem;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
        if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
        else {
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    pControl->DoPaint(hDC, rcPaint);
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else {
                    if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
        if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
            m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
        }
    }

    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
            m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
        }
    }

	CHARRANGE chRange = {0};
	bool bShow = true;
	if (GetSel(chRange) != false)
		bShow = chRange.cpMin == chRange.cpMax ? true : false;

	if (bShow && GetManager()->IsLayered() && m_bShowCaret && m_bFocused)
		CRenderEngine::DrawColor(hDC,m_rcPos,m_dwCaretColor);
}

void CRichEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
    }
    else if( _tcscmp(pstrName, _T("autovscroll")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_AUTOVSCROLL;
    }
    else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
    }
    else if( _tcscmp(pstrName, _T("autohscroll")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_AUTOHSCROLL;
    }
    else if( _tcscmp(pstrName, _T("wanttab")) == 0 ) {
        SetWantTab(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("wantreturn")) == 0 ) {
        SetWantReturn(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("wantctrlreturn")) == 0 ) {
        SetWantCtrlReturn(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("rich")) == 0 ) {
        SetRich(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("multiline")) == 0 ) {
        if( _tcscmp(pstrValue, _T("false")) == 0 ) m_lTwhStyle &= ~ES_MULTILINE;
    }
    else if( _tcscmp(pstrName, _T("readonly")) == 0 ) {
		if( _tcscmp(pstrValue, _T("true")) == 0 ) {m_lTwhStyle |= ES_READONLY; m_bReadOnly = true; }
    }
    else if( _tcscmp(pstrName, _T("password")) == 0 ) {
		if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_PASSWORD;
    }
	else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) {
		if( _tcscmp(pstrValue, _T("true")) == 0 ) m_bNum = true;
	}
    else if( _tcscmp(pstrName, _T("align")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_lTwhStyle &= ~(ES_CENTER | ES_RIGHT);
            m_lTwhStyle |= ES_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_lTwhStyle &= ~(ES_LEFT | ES_RIGHT);
            m_lTwhStyle |= ES_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_lTwhStyle &= ~(ES_LEFT | ES_CENTER);
            m_lTwhStyle |= ES_RIGHT;
        }
    }
    else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetTextColor(clrColor);
    }
	else if( _tcscmp(pstrName, _T("maxchar")) == 0 ) SetLimitText(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetTextPadding(rcTextPadding);
	}
	else if( _tcscmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
	else if( _tcscmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
	else if( _tcscmp(pstrName, _T("wordwrap")) == 0 ) {
		SetWordWrap(_tcsicmp(pstrValue,_T("true")) == 0);
	}
	else if (_tcsicmp(pstrName, _T("linenum")) == 0)	SetShowLineNum(_tcsicmp(pstrValue, _T("true")) == 0);
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

LRESULT CRichEditUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( !IsVisible() || !IsEnabled() ) return 0;
    if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
    if( uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0 ) return 0;

	if (uMsg == WM_IME_COMPOSITION)
	{
		// 解决微软输入法位置异常的问题
		HIMC hIMC = ImmGetContext(GetManager()->GetPaintWindow());
		if (hIMC) 
		{
			// Set composition window position near caret position
			POINT point;
			if (m_pManager->IsLayered())
			{
				point.x = m_rcPos.left;
				point.y = m_rcPos.top;
			}
			else
				GetCaretPos(&point);
			
			COMPOSITIONFORM Composition;
			Composition.dwStyle = CFS_POINT;
			Composition.ptCurrentPos.x = point.x;
			Composition.ptCurrentPos.y = point.y;
			ImmSetCompositionWindow(hIMC, &Composition);

			ImmReleaseContext(GetManager()->GetPaintWindow(),hIMC);
		}
		return 0;
	}

    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
        if( !m_pTwh->IsCaptured() ) {
            switch (uMsg) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                {
                    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    CControlUI* pHover = GetManager()->FindControl(pt);
                    if(pHover != this) {
                        bWasHandled = false;
                        return 0;
                    }
                }
                break;
            }
        }
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pTwh->IsCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE ) {
            RECT rc;
            m_pTwh->GetControlRect(&rc);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if( uMsg == WM_MOUSEWHEEL ) ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
            if( ::PtInRect(&rc, pt) && !GetManager()->IsCaptured() ) dwHitResult = HITRESULT_HIT;
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
        else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN ) {
            SetFocus();
        }
    }
#ifdef _UNICODE
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
#else
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR ) {
#endif
        if( !IsFocused() ) return 0;
    }
    else if( uMsg == WM_CONTEXTMENU ) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
        CControlUI* pHover = GetManager()->FindControl(pt);
        if(pHover != this) {
            bWasHandled = false;
            return 0;
        }
		GetManager()->SendNotify(this, DUI_MSGTYPE_MENU);
    }
    else
    {
        switch( uMsg ) {
        case WM_HELP:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }

	if(WM_CHAR == uMsg)
	{
		if (m_bNum && isdigit(wParam) == false)
			return false;
#ifndef _UNICODE
		// check if we are waiting for 2 consecutive WM_CHAR messages
		if ( IsAccumulateDBCMode() )
		{
			if ( (GetKeyState(VK_KANA) & 0x1) )
			{
				// turn off accumulate mode
				SetAccumulateDBCMode ( false );
				m_chLeadByte = 0;
			}
			else
			{
				if ( !m_chLeadByte )
				{
					// This is the first WM_CHAR message, 
					// accumulate it if this is a LeadByte.  Otherwise, fall thru to
					// regular WM_CHAR processing.
					if ( IsDBCSLeadByte ( (WORD)wParam ) )
					{
						// save the Lead Byte and don't process this message
						m_chLeadByte = (WORD)wParam << 8 ;

						//TCHAR a = (WORD)wParam << 8 ;
						return 0;
					}
				}
				else
				{
					// This is the second WM_CHAR message,
					// combine the current byte with previous byte.
					// This DBC will be handled as WM_IME_CHAR.
					wParam |= m_chLeadByte;
					uMsg = WM_IME_CHAR;

					// setup to accumulate more WM_CHAR
					m_chLeadByte = 0; 
				}
			}
		}
#endif
	}

    LRESULT lResult = 0;
	HRESULT Hr = TxSendMessage(uMsg, wParam, lParam, &lResult);
	if( Hr == S_OK ) bHandled = bWasHandled;
	else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR )
        bHandled = bWasHandled;
	else if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) {
        if( m_pTwh->IsCaptured() ) bHandled = bWasHandled;
    }
    return lResult;
}

void CRichEditUI::SetAccumulateDBCMode( bool bDBCMode )
{
	m_fAccumulateDBC = bDBCMode;
}

bool CRichEditUI::IsAccumulateDBCMode()
{
	return m_fAccumulateDBC;
}

ITextDocument* CRichEditUI::GetDoc()
{
	/*HRESULT hRet;
	ITextDocument* m_pTextDoc = NULL;
	IRichEditOle* pRichEditOle = NULL;
	TxSendMessage(EM_GETOLEINTERFACE,0, (LPARAM)&pRichEditOle,&hRet);
	hRet = pRichEditOle->QueryInterface(__uuidof(ITextDocument), (void**)&m_pTextDoc);
	pRichEditOle->Release();
	return m_pTextDoc;*/
	if (NULL == m_pTwh)
		return NULL;
	return m_pTwh->GetTextDocument();
}

ITextHost * CRichEditUI::GetTextHost()
{
	if (NULL == m_pTwh)
		return NULL;
	return m_pTwh->GetTextHost();
}

ITextServices* CRichEditUI::GetTextServices() const
{
	if (NULL == m_pTwh)
		return NULL;
	return m_pTwh->GetTextServices();
}

BOOL CRichEditUI::SetOleCallback(IRichEditOleCallback* pCallback)
{
	if (NULL == m_pTwh)
		return FALSE;
	return m_pTwh->SetOleCallback(pCallback);
}

BOOL CRichEditUI::CanPaste(UINT nFormat/* = 0*/)
{
	if (NULL == m_pTwh)
		return FALSE;
	return m_pTwh->CanPaste(nFormat);
}

void CRichEditUI::PasteSpecial(UINT uClipFormat, DWORD dwAspect/* = 0*/, HMETAFILE hMF/* = 0*/)
{
	if (NULL == m_pTwh)
		return;
	m_pTwh->PasteSpecial(uClipFormat, dwAspect, hMF);
}

LPOLEINPLACEACTIVEOBJECT COleInPlaceFrame::g_pActiveObject=NULL;

} // namespace DuiLib
