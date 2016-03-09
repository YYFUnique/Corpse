#include "StdAfx.h"
#include "ToolItemBase.h"

CToolItemBase::CToolItemBase(void)
{
	m_strItemText = _T("");
	m_hItemID = 0;
	m_nSubMenu = -1;
}

CToolItemBase::~CToolItemBase(void)
{
	
}

UINT CToolItemBase::GetIconResourceId()
{
	return m_hItemID;
}

CString& CToolItemBase::GetItemName()
{
	return m_strItemText;
}

INT CToolItemBase::GetSubItemMenu()
{
	return m_nSubMenu;
}

BOOL CToolItemBase::OnRButtonClick()
{
	return TRUE;
}