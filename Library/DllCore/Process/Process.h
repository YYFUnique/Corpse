#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/************************************************************************/
// 函数名称：GetProcessUserName
// 函数功能：获取进程用户名
// 函数参数：dwPID 进程ID 
//					 strProcessName     进程用户名
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
DLL_API BOOL GetProcessUserName(DWORD dwProcessId, CString& strProcessName);

/************************************************************************/
// 函数名称：GetProcessCommandLine
// 函数功能：获取进程的命令行
// 函数参数：dwPid 指定进程
//					 strCmdLine     指定进程的命令行
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
DLL_API BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine);

DLL_API BOOL IsRunAsAdmin();

DLL_API BOOL IsRunAsSystem();

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

DLL_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);