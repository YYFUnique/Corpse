#include "stdafx.h"
#include "AIToolKit.h"
#include "DllCore/File/MiniDump.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;

	do 
	{
		hRet = ::OleInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CMiniDump::InitDumpDebugInfo();

		CAIToolkit* pAiToolkit = new CAIToolkit();
		if (pAiToolkit == NULL) 
			break;

		pAiToolkit->Create(NULL, _T("亚信安全工具包"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pAiToolkit->CenterWindow();
		pAiToolkit->ShowWindow(true);
		//pPCMaster->HandleCmdLine(strCmdLine);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();
	} while (FALSE);
	
	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}