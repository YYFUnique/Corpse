#pragma once

class CTask : public CNotifyPump,public INotifyUI
{
public:
	CTask();
	~CTask();

public:
	
	virtual void Notify(TNotifyUI& msg);

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnFocus(TNotifyUI& msg);
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnTabSelect(TNotifyUI& msg);
	virtual void OnHeaderClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnItemClick( TNotifyUI &msg );

	static void SetPaintMagager(CPaintManagerUI* pPaintMgr);
private:
	CPaintManagerUI* m_pPaintManager;
};