#pragma once

class CKSPManager : public CNotifyPump, public IVirtualWndMgr
{
public:
	CKSPManager();
	~CKSPManager();

public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
	virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	void OnKSPMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnSelectChanged( TNotifyUI &msg );
private:
	CKSPInfo m_KSPInfo;
	CKvmSec	m_KvmSec;

	DWORD				   m_dwSelectPage;
	CPaintManagerUI* m_pPaintManager;
};