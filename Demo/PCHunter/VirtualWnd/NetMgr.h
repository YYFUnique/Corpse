#pragma once

class CNetMgr : public CVirtualWnd
{
public:
	CNetMgr();
	~CNetMgr();
	
protected:
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnSelectChanged(TNotifyUI& msg);
	//void SetPaintMagager(CPaintManagerUI* pPaintMgr);
protected:
	CAdapter		m_Adapter;
	CConnect		m_Connect;
	CRoute			m_Route;
	CProvider    m_Provider;
	CWlan			m_Wlan;
	CHostScan			m_Scan;
};