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

	strListColumn.Add(_T("线程ID"));
	strListColumn.Add(_T("ETHREAD"));
	strListColumn.Add(_T("Teb"));
	strListColumn.Add(_T("优先级"));
	strListColumn.Add(_T("线程入口"));
	strListColumn.Add(_T("所在模块"));
	strListColumn.Add(_T("切换次数"));
	strListColumn.Add(_T("线程状态"));
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