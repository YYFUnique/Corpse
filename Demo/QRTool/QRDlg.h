#pragma once

class CQRDlg : public WindowImplBase
{
public:
	CQRDlg(HWND hParent);
	~CQRDlg();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

protected:
	void OnClick(TNotifyUI& msg);
	void OnLookupFile(TNotifyUI& msg);
	void OnUpdateQRCode(TNotifyUI& msg);
private:
	HWND m_hParent;
};