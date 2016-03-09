#pragma once
#include "../ChangeEvent.h"

class CSkinChange : public WindowImplBase , public SkinChangedReceiver
{
public:
	CSkinChange(LPCTSTR lpszBkFile,DWORD dwBkColor);
	~CSkinChange();
public:
	LPCTSTR GetWindowClassName() const;
	void Notify(TNotifyUI& msg);

	virtual BOOL Receive(SkinChangedParam param);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void InsertGroup();
protected:
	virtual void OnClick(TNotifyUI& msg);
protected:
	CDuiString	m_strBkFile;
	DWORD		m_dwBkColor;
};