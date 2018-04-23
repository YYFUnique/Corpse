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
	CTrayIcon		m_TrayIcon;
	CPaintManagerUI* m_pPaintManager;
};