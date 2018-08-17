#include "stdafx.h"
#include "SignTool.h"

#include "Deamon/Deamon.h"
#include "Deamon/Service.h"
#include "Deamon/Monitor.h"

#include "Rpc/libRpc.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	//InstallService("Deamon");

	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = CoInitialize(NULL);
	if (FAILED(hRet))
		return 0;

	/*IMonitor* pMonitor = NULL;
	do 
	{
		hRet = CoCreateInstance(CLSID_Monitor,NULL,CLSCTX_LOCAL_SERVER,IID_IMonitor,(LPVOID*)&pMonitor);
		if (FAILED(hRet))
			break;
		hRet = pMonitor->AddAppInfo(_T("C:\\Aquarius.exe"));
		if (FAILED(hRet))
			break;

	} while (FALSE);

	if (pMonitor != NULL)
		pMonitor->Release();*/
	
	SetMonitorAppInfo(_T("C:\\Aquarius.exe"));

	CSignTool* pSignTool = new CSignTool();
	if (pSignTool == NULL) 
		return 0;

	pSignTool->Create(NULL, _T("自签名证书制作工具"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 800, 600);
	pSignTool->CenterWindow();
	pSignTool->ShowWindow(true);

	CPaintManagerUI::MessageLoop();

	CPaintManagerUI::Term();

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}