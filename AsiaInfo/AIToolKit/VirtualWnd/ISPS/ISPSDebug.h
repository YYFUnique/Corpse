#pragma once

class CISPSDebug : public CNotifyPump
{
public:
	CISPSDebug();
	~CISPSDebug();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnClick(TNotifyUI& msg);
		void OnLoadItem(TNotifyUI& msg);
		void OnSelectChanged(TNotifyUI& msg);
protected:
	void LoadSTCfg();
	void StoreSTCfg();

	void LoadMPCfg();
	void StoreMPCfg();

	void SetSTFilterType(DWORD dwFilter);
	void SetMPFilterType(DWORD dwFilter);

	void SelectSTAll(BOOL bSelect = TRUE);
	void EnableSTWindows(BOOL bEnable = TRUE);

	void SelectMPAll(BOOL bSelect = TRUE);
	void EnableMPWindows(BOOL bEnable = TRUE);
protected:
	BOOL					   m_bLoad;
	CPaintManagerUI* m_pPaintManager;
};