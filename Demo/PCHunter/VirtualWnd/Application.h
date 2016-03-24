#pragma once

typedef BOOL (WINAPI* ENDTASK_FUNC)(HWND hWnd,BOOL fShutDown,BOOL fForce);

class CApplication : public CBase
{
public:
	CApplication();
	~CApplication();

public:
	void OnAppMenu(CControlUI* pControl);

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint();

	static void GetProcessPathByHwnd(HWND hWnd,CDuiString& strProcesspath);
	static bool DosPathToNtPath(LPCTSTR lpszDosPath, CDuiString& strNtPath);
protected:
	void OnItemClick(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);

	static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
};