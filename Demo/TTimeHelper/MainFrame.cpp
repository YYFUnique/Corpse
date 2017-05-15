#include "stdafx.h"
#include "resource.h"
#include "TTimeHelper.h"
#include "MemLeakDetect.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CMemLeakDetect MemDetect;

	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CTTimeHelper* pTxMiniSkin = new CTTimeHelper();
		if (pTxMiniSkin == NULL) 
			break;

		pTxMiniSkin->Create(NULL, _T("电脑管家时间助手"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 800, 600);
		pTxMiniSkin->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		//释放打开的压缩包句柄
		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}