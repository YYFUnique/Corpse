#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/************************************************************************/
/* 函数名称：EnableDebugPriv                                                                  */
/* 函数作用：调整进程权限				     													   */
/* 输入参数：lpszPrivilegeName 权限描述符   如:SE_DEBUG_NAME		   */
/*	返 回 值：成功返回TRUE，失败返回FALSE								               */
/************************************************************************/
DLL_API BOOL EnableDebugPriv(LPCTSTR lpszPrivilegeName);

/************************************************************************/
/* 函数名称：InjectProcess                                                                         */
/* 函数作用：将文件例如dll注入远程进程													   */
/* 输入参数：lpszFilePath 文件路径															   */
/*					 dwRemoteProcessId 进程ID	         									   */
/*					dwWaitTime 等待时间，-1为一直等待，直到响应，单位毫秒  */
/*	返 回 值：成功返回TRUE，失败返回FALSE								              */
/************************************************************************/
DLL_API BOOL InjectProcess(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId, DWORD dwWaitTime);