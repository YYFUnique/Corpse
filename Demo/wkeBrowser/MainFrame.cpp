#include "stdafx.h"
#include "wkeBrowser.h"

#pragma comment(lib,"wke.lib")

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(NULL);

	wkeInit();
    CPaintManagerUI::SetInstance(hInstance);
	CWkeBrowser* pWkeBrowser = new CWkeBrowser();
    pWkeBrowser->Create(NULL, _T("wkeÄÚºËä¯ÀÀÆ÷"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);

	pWkeBrowser->CenterWindow();
	pWkeBrowser->ShowModal();

	wkeShutdown();
    ::CoUninitialize();
    return 0;
}
