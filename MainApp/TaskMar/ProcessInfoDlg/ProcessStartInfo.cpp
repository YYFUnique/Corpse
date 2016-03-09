#include "stdafx.h"
#include "ProcessStartInfo.h"

CProcessStartInfo::CProcessStartInfo()
{
	m_nSubMenuID = PROCESS_STARTTYPE;
}

BOOL CProcessStartInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("Ìí¼ÓÖÐ"));
	return TRUE;
}

BOOL CProcessStartInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	return TRUE;
}