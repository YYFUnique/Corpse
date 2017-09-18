#pragma once
// Required for VS 2008 (fails on XP and Win2000 without this fix)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#pragma warning (disable : 4251)
/*#include "YCLibCore.h"*/


#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <shellapi.h>
#include <crtdbg.h> 
#include <atlstr.h>

#include "Utils/DuiList.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

