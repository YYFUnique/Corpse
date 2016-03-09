#pragma once
#include <WtsApi32.h>

class CUserInfo : public CBase
{
public:
	CUserInfo();
	virtual ~CUserInfo();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void OnPaint();
	BOOL QuerySessionInfo(DWORD dwSessionId,WTS_INFO_CLASS WTSInfoClass,CDuiString& strInfo);
	BOOL QuerySessionState(WTS_CONNECTSTATE_CLASS WtsState,CDuiString& strState);
	BOOL QueryClientIp(DWORD dwSessionId,CDuiString& strInfo);
protected:

};