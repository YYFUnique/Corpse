#pragma once
class CWndMgr;
class CTask : public CNotifyPump,public INotifyUI
{
public:
	CTask();
	~CTask();
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	virtual void Notify(TNotifyUI& msg);

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnFocus(TNotifyUI& msg);
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnTabSelect(TNotifyUI& msg);
	virtual void OnHeaderClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnItemClick( TNotifyUI &msg );
private:
	CPaintManagerUI* m_pPaintManager;
	/*CWndMgr* m_pWndMgr;*/
};