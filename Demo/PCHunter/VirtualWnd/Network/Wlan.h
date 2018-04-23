#pragma once

class CWlan : public CNotifyPump
{
public:
	CWlan();
	~CWlan();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP();
	void OnLoadItem(TNotifyUI& msg);
protected:
	CPaintManagerUI* m_pPaintManager;
};