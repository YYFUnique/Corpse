#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif


/************************************************************************/
/* 函  数  名：SHDeleteDirectory                                                               */
/* 函数功能：删除目录，包括子目录															   */
/* 输入参数：lpszPathName 需要删除的目录											   */
/* 返  回 值：删除成功返回TRUE，失败返回FALSE									   */
/************************************************************************/
DLL_API BOOL SHDeleteDirectory(LPCTSTR lpszPathName);