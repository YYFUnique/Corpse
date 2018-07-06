#include "stdafx.h"
#include "HiDPIWnd.h"

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

		CHiDPIWnd* pHiDPIWnd = new CHiDPIWnd();
		if (pHiDPIWnd == NULL) 
			break;

		pHiDPIWnd->Create(NULL, _T("HiDPI Demo"), UI_WNDSTYLE_DIALOG, 0, 800, 600);
		pHiDPIWnd->CenterWindow();
		pHiDPIWnd->SetDPI(CDPI::GetMainMonitorDPI()/*120*/);
		pHiDPIWnd->ShowWindow(true);

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