#include "StdAfx.h"
#include "ISPSManager.h"

CISPSManager::CISPSManager()
{
	m_pPaintManager = NULL;
}

CISPSManager::~CISPSManager()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CISPSManager, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CISPSManager::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_ISPSPHISON);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_ISPSDEBUG);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_DEVICETAG);
}

void CISPSManager::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	m_Phison.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_ISPSPHISON, &m_Phison);

	m_Debug.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_ISPSDEBUG, &m_Debug);

	m_DeviceTag.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_DEVICETAG, &m_DeviceTag);
}

void CISPSManager::OnSelectChanged( TNotifyUI &msg )
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("ISPSWizard"));
	if (pTabLayout == NULL)
		return;

	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}