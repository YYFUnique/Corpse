#pragma once

#include "DllCore/DllCore.h"

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

typedef struct  _tgaPROCESS_ITEM_INFO
{
	DWORD dwProcessId;
	CString strProcessPath;
	CString strProcessName;
	CString strUserName;
	DWORD dwMemUse;
	DWORD dwThread;
	DWORD dwHandle;
	DWORD dwConsole;
	__int64 ullProcessUseTime;		//进程所用的系统时间，计算CPU使用率时用
}PROCESS_ITEM_INFO,*PPROCESS_ITEM_INFO;

typedef CDuiList<PROCESS_ITEM_INFO, const PROCESS_ITEM_INFO&> CProcessItemInfoList;

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

//DLL_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);

DLL_API BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath);

DLL_API BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath);

DLL_API BOOL TerminateProcessByProcessId(DWORD dwProcessId);

DLL_API BOOL GetProcessCurrentDirctory(DWORD dwPid,CString& strProcessCurrentDirctory);

DLL_API BOOL GetParentProcessID(DWORD dwPid,DWORD& dwParentProcessID);

DLL_API BOOL GetPriorityClass(DWORD dwPid,CString& strPricrityDescribe);

/************************************************************************/
// 函数名称：EnumProcessInfo
// 函数功能：枚举操作系统中的进程
// 函数参数：ProcessItemInfoList 进程列表结构体
// 返  回 值：如果执行成功，则返回对应文件的图标在ImgList中的索引
/************************************************************************/
DLL_API BOOL EnumProcessInfo(CProcessItemInfoList& ProcessItemInfoList);