#include "stdafx.h"
#include "TaskManage.h"
#include "CommonDlg/LsFunc.h"

CTaskManage g_TaskManage;

CTaskManage::CTaskManage()
{
	m_nID = ID_TOOL_TASK;
	m_strTipInfo = _T("远程主机任务管理");
}

CTaskManage::~CTaskManage()
{

}

BOOL CTaskManage::Launch()
{
	ShowShutDownDlg();
	return TRUE;
}