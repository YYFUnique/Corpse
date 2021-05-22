#pragma once

class CIPv4Param : public CNotifyPump
{
public:
	CIPv4Param();
	~CIPv4Param();

public:
	void SetOwner(IVirtualWndMgr* pVirtualWndMgr);
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnClick(TNotifyUI& msg);
		void OnLoadItem(TNotifyUI& msg);
private:
	BOOL m_bLoad;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder	m_DialogBuilder;
	CDialogBuilder	m_OptionBuilder;
	IVirtualWndMgr*	   m_pVirtualWndMgr;
	CPaintManagerUI* m_pPaintManager;
};