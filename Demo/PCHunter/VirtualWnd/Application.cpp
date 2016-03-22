#include "stdafx.h"
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
CApplication::CApplication()
{
	m_bTimer = TRUE;
}

CApplication::~CApplication()
{

}

void CApplication::OnPaint()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pList->GetCount())
		pList->RemoveAll();

	EnumDesktopWindows(NULL,EnumWindowsProc,NULL);
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

		CListTextElementUI* pListElementUI = new CListTextElementUI;
		pList->Add(pListElementUI);
		/*CDuiString strProcessPath, strBkImage;
		GetProcessPathByHwnd(hWnd, strProcessPath);
		SHFILEINFO shFileInfo;
		SHGetFileInfo(strProcessPath,0,&shFileInfo,sizeof(shFileInfo),SHGFI_ICON|SHGFI_SMALLICON);
		strBkImage.Format(_T("file='%s'"),shFileInfo.hIcon);*/

		TCHAR szWindowTitle[1024];
		GetWindowText(hWnd,szWindowTitle,_countof(szWindowTitle));
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