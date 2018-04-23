#include "StdAfx.h"
#include "SystemMgr.h"

CSystemMgr::CSystemMgr()
{
	m_pPaintManager = NULL;
}

CSystemMgr::~CSystemMgr()
{

}

DUI_BEGIN_MESSAGE_MAP(CSystemMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CSystemMgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_TRAYICON);
}

void CSystemMgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	m_TrayIcon.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_TRAYICON, &m_TrayIcon);
}

void CSystemMgr::OnSelectChanged( TNotifyUI &msg )
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("InfoWizard"));
	if (pTabLayout == NULL)
		return;

	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}

void CSystemMgr::OnTrayMenu(CControlUI* pControl)
{
	m_TrayIcon.OnTrayMenu(pControl);
}