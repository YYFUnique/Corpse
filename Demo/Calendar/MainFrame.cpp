#include "stdafx.h"
#include "Calendar.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CCalendar* pCalendar = new CCalendar();
		if (pCalendar == NULL) 
			break;

		pCalendar->Create(NULL, _T("×ÛºÏÈÕÀú"), UI_WNDSTYLE_DIALOG, 0, 0, 800, 600);
		pCalendar->CenterWindow();
		pCalendar->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}