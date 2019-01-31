#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#pragma warning(disable : 4251)

#include <atlstr.h>
#include "NetCore.h"

#pragma comment(lib,"ws2_32.lib")

#ifndef ASSERT
	#define ASSERT(expr)  _ASSERTE(expr)
#endif

#define s6_addr16 u.Word