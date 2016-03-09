#pragma once


class CTaskMgr : public CVirtualWnd
{
public:
	CTaskMgr();
	~CTaskMgr();
	
protected:
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnSelectChanged(TNotifyUI& msg);
	//void SetPaintMagager(CPaintManagerUI* pPaintMgr);
protected:
	CApplication m_App;
	CProcess		m_Process;
	CServices		m_Services;
	CCoreDriver m_CoreDriver;
	CUserInfo   m_UserInfo;
};