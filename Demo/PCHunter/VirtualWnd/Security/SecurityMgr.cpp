#include "StdAfx.h"
#include "SecurityMgr.h"

CSecurityMgr::CSecurityMgr()
{
	m_pPaintManager = NULL;
}

CSecurityMgr::~CSecurityMgr()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CSecurityMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CSecurityMgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_AUTOSTART);
}

void CSecurityMgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_AUTOSTART, &m_AutoStart);
	m_AutoStart.SetPaintMagager(pPaintManager);
}

void CSecurityMgr::OnSelectChanged( TNotifyUI &msg )
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("SecurityWizard"));
	if (pTabLayout == NULL)
		return;
	
	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}

void CSecurityMgr::NotifyTask(PCNTCHDR pNTCHDR)
{
	CVerticalLayoutUI* pVLayoutTask = (CVerticalLayoutUI*)m_pPaintManager->FindControl(_T("VLayoutSecurity"));
	if (pVLayoutTask == NULL)
		return;

	COptionUI* pTabWizard = (COptionUI*)pVLayoutTask->GetItemAt(GetTabViewIndex(pNTCHDR->strTabTo));
	if (pTabWizard == NULL)
		return;
	pTabWizard->Selected(TRUE);
}

int CSecurityMgr::GetTabViewIndex(LPCTSTR lpszTabName) const
{
	LPCTSTR lpszVirtualName[] = {VIRTUAL_WND_AUTOSTART};
	for (int n=0; n<_countof(lpszVirtualName); ++n)
	{
		if (_tcsicmp(lpszVirtualName[n], lpszTabName) == 0)
			return n;
	}

	return 0;
}