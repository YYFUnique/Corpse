#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

YCLIB_CORE_API LRESULT GetUserPicturePath(LPCTSTR lpszUserName, LPWSTR pwszPicPath, UINT picPathLen);