#pragma once

class CLogin : public WindowImplBase
{
public:
	CLogin();
	~CLogin();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	//virtual CControlUI* CreateControl(LPCTSTR pstrClass);

protected:
	void OnClick(TNotifyUI& msg);
	void OnLogin();

private:

};