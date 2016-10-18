#include "StdAfx.h"
#include "MutileDesktop.h"

#define LSDESKTOP		_T("{784E9FF1-5558-4e38-8D3C-85B06183443B}")
BOOL CMutileDesktop::m_bInit = FALSE;
HDESK CMutileDesktop::m_hDesktop[2] = {0,0};

DESKTOP_TYPE CMutileDesktop::m_CurrentDesktopType = DESKTOP_TYPE_UNDEFINED;

CMutileDesktop::CMutileDesktop()
{
	m_HotKey = -1;

	//创建窗口后，即运行InitWindow
	Create(NULL, _T("Desktop"), WS_POPUP|UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0);
}

CMutileDesktop::~CMutileDesktop()
{
	if (m_HotKey != -1)
	{
		UnregisterHotKey(m_hWnd,m_HotKey);
		m_HotKey = -1;
	}
}

void CMutileDesktop::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CMutileDesktop::GetWindowClassName() const
{
	return _T("duilibDesktop");
}

UILIB_RESOURCETYPE CMutileDesktop::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CMutileDesktop::GetZIPFileName() const
{
	return _T("");
}

CDuiString CMutileDesktop::GetSkinFile()
{
	return _T("mutiledesktop.xml");
}

CDuiString CMutileDesktop::GetSkinFolder()
{
	return _T("MutileDesktop");
}

void CMutileDesktop::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

LRESULT CMutileDesktop::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandle)
{
	switch(uMsg)
	{
		case WM_HOTKEY:
			{
				UINT  fuModifiers = ( UINT ) LOWORD(lParam);   // key-modifier flags    
				UINT  uVirtKey = ( UINT ) HIWORD(lParam);      // virtual-key code    
				if (fuModifiers == MOD_ALT && uVirtKey == VK_F8)
				{
					::SwitchDesktop(m_hDesktop[DESKTOP_TYPE_SYSTEM]);
					bHandle = TRUE;
				}
			}
			return 0;
		default:
			return WindowImplBase::MessageHandler(uMsg,wParam,lParam,bHandle);
	}
}

void CMutileDesktop::InitWindow()
{
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWnd=*this;
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	if (hWndCenter!=NULL)
		hWnd=hWndCenter;

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if( hWndCenter == NULL )
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if( xLeft < rcArea.left ) xLeft = rcArea.left;
	else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
	if( yTop < rcArea.top ) yTop = rcArea.top;
	else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;

	//将窗口在屏幕顶端居中显示
	SetWindowPos(m_hWnd, HWND_TOPMOST, xLeft, 0, 0, 0, SWP_NOSIZE);

	//注：   HotKeyId的合法取之范围是0x0000到0xBFFF之间，   GlobalAddAtom函数得到的值 
	//			在0xC000到0xFFFF之间，所以减掉0xC000来满足调用要求。 
	m_HotKey = GlobalAddAtom(_T("HOTKEY")) - 0xC000;
	RegisterHotKey(m_hWnd, m_HotKey, MOD_ALT, VK_F8);  
}

void CMutileDesktop::OnClick(TNotifyUI& msg)
{
	DESKTOP_TYPE DesktopType = DESKTOP_TYPE_UNDEFINED;
	if (msg.pSender == m_PaintManager.FindControl(_T("System")))
		DesktopType = DESKTOP_TYPE_SYSTEM;
	else if (msg.pSender == m_PaintManager.FindControl(_T("Security")))
		DesktopType = DESKTOP_TYPE_SECURITY;

	if (DesktopType != DESKTOP_TYPE_UNDEFINED)
	{
		::SwitchDesktop(m_hDesktop[DesktopType]);
		//	Close(IDOK);
	}
}

UINT CMutileDesktop::workThread(LPARAM lParam)
{
	DESKTOP_TYPE DesktopType = (DESKTOP_TYPE)lParam;

	do 
	{
		//在调用该函数前，不能能有任何与调用相关的函数调用，故一般在线程中使用
		if (SetThreadDesktop(m_hDesktop[DesktopType]) == FALSE)
			break;

		PROCESS_INFORMATION ProcessInfo;
		STARTUPINFO StartupInfo;

		//初始化创建进程必须的结构体
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
		StartupInfo.cb = sizeof(StartupInfo);
		StartupInfo.lpDesktop = LSDESKTOP;

		//当前仅只针对Win7有效，其他系统需要测试
		HWND hWnd = FindWindow(NULL,_T("#32769"));
		if (DesktopType != DESKTOP_TYPE_SYSTEM && IsWindow(hWnd) ==  FALSE)
		{
			//创建桌面外壳进程
			if (CreateProcess(_T("C:\\Windows\\explorer.exe"),NULL,  NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInfo) == FALSE)
				break;
		}

		CMutileDesktop* pMutileDesktop = new CMutileDesktop();
		if (pMutileDesktop == NULL)
			break;

		pMutileDesktop->ShowModal();

		if (DesktopType != DESKTOP_TYPE_SYSTEM)
		{
			CloseDesktop(m_hDesktop[DesktopType]);
			m_hDesktop[DesktopType] = NULL;
		}

	} while (FALSE);

	return TRUE;
}

BOOL CMutileDesktop::SwitchDesktop(DESKTOP_TYPE DesktopType)
{
	return ::SwitchDesktop(m_hDesktop[DesktopType]);
}

BOOL CMutileDesktop::CreateDesktop(DESKTOP_TYPE DesktopType)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (DesktopType == m_CurrentDesktopType)
			return FALSE;

		m_CurrentDesktopType = DesktopType;
		if (m_bInit == FALSE)
		{
			m_hDesktop[DESKTOP_TYPE_SYSTEM] = OpenInputDesktop(0,FALSE,GENERIC_ALL);
			m_hDesktop[DESKTOP_TYPE_SECURITY] = ::CreateDesktop(LSDESKTOP,NULL,NULL,DF_ALLOWOTHERACCOUNTHOOK,GENERIC_ALL,NULL);

			m_bInit = TRUE;
		}
		
		if (m_hDesktop[DesktopType] == NULL)
		{
			//SetErrorInfo(CUSTOM_ERROR, 0, _T("切换到安全桌面失败，未创建安全桌面"));
			break;
		}

		HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)workThread,(LPVOID)DesktopType,0,NULL);
		CloseHandle(hThread);
		
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}