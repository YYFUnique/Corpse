#pragma once

class CSfxMaker : public WindowImplBase
{
public:
	CSfxMaker();
	~CSfxMaker();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual void Notify(TNotifyUI& msg);
	void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

protected:
	BOOL OnBtnCheck();
	void OnNext();
	void OnPrev();
	DUI_DECLARE_MESSAGE_MAP()
	void OnTabSelect(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnMenuItemClick(CControlUI* pControl);
private:
	CMenuUI* pMenu;
};
