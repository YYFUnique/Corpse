#pragma once

class CTaskMgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CTaskMgr();
	~CTaskMgr();

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
	CApplication			m_App;
	CProcess					m_Process;
	CServices					m_Service;
	CUserInfo				m_UserInfo;
	CCoreDriver				m_CoreDriver;
	CViewCert				m_ViewCert;
	CPaintManagerUI* m_pPaintManager;
};