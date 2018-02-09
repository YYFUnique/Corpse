#include "StdAfx.h"
#include "DynamicPackTool.h"

//������߻������
#define  DYNAMIC_PACKAGE_TOOL_NAME		_T("{F5644FCB-751C-4d6f-811B-1A9914AC7D38}")


BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMutex = NULL;
	do 
	{
		hMutex = CreateMutex(NULL, TRUE, DYNAMIC_PACKAGE_TOOL_NAME);
		if (hMutex != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
			break;

		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CDynamicPackTool* pDynamicPackTool = new CDynamicPackTool();
		if (pDynamicPackTool == NULL) 
			break;

		pDynamicPackTool->Create(NULL, _T("NSIS �������"), UI_WNDSTYLE_FRAME, 0, 800, 600);
		pDynamicPackTool->ShowWindow(true);
		pDynamicPackTool->CenterWindow();
		CPaintManagerUI::MessageLoop();

		//�ͷŴ򿪵�ѹ�������
		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (hMutex != NULL)
		CloseHandle(hMutex);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}