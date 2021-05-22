#pragma once

#include "DllCore/Utils/DuiList.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef struct tagDESKTOP_INFO
{
	CString		strIconName;
	RECT		rcPos;
}DESKTOP_INFO, *PDESKTOP_INFO;

typedef CDuiList<DESKTOP_INFO, const DESKTOP_INFO&> CDesktopInfoList;

DLL_API HWND GetDesktopHWND();

DLL_API BOOL GetDestopIcon(HWND hDeskWnd, CDesktopInfoList& DesktopInfoList);

DLL_API BOOL GetDestopIcon64(HWND hDeskWnd, CDesktopInfoList& DesktopInfoList);

DLL_API BOOL GetDesktopIconRect(HWND hDeskWnd, LPCTSTR lpszIconName, LPRECT lpRect, BOOL bVague = FALSE);

DLL_API BOOL GetDesktopIconRect64(HWND hDeskWnd, LPCTSTR lpszIconName, LPRECT lpRect, BOOL bVague = FALSE);