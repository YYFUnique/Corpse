#pragma once

class CAboutDialog : public WindowImplBase
{
public:
	CAboutDialog(HWND hWnd);
	~CAboutDialog();
public:

	virtual void InitWindow();
	void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);

	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetWindowClassName(void) const;

	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
protected:
	void OnClick(TNotifyUI& msg);
};