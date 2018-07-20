#pragma once

class CNetworkMgr : public CNotifyPump,public IVirtualWndMgr
{
public:
	CNetworkMgr();
	~CNetworkMgr();

public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
public:
	void OnHostScanMenu(CControlUI* pControl);
	void OnRangeMenu(CControlUI* pControl);
	void OnRouteInfo(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnSelectChanged( TNotifyUI &msg );
private:
	CAdapter			m_Adapter;
	CRoute				m_Route;
	CSnmp				m_Snmp;
	CWlan				m_Wlan;
	CConnect			m_Connect;
	CHostScan		m_HostScan;
	CPaintManagerUI* m_pPaintManager;
};