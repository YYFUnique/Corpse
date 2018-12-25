#include "StdAfx.h"
#include "MzfFileMon.h"
#include "DllCore/File/MiniDump.h"
BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	HRESULT hRet = S_OK;
	BOOL bSuccess = FALSE;
	CPaintManagerUI::SetInstance(hInstance);
	
	do 
	{
		hRet = ::OleInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CMiniDump::InitDumpDebugInfo();

		CMzfFileMon* pMzfFileMon = new CMzfFileMon();
		if (pMzfFileMon == NULL) 
			break;

		pMzfFileMon->Create(NULL, _T("文件管家"), UI_WNDSTYLE_DIALOG, 0, 800, 600);
		pMzfFileMon->CenterWindow();
		pMzfFileMon->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}