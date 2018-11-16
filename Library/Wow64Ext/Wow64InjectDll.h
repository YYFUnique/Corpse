#pragma once

#include <cstddef>
#include <WinDef.h>
#include <tchar.h>

#ifdef LS_STATIC_LIB_CALL
#define SPEC_API 
#elif defined WOW64EXT_EXPORTS
#define SPEC_API __declspec(dllexport)
#else
#define SPEC_API __declspec(dllimport)
#endif

SPEC_API BOOL Wow64Injectx64(DWORD processid, LPCTSTR lpszFilePath, DWORD dwWaitTime = INFINITE);