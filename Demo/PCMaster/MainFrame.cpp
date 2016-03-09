#include "stdafx.h"
#include "PCMaster.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;
	do 
	{
		CString strCmdLine(szCmdLine);
		hMultex= CreateMutex(NULL,FALSE,_T("JumpListMutex"));
		if (hMultex == NULL /*&& ERROR_ALREADY_EXISTS*/)
		{
			//获取先前实例主窗口句柄  
			HANDLE hMMFFile = OpenFileMapping(FILE_MAP_READ, FALSE, _T("Local//MMFTestJumpList"));  
			if (hMMFFile == NULL) return 0;  
			LPVOID lpVoid = MapViewOfFile(hMMFFile, FILE_MAP_READ, 0, 0, 0);  
			HWND hWndPrev = NULL;  
			memcpy_s(&hWndPrev, sizeof(hWndPrev), lpVoid, sizeof(hWndPrev));  
			UnmapViewOfFile(hMMFFile);  
			CloseHandle(hMMFFile); 

			//将命令行参数发送到先前实例  
			COPYDATASTRUCT CopyDataStruct = {0};  
			CopyDataStruct.dwData = 1;  
			CopyDataStruct.lpData = (LPVOID)(LPCTSTR)strCmdLine;  
			CopyDataStruct.cbData = (strCmdLine.GetLength()+1)*sizeof(TCHAR);  
			SendMessage(hWndPrev, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&CopyDataStruct));  

			break;
		}

		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CPCMaster* pPCMaster = new CPCMaster();
		if (pPCMaster == NULL) 
			break;

		pPCMaster->Create(NULL, _T("电脑管家"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pPCMaster->CenterWindow();
		pPCMaster->ShowWindow(true);
		//pPCMaster->HandleCmdLine(strCmdLine);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

 	if (hMultex != NULL)
 		CloseHandle(hMultex);

	return bSuccess;
}