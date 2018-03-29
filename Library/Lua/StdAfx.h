#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif


#ifdef __cplusplus
extern "C"  {
#endif

#include "libLua/lua.h"  
#include "libLua/lauxlib.h"  
#include "libLua/lualib.h"  

#ifdef __cplusplus
}
#endif

#include <atlstr.h>
#include <TCHAR.h>
#include <Windows.h>

#pragma warning (disable : 4251)