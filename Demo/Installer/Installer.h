#pragma once

class CInstaller : public WindowImplBase
{
public:
	CInstaller();
	~CInstaller();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetResourceID() const;
	virtual void Notify(TNotifyUI& msg);
	void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnClick(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnMenuItemClick(CControlUI* pControl);
private:
	CMenuUI* pMenu;
};
