#pragma once

class CArpList : public WindowImplBase
{
public:
	CArpList(HWND hWnd);
	~CArpList();
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

protected:
	void ShowArpList();
};