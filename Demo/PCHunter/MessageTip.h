#pragma once
#include "ChangeEvent.h"

class CMessageTip : public WindowImplBase , public SkinChangedReceiver
{
public:
	CMessageTip(HWND hParent,LPCTSTR lpszBkFile,DWORD dwBkColor,LPCTSTR lpszWinTitle,LPCTSTR lpszMessage);
	~CMessageTip();

public:
	LPCTSTR GetWindowClassName() const;
	void Notify(TNotifyUI& msg);

	virtual BOOL Receive(SkinChangedParam param);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	HWND GetTrayWndHwnd();
	HWND FindNotifyIconOverflowWindow();
	BOOL GetTrayWndPosition(SIZE& size,POINT& pt);
	BOOL GetTrayInfo(HWND hTrayWnd,RECT& rcMenuPos);

	BOOL GetTrayInfo32(DWORD dwTrayPid,HWND hTrayWnd,RECT& rcMenuPos);
	BOOL GetTrayInfo64(DWORD dwTrayPid,HWND hTrayWnd,RECT& rcMenuPos);

	virtual void ShowWindow(bool bShow = true, bool bTakeFocus = true);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	virtual void OnClick(TNotifyUI& msg);
private:
	CDuiString	m_strBkFile;
	DWORD		m_dwBkColor;
	CDuiString	m_strWinTitle;
	CDuiString	m_strMessage;
};