#include "stdafx.h"
#include "TaskManage.h"
#include "CommonDlg/LsFunc.h"

CTaskManage g_TaskManage;

CTaskManage::CTaskManage()
{
	m_nID = ID_TOOL_TASK;
	m_strTipInfo = _T("Զ�������������");
}

CTaskManage::~CTaskManage()
{

}

BOOL CTaskManage::Launch()
{
	ShowShutDownDlg();
	return TRUE;
}