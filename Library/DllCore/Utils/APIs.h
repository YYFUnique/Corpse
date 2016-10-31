#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

DLL_API LRESULT GetUserPicturePath(LPCTSTR lpszUserName, LPWSTR pwszPicPath, UINT picPathLen);