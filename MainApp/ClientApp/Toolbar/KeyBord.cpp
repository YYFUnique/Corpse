#include "stdafx.h"
#include "KeyBord.h"

CKeyBorad g_KeyBorad;

CKeyBorad::CKeyBorad()
{
	m_nID = ID_TOOL_KEYBORD;
	m_strTipInfo = _T("���̼�¼");
}

CKeyBorad::~CKeyBorad()
{

}

BOOL CKeyBorad::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("������"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}