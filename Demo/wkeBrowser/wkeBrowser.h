#pragma once
#include "wkeModal.h"

#define WM_USER_TITLE_CHANGE         WM_USER + 289     // 浏览器标题改变
#define WM_USER_URL_CHANGE		     WM_USER + 290     // 浏览器URL改变
#define WM_USER_JS_NOTIFY				 WM_USER + 291     // js脚本函数调用c++函数

class CWkeBrowser : public WindowImplBase
{
public:
	CWkeBrowser();

	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;

     void InitWindow();
     void Notify(TNotifyUI& msg);
	 CControlUI* CreateControl(LPCTSTR pstrClassName);

	 void OnTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString url);
	 void OnURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString title);
	 LRESULT OnJsNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);
	 virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	void OnClick(TNotifyUI& msg);
	void OnReturn(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
private:
	CEditUI2				*m_pURLEdit;
	CLabelUI				*m_pLblStatusBar;
	//网页标题改变和URL改变的回调
	wkeClientHandler m_pWkeHandler; 
	
	CDuiString			m_URL;
	CDuiString			m_Title;

	CWkeModal*		m_pModal;
};