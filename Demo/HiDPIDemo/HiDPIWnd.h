#pragma once

class CHiDPIWnd : public WindowImplBase
{
public:
	CHiDPIWnd();
	~CHiDPIWnd();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

public:
	void SetDPI(int iDPI);
protected:
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
};