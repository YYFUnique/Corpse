#pragma once

class CIPv4Mgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CIPv4Mgr();
	~CIPv4Mgr();

public:
	virtual void NotifyTask(PCNTCHDR pNTCHDR);
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
	void AddVirtualWnd(LPCTSTR lpszVirtualWnd);
protected:
	UINT GetTabViewIndex(LPCTSTR lpszTabName) const;
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnSelectChanged(TNotifyUI &msg);
private:
	CIPv4Param				m_IPv4Param;
	CStdPtrArray			m_ResultView;
	CNotifyPump*			m_pNotifyPump;
	CPaintManagerUI*	m_pPaintManager;
};