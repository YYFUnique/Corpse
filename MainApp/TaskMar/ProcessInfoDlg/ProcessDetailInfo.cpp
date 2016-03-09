#include "StdAfx.h"
#include "ProcessDetailInfo.h"

CProcessDetailInfo::CProcessDetailInfo()
{
	m_TabCtrlID = IDI_ICON_DEFAULT;
	m_nSubMenuID = PROCESS_DETAIL_INFO;
}

BOOL CProcessDetailInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("信息字段"));
	strListColumn.Add(_T("值"));
	return TRUE;
}

BOOL CProcessDetailInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	ASSERT(ListCtrl);
	if (m_bIsAddList == FALSE)
	{
		int nIndex=0;
		ListCtrl.InsertItem(nIndex++,_T("进程ID"));
		ListCtrl.InsertItem(nIndex++,_T("父进程ID"));
		ListCtrl.InsertItem(nIndex++,_T("进程名称"));
		ListCtrl.InsertItem(nIndex++,_T("程序路径"));
		ListCtrl.InsertItem(nIndex++,_T("命令行"));
		ListCtrl.InsertItem(nIndex++,_T("当前目录"));
		ListCtrl.InsertItem(nIndex++,_T("有效权限"));
		ListCtrl.InsertItem(nIndex++,_T("Token有效权限"));
		ListCtrl.InsertItem(nIndex++,_T("创建时间"));
		ListCtrl.InsertItem(nIndex++,_T("进程优先级"));
		m_bIsAddList = TRUE;
	}	

	SetProcessProperty(ListCtrl);

	return TRUE;
}

void CProcessDetailInfo::SetProcessProperty(CListCtrl& ListCtrl)
{
	CTaskMarView* pView = (CTaskMarView*)GetProcessListView();
	ASSERT(pView);
	CString strPid,strExePath;
	if (pView->GetPIDAndPath(strPid,strExePath) == FALSE)
		return;

	int nInde=0;
	ListCtrl.SetItemText(nInde++,1,strPid);
	HANDLE hProcess ;
	DWORD dwPid,dwParentPid = 0;
	_stscanf_s(strPid,_T("%u"),&dwPid);
	if (GetProcessHandle(dwPid,hProcess) == TRUE)
	{

	}

	CString strParentPid;
	GetParentProcessID(dwPid,dwParentPid);
	strParentPid.Format(_T("%d"),dwParentPid);			//进程ID
	ListCtrl.SetItemText(nInde++,1,strParentPid);		//父进程ID
	CString strProcessName;
	if (strPid.CompareNoCase(_T("0")) == 0)
		strProcessName = _T("System Idle Process");
	else if (strPid.CompareNoCase(_T("4")) ==0 )	//如果是PID为4的进程，那么他的进程名称是固定的
		strProcessName = _T("System");
	else
		strProcessName = PathFindFileName(strExePath);

	ListCtrl.SetItemText(nInde++,1,strProcessName);	//进程名称
	ListCtrl.SetItemText(nInde++,1,strExePath);		//程序路径

	CString strCmdLine;
	GetProcessCommandLine(dwPid,strCmdLine);
	ListCtrl.SetItemText(nInde++,1,strCmdLine);		//命令行

	CString strProcessCurrentDir;
	GetProcessCurrentDirctory(dwPid,strProcessCurrentDir);
	ListCtrl.SetItemText(nInde++,1,strProcessCurrentDir);				//当前目录

	ListCtrl.SetItemText(nInde++,1,_T(""));	//有效权限

	ListCtrl.SetItemText(nInde++,1,_T(""));	//有效Token

	if (hProcess != NULL)
	{
		FILETIME CreateFileTime,ExitFileTime,KernelFileTime,UserFileTime;
		CString strCreateFileTime;
		if (GetProcessTimes(hProcess,&CreateFileTime,&ExitFileTime,&KernelFileTime,&UserFileTime) == TRUE)
			strCreateFileTime = CovenrtFileTimeToTimeString(CreateFileTime);
		ListCtrl.SetItemText(nInde++,1,strCreateFileTime);	//创建时间
		CloseHandle(hProcess);
	}

	CString strProcessPrority;
	GetPriorityClass(dwPid,strProcessPrority);
	ListCtrl.SetItemText(nInde++,1,strProcessPrority);
}

BOOL CProcessDetailInfo::OnRButtonDown(UINT nID)
{
	return TRUE;
}