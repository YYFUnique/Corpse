#include "stdafx.h"
#include "MainWnd.h"
#include "nsTBCIASkinEngine.h"

HWND g_hwndParent;
extern HINSTANCE g_hInstance;
extra_parameters* g_pluginParms;
std::map<HWND, WNDPROC> g_windowInfoMap;

#define NSMETHOD_INIT(parent) {\
					g_pluginParms = extra; \
					g_hwndParent = parent; \
					EXDLL_INIT(); }

TBCIAMainWnd* g_pMainWnd = NULL;

static UINT_PTR PluginCallback(enum NSPIM msg)
{
	if (msg == NSPIM_GUIUNLOAD)
	{
		if (g_pMainWnd != NULL)
			g_pMainWnd->Close(IDOK);
	}
	return 0;
}

void InitTBCIASkinEngine(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	extra->RegisterPluginCallback(g_hInstance, PluginCallback);
	{
		TCHAR szFileFolder[MAX_PATH], szZIPFile[MAX_PATH], szFileName[MAX_PATH];
		popstring(szFileFolder);					// 应用程序窗口名称
		popstring(szZIPFile);						// 皮肤路径
		popstring(szFileName);					// 皮肤文件

		CPaintManagerUI::SetInstance(g_hInstance);
		CPaintManagerUI::SetResourcePath(szFileFolder);

		g_pMainWnd = new TBCIAMainWnd();
		if (g_pMainWnd == NULL)
			return;

		g_pMainWnd->SetZIPFileName(szZIPFile);
		g_pMainWnd->SetSkinFile(szFileName);

		g_pMainWnd->Create(NULL, _T("NSIS 安装向导"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
		g_pMainWnd->CenterWindow();
		g_pMainWnd->ShowWindow(FALSE);

		pushint(int(g_pMainWnd->GetHWND()));
	}
}

void ShowPage(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		g_pMainWnd->ShowWindow(true);
		MSG pMsg = { 0 };
		while (::GetMessage(&pMsg, NULL, 0, 0))
		{
			::TranslateMessage(&pMsg);
			::DispatchMessage(&pMsg);
		}

		CPaintManagerUI::Term();
	}
}

void BackPage(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		TCHAR szTabName[MAX_PATH];

		popstring(szTabName);
		CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(g_pMainWnd->GetPaintManager()->FindControl(szTabName));

		if (pTabLayout) {
			// 当前选择页面索引
			int nIndex = pTabLayout->GetCurSel();
			if (nIndex > 1)
				pTabLayout->SelectItem(nIndex - 1);
			else
				pTabLayout->SelectItem(0);
		}
	}
}

void NextPage(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		TCHAR szTabName[MAX_PATH];

		popstring(szTabName);
		CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(g_pMainWnd->GetPaintManager()->FindControl(szTabName));

		if (pTabLayout) {
			// 当前选择页面索引
			int nIndex = pTabLayout->GetCurSel();
			// 显示页面最大索引
			int nMaxIndex = pTabLayout->GetCount() - 1;
			if (nIndex < nMaxIndex)
				pTabLayout->SelectItem(nIndex+1);
			else
				pTabLayout->SelectItem(nMaxIndex);
		}
	}
}

void FindControl(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	TCHAR szControlName[MAX_PATH];
	popstring(szControlName);

	CControlUI* pControl = (CControlUI*)g_pMainWnd->GetPaintManager()->FindControl(szControlName);
	if (pControl == NULL)
		pushint(-1);
	pushint(0);
}

void GetCheckStatus(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	TCHAR szCheckName[MAX_PATH];
	popstring(szCheckName);

	CCheckBoxUI* pControl = (CCheckBoxUI*)g_pMainWnd->GetPaintManager()->FindControl(szCheckName);
	if (pControl == NULL)
		pushint(-1);
	pushint(pControl->GetCheck());
}

void OnControlBindNSISScript(HWND hwndParent, int string_size, LPTSTR variables, stack_t **stacktop, extra_parameters *extra)
{
	TCHAR szControlName[MAX_PATH];

	popstring(szControlName);
	int nCallbackID = popint();
	g_pMainWnd->SaveToControlCallbackMap(szControlName, nCallbackID);
}

void SetProcessValue(HWND hwndParent, int string_size, LPTSTR variables, stack_t **stacktop, extra_parameters *extra)
{
	TCHAR szProcessName[MAX_PATH];
	popstring(szProcessName);
	int nProgress = popint();

	CProgressUI* pProgress = (CProgressUI*)g_pMainWnd->GetPaintManager()->FindControl(szProcessName);
	if (pProgress != NULL)
		pProgress->SetValue(nProgress);
}

void SetProgressChangeCallback(HWND hwndParent, int string_size, LPTSTR variables, stack_t **stacktop, extra_parameters *extra)
{
	int nCallbackID = popint();
	g_pMainWnd->SaveToControlCallbackMap(_T("ProcessItem"), nCallbackID);
}

void TBCIAClose(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra)
{
	PostQuitMessage(0);
}

void ExitTBCIASkinEngine(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra)
{
	ExitProcess(0);
}

void SetWindowTitle(HWND hwndParent, int string_size, LPTSTR variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	TCHAR szTitle[MAX_PATH];
	popstring(szTitle);
	::SetWindowText(g_pMainWnd->GetHWND(), szTitle);
}

BOOL CALLBACK TBCIAWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL res = 0;
	std::map<HWND, WNDPROC>::iterator iter = g_windowInfoMap.find(hwnd);
	if (iter != g_windowInfoMap.end())
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("hwnd=0x%X,Msg:%d.\r\n"),hwnd,message);
		OutputDebugString(strTipInfo);
		/*if (message == WM_PAINT)
		{
			ShowWindow(hwnd, SW_SHOW);
		}*/
		/*else*/ if (message == LVM_SETITEMTEXT) // TODO  安装细节显示  等找到消息再写
		{
			;
		}
		else if (message == PBM_SETPOS)
		{
			OutputDebugString(_T("PBM_SETPOS==PBM_SETPOS"));
			//CProgressUI* pProgress = static_cast<CProgressUI*>(g_pFrame->GetPaintManager().FindControl(g_tempParam));
			//pProgress->SetMaxValue(30000);
			//if (pProgress == NULL)
			//	return 0;
			//pProgress->SetValue((int)wParam);

			/*if (pProgress->GetValue() == 30000)
			{
				CTabLayoutUI* pTab = NULL;
				int currentIndex;
				pTab = static_cast<CTabLayoutUI*>(g_hInstance->GetPaintManager().FindControl(g_installPageTabName));
				if (pTab == NULL)
					return -1;
				currentIndex = pTab->GetCurSel();
				pTab->SelectItem(currentIndex + 1);
			}*/
		}
		else
		{
			res = CallWindowProc(iter->second, hwnd, message, wParam, lParam);
		}
	}
	return res;
}

void InstallCore( HWND hwndParent )
{
	TCHAR progressName[MAX_PATH];
	ZeroMemory(progressName, MAX_PATH);
	popstring(progressName);
	//g_tempParam = progressName;
	// 接管page instfiles的消息
	g_windowInfoMap[hwndParent] = (WNDPROC)SetWindowLong(hwndParent, GWL_WNDPROC, (long)TBCIAWindowProc);
	HWND hProgressHWND = FindWindowEx(FindWindowEx(hwndParent, NULL, _T("#32770"), NULL), NULL, _T("msctls_progress32"), NULL);
	g_windowInfoMap[hProgressHWND] = (WNDPROC)SetWindowLong(hProgressHWND, GWL_WNDPROC, (long)TBCIAWindowProc);
	HWND hInstallDetailHWND = FindWindowEx(FindWindowEx(hwndParent, NULL, _T("#32770"), NULL), NULL, _T("SysListView32"), NULL);
	g_windowInfoMap[hInstallDetailHWND] = (WNDPROC)SetWindowLong(hInstallDetailHWND, GWL_WNDPROC, (long)TBCIAWindowProc);
}

void StartInstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra)
{
	InstallCore( hWndParent );
}

void StartUninstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra)
{
	InstallCore( hWndParent );
}