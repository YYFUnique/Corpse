#pragma once

class CWinMTR : public WindowImplBase
{
public:
	CWinMTR();
	~CWinMTR();

public:
	virtual void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void InitWindow();

protected:
	CControlUI* GetViewObject(CTabLayoutUI* pTabLayout, int nIndex);
	LRESULT OnTabChange(WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnClick(TNotifyUI& msg);
	void OnItemSelect(TNotifyUI& msg);
protected:
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	CIPv4Mgr		m_IPv4Mgr;
};