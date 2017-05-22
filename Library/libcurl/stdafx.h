// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <process.h>
#include <stack>
#include <vector>


#include <sys/timeb.h>
#include <fstream>
using std::fstream;
#include <WinSock2.h>

#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "wldap32.lib" )

/*
#define ZLIB_WINAPI
#define CURL_STATICLIB
#include "libcurl/curl.h"
#pragma comment( lib, "libcurl/lib/libcurl_a.lib")
*/
//extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }