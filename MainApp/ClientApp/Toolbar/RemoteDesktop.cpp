#include "stdafx.h"
#include "RemoteDesktop.h"

CRemoteDesktop g_RemoteDesktop;

CRemoteDesktop::CRemoteDesktop()
{
	m_nID = ID_TOOL_DESKTOP;
	m_strTipInfo = _T("Զ������");
}

CRemoteDesktop::~CRemoteDesktop()
{

}

BOOL CRemoteDesktop::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("������"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}