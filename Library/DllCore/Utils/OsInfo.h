#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_CORE_API 
#elif defined  DLL_EXPORTS
#define DLL_CORE_API __declspec(dllexport)
#else
#define DLL_CORE_API __declspec(dllimport)
#endif

/************************************************************************/
/* 函  数  名：OsIsWow64Process                                                              */
/* 函数功能：判断当前进程是否运行在WOW64下										   */
/* 输入参数：无																						   */
/* 返  回 值：如果当前进程运行在WOW64中返回TRUE，否则返回FALSE    */
/************************************************************************/

DLL_CORE_API BOOL OsIsWow64Process();

/************************************************************************/
/* 函  数  名：OsIsVistaOrLater	                                                               */
/* 函数功能：判断当前系统是否是Vista及以后系统									   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista及以后系统返回TRUE，否则返回FALSE				   */
/************************************************************************/
DLL_CORE_API BOOL OsIsVistaOrLater();