#pragma once

class CSecurityMgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CSecurityMgr();
	~CSecurityMgr();

public:
	virtual void NotifyTask(PCNTCHDR pNTCHDR);
	void OnAppMenu(CControlUI* pControl);
	void	OnProcessMenu(CControlUI* pControl);
	void OnServiceMenu(CControlUI* pControl);
public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
protected:
	int GetTabViewIndex(LPCTSTR lpszTabName) const;
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnSelectChanged( TNotifyUI &msg );
private:
	CAutoStart				m_AutoStart;
	CPaintManagerUI* m_pPaintManager;
};