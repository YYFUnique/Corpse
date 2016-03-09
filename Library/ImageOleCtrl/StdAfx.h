// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>
#include <comdef.h>
#include "DuiLib/UIlib.h"
using namespace DuiLib;

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

extern HINSTANCE g_hinstThisDll;        // Our DLL's HINSTANCE
extern UINT      g_uDllLockCount;       // The module lock count

#ifdef _DEBUG
void TRACE (LPCTSTR lpszFormat, ...);
#else
inline void TRACE(LPCTSTR x, ...) {}
#endif

