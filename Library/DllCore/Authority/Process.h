#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// 模拟当前用户权限
#define  RunFunctionAsCurrentUser() \
	CImpersonateSessionUser ImpersionateSessionUser;	\
	ImpersionateSessionUser.Impersonate()

// 模拟System用户权限
#define  RunFunctionAsSystemUser() \
	CImpersonateSystemUser ImpersonateSystemUser; \
	ImpersonateSystemUser.Impersonate()

// 模拟TrustedInstall用户权限
#define RunFunctionAsTrustedInstallUser() \
	CImpersonateTrustedInstallUser ImpersonateTrustedInstallUser;	\
	ImpersonateTrustedInstallUser.Impersonate()

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

/************************************************************************/
// 函数名称：IsRunAsAdmin
// 函数功能：判断是否以管理员权限允许
// 函数参数：无
// 返  回 值：如果以管理员权限允许放回TRUE，否则放回FALSE
/************************************************************************/
DLL_API BOOL IsRunAsAdmin();

/************************************************************************/
// 函数名称：IsRunAsSystem
// 函数功能：判断是否以服务权限允许
// 函数参数：无
// 返  回 值：如果以服务权限允许放回TRUE，否则放回FALSE
/************************************************************************/
DLL_API BOOL IsRunAsSystem();

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

DLL_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);

DLL_API BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath);

DLL_API BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath);