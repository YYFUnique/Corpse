#include "StdAfx.h"
#include "Application.h"

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

void CApplication::OnRefresh(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList == NULL)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, (LPARAM)this);
}

void CApplication::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList == NULL)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, (LPARAM)this);
}

void CApplication::OnItemActive(TNotifyUI& msg)
{
	HWND hWnd = (HWND)msg.pSender->GetTag();
	if (IsWindow(hWnd) == FALSE)
		return;

	//前置显示窗口
	//SetForegroundWindow(hWnd);
	SwitchToThisWindow(hWnd,FALSE);
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
		MessageBox(NULL,_T("开发中，请继续留意^_^。"), MAIN_APP_NOTIFY_TITLE, MB_OK | MB_ICONINFORMATION);
	}
}

BOOL CApplication::EnumWindowsProc(HWND hWnd,LPARAM lParam)
{
	if (hWnd == NULL)
		return TRUE;

	CApplication* pApp = (CApplication*)lParam;
	if (pApp != NULL)
		pApp->GetAppInfoByHwnd(hWnd);

	return TRUE;
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

void CApplication::GetAppInfoByHwnd(HWND hWnd)
{
	if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd) && hWnd != m_pPaintManager->GetPaintWindow() && 
		((GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_TOOLWINDOW) !=WS_EX_TOOLWINDOW) &&
		(GetWindowLong(hWnd, GWL_HWNDPARENT)==0))
	{
		FillAppInfoIntoList(hWnd);
	}
}

void CApplication::FillAppInfoIntoList(HWND hWnd)
{
	TCHAR szWindowTitle[MAX_PATH*2];
	GetWindowText(hWnd, szWindowTitle, _countof(szWindowTitle));

	TCHAR szClassName[MAX_PATH];
	::GetClassName(hWnd, szClassName, _countof(szClassName));

	LPCTSTR lpszAppInfo = _T("正在运行");
	if (IsHungAppWindow(hWnd))
		lpszAppInfo = _T("未响应");

	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList != NULL)
	{
		TListInfoUI* pListInfo = pList->GetListInfo();
		//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
		pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);

		CFileListItemUI* pFileItem = NULL;
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(APP_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
		else
			pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

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
		pLabelTitle->SetText(szWindowTitle);
		pLabelTitle->SetFont(pListInfo->nFont);
		pLabelTitle->SetForeColor(pListInfo->dwTextColor);
		DWORD dwTextStyle = pLabelTitle->GetTextStyle();
		pLabelTitle->SetTextStyle(dwTextStyle | DT_END_ELLIPSIS);

		CLabelUI* pClassName = (CLabelUI*)pFileItem->FindSubControl(_T("ClassName"));
		pClassName->SetText(szClassName);
		pClassName->SetFont(pListInfo->nFont);
		pClassName->SetForeColor(pListInfo->dwTextColor);
		dwTextStyle = pClassName->GetTextStyle();
		pClassName->SetTextStyle(dwTextStyle | DT_END_ELLIPSIS);

		CLabelUI* pRunInfo = (CLabelUI*)pFileItem->FindSubControl(_T("RunInfo"));
		pRunInfo->SetText(lpszAppInfo);
		pRunInfo->SetFont(pListInfo->nFont);
		pRunInfo->SetForeColor(pListInfo->dwTextColor);
		dwTextStyle = pRunInfo->GetTextStyle();
		pRunInfo->SetTextStyle(dwTextStyle | DT_END_ELLIPSIS);
	}
}