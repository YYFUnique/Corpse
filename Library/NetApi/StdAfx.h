#pragma once
// Required for VS 2008 (fails on XP and Win2000 without this fix)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // ∑Ò‘Úlibuv±‡“Î≤ªπ˝
#endif

#pragma warning (disable : 4251)

//#define WIN32_LEAN_AND_MEAN	
//#define _CRT_SECURE_NO_DEPRECATE

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#include "NetApi.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

