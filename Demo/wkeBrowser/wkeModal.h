#pragma once

class CWkeModal : public WindowImplBase
{
public:
	CWkeModal();

	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;

	void InitWindow();
	wkeWebView GetWebView();
	void Notify(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClassName);

	/*static UINT ;*/
protected:
	wkeWebView m_pWebView;
};