#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <atlstr.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <windows.h>
#include <crtdbg.h> 
#include <vssym32.h>
#include <Shell/IShell/IShell.h>
#include <Include/Desktop.h>

using namespace IShell;

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifndef PRINT_MACRO
#define PRINT_VALUE(x) #x
#define PRINT_MACRO(x)	#x"="PRINT_VALUE(x)
#endif