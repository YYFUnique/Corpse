#include "StdAfx.h"
#include "WinMTR.h"
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

		CWinMTR* pWinMTR = new CWinMTR();
		if (pWinMTR == NULL) 
			break;

		pWinMTR->Create(NULL, _T("Ping & DNS ÍøÂçÕï¶Ï¹¤¾ß"), UI_WNDSTYLE_DIALOG, 0, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT);
		pWinMTR->CenterWindow();
		pWinMTR->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::OleUninitialize();

	return bSuccess;
}