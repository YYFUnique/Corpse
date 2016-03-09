#include "stdafx.h"
#include "MessageTip.h"
#include "ProcessData.h"

typedef struct _TRAYDATA
{	
	HWND hwnd;
	UINT uID;				 
	UINT uCallbackMessage;	 
	DWORD Reserved[2];		
	HICON hIcon;	         //托盘图标的句柄		 	
}TRAYDATA,*PTRAYDATA;

typedef struct _TBBUTTON64 {
	int iBitmap;
	int idCommand;
	BYTE fsState;
	BYTE fsStyle;
	BYTE bReserved[6];
	DWORD64 dwData;
	DWORD64 iString;
} TBBUTTON64, NEAR* PTBBUTTON64, *LPTBBUTTON64;

CMessageTip::CMessageTip(HWND hParent,LPCTSTR lpszBkFile,DWORD dwBkColor,LPCTSTR lpszWinTitle,LPCTSTR lpszMessage)
:m_strBkFile(lpszBkFile)
,m_dwBkColor(dwBkColor)
,m_strWinTitle(lpszWinTitle)
,m_strMessage(lpszMessage)
{
	Create(hParent,_T("MessageTip"),WS_POPUP|UI_WNDSTYLE_DIALOG,  WS_EX_TOOLWINDOW, 0, 0);
}

CMessageTip::~CMessageTip()
{

}

LPCTSTR CMessageTip::GetWindowClassName() const
{
	return _T("LsGuiFoundation");
}

void CMessageTip::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CMessageTip::OnFinalMessage(HWND hWnd)
{
	RemoveObserver();
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CMessageTip::InitWindow()
{
	POINT pt;
	SIZE size = m_PaintManager.GetInitSize();

	GetTrayWndPosition(size,pt);

	SetWindowPos(m_hWnd,HWND_TOPMOST,pt.x, pt.y, 0,0, SWP_SHOWWINDOW|SWP_NOSIZE);
	CLabelUI* pLabelTitle = (CLabelUI*)m_PaintManager.FindControl(_T("WinTitle"));
	pLabelTitle->SetText(m_strWinTitle);

	CTextUI* pTextMessage = (CTextUI*)m_PaintManager.FindControl(_T("DialogTipInfo"));
	pTextMessage->SetText(m_strMessage);
}

CDuiString CMessageTip::GetSkinFile()
{
	return _T("MessageTip.xml");
}

CDuiString CMessageTip::GetSkinFolder()
{
	return _T("pcmaster");
}

BOOL CMessageTip::Receive(SkinChangedParam lParam)
{
	m_strBkFile = lParam.bgimage;
	m_dwBkColor = lParam.bkcolor;
	CControlUI* pBackground = m_PaintManager.FindControl(_T("VLayoutTotal"));
	if (pBackground != NULL)
	{
		if (m_strBkFile.IsEmpty() == FALSE)
		{
			CDuiString strBkImage;
			strBkImage.Format(_T("file='%s' corner='600,200,1,1'"),m_strBkFile);
			pBackground->SetBkImage(strBkImage);
		}
		else
			pBackground->SetBkImage(_T(""));

		pBackground->SetBkColor(m_dwBkColor);
	}

	return TRUE;
}

LRESULT CMessageTip::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CMessageTip::OnClick(TNotifyUI& msg)
{
	Close();
}

void CMessageTip::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
	if (bShow != false)
	{
		POINT pt;
		SIZE size = m_PaintManager.GetInitSize();

		GetTrayWndPosition(size,pt);

		SetWindowPos(m_hWnd,HWND_TOPMOST,pt.x, pt.y, 0,0, SWP_NOSIZE);
	}

	WindowImplBase::ShowWindow(bShow,bTakeFocus);
}

HWND CMessageTip::GetTrayWndHwnd()
{
	HWND hWnd = FindWindow(_T("Shell_TrayWnd"),NULL);	//获取任务栏句柄
	if (hWnd != NULL)
	{
		hWnd = FindWindowEx(hWnd,NULL,_T("TrayNotifyWnd"),NULL);	//获取右下角区域
		if (hWnd != NULL)
		{
			HWND hPageWnd = FindWindowEx(hWnd,NULL,_T("SysPager"),NULL);	//
			if (hPageWnd != NULL)
				hWnd = FindWindowEx(hPageWnd,NULL,_T("ToolBarWindow32"),NULL);
			else
				hWnd = FindWindowEx(hWnd,NULL,_T("ToolBarWindow32"),NULL);

			return hWnd;
		}
	}

	return hWnd;
}

//获取溢出托盘区窗口句柄  
HWND CMessageTip::FindNotifyIconOverflowWindow()  
{  
	HWND hWnd = NULL;  

	hWnd = FindWindow(_T("NotifyIconOverflowWindow"), NULL);  
	if (hWnd != NULL)  
		hWnd = FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);  

	return hWnd;  	  
}

BOOL CMessageTip::GetTrayInfo(HWND hTrayWnd,RECT& rcMenuPos)
{
	BOOL bFindTray = FALSE;

	DWORD dwTrayPid;

	GetWindowThreadProcessId(hTrayWnd, &dwTrayPid);

	BOOL bWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(),&bWow64);

	if (bWow64 == FALSE)
		bFindTray = GetTrayInfo32(dwTrayPid,hTrayWnd,rcMenuPos);
	else
		bFindTray = GetTrayInfo64(dwTrayPid,hTrayWnd,rcMenuPos);

	return bFindTray;
}

BOOL CMessageTip::GetTrayInfo32(DWORD dwTrayPid,HWND hTrayWnd,RECT& rcMenuPos)
{
	BOOL bFindTray = FALSE;
	TRAYDATA pTrayData;
	TBBUTTON TbBtn;

	CProcessData<TBBUTTON> data(dwTrayPid);
	CProcessData<RECT> ProcessData(dwTrayPid);

	RECT rcTrayWnd;
	GetWindowRect(hTrayWnd,&rcTrayWnd);

	UINT nCount = ::SendMessage(hTrayWnd,TB_BUTTONCOUNT,NULL,NULL);
	for (UINT n=0;n<nCount;++n)
	{
		::SendMessage(hTrayWnd,TB_GETBUTTON,n,(LPARAM)data.GetData());
		data.ReadData(&TbBtn);
		data.ReadData<TRAYDATA>(&pTrayData,(LPCVOID)TbBtn.dwData);
		if (pTrayData.hwnd == GetParent(m_hWnd))
		{
			::SendMessage(hTrayWnd, TB_GETITEMRECT, n, (LPARAM)ProcessData.GetData());  
			ProcessData.ReadData(&rcMenuPos);
			OffsetRect(&rcMenuPos,rcTrayWnd.left,rcTrayWnd.top);
			bFindTray = TRUE;
			break;
		}
	}

	return bFindTray;
}

BOOL CMessageTip::GetTrayInfo64(DWORD dwTrayPid,HWND hTrayWnd,RECT& rcMenuPos)
{
	BOOL bFindTray = FALSE;
	TRAYDATA pTrayData;
	TBBUTTON64 TbBtn;

	CProcessData<TBBUTTON64> data(dwTrayPid);
	CProcessData<RECT> ProcessData(dwTrayPid);

	RECT rcTrayWnd;
	GetWindowRect(hTrayWnd,&rcTrayWnd);

	UINT nCount = ::SendMessage(hTrayWnd,TB_BUTTONCOUNT,NULL,NULL);
	for (UINT n=0;n<nCount;++n)
	{
		::SendMessage(hTrayWnd,TB_GETBUTTON,n,(LPARAM)data.GetData());
		data.ReadData(&TbBtn);
		data.ReadData<TRAYDATA>(&pTrayData,(LPCVOID)TbBtn.dwData);
		if (pTrayData.hwnd == GetParent(m_hWnd))
		{
			::SendMessage(hTrayWnd, TB_GETITEMRECT, n, (LPARAM)ProcessData.GetData());  
			ProcessData.ReadData(&rcMenuPos);
			OffsetRect(&rcMenuPos,rcTrayWnd.left,rcTrayWnd.top);
			bFindTray = TRUE;
			break;
		}
	}

	return bFindTray;
}

BOOL CMessageTip::GetTrayWndPosition(SIZE& size,POINT& pt)
{
	APPBARDATA AppBarData = {0};
	AppBarData.cbSize = sizeof(AppBarData);
	SHAppBarMessage(ABM_GETTASKBARPOS,&AppBarData); 

	HWND hTrayWnd = GetTrayWndHwnd();

	RECT rcMenuPos;
	BOOL bFindTray = GetTrayInfo(hTrayWnd,rcMenuPos);
	if (bFindTray == FALSE)
	{
		hTrayWnd = FindNotifyIconOverflowWindow();
		bFindTray = GetTrayInfo(hTrayWnd,rcMenuPos);
	}

	if (bFindTray == FALSE)
		return bFindTray;

	RECT rcTrayWnd;
	GetWindowRect(hTrayWnd,&rcTrayWnd);			

	RECT rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	if (AppBarData.uEdge == ABE_LEFT)
	{
		pt.x = rcTrayWnd.right;
		pt.y = rcMenuPos.top + (rcMenuPos.bottom - rcMenuPos.top)/2 - size.cy/2;
		//这里需要适当的修正
		if (pt.y + size.cy > rcWork.bottom)
			pt.y = rcWork.bottom - size.cy;
	}
	else if (AppBarData.uEdge == ABE_TOP)
	{
		pt.x = rcMenuPos.left + (rcMenuPos.right - rcMenuPos.left)/2 - size.cx/2;
		pt.y = rcTrayWnd.bottom;
		if (pt.x + size.cx > rcWork.right)
			pt.x = rcWork.right - size.cx;
	}
	else if (AppBarData.uEdge == ABE_RIGHT)
	{
		pt.x = rcTrayWnd.left - size.cx;
		pt.y = rcMenuPos.top + (rcMenuPos.bottom - rcMenuPos.top)/2 - size.cy/2;
		if (pt.y + size.cy > rcWork.bottom)
			pt.y = rcWork.bottom - size.cy;
	}
	else
	{
		pt.x = rcMenuPos.left + (rcMenuPos.right - rcMenuPos.left)/2 - size.cx/2;
		pt.y = rcTrayWnd.top - size.cy;
		if (pt.x + size.cx > rcWork.right)
			pt.x = rcWork.right - size.cx;
	}

	return bFindTray;
}