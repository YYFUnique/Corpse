#include "StdAfx.h"
#include "ToolbarMgr.h"

CToolbarMgr::CToolbarMgr(void)
{
	m_pCurrentToolbar = NULL;
}

CToolbarMgr::~CToolbarMgr(void)
{
}

CToolbarMain* CToolbarMgr::GetCurrentToolbar()
{
	return m_pCurrentToolbar;
}

BOOL CToolbarMgr::SetCurrentObjectByID(UINT nID)
{
	POSITION pos;
	CToolbarMain* pToolbarCtrl;
	pos = m_ToolbarList.GetHeadPosition();
	while(pos)
	{
		pToolbarCtrl = m_ToolbarList.GetNext(pos);
		if (pToolbarCtrl->GetCtrlId() == nID)
		{
			m_pCurrentToolbar = pToolbarCtrl;
			break;
		}
	}
	return m_pCurrentToolbar != NULL;
}

void CToolbarMgr::AddObject(CToolbarMain* pToolbar)
{
	m_ToolbarList.AddTail(pToolbar);
}

CToolbarMgr* AfxGetToolbarMgr()
{
	static CToolbarMgr s_CToolbarMgr;
	return &s_CToolbarMgr;
}