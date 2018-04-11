#pragma once

class CProcess : public CNotifyPump
{
public:
	CProcess();
	~CProcess();

public:
protected:
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnPaint(TNotifyUI& msg);
};