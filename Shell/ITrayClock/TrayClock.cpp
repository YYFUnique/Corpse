#include "TrayClock.h"
#include <atlstr.h>
#include <CommCtrl.h>
// 保存模块句柄，释放DLL时需要用到
static HMODULE gLibModule = 0;

// 用于保存时钟窗口原始处理过程,DLL卸载时应该还原
static LONG_PTR gOldWndProc = 0;

static BOOL m_bMouseTracking = 0;

HMENU hPopMenu = NULL;
static HWND m_hwndTooltip = NULL;
 TOOLINFO m_ToolTip;

#define WM_TCLOCK_RESVERED1		(WM_USER + 0x64)		// 未知保留中
#define WM_TCLOCK_LBUTTON		(WM_USER + 0x66)		// 任务栏时钟区域点击鼠标左键
#define WM_TCLOCK_TOOLTIP			(WM_USER + 0x67)		// 任务栏时钟显示ToolTip
#define WM_TCLOCK_HIGHLIGHT		(WM_USER + 0x68)		// 任务栏时钟背景高亮

typedef enum tagTCLOCK_HIGHLIGHT_WPARAM
{
	TCLOCK_DISABLE	= 1,
	TCLOCK_ENABLE		= 2,
	TCLOCK_UNKNOW = 3,
}TCLOCK_HIGHLIGHT_WPARAM;

 typedef enum tagMENU_ITEM
 {
	//MENU_ITEM_EXIT = 0,
	IDM_ITEM_TEST1		= 0,
	IDM_ITEM_TEST2		= 1,
	IDM_ITEM_SEP			=	2,
	IDM_ITEM_ENABLE		= 3,
	IDM_ITEM_DISABLE	=	4,
	IDM_ITEM_EXIT			=	5,
 }MENU_ITEM;

// 查找时钟窗口句柄
static HWND FindClockWindow()
{
	HWND hTrayWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (IsWindow(hTrayWnd))
	{
		HWND hNotifyWnd = FindWindowEx(hTrayWnd, 0, _T("TrayNotifyWnd"), NULL);
		if (IsWindow(hNotifyWnd))
			return FindWindowEx(hNotifyWnd, 0, _T("TrayClockWClass"), NULL);
	}
	return NULL;
}

static DWORD WINAPI FreeSelf(LPVOID param)
{
	FreeLibraryAndExitThread(gLibModule, EXIT_SUCCESS);
}

static void RestoreWndProc()
{
	if (gOldWndProc != 0)
		SetWindowLongPtr(FindClockWindow(), GWLP_WNDPROC, gOldWndProc);
}

CString GetMsgType(UINT uMsg)
{
	LPCTSTR lpszMsgType = _T("");
	switch(uMsg)
	{
		case WM_NCHITTEST:
				lpszMsgType = _T("NCHitTest");
			break;
		case WM_MOUSEMOVE:
				lpszMsgType = _T("MouseMove");
			break;
		case WM_SETCURSOR:
				lpszMsgType = _T("SetCursor");
			break;
		case WM_NCLBUTTONUP:
				lpszMsgType = _T("NCLButtonUp");
			break;
		case WM_PAINT:
				lpszMsgType = _T("Paint");
			break;
		case WM_ERASEBKGND:
				lpszMsgType = _T("EraseBkgnd");
			break;
		case WM_LBUTTONDOWN:
				lpszMsgType = _T("LButtonDown");
			break;
		case WM_RBUTTONUP:
				lpszMsgType = _T("RButtonUp");
			break;
		
		default:
			lpszMsgType = _T("未知消息类型");
	}

	CString strMsgType;
	strMsgType.Format(_T("%s[0x%X]"), lpszMsgType, uMsg);
	return strMsgType;
}

/*------------------------------------------------
  return size of clock
  high-order word: height, low-order word: width
--------------------------------------------------*/
LRESULT OnCalcRect(HWND hwnd)
{
	int hclock = 40;
	int wclock = 150;
	return MAKELONG(wclock,hclock);
}

void RefreshRebar(HWND hwndRebar)
{
	if(hwndRebar)
	{
		HWND hwnd;

		InvalidateRect(hwndRebar, NULL, TRUE);
		hwnd = GetWindow(hwndRebar, GW_CHILD);
		while(hwnd)
		{
			InvalidateRect(hwnd, NULL, TRUE);
			hwnd = GetWindow(hwnd, GW_HWNDNEXT);
		}
	}
}

void RefreshTaskbar(HWND hwndClock)
{
	HWND hwndTaskbar, hwndRebar, hwndTray/*, hwndStartButton*/;

	hwndTray = GetParent(hwndClock);
	hwndTaskbar = GetParent(hwndTray);
	hwndRebar = FindWindowEx(hwndTaskbar, NULL, _T("ReBarWindow32"), NULL);
	//hwndStartButton = FindWindowEx(hwndTaskbar, NULL, _T("Button"), NULL);

	//InvalidateRect(hwndStartButton, NULL, TRUE);
	InvalidateRect(hwndTray, NULL, TRUE);
	PostMessage(hwndTray, WM_SIZE, SIZE_RESTORED, 0);
 
	// 使用该行代码导致桌面应用闪烁，后面完善时移除该部分代码
	//RefreshRebar(hwndRebar);
	//PostMessage(hwndRebar, WM_SIZE, SIZE_RESTORED, 0);

	InvalidateRect(hwndTaskbar, NULL, TRUE);
	PostMessage(hwndTaskbar, WM_SIZE, SIZE_RESTORED, 0);
}


// 新的窗口处理过程,核心工作都在这里
LRESULT CALLBACK ClockWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if (uMsg == WM_PAINT)
	//{
	//	TCHAR szBuff[256];

	//	//获取当前时间
	//	SYSTEMTIME tmCur;
	//	GetLocalTime(&tmCur);
	//	//格式化时间
	//	_stprintf_s(szBuff, _T("^_^%02d:%02d\n%02dW%d"),  
	//						tmCur.wHour, tmCur.wMinute, 
	//						tmCur.wSecond, tmCur.wDayOfWeek);

	//	//显示时间
	//	PAINTSTRUCT ps;
	//	HDC hdc = BeginPaint(hWnd, &ps);
	//	// TODO: Add any drawing code here...
	//	RECT rt;
	//	GetClientRect(hWnd, &rt);
	//	DrawText(hdc, szBuff, (int)_tcslen(szBuff), &rt, DT_LEFT|DT_TOP);
	//	EndPaint(hWnd, &ps);
	//	return 0;
	//}

	
	
	// 为了能收到鼠标左击右击的消息，必须让Windows以为我们"命中"了工作区
	//if (uMsg == WM_NCHITTEST)
	//	return HTCLIENT;

	{
		CString strTipInfo;
		strTipInfo.Format(_T("uMsg : %s, wParam:%d, lParam:%d.\r\n"),GetMsgType(uMsg),wParam,lParam);
		//OutputDebugString(strTipInfo);
		if (uMsg == WM_NOTIFY)
		{
			CString strTipInfo;
			NMHDR* pnmHdr = (NMHDR*)lParam;
			strTipInfo.Format(_T("code:%d\r\n"),pnmHdr->code);
			//OutputDebugString(strTipInfo);
		}
	}

	//// 处理鼠标左击事件
	if (uMsg == WM_NCHITTEST)
		return HTCLIENT;

	// 处理鼠标左击事件
	//if (uMsg == WM_LBUTTONUP)
	//{
	//	OutputDebugString(_T("TClock WM_LBUTTONUP"));
	//	ShellExecute(0, TEXT("open"), TEXT("http://www.baidu.com/"), NULL, NULL, SW_SHOW);
	//	return 0;
	//}

	if (uMsg == WM_LBUTTONUP)
	{
		RefreshTaskbar(hWnd);
		RestoreWndProc();
		CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeSelf, NULL, 0, NULL));
		return 0;
	}

	/*if (uMsg == WM_NCCALCSIZE)
	{
		OutputDebugString(_T("WM_NCCALCSIZE"));
		return 0;
	}*/

	if (uMsg == WM_USER+100)
	{
		OutputDebugString(_T("WM_USER+100"));
		RECT rcTClock;
		GetClientRect(hWnd, &rcTClock);
		InvalidateRect(GetParent(hWnd), &rcTClock, TRUE);
		return OnCalcRect(hWnd);
	}
	if (uMsg == WM_WININICHANGE || uMsg ==  WM_TIMECHANGE || 
		uMsg == WM_USER+101 || uMsg == WM_SETFOCUS || uMsg == WM_KILLFOCUS)
	{
		OutputDebugString(_T("WM_USER+101"));
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	}

	// 处理鼠标右击事件，这里我们卸载DLL(自身)
	if (uMsg == WM_RBUTTONUP)
	{
		BOOL bDisable = FALSE;
		//OutputDebugString(_T("TClock WM_RBUTTONUP"));
		//ShellExecute(0, TEXT("open"), TEXT("http://www.163.com/"), NULL, NULL, SW_SHOW);
		UINT uFlag = MF_BYPOSITION|MF_STRING;
		POINT ptClickPoint;
		GetCursorPos(&ptClickPoint);
		hPopMenu = CreatePopupMenu();
		//InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ABOUT,_T("About"));
		if ( bDisable == TRUE )
		{
			uFlag |= MF_GRAYED;
		}
		InsertMenu(hPopMenu,0xFFFFFFFF,uFlag,IDM_ITEM_TEST2,_T("Test 2"));
		InsertMenu(hPopMenu,0xFFFFFFFF,uFlag,IDM_ITEM_TEST1,_T("Test 1"));
		InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,IDM_ITEM_SEP,_T("SEP"));
		if ( bDisable == TRUE )
		{
			InsertMenu(hPopMenu,0xFFFFFFFF, 
				MF_BYPOSITION|MF_STRING,IDM_ITEM_ENABLE,_T("Enable"));
		}
		else 
		{
			InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ITEM_DISABLE,_T("Disable"));    
		}
		InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,IDM_ITEM_SEP,_T("SEP"));    
		InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ITEM_EXIT,_T("Exit"));     
		SetForegroundWindow(hWnd);
		TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,
			ptClickPoint.x, ptClickPoint.y,0,hWnd,NULL);
		DestroyMenu(hPopMenu); 
		return 0;
	}

	/*if (uMsg == )*/

	 if (uMsg == WM_MOUSEMOVE)
	 {
		 // Start tracking this entire window again...
		if( !m_bMouseTracking ) {
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = hWnd;
			tme.dwHoverTime = m_hwndTooltip == NULL ? 100UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
			TrackMouseEvent(&tme);
			m_bMouseTracking = true;
		}
		return 0;
	 }


	 if (uMsg == WM_MOUSEHOVER)
	 {
		/* ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
		 m_ToolTip.cbSize = sizeof(TOOLINFO);
		 m_ToolTip.uFlags = TTF_IDISHWND;
		 m_ToolTip.hwnd = hWnd;
		 m_ToolTip.uId = (UINT_PTR) hWnd;
		 m_ToolTip.hinst = gLibModule;
		 m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) _T("Hello word") );
		 RECT rcWnd;
		 GetWindowRect(hWnd, &rcWnd);
		 
		 rcWnd.bottom = rcWnd.top;
		 rcWnd.top = rcWnd.bottom - 100;
		 m_ToolTip.rect = rcWnd;

		 if( m_hwndTooltip == NULL ) {
			 m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, gLibModule, NULL);
			 ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
		 }
		 ::SendMessage( m_hwndTooltip,TTM_SETMAXTIPWIDTH,0, 200);
		 ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
		 ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);*/

			PostMessage(hWnd, WM_TCLOCK_HIGHLIGHT, TCLOCK_ENABLE, 0);
			//PostMessage(hWnd, WM_TCLOCK_REDRAW, TRUE, 0);
			return 0;
	 }

	 if (uMsg == WM_GETMINMAXINFO)
	 {
		 CString strTipInfo;
		 strTipInfo.Format(_T("WM_GETMINMAXINFO : %d, wParam:%d, lParam:%d.\r\n"),uMsg,wParam,lParam);
		 OutputDebugString(strTipInfo);
		 /*GetWindowRect()*/
		 return 0;
	 }

	if (uMsg == WM_MOUSELEAVE)
	{
		/*if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
		if( m_bMouseTracking ) ::SendMessage(hWnd, WM_MOUSEMOVE, 0, (LPARAM) -1);*/
		m_bMouseTracking = false;
		PostMessage(hWnd, WM_TCLOCK_HIGHLIGHT, TCLOCK_DISABLE, 0);
	}

	LRESULT lRet =  WNDPROC(gOldWndProc)(hWnd, uMsg, wParam, lParam);
	//if (uMsg == WM_GETMINMAXINFO || uMsg == 0x467)
	

	return lRet;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			OutputDebugString(_T("DLL_PROCESS_ATTACH"));
			gLibModule = hModule;
			HWND hClock = FindClockWindow();
			if (IsWindow(hClock))
			{
				gLibModule = hModule;
				/*RECT rcWnd;
				GetWindowRect(hClock, &rcWnd);
				SetWindowPos(hClock,NULL,0,0,1,1,SWP_NOMOVE|SWP_NOZORDER);*/
				gOldWndProc = GetWindowLongPtr(hClock, GWLP_WNDPROC);
				if (gOldWndProc != 0)
				{
					SetWindowLongPtr(hClock, GWLP_WNDPROC, (LONG_PTR)&ClockWndProc);
				}
				RefreshTaskbar(hClock);
				//SetWindowPos(hClock,NULL,0,0,rcWnd.right-rcWnd.left+100,rcWnd.bottom-rcWnd.top,SWP_NOZORDER);
				//SendMessage(hClock, TB_SETBUTTONWIDTH, 0, MAKELONG(300,450));
				/*const int nOffset = 100;
				HWND hWndNotify = FindWindow(_T("TrayNotifyWnd"),NULL);
				RECT rectNotify;
				GetWindowRect(hWndNotify, &rectNotify);
				CString strTipInfo;
				strTipInfo.Format(_T("left:%d,right:%d,top:%d,bottom:%d"),rectNotify.left,rectNotify.right,rectNotify.top,rectNotify.bottom);
				OutputDebugString(strTipInfo);
				SetWindowPos(hWndNotify, NULL, 
								0,0, rectNotify.right - rectNotify.left+nOffset, rectNotify.bottom - rectNotify.top, 
								SWP_NOZORDER|SWP_NOMOVE);*/
			}
			break;
		}

	case DLL_PROCESS_DETACH:
		{
			OutputDebugString(_T("DLL_PROCESS_DETACH"));
			RestoreWndProc();
			break;
		}
	}
	return TRUE;
}