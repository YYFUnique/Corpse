#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

typedef struct _tgaWINDOW_INFO 
{
	CString strWndTitle;
	CString strClassName;
	HWND hWnd;
	DWORD dwWndId;
	DWORD dwThreadId;
	DWORD dwStyle;
	DWORD dwStyleEx;
	BOOL bVisible;
	BOOL bEnable;
	BOOL IsUnicodeWnd;
	CString strDesktop;
	DWORD_PTR pWndProc;
}WINDOW_INFO,*PWINDOW_INFO;

typedef CList<WINDOW_INFO,WINDOW_INFO&> CWindowInfoList;

typedef struct tgaPROCESSWND
{
	DWORD dwPid;
	BOOL bShowChildWnd;
	CWindowInfoList WindList;
}PROCESS_WND,*PPROCESS_WND;


LS_COMMON_API HWND GetTrayWndHwnd();

LS_COMMON_API HWND GetOverflowHwnd();

LS_COMMON_API BOOL GetWndInformation(HWND hWnd,CWindowInfoList& WindowInfoList);

LS_COMMON_API BOOL CopyScreenToBitmap(LPRECT lpRect,HBITMAP& hDesktopMap,BOOL bSave = FALSE);

LS_COMMON_API CBitmap* CoverDesktopBmp(LPRECT lpRect,HBITMAP hDesktopBitMap,HBITMAP hCoverBitMap);

LS_COMMON_API void EnsureRectVisible(LPRECT lpRect,int nRectWidth);

LS_COMMON_API void EnsureRectIsVisible(LPRECT lpRect,LPRECT lpRectContainer);
