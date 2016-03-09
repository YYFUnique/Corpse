#include "StdAfx.h"
#include "BaseDialog.h"
#include "360Control.h"
#include "Resource.h"
#include <memory>

BaseDialog::BaseDialog()
{
	
}

void BaseDialog::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit"))
	{
		//SetIcon(IDI_SMALL);
	}
	else if ( msg.sType == _T("click"))
	{
		if( msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn"))) )
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

		if( msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn"))) )
			PostQuitMessage(0);

		if( msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn"))) )
			::IsZoomed(*this) ? SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0) : SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else if(msg.sType==_T("setfocus"))
	{
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		if(name==_T("examine"))
			pControl->SelectItem(0);
		else if(name==_T("trojan"))
			pControl->SelectItem(1);
		else if(name==_T("plugins"))
			pControl->SelectItem(2);
		else if(name==_T("vulnerability"))
			pControl->SelectItem(3);
		else if(name==_T("rubbish"))
			pControl->SelectItem(4);
		else if(name==_T("cleanup"))
			pControl->SelectItem(5);
		else if(name==_T("fix"))
			pControl->SelectItem(6);
		else if(name==_T("tool"))
			pControl->SelectItem(7);
	}
}

LRESULT BaseDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static std::auto_ptr<MessageMap> customMessageMap(InitMessageMap());

	BOOL bHandled = TRUE;
	LRESULT lRes = 0;

	if ( customMessageMap->find(uMsg) != customMessageMap->end() )
	{
		CustomMsgHandler handler = (*customMessageMap)[uMsg];
		lRes = (this->*handler)(wParam, lParam, bHandled);

		if ( bHandled ) return lRes;
	}

	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

BaseDialog::MessageMap* BaseDialog::InitMessageMap()
{
	MessageMap* map = new MessageMap;

	(*map)[WM_CREATE] = &BaseDialog::OnCreate;
	(*map)[WM_DESTROY] = &BaseDialog::OnDestory;
	(*map)[WM_ERASEBKGND] = &BaseDialog::OnErasebkgnd;
	(*map)[WM_SIZE] = &BaseDialog::OnSize;
	(*map)[WM_NCPAINT] = &BaseDialog::OnNcPaint;
	(*map)[WM_NCACTIVATE] = &BaseDialog::OnNcActive;
	(*map)[WM_NCCALCSIZE] = &BaseDialog::OnNcCalSize;
	(*map)[WM_NCHITTEST] = &BaseDialog::OnNcHitTest;
	(*map)[WM_SYSCOMMAND] = &BaseDialog::OnSysCommand;
	(*map)[WM_GETMINMAXINFO] = &BaseDialog::OnGetMinMaxInfo;

	return map;
}

HRESULT BaseDialog::OnCreate( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;
	C360Dui DialogCallback;
	CControlUI* pRoot = builder.Create(_T("main_dlg.xml"), (UINT)0, &DialogCallback, &m_pm);
	ASSERT(pRoot);
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	return 0;
}

HRESULT BaseDialog::OnDestory( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	::PostQuitMessage(0L);
	return 0;
}

HRESULT BaseDialog::OnErasebkgnd( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 1;
}

HRESULT BaseDialog::OnSize( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) 
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; 
		rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, 
			rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return 0;
}

HRESULT BaseDialog::OnNcPaint( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 0;
}

HRESULT BaseDialog::OnNcActive( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

HRESULT BaseDialog::OnNcCalSize( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 0;
}

HRESULT BaseDialog::OnNcHitTest( WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	POINT pt; 
	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) ) 
	{
		RECT rcSizeBox = m_pm.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top ) 
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) 
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left 
		&& pt.x < rcClient.right - rcCaption.right
		&& pt.y >= rcCaption.top 
		&& pt.y < rcCaption.bottom ) 
	{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));

			if( pControl 
				&& _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 
				&& _tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 )
			{
				return HTCAPTION;
			}
	}

	return HTCLIENT;
}

LRESULT BaseDialog::OnSysCommand(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) 
	{
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}

	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(WM_SYSCOMMAND, wParam, lParam);

	return 1L;
}

LRESULT BaseDialog::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);

	/// 窗口最大化时裁剪阴影所占区域
	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left-5;
	lpMMI->ptMaxPosition.y	= rcWork.top-3;
	lpMMI->ptMaxSize.x		= rcWork.right+10;
	lpMMI->ptMaxSize.y		= rcWork.bottom+10;

	bHandled = FALSE;
	return 0;
}