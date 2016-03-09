#pragma once
#include "../ChangeEvent.h"

class CMessageTip : public WindowImplBase , public SkinChangedReceiver
{
public:
	CMessageTip(LPCTSTR lpszBkFile,DWORD dwBkColor,LPCTSTR lpszWinTitle,LPCTSTR lpszMessage);
	~CMessageTip();

public:
	LPCTSTR GetWindowClassName() const;
	void Notify(TNotifyUI& msg);

	virtual BOOL Receive(SkinChangedParam param);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	virtual void OnClick(TNotifyUI& msg);
private:
	CDuiString	m_strBkFile;
	DWORD		m_dwBkColor;
	CDuiString	m_strWinTitle;
	CDuiString	m_strMessage;
};