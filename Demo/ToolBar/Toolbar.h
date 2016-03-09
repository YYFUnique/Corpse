#pragma once

class CToolbar : public WindowImplBase
{
public:
	CToolbar();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual void Notify(TNotifyUI& msg);
	void InitWindow();
	CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
	void OnClick(TNotifyUI& msg);
	void OnItemMenu(TNotifyUI& msg);
protected:
	CDuiString  m_strSkinFile;
	CDuiString  m_strZipFileName;
};