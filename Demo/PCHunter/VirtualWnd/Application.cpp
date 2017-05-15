#include "stdafx.h"
#include <Psapi.h>
#include <Winuser.h>
#pragma comment(lib,"Psapi.lib")


CApplication::CApplication()
{
	m_bTimer = TRUE;
}

CApplication::~CApplication()
{

}

void CApplication::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE)
		OnItemClick(msg);
	else if (msg.sType == DUI_MSGTYPE_MENU)
		OnMenu(msg);
	/*else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
	{
		CDuiString strTipInfo;
		CListUI* pListText = (CListUI*)msg.pSender->GetInterface(DUI_CTR_LIST);
		CListTextElementUI* pControl = (CListTextElementUI*)pListText->GetItemAt(msg.wParam);
		OutputDebugString(pControl->GetText(0));
	}*/
	else
		CBase::Notify(msg);
}

void CApplication::OnPaint()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList == NULL)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	EnumDesktopWindows(NULL,EnumWindowsProc,NULL);
}

void CApplication::OnItemClick(TNotifyUI& msg)
{
	HWND hWnd = (HWND)msg.pSender->GetTag();
	if (hWnd == NULL)
		return;

	//前置显示窗口
	SetForegroundWindow(hWnd);
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
		STRINGorID strXmlFile(_T("AppMenu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

BOOL CApplication::EnumWindowsProc(HWND hWnd,LPARAM lParam)
{
	if (hWnd == NULL)
		return TRUE;

	if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd) && hWnd != m_pPaintManager->GetPaintWindow() && 
		((GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_TOOLWINDOW) !=WS_EX_TOOLWINDOW) &&
		(GetWindowLong(hWnd, GWL_HWNDPARENT)==0))
	{
		CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
		if (pList != NULL)
		{
			TListInfoUI* pListInfo = pList->GetListInfo();
			pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);						//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
		}
		
		CListTextElementUI* pListElementUI = new CListTextElementUI;
		pList->Add(pListElementUI);

		/*CDuiString strProcessPath, strBkImage;
		GetProcessPathByHwnd(hWnd, strProcessPath);
		SHFILEINFO shFileInfo;
		SHGetFileInfo(strProcessPath,0,&shFileInfo,sizeof(shFileInfo),SHGFI_ICON|SHGFI_SMALLICON);
		//strBkImage.Format(_T("file='%s'"),shFileInfo.hIcon);
		*/

		TCHAR szWindowTitle[1024];
		GetWindowText(hWnd,szWindowTitle,_countof(szWindowTitle));
		//pListElementUI->SetImageList(1,24,16);
		/*pListElementUI->SetStyle()*/
		/*pListElementUI->GetOwner()->GetListInfo()*/
		pListElementUI->SetTag((UINT_PTR)hWnd);
		pListElementUI->SetText(0,szWindowTitle);
		pListElementUI->SetText(1,IsHungAppWindow(hWnd) ? _T("未响应") :_T("正在运行"));
	}

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

void CApplication::OnAppMenu(CControlUI* pControl)
{
	CListUI* pAppList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pAppList->GetCurSel() == -1)
		return;

	CListTextElementUI* pItem = (CListTextElementUI*)pAppList->GetItemAt(pAppList->GetCurSel());
	HWND hWnd = (HWND)pItem->GetTag();
	if (IsWindow(hWnd) == FALSE)
		return;

	CDuiString strItemName = pControl->GetName();

	if (strItemName == _T("Switch"))
	{
		/*//隐藏任务管理器
		//ShowWindow(m_pPaintManager->GetPaintWindow(), SW_SHOWMINNOACTIVE);
		//前置目标任务
		if (IsIconic(hWnd))
			ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);*/
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
}