#include "StdAfx.h"
#include "KSPManager.h"

CKSPManager::CKSPManager()
{
	m_dwSelectPage = 0;
}

CKSPManager::~CKSPManager()
{

}

DUI_BEGIN_MESSAGE_MAP(CKSPManager, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()


void CKSPManager::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_KSPINFO);
}

void CKSPManager::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	m_KSPInfo.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_KSPINFO, &m_KSPInfo);
}

BOOL CKSPManager::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = FALSE;
	if (m_dwSelectPage == 0)	//Ñ¡ÔñKSP tabÒ³
		m_KSPInfo.HandleMessage(uMsg, wParam, lParam, bHandle);

	return bHandle;
}

void CKSPManager::OnSelectChanged( TNotifyUI &msg )
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("ISPSWizard"));
	if (pTabLayout == NULL)
		return;

	m_dwSelectPage = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(m_dwSelectPage);
	pTabLayout->SelectItem(m_dwSelectPage);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}