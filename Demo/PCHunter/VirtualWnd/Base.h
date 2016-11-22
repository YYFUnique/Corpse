#pragma once

class CBase : public INotifyUI
{
public:
	CBase();
	virtual ~CBase();

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint() = 0;
	virtual BOOL OnInitDialog();
	virtual void SetFocus();
	
public:

	BOOL IsTimerOn();
	BOOL ShouldShowData();
	CDuiString GetWndName();
	static void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	void SetShowData(BOOL bShow /*= TRUE*/);
protected:
	CDuiString m_strWndName;
	BOOL m_bTimer;
	BOOL m_bShowData;
	static CPaintManagerUI* m_pPaintManager;
};