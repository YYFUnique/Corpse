#pragma once

#ifdef LS_STATIC_LIB_CALL
#define RPC_API 
#elif defined  LS_RPC_EXPORTS
#define RPC_API __declspec(dllexport)
#else
#define RPC_API __declspec(dllimport)
#endif

BOOL InitRpcModule();

void UninitRpcModule();

RPC_API BOOL MyRpcTest();

RPC_API BOOL UserAppExitNotify(DWORD dwProcessId);

RPC_API BOOL SetMonitorAppInfo(LPCTSTR lpszFilePath);