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
/* �������ƣ�InitLsCIAMessageBox                                                           */
/* �������ܣ���ʼ����ʾ�Ի���																   */
/* ����������hWndParent �����ھ��														   */
/*					 nStringSize �ַ�������������											   	   */
/*					szVariables �ַ���																  */
/*					pStackTop ջָ��																  */
/*					lParam        ��������															  */
/* ��  ��  ֵ����										 	  */
/************************************************************************/
NSIS_API void  InitLsCIAMessageBox(HWND hWndParent, int nStringSize, LPTSTR szVariables, StackInfo** pStackTop, PARAMEX* lParam);

/************************************************************************/
/* �������ƣ�InitLsCIAMessageBox                                                           */
/* �������ܣ���ʼ����ʾ�Ի���																   */
/* ����������hWndParent �����ھ��														   */
/*					 lpszTitle �Ի��򴰿ڱ���													   	   */
/*					 lpszText �Ի�������															   */
/* ��  �� ֵ��																								*/
/************************************************************************/
NSIS_API UINT TBCIAMessageBox(HWND hWndParent,LPCTSTR lpszTitle,LPCTSTR lpszText);

NSIS_API void StartInstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);
NSIS_API void StartUninstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);
NSIS_API void InstallCore(HWND hWndParent);

NSIS_API void ShowPage(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);

NSIS_API void FindControl(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra);