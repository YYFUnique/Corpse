#pragma once

#include "pluginapi.h"
#include "EngineMsg.h"

#ifdef  LSCIASKINENGINE_EXPORTS
#define NSIS_API  extern "C" __declspec(dllexport)
#else
#define NSIS_API  __declspec(dllimport)
#endif

typedef	extra_parameters PARAMEX;
typedef	std::map<HWND, WNDPROC> CWindowInfoMap;
DuiLib::CLsCIAMessageBox* g_pMessageBox = NULL;


NSIS_API void InitTBCIASkinEngine(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);
/************************************************************************/
/* 函数名称：InitLsCIAMessageBox                                                           */
/* 函数功能：初始化提示对话框																   */
/* 函数参数：hWndParent 父窗口句柄														   */
/*					 nStringSize 字符串缓冲区长度											   	   */
/*					szVariables 字符串																  */
/*					pStackTop 栈指针																  */
/*					lParam        其他参数															  */
/* 返  回  值：无										 	  */
/************************************************************************/
NSIS_API void  InitLsCIAMessageBox(HWND hWndParent, int nStringSize, LPTSTR szVariables, StackInfo** pStackTop, PARAMEX* lParam);

/************************************************************************/
/* 函数名称：InitLsCIAMessageBox                                                           */
/* 函数功能：初始化提示对话框																   */
/* 函数参数：hWndParent 父窗口句柄														   */
/*					 lpszTitle 对话框窗口标题													   	   */
/*					 lpszText 对话框内容															   */
/* 返  回 值：																								*/
/************************************************************************/
NSIS_API UINT TBCIAMessageBox(HWND hWndParent,LPCTSTR lpszTitle,LPCTSTR lpszText);

NSIS_API void StartInstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);
NSIS_API void StartUninstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);
NSIS_API void InstallCore(HWND hWndParent);

NSIS_API void ShowPage(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);

NSIS_API void FindControl(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);