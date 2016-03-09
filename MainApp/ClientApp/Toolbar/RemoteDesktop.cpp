#include "stdafx.h"
#include "RemoteDesktop.h"

CRemoteDesktop g_RemoteDesktop;

CRemoteDesktop::CRemoteDesktop()
{
	m_nID = ID_TOOL_DESKTOP;
	m_strTipInfo = _T("远程桌面");
}

CRemoteDesktop::~CRemoteDesktop()
{

}

BOOL CRemoteDesktop::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("启动中"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}