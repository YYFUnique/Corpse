#pragma once

#include "../Authority/ImpersonateSessionUser.h"
#include "../Authority/ImpersonateSystemUser.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//模拟当前用户权限
#define  RunFunctionAsCurrentUser() \
	CImpersonateSessionUser ImpersionateSessionUser;	\
	ImpersionateSessionUser.Impersonate()

//模拟System用户权限
#define  RunFunctionAsSystemUser() \
	CImpersonateSystemUser ImpersonateSystemUser; \
	ImpersonateSystemUser.Impersonate()

#include <Shlwapi.h>
#include <Psapi.h>
//#include <NtDll.h>

typedef struct  _tgaLS_SYSTEM_PROCESS
{
	DWORD dwProcessId;
	CString strProcessPath;
	CString strProcessName;
	CString strUserName;
	DWORD dwMemUse;
	DWORD dwThread;
	DWORD dwHandle;
	DWORD dwConsole;
	CString strFileVersion;
	CString strCompanyName;
	CString strDescrible;
	__int64 ullProcessUseTime;		//进程所用的系统时间，计算CPU使用率时用
	DWORD dwCPU;
}LS_SYSTEM_PROCESS,*PLS_SYSTEM_PROCESS;

typedef CList<LS_SYSTEM_PROCESS,LS_SYSTEM_PROCESS&> CLsSystemProcessList;

/************************************************************************/
// 函数名称：EnumSystemProcess
// 函数功能：枚举操作系统中的进程
// 函数参数：LsSystemProcessList 进程列表结构体
// 返  回 值：如果执行成功，则返回对应文件的图标在ImgList中的索引
/************************************************************************/
LS_COMMON_API BOOL EnumSystemProcess(CLsSystemProcessList& LsSystemProcessList);

/************************************************************************/
// 函数名称：GetAppIcon
// 函数功能：获取文件对应的图标
// 函数参数：lpszProcessPath 文件路径
//					 ImgList 图标文件列表
// 返  回 值：如果执行成功，则返回对应文件的图标在ImgList中的索引
//					，否则返回0，请自行初始化第1个图标
/************************************************************************/
LS_COMMON_API UINT GetModuleFileIcon(LPCTSTR lpszProcessPath,CImageList* ImgList);

/************************************************************************/
// 函数名称：GetProcessHandle
// 函数功能：打开指定进程ID的Handle
// 函数参数：dwPID		进程ID
//					 hProcess 进程句柄
// 返  回 值：如果执行成功，返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);

/************************************************************************/
// 函数名称：GetProcessFullPath
// 函数功能：获取指定进程ID的全路径
// 函数参数：dwPID			进程ID
//					 strFullPath  进程路径
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath);

/************************************************************************/
// 函数名称：ConvertToDriveLetter
// 函数功能：将设备路径转换为具体符号连接
// 函数参数：lpszDeviceName 设备名称
//					 strSysbolLnkName 符号连接名称
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL ConvertToDriveLetter(LPCTSTR lpszDeviceName,CString& strSysbolLnkName);

/************************************************************************/
// 函数名称：DosPathToNtPath
// 函数功能：将物理路径转换为逻辑路径
// 函数参数：lpszDosPath 文件的物理路径
//					 strNtPath     文件的逻辑路径
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath);

/************************************************************************/
// 函数名称：GetProcessUserName
// 函数功能：获取进程用户名
// 函数参数：dwPID 进程ID 
//					 strProcessName     进程用户名
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessUserName(DWORD dwPID,CString& strProcessName);


/************************************************************************/
// 函数名称：GetProcessCommandLine
// 函数功能：获取进程的命令行
// 函数参数：dwPid 指定进程
//					 strCmdLine     指定进程的命令行
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine);

/************************************************************************/
// 函数名称：GetProcessCommandLine
// 函数功能：获取进程的命令行
// 函数参数：dwPid 指定进程
//					 strCmdLine     指定进程的命令行
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessCurrentDirctory(DWORD dwPid,CString& strProcessCurrentDirctory);

/************************************************************************/
// 函数名称：GetParentProcessID
// 函数功能：获取指定进程的父进程ID
// 函数参数：dwCurrentProcessID 指定进程ID
//					 dwParentProcessID     父进程ID
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetParentProcessID(DWORD dwCurrentProcessID,DWORD& dwParentProcessID);

/************************************************************************/
// 函数名称：GetPriorityClass
// 函数功能：获取进程优先级
// 函数参数：dwPid 进程ID
//					 strPricrityDescribe 优先级描述
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetPriorityClass(DWORD dwPid,CString& strPricrityDescribe);

/************************************************************************/
// 函数名称：GetHardDriveLetter
// 函数功能：获取盘符数组
// 函数参数：strDriveLetterArray 磁盘盘符数组
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/

LS_COMMON_API BOOL TerminateProcessByProcessId(DWORD dwProcessId);

LS_COMMON_API BOOL TerminateProcessTreeByProcessId(DWORD dwPid);

LS_COMMON_API BOOL FreeRemoteLibrary(DWORD dwProcessID,LPCTSTR lpszModulePath);

LS_COMMON_API DWORD GetThreadEntryPtr(DWORD dwThreadID);

LS_COMMON_API BOOL GetFilePathByFileHandle(HANDLE hFile,CString& strFilePath);

LS_COMMON_API CString GetModuleAddressRange(MODULEINFO& ModuleInfo);

LS_COMMON_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

LS_COMMON_API BOOL IsSystemUserProcess();