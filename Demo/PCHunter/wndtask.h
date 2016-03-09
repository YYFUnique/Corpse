#pragma once

class CTaskWnd : public CNotifyPump
{
public:
	CTaskWnd();
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

public:
	BOOL OnInitDialog();
protected:
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnHeaderClick(TNotifyUI& msg);
	virtual void OnSetFocus(TNotifyUI& msg);

protected:
	CPaintManagerUI* m_pPaintManager;
};