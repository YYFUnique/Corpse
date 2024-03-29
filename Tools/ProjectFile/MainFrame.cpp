#include "StdAfx.h"
#include "ProjectFile.h"
#include "MemLeakDetect.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CProjectFile* pProjectFile = new CProjectFile();
		if (pProjectFile == NULL) 
			break;

		pProjectFile->Create(NULL, _T("工程文件拷贝工具"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 800, 600);
		pProjectFile->CenterWindow();
		pProjectFile->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}