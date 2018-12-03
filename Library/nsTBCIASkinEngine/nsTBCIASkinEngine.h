#pragma once

#include "pluginapi.h"

#ifdef  TBCIASKINENGINE_EXPORTS
#define NSIS_API  extern "C" __declspec(dllexport)
#else
#define NSIS_API  __declspec(dllimport)
#endif

// 初始化 Duilib 界面库
NSIS_API void InitTBCIASkinEngine(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra);

NSIS_API void ShowPage(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void BackPage(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra);
NSIS_API void NextPage(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra);
NSIS_API void SetWindowTitle(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void FindControl(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void GetCheckStatus(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra);
NSIS_API void TBCIAClose(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void ExitTBCIASkinEngine(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void OnControlBindNSISScript(HWND hwndParent, int string_size, LPTSTR variables, stack_t **stacktop, extra_parameters *extra);


NSIS_API void SetProcessValue(HWND hwndParent, int string_size, LPTSTR variables, stack_t **stacktop, extra_parameters *extra);
//NSIS_API void SetProgressChangeCallback(HWND hwndParent, int string_size, LPTSTR variables, stack_t **stacktop, extra_parameters *extra);

NSIS_API void StartInstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);
NSIS_API void StartUninstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra);