#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef ULONG (WINAPI *PFNNtUnmapViewOfSection)(IN HANDLE ProcessHandle, IN PVOID BaseAddress);

/************************************************************************/
/* 函数名称：EnablePrivilege	                                                                  */
/* 函数作用：调整进程权限				     													   */
/* 输入参数：lpszPrivilegeName 权限描述符   如:SE_DEBUG_NAME		   */
/*					 bEnable 是否使能特权															   */
/*	返 回 值：成功返回TRUE，失败返回FALSE								               */
/************************************************************************/
DLL_API BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable = TRUE);

/************************************************************************/
/* 函数名称：InjectProcess                                                                         */
/* 函数作用：将文件例如dll注入远程进程													   */
/* 输入参数：lpszFilePath 文件路径															   */
/*					 dwRemoteProcessId 进程ID	         									   */
/*					dwWaitTime 等待时间，-1为一直等待，直到响应，单位毫秒  */
/*	返 回 值：成功返回TRUE，失败返回FALSE								              */
/************************************************************************/
//DLL_API BOOL InjectProcess(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId, DWORD dwWaitTime);

/************************************************************************/
/* 函数名称：UnLoadViewOfModule                                                         */
/* 函数作用：卸载远程进程指定模块      													   */
/* 输入参数：dwProcessId 远程进程ID														   */
/*					 lpBaseAddr 模块基地址			         									   */
/*	返 回 值：成功返回TRUE，失败返回FALSE								              */
/************************************************************************/
DLL_API BOOL UnLoadViewOfModule(DWORD dwProcessId, LPVOID lpBaseAddr);

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);
/************************************************************************/
/* 函数名称：ModifyAccessToEveryOne                                                   */
/* 函数作用：将注册表项的访问权限修改为任何人										   */
/* 输入参数：lpszRegPath 需要修改的注册表项路径									   */
/* 参数举例：CLASSES_ROOT\SomePath												   */
/*					 \\ComputerName\CLASSES_ROOT\SomePath.					   */
/*	返 回 值：成功返回TRUE，失败返回FALSE								               */
/************************************************************************/
DLL_API BOOL ModifyAccessToEveryOne(LPCTSTR lpszRegPath);