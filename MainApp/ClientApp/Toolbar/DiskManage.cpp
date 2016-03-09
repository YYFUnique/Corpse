#include "stdafx.h"
#include "DiskManage.h"

CDiskManage g_DiskManage;

CDiskManage::CDiskManage()
{
	m_nID = ID_TOOL_DISK;
	m_strTipInfo = _T("磁盘管理");
}

CDiskManage::~CDiskManage()
{

}

BOOL CDiskManage::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("启动中"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}