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

	strListColumn.Add(_T("����"));
	strListColumn.Add(_T("��ʾ����"));
	strListColumn.Add(_T("����"));
	strListColumn.Add(_T("״̬"));
	strListColumn.Add(_T("����ID"));
	strListColumn.Add(_T("������ʽ"));
	strListColumn.Add(_T("�ļ�·��"));
	strListColumn.Add(_T("����"));
	return TRUE;
}

BOOL CProcessServiceInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	return TRUE;
}