// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

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