#include "StdAfx.h"
#include "Application.h"
#include "DllCore/Log/LogHelper.h"
#include <Psapi.h>
#include <Winuser.h>
#pragma comment(lib,"Psapi.lib")

CApplication::CApplication()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

CApplication::~CApplication()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CApplication, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TIMEREX, OnTimerEx)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE, OnItemActive)
DUI_END_MESSAGE_MAP()

void CApplication::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CApplication::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("App"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(APPLICATION_MENU);
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

void CApplication::OnTimerEx(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList == NULL)
		return;

	CRunAppInfoList RunApps;
	if (GetRunAppsInfo(RunApps) == FALSE)
		return;

	// 判断是否存在窗口创建
	POSITION pos = RunApps.GetHeadPosition();
	while(pos)
	{
		const RUNNING_APP_INFO& RunAppInfo = RunApps.GetNext(pos);

		BOOL bFindWnd = FALSE;
		for (int n=0; n<pList->GetCount(); ++n)
		{
			CFileListItemUI* pAppInfo = (CFileListItemUI*)pList->GetItemAt(n);

			CString strTitle = pAppInfo->GetSubControlText(_T("Title"));
			CString strClassName = pAppInfo->GetSubControlText(_T("ClassName"));
			CString strRunInfo = pAppInfo->GetSubControlText(_T("RunInfo"));

			if ((HWND)pAppInfo->GetTag() != RunAppInfo.hMainWnd)
				continue;

			bFindWnd = TRUE;
			if (strTitle.CompareNoCase(RunAppInfo.strTitle) != 0)
				pAppInfo->SetSubControlText(_T("Title"), RunAppInfo.strTitle);
			if (strClassName.CompareNoCase(RunAppInfo.strClassName) != 0)
				pAppInfo->SetSubControlText(_T("ClassName"), RunAppInfo.strClassName);
			if (strRunInfo.CompareNoCase(_T("正在运行")) != 0)
				pAppInfo->SetSubControlText(_T("RunInfo"), RunAppInfo.bRunning ? _T("无响应") : _T("正在运行"));

			break;
		}

		if (bFindWnd == FALSE)
		{
			TListInfoUI* pListInfo = pList->GetListInfo();
			// 添加新创建的窗口
			if (RunAppInfo.hMainWnd != m_pPaintManager->GetPaintWindow())
				CreateNewAppItem(pList, pListInfo, RunAppInfo);
		}
	}

	// 判断是否存在窗口销毁
	for (int n=0; n<pList->GetCount(); ++n)
	{
		CFileListItemUI* pAppInfo = (CFileListItemUI*)pList->GetItemAt(n);

		CString strTitle = pAppInfo->GetSubControlText(_T("Title"));
		CString strClassName = pAppInfo->GetSubControlText(_T("ClassName"));
		CString strRunInfo = pAppInfo->GetSubControlText(_T("RunInfo"));

		BOOL bFindWnd = FALSE;
		POSITION pos = RunApps.GetHeadPosition();
		while(pos)
		{
			const RUNNING_APP_INFO& RunAppInfo = RunApps.GetNext(pos);

			if ((HWND)pAppInfo->GetTag() != RunAppInfo.hMainWnd)
				continue;

			bFindWnd = TRUE;
			break;
		}
		if (bFindWnd == FALSE)
			pList->RemoveAt(n);
	}
}

void CApplication::OnRefresh(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList == NULL)
		return;

	CRunAppInfoList RunApps;
	if (GetRunAppsInfo(RunApps) == FALSE)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	TListInfoUI* pListInfo = pList->GetListInfo();
	//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
	pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);

	POSITION pos = RunApps.GetHeadPosition();
	while(pos)
	{
		const RUNNING_APP_INFO& RunAppInfo = RunApps.GetNext(pos);
		if (RunAppInfo.hMainWnd != m_pPaintManager->GetPaintWindow())
			CreateNewAppItem(pList, pListInfo, RunAppInfo);
	}
}

void CApplication::CreateNewAppItem(CListUI* pList, const TListInfoUI* pListInfo, const RUNNING_APP_INFO& RunAppInfo)
{
	CFileListItemUI* pFileItem = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(APP_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
	else
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

	HWND hWnd = RunAppInfo.hMainWnd;
	pList->Add(pFileItem);
	pFileItem->SetTag((UINT_PTR)hWnd);
	pFileItem->SetFixedHeight(27);

	HICON	hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, NULL);
	if (hIcon == NULL)
		hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL2, NULL);
	if (hIcon == NULL)
		hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICONSM);
	if (hIcon != NULL)
	{
		CPictureUI* pPicIcon = (CPictureUI*)pFileItem->FindSubControl(_T("Pic"));
		pPicIcon->SetVisible(true);
		pPicIcon->SetIcon(hIcon);
	}

	CLabelUI* pLabelTitle = (CLabelUI*)pFileItem->FindSubControl(_T("Title"));
	pLabelTitle->SetText(RunAppInfo.strTitle);
	pLabelTitle->SetFont(pListInfo->nFont);
	pLabelTitle->SetForeColor(pListInfo->dwTextColor);
	pLabelTitle->AppendTextStyle(DT_END_ELLIPSIS);

	CLabelUI* pClassName = (CLabelUI*)pFileItem->FindSubControl(_T("ClassName"));
	pClassName->SetText(RunAppInfo.strClassName);
	pClassName->SetFont(pListInfo->nFont);
	pClassName->SetForeColor(pListInfo->dwTextColor);
	pClassName->AppendTextStyle(DT_END_ELLIPSIS);

	CLabelUI* pRunInfo = (CLabelUI*)pFileItem->FindSubControl(_T("RunInfo"));
	pRunInfo->SetText(RunAppInfo.bRunning ? _T("无响应") : _T("正在运行"));
	pRunInfo->SetFont(pListInfo->nFont);
	pRunInfo->SetForeColor(pListInfo->dwTextColor);
	pRunInfo->AppendTextStyle(DT_END_ELLIPSIS);
}

void CApplication::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	OnRefresh(msg);
}

void CApplication::OnItemActive(TNotifyUI& msg)
{
	HWND hWnd = (HWND)msg.pSender->GetTag();
	if (IsWindow(hWnd) == FALSE)
		return;

	//前置显示窗口
	//SetForegroundWindow(hWnd);
	if (IsIconic(hWnd))
		ShowWindow(hWnd, SW_RESTORE);
	SwitchToThisWindow(hWnd, TRUE);
}

void CApplication::OnAppMenu(CControlUI* pControl)
{
	CListUI* pAppList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pAppList->GetCurSel() == INVALID_ITEM_INDEX)
		return;

	CListTextElementUI* pItem = (CListTextElementUI*)pAppList->GetItemAt(pAppList->GetCurSel());
	HWND hWnd = (HWND)pItem->GetTag();
	if (IsWindow(hWnd) == FALSE)
		return;

	CDuiString strItemName = pControl->GetName();

	if (strItemName == _T("Switch"))
	{
		//隐藏任务管理器
		//前置目标任务
		if (IsIconic(hWnd))
			ShowWindow(hWnd, SW_RESTORE);
		SwitchToThisWindow(hWnd,TRUE);
	}
	else if (strItemName == _T("Advanced"))
	{
		if (IsIconic(hWnd))
			ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
	}
	else if (strItemName == _T("Maximized"))
		::ShowWindow(hWnd, SW_MAXIMIZE);
	else if (strItemName == _T("Minimized"))
		::ShowWindow(hWnd, SW_MINIMIZE);
	else if (strItemName == _T("Terminate"))
	{	//结束一个任务
		DWORD_PTR dwResult = 0;
		LRESULT lRet = SendMessageTimeout(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0, SMTO_BLOCK|SMTO_ABORTIFHUNG, 250, &dwResult);
		if (lRet == ERROR_TIMEOUT )
		{
			DWORD dwPid = 0;
			GetWindowThreadProcessId(hWnd,&dwPid);
			HANDLE  hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}
	}
	else if (strItemName == _T("ProcessInfo"))
	{
		MessageBox(m_pPaintManager->GetPaintWindow(),_T("开发中，请继续留意^_^。"), MAIN_APP_NOTIFY_TITLE, MB_OK | MB_ICONINFORMATION);
	}
}

BOOL CApplication::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	if (hWnd == NULL)
		return TRUE;

	if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd) && 
		((GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) !=WS_EX_TOOLWINDOW) &&
		(GetWindowLong(hWnd, GWL_HWNDPARENT) == 0))
	{
		CRunAppInfoList* pRunApps = (CRunAppInfoList*)lParam;

		RUNNING_APP_INFO RunAppInfo;
		RunAppInfo.hMainWnd = hWnd;

		// 获取窗口标题
		TCHAR szTitle[MAX_PATH*2];
		GetWindowText(hWnd, szTitle, _countof(szTitle));
		RunAppInfo.strTitle = szTitle;

		// 获取窗口类名称
		TCHAR szClassName[MAX_PATH];
		::GetClassName(hWnd, szClassName, _countof(szClassName));
		RunAppInfo.strClassName = szClassName;

		// 获取窗口是否无响应
		RunAppInfo.bRunning = IsHungAppWindow(hWnd);

		pRunApps->AddTail(RunAppInfo);
	}

	return TRUE;
}

BOOL CApplication::GetRunAppsInfo(CRunAppInfoList& RunApps)
{
	return EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, (LPARAM)&RunApps);
}

void CApplication::GetProcessPathByHwnd(HWND hWnd,CDuiString& strProcesspath)
{
	HANDLE hProcess = NULL;
	do 
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hWnd,&dwPid);
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwPid);
		if (hProcess == NULL)
			break;

		TCHAR szProcessName[MAX_PATH];
		if (GetProcessImageFileName(hProcess,szProcessName,_countof(szProcessName)) == 0)
			break;

		DosPathToNtPath(szProcessName,strProcesspath);

	} while (FALSE);

	if (hProcess)
		CloseHandle(hProcess);
}

bool CApplication::DosPathToNtPath(LPCTSTR lpszDosPath, CDuiString& strNtPath)
{
	TCHAR			szDriveStr[500];
	TCHAR			szDrive[3];
	TCHAR			szDevName[100];
	INT				cchDevName;
	INT				i;

	//ASSERT_STR_NOT_NULL(lpszDosPath);
	//获取本地磁盘字符串
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for(i = 0; szDriveStr[i]; i += 4)
		{
			if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = _T('\0');
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if(_tcsnicmp(lpszDosPath, szDevName, cchDevName) == 0)//命中
			{
				strNtPath.Format(_T("%s%s"),szDrive,lpszDosPath+cchDevName);
				return TRUE;
			}
		}
	}
	strNtPath = lpszDosPath;
	return FALSE;
}
