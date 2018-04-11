#pragma once

class CCoreDriver : public CNotifyPump
{
public:
	CCoreDriver();
	~CCoreDriver();

protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnMenu(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	BOOL GetDriverInfo(CSrvInfoList& SvrInfoList);
	static BOOL FormatStartType(DWORD dwStartType, CDuiString& strStartType);
	static BOOL FormatRunStatus(DWORD dwRunStatus, CDuiString& strRunStatus);
	static BOOL FormatDriverType(DWORD dwDriverType, CDuiString& strDriverType);
protected:
	CControlBuilder	m_RootBuilder;
	CDialogBuilder m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};