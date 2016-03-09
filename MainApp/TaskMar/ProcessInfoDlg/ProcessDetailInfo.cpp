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

	strListColumn.Add(_T("��Ϣ�ֶ�"));
	strListColumn.Add(_T("ֵ"));
	return TRUE;
}

BOOL CProcessDetailInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	ASSERT(ListCtrl);
	if (m_bIsAddList == FALSE)
	{
		int nIndex=0;
		ListCtrl.InsertItem(nIndex++,_T("����ID"));
		ListCtrl.InsertItem(nIndex++,_T("������ID"));
		ListCtrl.InsertItem(nIndex++,_T("��������"));
		ListCtrl.InsertItem(nIndex++,_T("����·��"));
		ListCtrl.InsertItem(nIndex++,_T("������"));
		ListCtrl.InsertItem(nIndex++,_T("��ǰĿ¼"));
		ListCtrl.InsertItem(nIndex++,_T("��ЧȨ��"));
		ListCtrl.InsertItem(nIndex++,_T("Token��ЧȨ��"));
		ListCtrl.InsertItem(nIndex++,_T("����ʱ��"));
		ListCtrl.InsertItem(nIndex++,_T("�������ȼ�"));
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
	strParentPid.Format(_T("%d"),dwParentPid);			//����ID
	ListCtrl.SetItemText(nInde++,1,strParentPid);		//������ID
	CString strProcessName;
	if (strPid.CompareNoCase(_T("0")) == 0)
		strProcessName = _T("System Idle Process");
	else if (strPid.CompareNoCase(_T("4")) ==0 )	//�����PIDΪ4�Ľ��̣���ô���Ľ��������ǹ̶���
		strProcessName = _T("System");
	else
		strProcessName = PathFindFileName(strExePath);

	ListCtrl.SetItemText(nInde++,1,strProcessName);	//��������
	ListCtrl.SetItemText(nInde++,1,strExePath);		//����·��

	CString strCmdLine;
	GetProcessCommandLine(dwPid,strCmdLine);
	ListCtrl.SetItemText(nInde++,1,strCmdLine);		//������

	CString strProcessCurrentDir;
	GetProcessCurrentDirctory(dwPid,strProcessCurrentDir);
	ListCtrl.SetItemText(nInde++,1,strProcessCurrentDir);				//��ǰĿ¼

	ListCtrl.SetItemText(nInde++,1,_T(""));	//��ЧȨ��

	ListCtrl.SetItemText(nInde++,1,_T(""));	//��ЧToken

	if (hProcess != NULL)
	{
		FILETIME CreateFileTime,ExitFileTime,KernelFileTime,UserFileTime;
		CString strCreateFileTime;
		if (GetProcessTimes(hProcess,&CreateFileTime,&ExitFileTime,&KernelFileTime,&UserFileTime) == TRUE)
			strCreateFileTime = CovenrtFileTimeToTimeString(CreateFileTime);
		ListCtrl.SetItemText(nInde++,1,strCreateFileTime);	//����ʱ��
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