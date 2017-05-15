#pragma once

class CHardware : public CBase
{
public:
	CHardware();
	~CHardware();

public:
	BOOL DeviceChanged(WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnPaint();
	virtual void SetFocus();

//protected:
//	void RegisterHardNotify();
//	void UnRegisterHardNotify();

private:
	HANDLE m_hDevNotify;
};