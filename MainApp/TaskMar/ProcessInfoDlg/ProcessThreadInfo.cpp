#include "stdafx.h"
#include "ProcessThreadInfo.h"

CProcessThreadInfo::CProcessThreadInfo()
{
	m_nSubMenuID = PROCESS_THREAD;
}

// CProcessThreadInfo::~CProcessThreadInfo()
// {
// 
// }

BOOL CProcessThreadInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("�߳�ID"));
	strListColumn.Add(_T("ETHREAD"));
	strListColumn.Add(_T("Teb"));
	strListColumn.Add(_T("���ȼ�"));
	strListColumn.Add(_T("�߳����"));
	strListColumn.Add(_T("����ģ��"));
	strListColumn.Add(_T("�л�����"));
	strListColumn.Add(_T("�߳�״̬"));
	return TRUE;
}

BOOL CProcessThreadInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	return TRUE;
}

BOOL CProcessThreadInfo::OnRButtonDown(UINT nID)
{
	return TRUE;
}