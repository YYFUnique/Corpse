#include "StdAfx.h"
#include "TrayClock.h"
#include "Define.h"
#include <uxtheme.h>
#include <vssym32.h>

#pragma comment(lib,"UxTheme.lib")
#pragma comment(lib, "Msimg32.lib")
extern HMODULE gLibModule;

static BOOL m_bMouseTracking = 0;

CTrayClock::CTrayClock()
{
	m_bFocus = FALSE;
	ClearClockDC();
}

CTrayClock::~CTrayClock()
{
	
}

void CTrayClock::OnFinalMessage(HWND hWnd)
{
	WinImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CTrayClock::Init(HWND hWnd)
{
	SetWindowTheme(hWnd, L"TrayNotify", NULL);
	WinImplBase::Init(hWnd);
	SetTimer(hWnd, TCLOCK_TIMER_ID_REFRESH, 1000, NULL);
}

BOOL SaveBitmapFile(HBITMAP hBitmap,LPCTSTR lpszFileName)
{
	HDC hDC;         
	//设备描述表
	int iBits;
	//当前显示分辨率下每个像素所占字节数
	WORD wBitCount;   
	//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD  dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP  Bitmap;
	//位图属性结构
	BITMAPFILEHEADER   bmfHdr;
	//位图文件头结构
	BITMAPINFOHEADER   bi;
	//位图信息头结构
	LPBITMAPINFOHEADER lpbi;
	//指向位图信息头结构
	HANDLE  fh, hDib, hPal;
	HPALETTE  hOldPal=NULL;
	//定义文件，分配内存句柄，调色板句柄
	//计算位图文件每个像素所占字节数
	hDC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);
	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize              = sizeof(BITMAPINFOHEADER);
	bi.biWidth             = Bitmap.bmWidth;
	bi.biHeight            = Bitmap.bmHeight;
	bi.biPlanes            = 1;
	bi.biBitCount          = wBitCount;
	bi.biCompression       = BI_RGB;
	bi.biSizeImage         = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed           = 0;
	bi.biClrImportant      = 0;
	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;

	//为位图内容分配内存
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	//恢复调色板   
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件    
	fh=CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh==INVALID_HANDLE_VALUE)
		return FALSE;

	//设置位图文件头
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize , &dwWritten, NULL); 

	GlobalUnlock(hDib);
	GlobalFree(hDib);

	CloseHandle(fh);
	return true;
}

void CTrayClock::ClearClockDC()
{
	//m_ClockWidth = -1;

	if(m_hdcClock) DeleteDC(m_hdcClock); 
	m_hdcClock = NULL;
	if(m_hbmpClock) DeleteObject(m_hbmpClock);
	m_hbmpClock = NULL;

	if(m_hdcClockBack) DeleteDC(m_hdcClockBack); 
	m_hdcClockBack = NULL;
	if(m_hbmpClockBack) DeleteObject(m_hbmpClockBack);
	m_hbmpClockBack = NULL;
}

void CTrayClock::CreateClockDC(HWND hWnd)
{
	COLORREF col;
	HDC hdc;

	ClearClockDC();

	//if(g_bNoClock) return;

	RECT rcClock;
	GetClientRect(hWnd, &rcClock);

	hdc = GetDC(NULL);

	if (CreateOffScreenDC(hdc, &m_hdcClock, &m_hbmpClock, rcClock.right, rcClock.bottom) == FALSE) // dllutl.c
	{
		OutputDebugString(_T("CreateOffScreenDC"));
		ReleaseDC(NULL, hdc);
		return;
	}

	//SelectObject(m_hdcClock, m_hFont);
	// 输出文字无背景演示
	SetBkMode(m_hdcClock, TRANSPARENT);

	/*if(m_nTextPos == 1)
		SetTextAlign(m_hdcClock, TA_LEFT|TA_TOP);
	else if(m_nTextPos == 2)
		SetTextAlign(m_hdcClock, TA_RIGHT|TA_TOP);
	else
		SetTextAlign(m_hdcClock, TA_CENTER|TA_TOP);*/

	col = 0x00FFFFFF;
	if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
	SetTextColor(m_hdcClock, col);

	/* ------- background -------- */

	if(m_bFillTray)
	{
		hWnd = GetParent(hWnd);
		GetClientRect(hWnd, &rcClock);
	}

	if (CreateOffScreenDC(hdc, &m_hdcClockBack, &m_hbmpClockBack, rcClock.right, rcClock.bottom) == FALSE) // dllutl.c
	{
		ClearClockDC();
		ReleaseDC(NULL, hdc);
		return;
	}

	FillClock(hWnd, m_hdcClockBack, &rcClock);
	//SaveBitmapFile(m_hbmpClockBack, _T("C:\\test2\\ColockBack2.bmp"));
	ReleaseDC(NULL, hdc);
}

BOOL CTrayClock::CreateOffScreenDC(HDC hdc, HDC *phdcMem, HBITMAP *phbmp, int width, int height)
{
	*phdcMem = CreateCompatibleDC(hdc);
	if(!*phdcMem) { *phbmp = NULL; return FALSE; }

	BITMAPINFO dib = { 0 };
	dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
	dib.bmiHeader.biWidth           = width;
	dib.bmiHeader.biHeight          = -height;
	dib.bmiHeader.biPlanes          = 1;
	dib.bmiHeader.biBitCount        = 32;
	dib.bmiHeader.biCompression     = BI_RGB;
	*phbmp = CreateDIBSection(hdc,&dib, DIB_RGB_COLORS, NULL, NULL, 0);
	HBITMAP hbmOld =(HBITMAP)SelectObject(*phdcMem, phbmp);

	/**phbmp = CreateCompatibleBitmap(hdc, width, height);
	if(!*phbmp)
	{
		DeleteDC(*phdcMem); *phdcMem = NULL;
		return FALSE;
	}*/

	SelectObject(*phdcMem, *phbmp);
	return TRUE;
}

void CTrayClock::CopyClockBack(HWND hwnd, HDC hdcDest, HDC hdcSrc, int w, int h)
{
	int xsrc = 0, ysrc = 0;

	if(m_bFillTray)
	{
		HWND hwndTray;
		RECT rc;
		POINT pt;

		hwndTray = GetParent(hwnd);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndTray, &pt);
		xsrc = pt.x; ysrc = pt.y;
	}

	BitBlt(hdcDest, 0, 0, w, h, hdcSrc, xsrc, ysrc, SRCCOPY);
}

void CTrayClock::CopyParentSurface(HWND hwnd, HDC hdcDest, int xdst, int ydst, int w, int h, int xsrc, int ysrc)
{
	HDC hdcTemp, hdcMem;
	HBITMAP hbmp;
	RECT rcParent;

	GetWindowRect(GetParent(hwnd), &rcParent);

	hdcTemp = GetDC(NULL);

	if (CreateOffScreenDC(hdcTemp, &hdcMem, &hbmp, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top) == FALSE)
	{
		ReleaseDC(NULL, hdcTemp);
		return;
	}

	SendMessage(GetParent(hwnd), WM_PRINTCLIENT, (WPARAM)hdcMem, (LPARAM)PRF_CLIENT);

	BitBlt(hdcDest, xdst, ydst, w, h, hdcMem, xsrc, ysrc, SRCCOPY);

	DeleteDC(hdcMem);
	DeleteObject(hbmp);

	ReleaseDC(NULL, hdcTemp);
}

void CTrayClock::FillClock(HWND hwnd, HDC hdc, const RECT *prc)
{
	HBRUSH hbr;
	COLORREF col;

	RECT rc, rcTray;
	GetWindowRect(hwnd, &rc);
	GetWindowRect(GetParent(hwnd), &rcTray);
	CopyParentSurface(hwnd, hdc, 0, 0, prc->right, prc->bottom,
		rc.left - rcTray.left, rc.top - rcTray.top);
}

void CTrayClock::FillClockBG(HDC hdcPaint)
{
	if (IsThemeActive())
	{
		RECT rcWnd;
		GetWindowRect(m_hWnd, &rcWnd);
		HTHEME hTheme = OpenThemeData(m_hWnd, VSCLASS_CLOCK);
		if (IsThemeBackgroundPartiallyTransparent(hTheme, CLP_TIME, 2))
		{
			DrawThemeParentBackground(m_hWnd, hdcPaint, &rcWnd);
		}

		DrawThemeBackground(hTheme, hdcPaint, CLP_TIME, 2, &rcWnd, NULL);
	}
}

void CTrayClock::DrawClock(HWND hWnd, HDC hdc, const SYSTEMTIME* pt)
{
	RECT rcClock;
	TEXTMETRIC tm;
	int x, y, wclock, hclock, wtext, htext;
	int len;
	wchar_t s[MAX_PATH],
		*p, *sp, *ep;
	DWORD dwRop = SRCCOPY;

	if(!m_hdcClock) CreateClockDC(hWnd); 

	GetClientRect(hWnd, &rcClock);
	wclock = rcClock.right;
	hclock = rcClock.bottom;

	// copy m_hdcClockBack to m_hdcClock
	//CopyClockBack(hWnd, m_hdcClock, m_hdcClockBack, wclock, hclock);

	if (TRUE)
	{
		RECT rcWnd;
		GetWindowRect(m_hWnd, &rcWnd);
		RECT rcPos = {0,0,rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top};
		HTHEME hTheme = OpenThemeData(m_hWnd, VSCLASS_CLOCK);
		if (hTheme == NULL)
		{
			OutputDebugString(_T("1111111111111111"));
		}

		HDC hMem = CreateCompatibleDC(hdc);

		int cx = rcWnd.right-rcWnd.left;
		int cy = rcWnd.bottom-rcWnd.top;

		BITMAPINFO dib = { 0 };
		dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biWidth           = 200;
		dib.bmiHeader.biHeight          = -50;
		dib.bmiHeader.biPlanes          = 1;
		dib.bmiHeader.biBitCount        = 32;
		dib.bmiHeader.biCompression     = BI_RGB;
		HBITMAP hbm = CreateDIBSection(hdc,&dib, DIB_RGB_COLORS, NULL, NULL, 0);
		HBITMAP hbmOld =(HBITMAP)SelectObject(hMem, hbm);

		//LRESULT lRet = DrawThemeEdge(hTheme, hdc, CLP_TIME, CLS_NORMAL, &rcWnd, EDGE_ETCHED, BF_LEFT|BF_RIGHT, NULL);
		/*RECT rcClock = {1,1,38,38};*/
		if (IsThemeBackgroundPartiallyTransparent(hTheme, CLP_TIME, 1))
		{
			OutputDebugString(_T("IsThemeBackgroundPartiallyTransparent"));
			DrawThemeParentBackground(m_hWnd, hMem, &rcPos);
		}
		//rcPos是相对于画布句柄位置
		LRESULT lRet = DrawThemeBackground(hTheme, hMem, CLP_TIME, 1, &rcPos, NULL);
		SaveBitmapFile(hbm,_T("C:\\test2\\clock2.bmp"));
		//LRESULT lRet = DrawThemeBackground(hTheme, hdc, CLP_TIME, 2, &rcWnd, NULL);
		/*if (lRet != S_OK )
		{
			CString strTipInfo;
			strTipInfo.Format(_T("0x%08X"),lRet);
			OutputDebugString(strTipInfo);
		}
		else
		{
			OutputDebugString(_T("S_OK"));
		}*/
		CloseThemeData(hTheme);

		BLENDFUNCTION blendFunction;
		blendFunction.AlphaFormat = AC_SRC_ALPHA;
		blendFunction.BlendFlags = 0;
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.SourceConstantAlpha = 255;
		AlphaBlend(m_hdcClock,0,0,rcPos.right,rcPos.bottom,hMem,0,0,rcPos.right,rcPos.bottom,blendFunction);
	}

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	_stprintf_s(s, _countof(s), _T("%d-%02d-%02d %02d:%02d:%02d"),SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour,SysTime.wMinute,SysTime.wSecond);

	GetTextMetrics(m_hdcClock, &tm);

	GetClockTextSize(m_hdcClock, &tm, s, &wtext, &htext);

	y = (hclock - htext)/2 - tm.tmInternalLeading/2 + 0;

	int m_nTextPos = 1;

	if(m_nTextPos == 1)
		x = (tm.tmAveCharWidth * 2) / 3;
	else if(m_nTextPos == 2)
		x = wclock - (tm.tmAveCharWidth * 2) / 3;
	else
		x = wclock / 2;

	p = s;
	while(*p)
	{
		sp = p;
		while(*p && *p != 0x0d) p++;
		ep = p;
		if(*p == 0x0d) p += 2;
		TextOutW(m_hdcClock, x, y, sp, ep - sp);

		if(*p) y += tm.tmHeight - tm.tmInternalLeading + 2 + 5;
	}

	//if(g_nBlink > 0 && (g_nBlink % 2) == 0) dwRop = NOTSRCCOPY;
	
	//ClearClockDC();

	
	BitBlt(hdc, 0, 0, wclock, hclock, m_hdcClock, 0, 0, dwRop);
	ClearClockDC();
	if (m_bFocus)
	{
		OutputDebugString(_T("focus"));
		//HBITMAP hPngFile  = LoadImageFromRes(IDB_PNG_FOCUS, gLibModule, _T("PNG"));
		//HDC hDCMem = CreateCompatibleDC(hdc);
		////HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, wclock, hclock);

		////TextOutW(hDCMem, 3,3,_T("PNG"),3);
		////SaveBitmapFile(hPngFile, _T("C:\\test2\\png.bmp"));
		//BITMAP bmp;
		//GetObject(hPngFile, sizeof(BITMAP), &bmp);	//得到一个位图对象

		//SelectObject(hDCMem, hPngFile);
		//HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDCMem, hPngFile);
		////BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hDCMem, 0, 0, SRCCOPY);

		//BLENDFUNCTION blendFunction;
		//blendFunction.AlphaFormat = 0;
		//blendFunction.BlendFlags = 0;
		//blendFunction.BlendOp = AC_SRC_OVER;
		//blendFunction.SourceConstantAlpha = 255;

		//AlphaBlend(hdc, 0, 0, 2, bmp.bmHeight, hDCMem, 0, 0, 2, bmp.bmHeight, blendFunction);

		//AlphaBlend(hdc, wclock - 2, 0, 2, bmp.bmHeight, hDCMem, bmp.bmWidth - 2, 0, 2, bmp.bmHeight , blendFunction);

		//AlphaBlend(hdc, 10, hclock - 10,wclock - 20,10,hDCMem,2,28,20,10,blendFunction);
// 		RECT rcWnd;
// 		GetWindowRect(hWnd, &rcWnd);
		/*DrawFocusRect(hdc, &rcWnd);*/
		/*DrawEdge(hdc, &rcWnd, BDR_SUNKENINNER, BF_BOTTOM);*/
		/*DrawFrameControl*/
		/*BLENDFUNCTION blendFunction;
		blendFunction.AlphaFormat = AC_SRC_ALPHA;
		blendFunction.BlendFlags = 0;
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.SourceConstantAlpha = 255;
		UpdateLayeredWindow(hWnd,screenDC,&ptSrc,&wndSize,memDC,&ptSrc,0,&blendFunction,2);*/

		//HDC hdc2 = GetDC(m_hWnd);
		/*HDC*/
		RECT rcWnd;
		GetWindowRect(m_hWnd, &rcWnd);
		RECT rcPos = {0,0,rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top};
		HTHEME hTheme = OpenThemeData(m_hWnd, VSCLASS_CLOCK);
		if (hTheme == NULL)
		{
			OutputDebugString(_T("1111111111111111"));
		}

		HDC hMem = CreateCompatibleDC(hdc);

		int cx = rcWnd.right-rcWnd.left;
		int cy = rcWnd.bottom-rcWnd.top;

		BITMAPINFO dib = { 0 };
		dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biWidth           = 200;
		dib.bmiHeader.biHeight          = -50;
		dib.bmiHeader.biPlanes          = 1;
		dib.bmiHeader.biBitCount        = 32;
		dib.bmiHeader.biCompression     = BI_RGB;
		HBITMAP hbm = CreateDIBSection(hdc,&dib, DIB_RGB_COLORS, NULL, NULL, 0);
		HBITMAP hbmOld =(HBITMAP)SelectObject(hMem, hbm);
		
		//LRESULT lRet = DrawThemeEdge(hTheme, hdc, CLP_TIME, CLS_NORMAL, &rcWnd, EDGE_ETCHED, BF_LEFT|BF_RIGHT, NULL);
		/*RECT rcClock = {1,1,38,38};*/
		if (IsThemeBackgroundPartiallyTransparent(hTheme, CLP_TIME, 2))
		{
			OutputDebugString(_T("IsThemeBackgroundPartiallyTransparent"));
			//DrawThemeParentBackground(m_hWnd, hMem, &rcPos);
		}
		//rcPos是相对于画布句柄位置
		LRESULT lRet = DrawThemeBackground(hTheme, hMem, CLP_TIME, 2, &rcPos, NULL);
		/*SaveBitmapFile(hbm,_T("C:\\test2\\clock2.bmp"));*/
		//LRESULT lRet = DrawThemeBackground(hTheme, hdc, CLP_TIME, 2, &rcWnd, NULL);
		if (lRet != S_OK )
		{
			CString strTipInfo;
			strTipInfo.Format(_T("0x%08X"),lRet);
			OutputDebugString(strTipInfo);
		}
		else
		{
			OutputDebugString(_T("S_OK"));
		}
		CloseThemeData(hTheme);
 		
		BLENDFUNCTION blendFunction;
		blendFunction.AlphaFormat = AC_SRC_ALPHA;
		blendFunction.BlendFlags = 0;
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.SourceConstantAlpha = 255;
		AlphaBlend(hdc,0,0,rcPos.right,rcPos.bottom,hMem,0,0,rcPos.right,rcPos.bottom,blendFunction);

		//DeleteObject(hPngFile);

		//SelectObject(hDCMem, hOldBitmap);
		//DeleteDC(hDCMem);
	}
	//if(wtext + tm.tmAveCharWidth * 2 /*+ m_dwidth > m_ClockWidth*/)
	//{
	//	 //wtext + tm.tmAveCharWidth * 2 + 5;
	//	PostMessage(GetParent(GetParent(hWnd)), WM_SIZE, SIZE_RESTORED, 0);
	//	InvalidateRect(GetParent(GetParent(hWnd)), NULL, TRUE);
	//}
}

LRESULT CTrayClock::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_TIMER:
			{
				if (wParam == TCLOCK_TIMER_ID_REFRESH)
				{
					InvalidateRect(m_hWnd, NULL, TRUE);
					return TRUE;
				}
			}
			break;
		case WM_THEMECHANGED:
				ClearClockDC();
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc;
				//if(g_bNoClock) break;
				hdc = BeginPaint(m_hWnd, &ps);
				DrawClock(m_hWnd, hdc, NULL);
				EndPaint(m_hWnd, &ps);
				return TRUE;
			}
			break;
		case WM_TCLOCK_HIGHLIGHT:
			{
				TCLOCK_HIGHLIGHT_WPARAM TClockwParam = (TCLOCK_HIGHLIGHT_WPARAM)wParam;
				if (TClockwParam == TCLOCK_DISABLE)
					m_bFocus = FALSE;
				else
					m_bFocus = TRUE;
				InvalidateRect(m_hWnd,NULL,TRUE);
			}
			break;
		case WM_NCHITTEST:
			return HTCLIENT;
		case WM_MOUSEMOVE:
			{
			 // Start tracking this entire window again...
				if( !m_bMouseTracking ) {
					TRACKMOUSEEVENT tme = { 0 };
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER | TME_LEAVE;
					tme.hwndTrack = m_hWnd;
					tme.dwHoverTime = 100;//m_hwndTooltip == NULL ? 100UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
					TrackMouseEvent(&tme);
					m_bMouseTracking = true;
				}

			}
			break;
		case WM_MOUSEHOVER:
			{
				//OutputDebugString(_T("WM_MOUSEHOVER"));
				PostMessage(m_hWnd, WM_TCLOCK_HIGHLIGHT, TCLOCK_ENABLE, 0);
				InvalidateRect(m_hWnd,NULL,TRUE);
				//::CallWindowProc(m_OldWndProc, m_hWnd, WM_PAINT, wParam, lParam);		
				//HDC hdc = GetDC(m_hWnd);
				//RECT rcWnd;
				//GetWindowRect(m_hWnd, &rcWnd);
				//HTHEME hTheme = OpenThemeData(m_hWnd, VSCLASS_CLOCK);
				//if (hTheme == NULL)
				//{
				//	OutputDebugString(_T("1111111111111111"));
				//}
				//InflateRect(&rcWnd, 1, 1);

				////LRESULT lRet = DrawThemeEdge(hTheme, hdc, CLP_TIME, CLS_NORMAL, &rcWnd, EDGE_ETCHED, BF_LEFT|BF_RIGHT, NULL);
				//LRESULT lRet = DrawThemeBackground(hTheme, hdc, CLP_TIME, 3, &rcWnd, NULL);
				////LRESULT lRet = DrawThemeBackground(hTheme, hdc, CLP_TIME, 2, &rcWnd, NULL);
				//if (lRet != S_OK )
				//{
				//	CString strTipInfo;
				//	strTipInfo.Format(_T("0x%08X"),lRet);
				//	OutputDebugString(strTipInfo);
				//}
				//else
				//{
				//	OutputDebugString(_T("S_OK"));
				//}
				//CloseThemeData(hTheme);
			}
			break;
		case WM_MOUSELEAVE:
			{
				OutputDebugString(_T("WM_MOUSELEAVE"));
				m_bMouseTracking = false;
				PostMessage(m_hWnd, WM_TCLOCK_HIGHLIGHT, TCLOCK_DISABLE, 0);
			}
			break;
		case WM_RBUTTONUP:
				FreeRemoteLibrary(m_hWnd);
			break;
		default:
			return WinImplBase::HandleMessage(uMsg, wParam, lParam);
	}	
	return 0;
}

LRESULT CTrayClock::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
		case WM_TCLOCK_CALC_SIZE:
				return OnCalcRect(m_hWnd);
			break;
		default:
			return WinImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	}
}

void CTrayClock::GetClockTextSize(HDC hdc, const TEXTMETRIC* ptm, const wchar_t* str, int *wout, int *hout)
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
		while(*p && *p != _T('\n')) p++;
		ep = p;
		if(*p == _T('\n')) p += 2;

		if (GetTextExtentPoint32W(hdc, sp, ep - sp, &sz) == 0)
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

LRESULT CTrayClock::OnCalcRect(HWND hWnd)
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

	_stprintf_s(s, _countof(s), _T("%s"), _T("2018-08-31 15:17:00"));

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