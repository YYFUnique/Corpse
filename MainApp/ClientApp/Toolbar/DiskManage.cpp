#include "stdafx.h"
#include "DiskManage.h"

CDiskManage g_DiskManage;

CDiskManage::CDiskManage()
{
	m_nID = ID_TOOL_DISK;
	m_strTipInfo = _T("���̹���");
}

CDiskManage::~CDiskManage()
{

}

BOOL CDiskManage::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("������"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}