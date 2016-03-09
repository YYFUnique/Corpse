#include "StdAfx.h"
#include "ToolbarMain.h"
#include "ToolbarMgr.h"
CToolbarMain::CToolbarMain(void)
{
	m_nID = 0;
	m_strTipInfo = _T("");
	AfxGetToolbarMgr()->AddObject(this);
}

CToolbarMain::~CToolbarMain(void)
{
}

UINT CToolbarMain::GetCtrlId()
{
	return m_nID;
}

CString& CToolbarMain::GetCtrlTipInfo()
{
	return m_strTipInfo;
}