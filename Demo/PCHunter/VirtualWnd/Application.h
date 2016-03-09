#pragma once

class CApplication : public CBase
{
public:
	CApplication();
	~CApplication();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void OnPaint();
	static void GetProcessPathByHwnd(HWND hWnd,CDuiString& strProcesspath);
	static bool DosPathToNtPath(LPCTSTR lpszDosPath, CDuiString& strNtPath);
protected:
	
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
};