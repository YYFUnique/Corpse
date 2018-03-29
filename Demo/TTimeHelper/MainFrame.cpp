#include "stdafx.h"
#include "resource.h"
#include "TxMiniSkin.h"
#include "TTimeHelper.h"
#include "MemLeakDetect.h"
#include "DllCore/File/MiniDump.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
//#ifdef _DEBUG
//	CMemLeakDetect MemDetect;
//#endif	

	CMiniDump::InitDumpDebugInfo(MiniDumpWithPrivateReadWriteMemory);
	CTxMiniSkin::SetAutoStart(TRUE);

	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		hMultex= CreateMutex(NULL, FALSE, _T("TTimeHelper"));
		if (hMultex != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
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

	//关闭互斥句柄
	if (hMultex != NULL)
	{
		CloseHandle(hMultex);
		hMultex = NULL;
	}

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}