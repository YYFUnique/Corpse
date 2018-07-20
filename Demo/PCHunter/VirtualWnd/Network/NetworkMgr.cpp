#include "StdAfx.h"
#include "NetworkMgr.h"

CNetworkMgr::CNetworkMgr()
{
	m_pPaintManager = NULL;
}

CNetworkMgr::~CNetworkMgr()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CNetworkMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CNetworkMgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_ADAPTER);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_ROUTE);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_HOSTSCAN);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_SNMP);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_WLAN);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_CONNECT);
}

void CNetworkMgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	m_Adapter.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_ADAPTER, &m_Adapter);

	m_Route.SetPaintMagager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_ROUTE, &m_Route);

	m_HostScan.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_HOSTSCAN, &m_HostScan);

	m_Snmp.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_SNMP, &m_Snmp);

	m_Wlan.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_WLAN, &m_Wlan);

	m_Connect.SetPaintManager(pPaintManager);
	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_CONNECT, &m_Connect);
}

void CNetworkMgr::OnSelectChanged( TNotifyUI &msg )
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("NetWizard"));
	if (pTabLayout == NULL)
		return;
	
	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}

void CNetworkMgr::OnHostScanMenu(CControlUI* pControl)
{
	m_HostScan.OnHostScanMenu(pControl);
}

void CNetworkMgr::OnRangeMenu(CControlUI* pControl)
{
	m_HostScan.OnRangeMenu(pControl);
}

void CNetworkMgr::OnRouteInfo(CControlUI* pControl)
{
	m_Route.OnRouteInfo(pControl);
}