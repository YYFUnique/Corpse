#include "stdafx.h"
#include "PCHunter.h"
#include <NetCon.h>
#include "DllCore/File/MiniDump.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{	 
	CMiniDump::InitDumpDebugInfo();

	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;

	do 
	{
		hRet = ::OleInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CPCHunter* pPCHunter = new CPCHunter();
		if (pPCHunter == NULL) 
			break;

		pPCHunter->Create(NULL, _T("���Թܼ�"), UI_WNDSTYLE_FRAME, 0, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT);
		pPCHunter->CenterWindow();
		pPCHunter->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::OleUninitialize();

	return bSuccess;
}