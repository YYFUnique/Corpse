#include "stdafx.h"
#include "ProcessServiceInfo.h"

CProcessServiceInfo::CProcessServiceInfo()
{
	m_nSubMenuID = PROCESS_SERVICE;
}

//CProcessServiceInfo::~CProcessServiceInfo()
//{
//
//}

BOOL CProcessServiceInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("名称"));
	strListColumn.Add(_T("显示名称"));
	strListColumn.Add(_T("类型"));
	strListColumn.Add(_T("状态"));
	strListColumn.Add(_T("进程ID"));
	strListColumn.Add(_T("启动方式"));
	strListColumn.Add(_T("文件路径"));
	strListColumn.Add(_T("描述"));
	return TRUE;
}

BOOL CProcessServiceInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	return TRUE;
}