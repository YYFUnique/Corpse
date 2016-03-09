#pragma once

class CBase : public INotifyUI
{
public:
	CBase();
	virtual ~CBase();

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint() = 0;
	virtual void SetFocus();
	static void SetPaintManager(CPaintManagerUI* pPaintMgr);
	CDuiString GetWndName();
	BOOL IsTimerOn();
	BOOL ShouldShowData();
protected:
	void SetShowData(BOOL bShow /*= TRUE*/);
protected:
	static CPaintManagerUI* m_pPaintManager;
	CDuiString m_strWndName;
	BOOL m_bTimer;
	BOOL m_bShowData;
};