#pragma once

class COEMInfoModify : public WindowImplBase
{
public:
	COEMInfoModify();
	~COEMInfoModify();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetResourceID() const;

	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
protected:
	void OnClick(TNotifyUI& msg);
	void OnSaveOEMInfo(TNotifyUI& msg);
	void OnGetLogoFilePath(TNotifyUI& msg);
	void OnSetDefault(TNotifyUI& msg);
};