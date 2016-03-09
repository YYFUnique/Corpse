#include "stdafx.h"
#include "ProcessMemInfo.h"

CProcessMemInfo::CProcessMemInfo()
{
	m_nSubMenuID = PROCESS_MEMROY;
}

// CProcessMemInfo::~CProcessMemInfo()
// {
// 
// }

BOOL CProcessMemInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("地址"));
	strListColumn.Add(_T("大小(Hex)"));
	//strListColumn.Add(_T(""));
	strListColumn.Add(_T("保护方式"));
	strListColumn.Add(_T("类型"));
	strListColumn.Add(_T("状态"));
	strListColumn.Add(_T("模块名"));
	return TRUE;
}

BOOL CProcessMemInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	return TRUE;
}

BOOL CProcessMemInfo::OnRButtonDown(UINT nID)
{
	return TRUE;
}