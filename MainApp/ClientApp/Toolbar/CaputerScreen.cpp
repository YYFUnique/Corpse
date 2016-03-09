#include "stdafx.h"
#include "CaputerScreen.h"

CCaputerScreen g_CaputerScreen;

CCaputerScreen::CCaputerScreen()
{
	m_nID = ID_TOOL_VIDEO;
	m_strTipInfo = _T("²¶×½ÊÓÆÁ");
}

CCaputerScreen::~CCaputerScreen()
{

}

BOOL CCaputerScreen::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("Æô¶¯ÖÐ"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}