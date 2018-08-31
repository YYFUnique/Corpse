#include "StdAfx.h"
#include "TrayClock.h"
#include "Define.h"
#include "Menu.h"
#include "MenuHandler.h"

// 保存模块句柄，释放DLL时需要用到
static HMODULE gLibModule = 0;

// 用于保存时钟窗口原始处理过程,DLL卸载时应该还原
static LONG_PTR gOldWndProc = 0;

static BOOL m_bMouseTracking = 0;

HMENU hPopMenu = NULL;
static HWND m_hwndTooltip = NULL;
CMenu* pPopupMenu = NULL;
CMenuHandler* pMenuHandler = NULL;

 TOOLINFO m_ToolTip;

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

//void LoadDrawingSetting(HWND hwnd)
//{
//	char fontname[80];
//	int size, langid, codepage;
//	LONG weight, italic;
//
//	/* ------- colors ------------- */
//
//	m_fillbackcolor = GetMyRegLong(NULL, "UseBackColor",
//		g_bVisualStyle? FALSE: TRUE);
//
//	m_colback = GetMyRegLong(NULL, "BackColor",
//		0x80000000 | COLOR_3DFACE);
//
//	if((!g_winver&WINXP) && m_fillbackcolor == FALSE)
//	{
//		m_fillbackcolor = TRUE;
//		m_colback = 0x80000000 | COLOR_3DFACE;
//	}
//
//	m_colback2 = m_colback;
//	if((g_winver&WIN98) || (g_winver&WIN2000))
//	{
//		if(GetMyRegLong(NULL, "UseBackColor2", TRUE))
//			m_colback2 = GetMyRegLong(NULL, "BackColor2", m_colback);
//		m_grad = GetMyRegLong(NULL, "GradDir", GRADIENT_FILL_RECT_H);
//	}
//
//	m_bFillTray = FALSE;
//	if(m_fillbackcolor && ((g_winver&WINME) || (g_winver&WIN2000)))
//		m_bFillTray = GetMyRegLong(NULL, "FillTray", FALSE);
//
//	m_colfore = GetMyRegLong(NULL, "ForeColor", 
//		0x80000000 | COLOR_BTNTEXT);
//
//	/* ------- font ------------- */
//
//	GetMyRegStr(NULL, "Font", fontname, 80, "");
//
//	if(fontname[0] == 0)
//	{
//		HFONT hfont;
//		LOGFONT lf;
//		hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
//		if(hfont)
//		{
//			GetObject(hfont, sizeof(lf), (LPVOID)&lf);
//			strcpy(fontname, lf.lfFaceName);
//		}
//		else strcpy(fontname, "System");
//	}
//
//	size = GetMyRegLong(NULL, "FontSize", 9);
//	if(size == 0) size = 9;
//	weight = GetMyRegLong(NULL, "Bold", 0);
//	if(weight) weight = FW_BOLD;
//	else weight = 0;
//	italic = GetMyRegLong(NULL, "Italic", 0);
//
//	if(m_hFont) DeleteObject(m_hFont);
//
//	langid = GetMyRegLong(NULL, "Locale", (int)GetUserDefaultLangID());
//	codepage = GetCodePage(langid);
//
//	// font.c
//	m_hFont = CreateMyFont(fontname, size, weight, italic, codepage);
//
//	/* ------- size and position ------------- */
//
//	g_bFitClock = FALSE;
//	if(g_winver&WINXP)
//		g_bFitClock = GetMyRegLong(NULL, "FitClock", TRUE);
//
//	m_ClockWidth = -1;
//}

void GetClockTextSize(HDC hdc, const TEXTMETRIC* ptm,
					  const wchar_t* str, int *wout, int *hout)
{
	int w, h;
	int heightFont;
	const wchar_t *p, *sp, *ep;
	SIZE sz;

	p = str; w = 0; h = 0;

	heightFont = ptm->tmHeight - ptm->tmInternalLeading;
	while(*p)
	{
		sp = p;
		while(*p && *p != 0x0d) p++;
		ep = p;
		if(*p == 0x0d) p += 2;

		if(GetTextExtentPoint32W(hdc, sp, ep - sp, &sz) == 0)
			sz.cx = (ep - sp) * ptm->tmAveCharWidth;
		if(w < sz.cx) w = sz.cx;
		h += heightFont;

		if(*p) h += 2 + 0;
	}

	*wout = w; *hout = h;
}

/*------------------------------------------------
  return size of clock
  high-order word: height, low-order word: width
--------------------------------------------------*/
LRESULT OnCalcRect(HWND hWnd)
{
	TEXTMETRIC tm;
	HDC hDC;
	//HFONT hOldFont;
	wchar_t s[MAX_PATH];
	int wclock, hclock;

	if ((GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE) == FALSE)
		return 0;

	hDC = GetDC(hWnd);
	//if (m_hFont) hOldFont = SelectObject(hDC, m_hFont);
	GetTextMetrics(hDC, &tm);

	/*if(g_sdisp2[0]) wcscpy(s, g_sdisp2);
	else if(g_sdisp1[0]) wcscpy(s, g_sdisp1);
	else MakeFormat(s, NULL, NULL, BUFSIZE_FORMAT);*/

	_stprintf(s, _T("%s"), _T("2018-08-31 15:17:00"));

	//if(g_scat1[0]) wcscat(s, g_scat1);
	//if(g_scat2[0]) wcscat(s, g_scat2);

	GetClockTextSize(hDC, &tm, s, &wclock, &hclock);

	wclock += tm.tmAveCharWidth * 2 + 5;
	hclock += (tm.tmHeight - tm.tmInternalLeading) / 2 + 5;
	if(hclock < 4) hclock = 4;

	//if(wclock > m_ClockWidth) m_ClockWidth = wclock;

	if(TRUE)
	{
		RECT rcTray, rcTaskbar;

		GetWindowRect(GetParent(hWnd), &rcTray);
		GetClientRect(GetParent(GetParent(hWnd)), &rcTaskbar);

		// horizontal task bar
		if(rcTaskbar.right - rcTaskbar.left >
			rcTaskbar.bottom - rcTaskbar.top)
		{
			hclock = rcTray.bottom - rcTray.top;
		}
		// vertical task bar
		else
			wclock = rcTray.right - rcTray.left;
	}

	//if(m_hFont) SelectObject(hDC, hOldFont);
	ReleaseDC(hWnd, hDC);

	return (hclock << 16) + wclock;
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

void FreeRemoteLibrary(HWND hWnd)
{
	RefreshTaskbar(hWnd);
	RestoreWndProc();
	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeSelf, NULL, 0, NULL));
}

void DrawClock(HWND hwnd, HDC hdc, const SYSTEMTIME* pt)
{
	//RECT rcClock;
	//TEXTMETRIC tm;
	//int x, y, wclock, hclock, wtext, htext;
	//int len;
	//wchar_t s[MAX_PATH], *p, *sp, *ep;
	//DWORD dwRop = SRCCOPY;

	//if(!m_hdcClock) CreateClockDC(hwnd);

	//if(!m_hdcClock) return;

	//GetClientRect(hwnd, &rcClock);
	//wclock = rcClock.right;
	//hclock = rcClock.bottom;

	//// copy m_hdcClockBack to m_hdcClock
	//CopyClockBack(hwnd, m_hdcClock, m_hdcClockBack, wclock, hclock);

	//if(GetFocus() == hwnd)
	//	DrawFocusRect(m_hdcClock, &rcClock);

	//if(g_sdisp2[0]) wcscpy(s, g_sdisp2);
	//else if(g_sdisp1[0]) wcscpy(s, g_sdisp1);
	//else MakeFormat(s, pt, NULL, BUFSIZE_FORMAT);  // format.c

	//if(g_scat1[0])
	//{
	//	len = wcslen(s);
	//	if(len > 0 && s[len - 1] != 0x0a && s[len - 1] != ' ')
	//		wcscat(s, L" ");
	//	wcscat(s, g_scat1);
	//}
	//if(g_scat2[0])
	//{
	//	len = wcslen(s);
	//	if(len > 0 && s[len - 1] != 0x0a && s[len - 1] != ' ')
	//		wcscat(s, L" ");
	//	wcscat(s, g_scat2);
	//}

	//GetTextMetrics(m_hdcClock, &tm);

	//GetClockTextSize(m_hdcClock, &tm, s, &wtext, &htext);

	//y = (hclock - htext)/2 - tm.tmInternalLeading/2 + m_dvpos;

	//if(m_nTextPos == 1)
	//	x = (tm.tmAveCharWidth * 2) / 3;
	//else if(m_nTextPos == 2)
	//	x = wclock - (tm.tmAveCharWidth * 2) / 3;
	//else
	//	x = wclock / 2;

	//p = s;
	//while(*p)
	//{
	//	sp = p;
	//	while(*p && *p != 0x0d) p++;
	//	ep = p;
	//	if(*p == 0x0d) p += 2;
	//	TextOutW(m_hdcClock, x, y, sp, ep - sp);

	//	if(*p) y += tm.tmHeight - tm.tmInternalLeading
	//		+ 2 + m_dlineheight;
	//}

	//if(g_nBlink > 0 && (g_nBlink % 2) == 0) dwRop = NOTSRCCOPY;

	//BitBlt(hdc, 0, 0, wclock, hclock, m_hdcClock, 0, 0, dwRop);

	//if(wtext + tm.tmAveCharWidth * 2 + m_dwidth > m_ClockWidth)
	//{
	//	m_ClockWidth = wtext + tm.tmAveCharWidth * 2 + m_dwidth;
	//	PostMessage(GetParent(GetParent(hwnd)), WM_SIZE, SIZE_RESTORED, 0);
	//	InvalidateRect(GetParent(GetParent(hwnd)), NULL, TRUE);
	//}
}

void OnPaint(HWND hwnd, HDC hdc, const SYSTEMTIME* pt)
{
	DrawClock(hwnd, hdc, pt);
}

// 新的窗口处理过程,核心工作都在这里
LRESULT CALLBACK ClockWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	// 为了能收到鼠标左击右击的消息，必须让Windows以为我们"命中"了工作区
	if (uMsg == WM_NCHITTEST)
		return HTCLIENT;

	if (uMsg == WM_USER+100)
	{
		RECT rcTClock;
		GetClientRect(hWnd, &rcTClock);
		InvalidateRect(GetParent(hWnd), &rcTClock, TRUE);
		return OnCalcRect(hWnd);
	}
	if (uMsg == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		//if(g_bNoClock) break;
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hWnd, hdc, NULL);
		EndPaint(hWnd, &ps);
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
			
		pPopupMenu->CreatePopupMenu();
		
		pPopupMenu->InsertMenu(0xFFFFFFFF, uFlag, IDM_ITEM_TEST1, _T("Test 1"));
		pPopupMenu->InsertMenu(0xFFFFFFFF, uFlag, IDM_ITEM_TEST2, _T("Test 2"));
		pPopupMenu->InsertMenu(0xFFFFFFFF, MF_SEPARATOR, IDM_ITEM_SEP, _T("SEP"));    
		pPopupMenu->InsertMenu(0xFFFFFFFF, MF_BYPOSITION|MF_STRING, IDM_ITEM_EXIT, _T("退出"));

		pPopupMenu->ShowPopupMenu(hWnd, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN, &ptClickPoint);

		pPopupMenu->DestroyMenu();
		return 0;
	}

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
		PostMessage(hWnd, WM_TCLOCK_HIGHLIGHT, TCLOCK_ENABLE, 0);
		return 0;
	}

	if (uMsg == WM_COMMAND)
	{
		 LRESULT lRet = pPopupMenu->OnCmd(hWnd, wParam, lParam);
		 if (lRet == FALSE)
			 return 0;
	}

	if (uMsg == WM_MOUSELEAVE)
	{
		m_bMouseTracking = false;
		PostMessage(hWnd, WM_TCLOCK_HIGHLIGHT, TCLOCK_DISABLE, 0);
	}

	return WNDPROC(gOldWndProc)(hWnd, uMsg, wParam, lParam);	
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

				pPopupMenu = new CMenu;
				pMenuHandler = new CMenuHandler;
				pPopupMenu->SetMenuHandler(pMenuHandler);

				gOldWndProc = GetWindowLongPtr(hClock, GWLP_WNDPROC);
				if (gOldWndProc != NULL)
					SetWindowLongPtr(hClock, GWLP_WNDPROC, (LONG_PTR)&ClockWndProc);
				
				RefreshTaskbar(hClock);
			}
			break;
		}

	case DLL_PROCESS_DETACH:
		{
			OutputDebugString(_T("DLL_PROCESS_DETACH"));
			if (pMenuHandler != NULL)
				delete pMenuHandler;

			if (pPopupMenu != NULL)
				delete pPopupMenu;
			break;
		}
	}
	return TRUE;
}