#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#pragma warning(disable : 4251)

#include <WinSock2.h>
#include <ws2def.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <vector>
#include <list>

#include "NetCore.h"

#pragma comment(lib,"ws2_32.lib")

#ifndef ASSERT
	#define ASSERT(expr)  _ASSERTE(expr)
#endif
