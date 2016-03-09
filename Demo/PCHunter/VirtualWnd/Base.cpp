#include "stdafx.h"

CPaintManagerUI* CBase::m_pPaintManager;
CBase::CBase()
	:m_strWndName(_T(""))
	,m_bShowData(TRUE)
{

}

CBase::~CBase()
{

}

void CBase::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_TIMER)
		SetShowData(TRUE);
	else if (msg.sType == DUI_MSGTYPE_SETFOCUS)
		SetFocus();
	else if (msg.sType == DUI_MSGTYPE_REFRESH)
		OnPaint();
}

void CBase::SetFocus()
{
	if (ShouldShowData() == FALSE)
		return;
	SetShowData(FALSE);
	OnPaint();
}


void CBase::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

CDuiString CBase::GetWndName()
{
	return m_strWndName;
}

BOOL CBase::IsTimerOn()
{
	return m_bTimer;
}

BOOL CBase::ShouldShowData()
{
	return m_bShowData;
}

void CBase::SetShowData(BOOL bShow /*= TRUE*/)
{
	m_bShowData = bShow;
}