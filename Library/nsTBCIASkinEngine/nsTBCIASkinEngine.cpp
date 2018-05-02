#include "stdafx.h"
#include "MainWnd.h"
#include "nsTBCIASkinEngine.h"

//#define	NSIS_PROGRESS_ID				1004		//NSIS进度条ID
HWND g_hwndParent;
extern HINSTANCE g_hInstance;
extra_parameters* g_pluginParms;


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

void InitTBCIASkinEngine(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
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
		g_pMainWnd->ShowWindow(true);

		pushint((int)g_pMainWnd->GetHWND());
	}
}

void ShowPage(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		g_pMainWnd->ShowWindow(true);
		MSG msg = { 0 };
		while(::GetMessage(&msg, NULL, 0, 0)) 
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		CPaintManagerUI::Term();
	}
}

void BackPage(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
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

void NextPage(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
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

void ShowMsgBox(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		TCHAR szTitle[MAX_PATH], szText[MAX_PATH];
		popstring(szText);
		popstring(szTitle);

		BOOL bSuccess = FALSE;

		do 
		{
			if (CPaintManagerUI::GetInstance() == NULL)
				CPaintManagerUI::SetInstance(g_hInstance);

			TBCIAMessageBox* pMessageBox = new TBCIAMessageBox(); 
			if (pMessageBox == NULL)
				break;

			HWND hParent = NULL;
			if (g_pMainWnd != NULL)
				hParent = g_pMainWnd->GetHWND();

			pMessageBox->Create(hParent, _T("TBCIAMessageBox"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW , 0, 0, 800, 600);
			pMessageBox->CenterWindow(); 

			CControlUI* pTitle = static_cast<CControlUI*>(pMessageBox->GetPaintManager()->FindControl(_T("Title"))); 
			CControlUI* pTipText = static_cast<CControlUI*>(pMessageBox->GetPaintManager()->FindControl(_T("MsgText"))); 

			if (pTitle != NULL) 
				pTitle->SetText(szTitle); 

			if (pTipText != NULL)
				pTipText->SetText(szText);

			if (pMessageBox->ShowModal() != IDYES)
				break;

			bSuccess = TRUE;
		} while (FALSE);

		// 返回0表示不继续执行，非0代表继续执行
		if (bSuccess == FALSE)
			pushint(FALSE);
		else
			pushint(TRUE);
	}
}

void FindControl(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
{
	TCHAR szControlName[MAX_PATH];
	popstring(szControlName);

	CControlUI* pControl = (CControlUI*)g_pMainWnd->GetPaintManager()->FindControl(szControlName);
	if (pControl == NULL)
		pushint(-1);
	pushint(0);
}

void GetCheckStatus(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	TCHAR szCheckName[MAX_PATH];
	popstring(szCheckName);

	CCheckBoxUI* pControl = (CCheckBoxUI*)g_pMainWnd->GetPaintManager()->FindControl(szCheckName);
	if (pControl == NULL)
		pushint(-1);
	pushint(pControl->GetCheck());
}

void OnControlBindNSISScript(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	TCHAR szControlName[MAX_PATH];

	popstring(szControlName);
	int nCallbackID = popint();
	g_pMainWnd->SaveToControlCallbackMap(szControlName, nCallbackID);
}

void TBCIAClose(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra)
{
	PostQuitMessage(0);
}

void ExitTBCIASkinEngine(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, stack_t** pStackTop, extra_parameters *extra)
{
	ExitProcess(0);
}

void SetWindowTitle(HWND hwndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	TCHAR szTitle[MAX_PATH];
	popstring(szTitle);
	::SetWindowText(g_pMainWnd->GetHWND(), szTitle);
}