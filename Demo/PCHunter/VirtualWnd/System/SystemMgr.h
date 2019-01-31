#pragma once

class CSystemMgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CSystemMgr();
	~CSystemMgr();

public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
public:
	void OnTrayMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnSelectChanged( TNotifyUI &msg );
private:
	CSystemInfo	m_SystemInfo;
	CLocalUser		m_LocalUser;
	CTrayIcon		m_TrayIcon;
	CPaintManagerUI* m_pPaintManager;
};