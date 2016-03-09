#include "StdAfx.h"
#include "UIWkeBrowser.h"

namespace DuiLib
{
	CWkeBrowserUI::CWkeBrowserUI()
		: m_nButtonState(0)
		, m_fZoom(1.0)
		, m_bEdit(false)
	{
		m_pWebView = NULL;//
	}

	CWkeBrowserUI::~CWkeBrowserUI(void)
	{

	}

	LPCTSTR CWkeBrowserUI::GetClass() const
	{
		return _T("WkeBroswerUI");
	}

	LPVOID CWkeBrowserUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_WKEBROWSER) == 0 ) return static_cast<CWkeBrowserUI*>(this);
		else
			return CControlUI::GetInterface(pstrName);
	}

	void CWkeBrowserUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_SETFOCUS){
			m_pWebView->focus(); 
		}else if (event.Type == UIEVENT_KILLFOCUS){
			m_pWebView->unfocus();
		}else if (event.Type == UIEVENT_TIMER){ 
			m_pWebView->tick();
		}else if (event.Type == UIEVENT_WINDOWSIZE){
			m_pWebView->resize(GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam)); 
		}else if (event.Type == UIEVENT_CHAR){
			unsigned int flags = 0;
			if (HIWORD(event.lParam) & KF_REPEAT)
				flags |= WKE_REPEAT;
			if (HIWORD(event.lParam) & KF_EXTENDED)
				flags |= WKE_EXTENDED;

			m_pWebView->keyPress(event.chKey, flags, false);
		}else if (event.Type == UIEVENT_KEYDOWN){
			unsigned int flags = 0;
			if (HIWORD(event.lParam) & KF_REPEAT)
				flags |= WKE_REPEAT;
			if (HIWORD(event.lParam) & KF_EXTENDED)
				flags |= WKE_EXTENDED;

			m_pWebView->keyDown(event.chKey, flags, false);
			if ( event.chKey == VK_F5 )
				Refresh();
			else if (event.chKey == _T('I') && MK_CONTROL & event.wKeyState)
				SetEditable(m_bEdit == false);
			else if(event.chKey == _T('0') && MK_CONTROL & event.wKeyState)
				SetZoomFactor(1.0);
			else if(event.chKey == VK_OEM_PLUS && MK_CONTROL & event.wKeyState)
				SetZoomFactor(1.5);
		}else if (event.Type == UIEVENT_KEYUP){
			unsigned int flags = 0;
			if (HIWORD(event.lParam) & KF_REPEAT)
				flags |= WKE_REPEAT;
			if (HIWORD(event.lParam) & KF_EXTENDED)
				flags |= WKE_EXTENDED;

			m_pWebView->keyUp(event.chKey, flags, false);
		}else if (event.Type == UIEVENT_CONTEXTMENU){

			m_pWebView->contextMenuEvent(event.ptMouse.x, event.ptMouse.y, m_nButtonState);
		}else if (event.Type == UIEVENT_MOUSEMOVE){

			MouseEvent(WM_MOUSEMOVE,event);
		}else if (event.Type == UIEVENT_BUTTONDOWN){
			m_nButtonState |= WKE_LBUTTON;

			MouseEvent(WM_LBUTTONDOWN,event);
		}else if (event.Type == UIEVENT_BUTTONUP){
			m_nButtonState &= ~WKE_LBUTTON;

			MouseEvent(WM_LBUTTONUP,event);
		}else if (event.Type == UIEVENT_RBUTTONUP){
			m_nButtonState &= ~WKE_RBUTTON;

			MouseEvent(WM_RBUTTONUP,event);
		}else if (event.Type == UIEVENT_RBUTTONDOWN){
			m_nButtonState |= WKE_RBUTTON;

			MouseEvent(WM_RBUTTONDOWN,event);
		}else if (event.Type == UIEVENT_DBLCLICK){

			MouseEvent(WM_LBUTTONDBLCLK,event);
		}else if (event.Type == UIEVENT_SCROLLWHEEL){

			POINT pt = { GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam) };
			::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
			pt.x -= m_rcItem.left;
			pt.y -= m_rcItem.top;
			int nFlag=GET_X_LPARAM(event.wParam);
			int delta = (nFlag==SB_LINEDOWN)?-WHEEL_DELTA:WHEEL_DELTA;

			m_pWebView->mouseWheel(pt.x, pt.y, delta, m_nButtonState);
		}else if (event.Type == UIEVENT_IME_STARTCOMPOSITION){
			wkeRect caret = m_pWebView->getCaret();

			CANDIDATEFORM form;
			form.dwIndex = 0;
			form.dwStyle = CFS_EXCLUDE;
			form.ptCurrentPos.x = caret.x + m_rcItem.left;
			form.ptCurrentPos.y = caret.y + caret.h + m_rcItem.top;
			form.rcArea.top = caret.y + m_rcPaint.top;
			form.rcArea.bottom = caret.y + caret.h +m_rcPaint.top;
			form.rcArea.left = caret.x +m_rcPaint.left;
			form.rcArea.right = caret.x + caret.w +m_rcPaint.left;

			HIMC hIMC = ImmGetContext(m_pManager->GetPaintWindow());
			ImmSetCandidateWindow(hIMC, &form);
			ImmReleaseContext(m_pManager->GetPaintWindow(), hIMC);
		}else
			CControlUI::DoEvent(event); 
	}

	void CWkeBrowserUI::MouseEvent(UINT uMsg,TEventUI& event)
	{
		if (m_pWebView)
			m_pWebView->mouseEvent(uMsg,event.ptMouse.x-m_rcItem.left,event.ptMouse.y-m_rcItem.top, m_nButtonState);
	}

	void CWkeBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName,_T("url")) == 0)	SetUrl(pstrValue);
		else if (_tcscmp(pstrName,_T("editable")) == 0) SetEditable( _tcscmp(pstrValue,_T("true")) == 0);
		else
			CControlUI::SetAttribute(pstrName,pstrValue);
	}

	void CWkeBrowserUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if (m_pWebView == NULL)
			return;

		BitBlt(hDC,m_rcItem.left,m_rcItem.top,m_rcItem.right - m_rcItem.left,
			m_rcItem.bottom - m_rcItem.top, m_pWebView->getViewDC(),0,0,SRCCOPY);
	}

	void CWkeBrowserUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if (m_pWebView)
		{
			m_pWebView->resize(m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top);
			m_pWebView->tick();
		}
	}

	void CWkeBrowserUI::InitBrowser(wkeWebView pWebView,UINT nTimerID)
	{
		m_pWebView = pWebView;
		if (m_pWebView == NULL)
			return;

		m_pWebView->setTransparent(false);
		m_pWebView->setBufHandler((_wkeBufHandler*)this);

		m_pWebView->setPaintWnd(m_pManager->GetPaintWindow());

		CDuiRect rect(m_rcItem);
		m_pWebView->resize(rect.GetWidth(),rect.GetHeight());

		m_pManager->SetTimer(this,nTimerID,50);
		if (m_strUrl.IsEmpty() == false)
			Navigate(m_strUrl);
	}

	void CWkeBrowserUI::onBufUpdated(const HDC hdc,int x, int y, int cx, int cy)
	{
		RECT rcValide={ x, y, x+cx, y+cy };
		::OffsetRect(&rcValide, m_rcItem.left, m_rcItem.top);
		HWND hWnd=m_pManager->GetPaintWindow();
		::InvalidateRect(hWnd, &rcValide, TRUE);
	}

	void CWkeBrowserUI::Navigate(LPCTSTR lpszUrl)
	{
		if (m_pWebView)
			m_pWebView->loadURL(lpszUrl);
	}

	void CWkeBrowserUI::SetUrl(LPCTSTR lpszUrl)
	{
		if (m_strUrl == lpszUrl)
			return;
		m_strUrl = lpszUrl;
	}

	void CWkeBrowserUI::Refresh()
	{
		if (m_pWebView)
			m_pWebView->reload();
	}

	bool CWkeBrowserUI::CanGoBack() const
	{
		return m_pWebView?m_pWebView->canGoBack():false;
	}

	bool CWkeBrowserUI::GoBack()
	{
		return m_pWebView?m_pWebView->goBack():false;
	}

	bool CWkeBrowserUI::CanGoForward() const
	{
		return m_pWebView?m_pWebView->canGoForward():false;
	}

	bool CWkeBrowserUI::GoForward()
	{
		return m_pWebView?m_pWebView->goForward():false;
	}

	void CWkeBrowserUI::SetZoomFactor(float factor)
	{
		if (m_pWebView)
			m_pWebView->setZoomFactor(factor);
	}

	float CWkeBrowserUI::GetZoomFactor()
	{
		return m_fZoom;
	}

	void CWkeBrowserUI::SetEditable(bool bEditable /*= true*/)
	{
		m_bEdit = bEditable;
		if (m_pWebView)
			m_pWebView->setEditable(bEditable);
	}

	void CWkeBrowserUI::SetClientHandler(PwkeClientHandler pwkeClientHandler)
	{
		if (m_pWebView)
			m_pWebView->setClientHandler(pwkeClientHandler);
	}
}