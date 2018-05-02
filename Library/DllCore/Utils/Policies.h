#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

DLL_API void SetTaskmgrStatus(BOOL bEnable = TRUE);

DLL_API void SetTaskmgrStatusForMachine(BOOL bEnable = TRUE);