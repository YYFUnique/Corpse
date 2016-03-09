#include "stdafx.h"
#include "CmdOrder.h"

CCmdOrder g_CmdOrder;

CCmdOrder::CCmdOrder()
{
	m_nID = ID_TOOL_CMD;
	m_strTipInfo = _T("CMDÃüÁî");
}

CCmdOrder::~CCmdOrder()
{

}

BOOL CCmdOrder::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("Æô¶¯ÖÐ"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}