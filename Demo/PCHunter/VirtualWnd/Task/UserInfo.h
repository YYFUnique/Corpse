#pragma once
#include <WtsApi32.h>

class CUserInfo : public CNotifyPump
{
public:
	CUserInfo();
	virtual ~CUserInfo();
public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnPaint(TNotifyUI& msg);
protected:
	BOOL QuerySessionInfo(DWORD dwSessionId,WTS_INFO_CLASS WTSInfoClass,CDuiString& strInfo);
	BOOL QuerySessionState(WTS_CONNECTSTATE_CLASS WtsState,CDuiString& strState);
	BOOL QueryClientIp(DWORD dwSessionId,CDuiString& strInfo);
protected:
	CPaintManagerUI* m_pPaintManager;
};