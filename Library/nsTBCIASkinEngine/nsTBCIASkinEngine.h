#pragma once

#include "pluginapi.h"

#ifdef  TBCIASKINENGINE_EXPORTS
#define NSIS_API  extern "C" __declspec(dllexport)
#else
#define NSIS_API  __declspec(dllimport)
#endif

// 初始化 Duilib 界面库
NSIS_API void InitTBCIASkinEngine(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra);

// 显示对话框
NSIS_API void ShowMsgBox(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra);

NSIS_API void ShowPage(HWND hwndParent, int nStringSize, TCHAR* lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void BackPage(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra);
NSIS_API void NextPage(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra);
NSIS_API void SetWindowTitle(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void FindControl(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void GetCheckStatus(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra);
NSIS_API void TBCIAClose(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void ExitTBCIASkinEngine(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void OnControlBindNSISScript(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);