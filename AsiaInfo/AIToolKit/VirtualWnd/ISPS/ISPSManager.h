#pragma once

class CISPSManager : public CNotifyPump, public IVirtualWndMgr
{
public:
	CISPSManager();
	~CISPSManager();

public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
public:
	//void OnTrayMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnSelectChanged( TNotifyUI &msg );
private:
	CPhison					m_Phison;
	CISPSDebug			m_Debug;
	CISPSInterfaceTest	m_InterfaceTest;

	CPaintManagerUI* m_pPaintManager;
};