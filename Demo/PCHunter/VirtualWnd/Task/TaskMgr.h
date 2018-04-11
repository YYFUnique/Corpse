#pragma once
#include "../IVirtualWndMgr.h"
#include "Application.h"
#include "Services.h"
#include "UserInfo.h"
#include "CoreDriver.h"

class CTaskMgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CTaskMgr();
	~CTaskMgr();

public:
	void OnAppMenu(CControlUI* pControl);
	void OnServiceMenu(CControlUI* pControl);
public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnSelectChanged( TNotifyUI &msg );
private:
	CApplication			m_App;
	//CProcess					m_Process;
	CServices					m_Service;
	CUserInfo				m_UserInfo;
	CCoreDriver				m_CoreDriver;
	CPaintManagerUI* m_pPaintManager;
};