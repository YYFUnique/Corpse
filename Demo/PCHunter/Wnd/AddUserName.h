#pragma once

class CAddUserName : public WindowImplBase
{
public:
	CAddUserName(HWND hParent);
	~CAddUserName();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
public:
	void SetUserNameByEdit();
	void OnClick(TNotifyUI& msg);
	LPCTSTR GetUserName();
	void SetUserName(LPCTSTR lpszUserName);
protected:
	CDuiString m_strUserName;
	HWND		  m_hParent;
};