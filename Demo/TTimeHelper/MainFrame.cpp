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

		pTxMiniSkin->Create(NULL, _T("���Թܼ�ʱ������"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 800, 600);
		pTxMiniSkin->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		//�ͷŴ򿪵�ѹ�������
		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	//�رջ�����
	if (hMultex != NULL)
	{
		CloseHandle(hMultex);
		hMultex = NULL;
	}

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}