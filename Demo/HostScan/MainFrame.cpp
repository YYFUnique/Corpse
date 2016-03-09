#include "stdafx.h"
#include "MainFrame.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CHostScan* pHostScan = new CHostScan();
	if (pHostScan != NULL)
	{
		pHostScan->Create(NULL, _T("主机信息扫描工具"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
		pHostScan->CenterWindow();
		pHostScan->ShowWindow();

		CPaintManagerUI::MessageLoop();
		delete pHostScan;
	}

	CPaintManagerUI::Term();
	WindowImplBase::Cleanup();

	return 0;
}