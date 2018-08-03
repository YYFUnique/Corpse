#pragma once

class CHighClass : public WindowImplBase
{
public:
	CHighClass(HWND hParent, LPCTSTR lpszCertName);
	~CHighClass();

public:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
public:
	CDuiString GetCertNameInfo() const;
protected:
	void OnClick(TNotifyUI& msg);
	void OnUpdate();
protected:
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
private:
	CDuiString m_strCertNameInfo;
};