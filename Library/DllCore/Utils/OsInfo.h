#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_CORE_API 
#elif defined  DLL_EXPORTS
#define DLL_CORE_API __declspec(dllexport)
#else
#define DLL_CORE_API __declspec(dllimport)
#endif

DLL_CORE_API BOOL OsIsWow64Process();

DLL_CORE_API BOOL OsIsVistaOrLater();