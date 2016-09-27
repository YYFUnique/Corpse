#pragma once

class CEncryptProcess : public WindowImplBase
{
public:
	CEncryptProcess(HWND hParent);
	~CEncryptProcess();

public:
	virtual void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);

	void InitWindow();
	//LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	void SetEncryptProcess(UINT nProcess);

protected:
	void OnClick(TNotifyUI& msg);
};