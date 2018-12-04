#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/************************************************************************/
/* 函  数  名：ShowDesktop		                                                               */
/* 函数功能：最小化所有窗口，显示桌面													   */
/* 输入参数：无																						   */
/* 返  回 值：无																						   */
/************************************************************************/
DLL_API void ShowDesktop();