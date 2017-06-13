#include "stdafx.h"
#include "SkinEngine.h"
#include "nsTBCIASkinEngine.h"

#define	NSIS_PROGRESS_ID				1004		//NSIS进度条ID

extern HINSTANCE g_hInstance;
extra_parameters* g_pluginParms;

CWindowInfoMap	g_WindowMap;

CString g_strBoxLayoutFileName;
CString g_strBoxTitleControlName;
CString g_strBoxTextControlName;
BOOL g_bMSGLoopFlag = TRUE;
CString g_strBoxCloseBtnControlName;
CString g_strBoxYESBtnControlName;
CString g_strBoxNOBtnControlName;
CString	 strProgressName;		//进度条名称
CString strInstallPageName;	//安装程序Tab页名称
CSkinEngine* g_pSkinEngine = NULL;
UINT nMaxPos = 0;
BOOL bCancleOrExit = FALSE;
HWND hNISISMain = NULL;
static UINT_PTR PluginCallback(enum NSPIM msg)
{
	return 0;
}

void InitTBCIASkinEngine(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
	g_pluginParms = extra;
	g_nStringSize = nStringSize; 
	g_pStacktop  = pStackTop;      
	g_pVariables = lpszVariables; 

	extra->RegisterPluginCallback(g_hInstance, PluginCallback);
	{
		CString strExecutableName,strSkinFileFolder,strSkinFilePath;
		PopString(strExecutableName);		//可执行文件名称
		PopString(strSkinFileFolder);			//皮肤路径
		PopString(strSkinFilePath);				//皮肤文件
		PopString(strInstallPageName);		//安装页面tab的名字

		CPaintManagerUI::SetInstance(g_hInstance);
		CPaintManagerUI::SetResourcePath(strSkinFileFolder);
		g_pSkinEngine = new CSkinEngine();
		if (g_pSkinEngine == NULL)
			return;

		g_pSkinEngine->SetSkinFile(strSkinFilePath);
		g_pSkinEngine->Create( NULL, strExecutableName, UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 0, 0 );
		g_pSkinEngine->CenterWindow();
		
		HWND hFrameHwnd = g_pSkinEngine->GetHWND();

		PushInt((int)hFrameHwnd);
	}
}

UINT TBCIAMessageBox( HWND hWndParent, LPCTSTR lpszTitle, LPCTSTR lpszText )
{
	if( g_pMessageBox == NULL )
	{
		g_pMessageBox = new DuiLib::CLsCIAMessageBox();
		if ( g_pMessageBox == NULL ) 
			return IDNO;

		g_pMessageBox->Create( hWndParent, _T(""), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 0, 0 );
		g_pMessageBox->CenterWindow();
	}

	CControlUI* pTitleControl = static_cast<CControlUI*>(g_pMessageBox->GetPaintManager().FindControl( g_strBoxTitleControlName ));
	CControlUI* pTipTextControl = static_cast<CControlUI*>(g_pMessageBox->GetPaintManager().FindControl( g_strBoxTextControlName ));
	if( pTitleControl != NULL )
		pTitleControl->SetText( lpszTitle );
	if( pTipTextControl != NULL )
		pTipTextControl->SetText( lpszText );

	if( g_pMessageBox->ShowModal() == -1 )
		return IDYES;

	return IDNO;
}

void  InitLsCIAMessageBox(HWND hWndParent, int nStringSize, LPTSTR szVariables, StackInfo** pStackTop, PARAMEX* lParam)
{
 	/*PopString(g_strBoxLayoutFileName);*/

	PopString(g_strBoxTitleControlName);
	PopString(g_strBoxTextControlName);

	PopString(g_strBoxCloseBtnControlName);
	PopString(g_strBoxYESBtnControlName);
	PopString(g_strBoxNOBtnControlName);
}

BOOL CALLBACK LsCIAWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = FALSE;
	if (hWnd == g_pSkinEngine->GetHWND())
	{
		g_pSkinEngine->HandleMessage(uMsg,wParam,lParam);
		return TRUE;
	}

	CWindowInfoMap::iterator itRator = g_WindowMap.find(hWnd);
	if( itRator != g_WindowMap.end() )
	{
		if (hWnd == hNISISMain)
			ShowWindow(hWnd,SW_HIDE);	//将NSIS本身的窗口隐藏起来
		if( uMsg == LVM_SETITEMTEXT ) // TODO  安装细节显示  等找到消息再写
		{
			;
		}
		else if( uMsg == PBM_SETPOS)	//进度条消息
		{
			CProgressUI* pProgress = static_cast<CProgressUI*>(g_pSkinEngine->GetPaintManager().FindControl(strProgressName));
			if (pProgress == NULL)
				return FALSE;
			pProgress->SetValue((int)wParam);
			if (pProgress->GetValue() == nMaxPos)
				g_pSkinEngine->OnInstallOrUninstallOK();
		}
		else
		{
			bSuccess = CallWindowProc(itRator->second, hWnd, uMsg, wParam, lParam);
		}
	}	
	return bSuccess;
}

void StartInstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
	InstallCore(hWndParent);
}

void StartUninstall(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
	InstallCore(hWndParent);
}
 
void InstallCore(HWND hWndParent)
{
	PopString(strProgressName);
	//接管page instfiles的消息
	//由于不能禁用NSIS进度条窗口，故采用HOOK窗口处理过程的方式
	hNISISMain = hWndParent;
	g_WindowMap[hWndParent] = (WNDPROC) SetWindowLong(hWndParent, GWL_WNDPROC, (long) LsCIAWindowProc);
	HWND hProgressHWND = FindWindowEx(FindWindowEx(hWndParent, NULL, _T("#32770"), NULL ), NULL, _T("msctls_progress32"), NULL );
	//获取NSIS进度条最大值
	PBRANGE ProgressRange;
	::SendMessage(hProgressHWND, PBM_GETRANGE, (WPARAM) FALSE, (LPARAM) &ProgressRange);
	nMaxPos = ProgressRange.iHigh;	//进度条的最大值
	//设置进度条的最大值
	CProgressUI* pProgress = static_cast<CProgressUI*>(g_pSkinEngine->GetPaintManager().FindControl(strProgressName));
	if (pProgress != NULL)
		pProgress->SetMaxValue(nMaxPos);

	g_WindowMap[hProgressHWND] = (WNDPROC) SetWindowLong(hProgressHWND, GWL_WNDPROC, (long) LsCIAWindowProc);
	HWND hInstallDetailHWND = FindWindowEx( FindWindowEx( hWndParent, NULL, _T("#32770"), NULL ), NULL, _T("SysListView32"), NULL ); 
	g_WindowMap[hInstallDetailHWND] = (WNDPROC) SetWindowLong(hInstallDetailHWND, GWL_WNDPROC, (long) LsCIAWindowProc);
}

void FindControl(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
	CString strControlName;
	PopString(strControlName);
	CControlUI* pControl = g_pSkinEngine->GetPaintManager().FindControl(strControlName);
	if( pControl == NULL )
		PushInt(FALSE);
	else
		PushInt(TRUE);
}

void ShowPage(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
 	ShowWindow( g_pSkinEngine->GetHWND(), TRUE );
// 	MSG msg = { 0 };
// 	while( ::GetMessage(&msg, NULL, 0, 0) && g_bMSGLoopFlag ) 
// 	{
// 		::TranslateMessage(&msg);
// 		::DispatchMessage(&msg);
// 	}
	CPaintManagerUI::MessageLoop();
	//仅在NSIS释放文件之前退出或取消安装时有效
	if (bCancleOrExit)
		ExitProcess(0);
}

void  ExitLsCIASkinEngine(HWND hWndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
	ExitProcess(0);
}

void TBCIASendMessage(HWND hwndParent, int nStringSize, LPTSTR lpszVariables, StackInfo** pStackTop, extra_parameters *extra)
{
	int nValue;
	if (PopInt(nValue) == FALSE)
		return;
	HWND hWnd = (HWND)nValue;
	CString strMsgID,wParam,lParam;
	PopString(strMsgID);
	PopString(wParam);
	PopString(lParam);

	if (strMsgID.CompareNoCase(_T("WM_TBCIAMIN")) == 0)
		::SendMessage(hWnd, WM_LSCIAMIN,0,0);
	else if(strMsgID.CompareNoCase(_T("WM_TBCIACLOSE")) == 0)
		::SendMessage(hWnd, WM_LSCIACLOSE,0,0);
	else if(strMsgID.CompareNoCase(_T("WM_TBCIABACK")) == 0)
		::SendMessage(hWnd, WM_LSCIABACK,0,0);
	else if(strMsgID.CompareNoCase(_T("WM_TBCIANEXT")) == 0)
		::SendMessage(hWnd, WM_LSCIANEXT,0,0);
	else if(strMsgID.CompareNoCase(_T("WM_TBCIAFINASH")) == 0)
		::SendMessage(hWnd, WM_LSCIAFINASH,0,0);
	else if(strMsgID.CompareNoCase(_T("WM_TBCIAINSTALL")) == 0)
		::SendMessage(hWnd,WM_LSCIAINSTALL,(WPARAM)(LPCTSTR)wParam,0);
}