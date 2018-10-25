#pragma once

class CFileProcMgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CFileProcMgr();
	~CFileProcMgr();

public:
	void OnAppMenu(CControlUI* pControl);
	//void OnServiceMenu(CControlUI* pControl);
public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnSelectChanged( TNotifyUI &msg );
private:
	CHideFile m_HideFile;
	CPaintManagerUI* m_pPaintManager;
};