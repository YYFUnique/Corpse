#pragma once

#ifdef LS_STATIC_LIB_CALL
#define	LS_COMMON_API
#elif defined LS_EXPORTS
#define	LS_COMMON_API  __declspec(dllexport)
#else
#define	LS_COMMON_API  __declspec(dllimport)
#endif

LS_COMMON_API BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , CString& strValue);