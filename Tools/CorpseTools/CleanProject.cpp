#include "StdAfx.h"
#include "CleanProject.h"

CCleanProject::CCleanProject(void)
{
	m_strItemText  = _T("����������");
	m_hItemID = IDI_ICON_CLEAN_PROJECT;
	m_nSubMenu = 0;
}

CCleanProject::~CCleanProject(void)
{

}

BOOL CCleanProject::OnLButtonDbClick()
{
	MessageBox(NULL,_T("OK"),_T("�Ի���"),MB_OK);
	return TRUE;
}

BOOL CCleanProject::OnRButtonClick()
{
	return TRUE;
}