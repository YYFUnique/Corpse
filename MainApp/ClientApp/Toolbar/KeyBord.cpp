#include "stdafx.h"
#include "KeyBord.h"

CKeyBorad g_KeyBorad;

CKeyBorad::CKeyBorad()
{
	m_nID = ID_TOOL_KEYBORD;
	m_strTipInfo = _T("¼üÅÌ¼ÇÂ¼");
}

CKeyBorad::~CKeyBorad()
{

}

BOOL CKeyBorad::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("Æô¶¯ÖÐ"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}