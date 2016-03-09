#pragma once

class CWlan : public CBase
{
public:
	CWlan();
	~CWlan();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void OnPaint();
protected:
	/*void ShowWindow();*/
	//static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
};