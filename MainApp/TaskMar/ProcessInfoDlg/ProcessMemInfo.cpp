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

	strListColumn.Add(_T("��ַ"));
	strListColumn.Add(_T("��С(Hex)"));
	//strListColumn.Add(_T(""));
	strListColumn.Add(_T("������ʽ"));
	strListColumn.Add(_T("����"));
	strListColumn.Add(_T("״̬"));
	strListColumn.Add(_T("ģ����"));
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