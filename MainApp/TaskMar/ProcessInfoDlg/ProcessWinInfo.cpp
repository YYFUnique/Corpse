#include "stdafx.h"
#include "ProcessWinInfo.h"
#include "LsCommon/WndFunc/WndFunc.h"



BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)  
{  
	DWORD dwPid = 0;  
	PPROCESS_WND pProcessWnd = (PPROCESS_WND)lParam;
	GetWindowThreadProcessId(hWnd, &dwPid); // 获得找到窗口所属的进程  

	if(dwPid == pProcessWnd->dwPid) // 判断是否是目标进程的窗口  
	{  
		GetWndInformation(hWnd,pProcessWnd->WindList);
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // 递归查找子窗口  
	}  
	return TRUE;  
}  

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{  
	DWORD dwPid = 0; 
	PPROCESS_WND pProcessWnd = (PPROCESS_WND)lParam;
	GetWindowThreadProcessId(hWnd, &dwPid); // 获得找到窗口所属的进程
	if(dwPid == pProcessWnd->dwPid) // 判断是否是目标进程的窗口  
	{ 
		GetWndInformation(hWnd,pProcessWnd->WindList);
		if (pProcessWnd->bShowChildWnd)
			EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // 继续查找子窗口  
	}  
	return TRUE;  
} 

CProcessWinInfo::CProcessWinInfo()
{
	m_nSubMenuID = PROCESS_WINDOWS;
	m_bShowChildWnd = FALSE;
	bShowImageList = TRUE;
	m_bShowTitleWnd = TRUE;
}

// CProcessWinInfo::~CProcessWinInfo()
// {
// 
// }

BOOL CProcessWinInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("窗口名称"));
	strListColumn.Add(_T("窗口类名"));
	strListColumn.Add(_T("窗口句柄"));
	strListColumn.Add(_T("窗口ID"));
	strListColumn.Add(_T("线程ID"));
	strListColumn.Add(_T("风格"));
	strListColumn.Add(_T("扩展风格"));
	strListColumn.Add(_T("可视"));
	strListColumn.Add(_T("可用"));
	strListColumn.Add(_T("Unicode"));
	strListColumn.Add(_T("桌面"));
	strListColumn.Add(_T("窗口过程"));
	return TRUE;
}

BOOL CProcessWinInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	CTaskMarView* pTaskMar = (CTaskMarView*)GetProcessListView();
	ASSERT(pTaskMar);
	
	ListCtrl.SetRedraw(FALSE);
	ListCtrl.DeleteAllItems();
	PROCESS_WND ProcessWnd;
	ProcessWnd.dwPid = pTaskMar->GetProcessID();
	ProcessWnd.WindList.RemoveAll();
	ProcessWnd.bShowChildWnd = m_bShowChildWnd;
	EnumWindows(EnumWindowCallBack, (LPARAM)&ProcessWnd);  

	if (m_ModuleImg.GetImageCount())
		m_ModuleImg.SetImageCount(0);

	POSITION pos = ProcessWnd.WindList.GetHeadPosition();
	m_ModuleImg.Add(AfxGetApp()->LoadIcon(IDI_ICON_DEFAULT));
	while(pos)
	{
		WINDOW_INFO& WndInfo = ProcessWnd.WindList.GetNext(pos);
		if (m_bShowTitleWnd == TRUE && WndInfo.strWndTitle.IsEmpty())
			continue;

		HICON hIcon = (HICON)SendMessage(WndInfo.hWnd,WM_GETICON,ICON_SMALL2,0);

		int nIconIndex = 0;
		if (hIcon != NULL)
			nIconIndex = m_ModuleImg.Add(hIcon);

		int nIndex = ListCtrl.InsertItem(LVIF_IMAGE|LVIF_TEXT|LVIF_PARAM,0x7FFFFFFF,WndInfo.strWndTitle,0,0,nIconIndex,WndInfo.pWndProc);
		int nSubItem = 0;
		ListCtrl.SetItemText(nIndex,++nSubItem,WndInfo.strClassName);
		ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumToHexString((DWORD_PTR)WndInfo.hWnd));
		ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumToString(WndInfo.dwWndId));
		ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumToString(WndInfo.dwThreadId));
		ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumToHexStringNoPre(WndInfo.dwStyle));
		ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumToHexStringNoPre(WndInfo.dwStyleEx));
		ListCtrl.SetItemText(nIndex,++nSubItem,WndInfo.bVisible == TRUE ? _T("是") : _T("否"));
		ListCtrl.SetItemText(nIndex,++nSubItem,WndInfo.bEnable == TRUE ? _T("是") : _T("否"));
		ListCtrl.SetItemText(nIndex,++nSubItem,WndInfo.IsUnicodeWnd == TRUE ? _T("是") : _T("否"));
		ListCtrl.SetItemText(nIndex,++nSubItem,WndInfo.strDesktop);
		ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumToHexString(WndInfo.pWndProc));
	}

	ListCtrl.SetRedraw(TRUE);
	return TRUE;
}

BOOL CProcessWinInfo::OnRButtonDown(UINT nID)
{
	return TRUE;
}