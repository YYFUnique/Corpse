#include "stdafx.h"

CNetMgr::CNetMgr()
{
	AddVirtualWnd(_T("adapter"),&m_Adapter);
	AddVirtualWnd(_T("connect"),&m_Connect);
	AddVirtualWnd(_T("route"),&m_Route);
	AddVirtualWnd(_T("provider"),&m_Provider);
	AddVirtualWnd(_T("wlan"),&m_Wlan);
	AddVirtualWnd(_T("scan"),&m_Scan);
	AddVirtualWnd(_T("snmp"),&m_SNMP);
}

CNetMgr::~CNetMgr()
{
	RemoveVirtualWnd(_T("adapter"));
	RemoveVirtualWnd(_T("connect"));
	RemoveVirtualWnd(_T("route"));
	RemoveVirtualWnd(_T("provider"));
	RemoveVirtualWnd(_T("wlan"));
	RemoveVirtualWnd(_T("scan"));
	RemoveVirtualWnd(_T("snmp"));
}

DUI_BEGIN_MESSAGE_MAP(CNetMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_END_MESSAGE_MAP()

//void CTaskMgr::SetPaintMagager(CPaintManagerUI* pPaintMgr)
//{
//	CVirtualWnd::SetPaintMagager(pPaintMgr);
//	CBase::SetPaintManager(m_pPaintManager);
//}

void CNetMgr::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetName() == _T("VLayoutNet"))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("NetWizard"));
		if (pTabLayout != NULL)
			pTabLayout->SelectItem(_ttoi(msg.pSender->GetUserData()));
	}
}

void CNetMgr::OnHostScanMenu(CControlUI* pControl)
{
	m_Scan.OnHostScanMenu(pControl);
}

void CNetMgr::OnRangeMenu(CControlUI* pControl)
{
	m_Scan.OnRangeMenu(pControl);
}

void CNetMgr::OnRouteInfo(CControlUI* pControl)
{
	m_Route.OnRouteInfo(pControl);
}