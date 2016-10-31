#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include <Windows.h>
#include <shellapi.h>
#include <Shlwapi.h>
#include <shlobj.h>
#pragma comment(lib,"shlwapi.lib")

/************************************************************************/
/* 窗口隐藏Demo                                                                     */
/************************************************************************/

typedef enum _MONITORPOWER
{
	MONITORPOWER_OPEN = 1,	//开启显示器
	MONITORPOWER_CLOSE,		//关闭显示器
};

typedef enum _HIDE_MODE
{
	HIDE_MODE_NONE = 0,
	HIDE_MODE_LEFT,
	HIDE_MODE_TOP,
	HIDE_MODE_RIGHT,
	HIDE_MODE_BOTTOM,
}HIDE_MODE;

#define	TIMER_SHOW_HIDE_WINDOW				1000		//显示隐藏窗口动态过程	
#define	TIMER_IDENTIFIER_HIDE_WINDOW		1001	   //检测是否需要隐藏窗口

#define	HIDE_WINDOWS_STEPS							10				//显示隐藏窗口步长
#define    INTERVAL												10			//触发粘附时鼠标与屏幕边界的最小间隔,单位为象素
#define    INFALTE													10			//触发收缩时鼠标与窗口边界的最小间隔,单位为象素
#define	 WINTOP													0				//窗口边缘高度
class C360SafeFrameWnd : public CWindowWnd, public INotifyUI
{
public:
	C360SafeFrameWnd() { m_HideMode=HIDE_MODE_NONE;m_bFinish = TRUE;m_bHide=TRUE;m_bStartTest=FALSE;};
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Init() {
		m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("BtnClose")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("BtnMin")));

		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if(/* !::IsIconic(*this) &&*/ (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		m_nHeight = GetSystemMetrics(SM_CYEDGE);
		m_nWidth  = GetSystemMetrics(SM_CXFRAME);

		DragAcceptFiles(m_hWnd,TRUE);

		RECT rcClient;
		GetWindowRect(m_hWnd,&rcClient);
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, rcClient.right,rcClient.bottom, SWP_SHOWWINDOW | SWP_NOMOVE);
	}

	void OnPrepare() {
	}

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) OnPrepare();
		else if( msg.sType == _T("click") ) {
			if( msg.pSender == m_pCloseBtn ) {
				PostQuitMessage(0);
				return; 
			}
			else if( msg.pSender == m_pMinBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
			else if( msg.pSender == m_pMaxBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
			else if( msg.pSender == m_pRestoreBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
			else if( msg.pSender == (CButtonUI*)m_pm.FindControl(_T("BtnCloseDisplay"))){
				SendMessage(WM_SYSCOMMAND,SC_MONITORPOWER,MONITORPOWER_CLOSE);}
			else if( msg.pSender == (CButtonUI*)m_pm.FindControl(_T("BtnLock"))){
				LockWorkStation();}
			else if( msg.pSender == (CButtonUI*)m_pm.FindControl(_T("BtnIconCache"))){
				SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,NULL,NULL);}
			else if(msg.pSender == (CButtonUI*)m_pm.FindControl(_T("BtnChangeColor"))){
				
			}
			else if(msg.pSender == (CButtonUI*)m_pm.FindControl(_T("BtnShareTo")))
			{
				if (CPaintManagerUI::GetResourceZip() == _T("small.zip"))
					CPaintManagerUI::SetResourceZip(_T("full.zip"));
				else
					CPaintManagerUI::SetResourceZip(_T("small.zip"));

				CPaintManagerUI::ReloadSkin();
			}
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

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION|WS_CLIPSIBLINGS;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		//CDialogBuilderCallbackEx cb;
		CControlUI* pRoot = builder.Create(_T("MainFrame.xml"), (UINT)0,  NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		Init();
		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
        if( ::IsIconic(*this) ) bHandled = FALSE;
        return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if (m_HideMode != HIDE_MODE_NONE && m_bStartTest == FALSE )
		{
			m_bStartTest = TRUE;
			SetTimer(m_hWnd,TIMER_IDENTIFIER_HIDE_WINDOW,200,NULL);
		}

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = m_pm.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
				if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
         SIZE szRoundCorner = m_pm.GetRoundCorner();
         if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
             CDuiRect rcWnd;
             ::GetWindowRect(*this, &rcWnd);
             rcWnd.Offset(-rcWnd.left, -rcWnd.top);
             rcWnd.right++; rcWnd.bottom++;
             HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
             ::SetWindowRgn(*this, hRgn, TRUE);
             ::DeleteObject(hRgn);
         }

        bHandled = FALSE;
        return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-rcWork.left, -rcWork.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;
		lpMMI->ptMaxSize.x		= rcWork.right;
		lpMMI->ptMaxSize.y		= rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		return lRes;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == TIMER_IDENTIFIER_HIDE_WINDOW)
		{
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			POINT pt;
			GetCursorPos(&pt);
			if ((PtInRect(&rcWnd,pt) == TRUE && m_bHide == FALSE) ||
				(PtInRect(&rcWnd,pt) == FALSE && m_bHide == TRUE))
			{
				KillTimer(m_hWnd,TIMER_IDENTIFIER_HIDE_WINDOW);
				m_bFinish = FALSE;
				SetTimer(m_hWnd,TIMER_SHOW_HIDE_WINDOW,5,NULL);
			}
		}

		if (wParam == TIMER_SHOW_HIDE_WINDOW)
		{
			if(m_bFinish) //如果收缩或显示过程完毕则关闭Timer	  
			{
				KillTimer(m_hWnd,TIMER_SHOW_HIDE_WINDOW);
				m_bStartTest = FALSE;
				m_bHide = m_bHide == TRUE ? FALSE : TRUE;
			}
			else
			{
				m_bHide ? DoWinHide() : DoWinShow();
			}
		}
		
		return 0;
	}

	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt;
		GetCursorPos(&pt);
		PRECT pRect = (PRECT)lParam;
		INT nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		INT nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
		INT height = pRect->bottom - pRect->top;
		INT width  = pRect->right - pRect->left;

		if (pRect->top<INTERVAL)
		//粘在上边
			m_HideMode = HIDE_MODE_TOP;
		else
			m_HideMode = HIDE_MODE_NONE;
		return 0;
	}

	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TCHAR szFilePathName[MAX_PATH];
		HDROP hDropFile = (HDROP)wParam;

		UINT nCount = DragQueryFile(hDropFile, 0, szFilePathName, sizeof(szFilePathName)); //得到文件路径
		if (PathFileExists(szFilePathName))
		{
			CControlUI* pControlUI = m_pm.FindControl(_T("VLayoutTotal"));
			ASSERT(pControlUI);
			CDuiString strBkImage;
			strBkImage.Format(_T("file='%s'"),szFilePathName);
			pControlUI->SetBkImage(strBkImage);
			pControlUI->Invalidate();
		}
		DragFinish(hDropFile);
		return 0;
	}

	void DoWinHide()
	{
		if(m_HideMode == HIDE_MODE_NONE)
		{
			m_bFinish = TRUE;
			return;
		}

		RECT rcWnd;
		GetWindowRect(m_hWnd,&rcWnd);

		INT nHeight = rcWnd.bottom - rcWnd.top;
		INT nWidht  = rcWnd.right - rcWnd.left;

		INT nSteps = 0;
		switch(m_HideMode)
		{
			case HIDE_MODE_TOP:
				nSteps = nHeight/HIDE_WINDOWS_STEPS;
				rcWnd.bottom -= nSteps;
				if(rcWnd.bottom <= m_nHeight)
				{   //你可以把下面一句替换上面的 ...+=|-=steps 达到取消抽屉效果
					//更好的办法是添加个BOOL值来控制,其他case同样.
					rcWnd.bottom = m_nHeight;
					m_bFinish = TRUE;  //完成隐藏过程
				}
				rcWnd.top = rcWnd.bottom - nHeight; 
				break;
		}
		SetWindowPos(HWND_TOPMOST,&rcWnd);
		m_bHide = TRUE;
	}

	void DoWinShow()
	{
		if(m_HideMode == HIDE_MODE_NONE)
		{
			m_bFinish = TRUE;
			return;
		}

		RECT rcWnd;
		GetWindowRect(m_hWnd,&rcWnd);

		INT nHeight = rcWnd.bottom - rcWnd.top;
		INT nWidht  = rcWnd.right - rcWnd.left;

		INT nSteps = 0;
		switch(m_HideMode)
		{
		case HIDE_MODE_TOP:
			nSteps = nHeight/HIDE_WINDOWS_STEPS;
			rcWnd.top += nSteps;
			if(rcWnd.top >= WINTOP)
			{   //你可以把下面一句替换上面的 ...+=|-=steps 达到取消抽屉效果
				//更好的办法是添加个BOOL值来控制,其他case同样.
				rcWnd.top = WINTOP;
				m_bFinish = TRUE;  //完成隐藏过程
			}
			rcWnd.bottom = rcWnd.top + nHeight; 
			break;
		}
		SetWindowPos(HWND_TOPMOST,&rcWnd);
		m_bHide = FALSE;
	}

	void SetWindowPos(HWND hWndInsertAfter,PRECT pRect)
	{
		::SetWindowPos(m_hWnd,hWndInsertAfter,pRect->left,pRect->top,pRect->right-pRect->left,pRect->bottom-pRect->top,SWP_SHOWWINDOW);
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_TIMER:					lRes = OnTimer(uMsg,wParam,lParam,bHandled);break;
		case WM_MOVING:			lRes = OnMoving(uMsg,wParam,lParam,bHandled);break;
		case WM_DROPFILES:		lRes = OnDropFiles(uMsg,wParam,lParam,bHandled);break;
		default:
		bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

public:
	CPaintManagerUI m_pm;

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	BOOL		   m_bFinish;
	BOOL		   m_bHide;
	BOOL		   m_bStartTest;
	int			   m_nHeight,m_nWidth;
	/*BOOL */
	HIDE_MODE m_HideMode;
	//...
};
