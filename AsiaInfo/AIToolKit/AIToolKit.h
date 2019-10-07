#pragma once

#include "Utils/UIPlatform.h"

class CAIToolkit : public WindowImplBase
{
public:
	CAIToolkit();
	~CAIToolkit();

public:
	virtual void OnFinalMessage(HWND hWnd);
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	//virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

protected:
	CControlUI* GetViewObject(CTabLayoutUI* pTabLayout,int nIndex);
	LRESULT OnClose(UINT , WPARAM , LPARAM , BOOL& bHandled);
	LRESULT OnMenuClick(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnF5Down(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnClick(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);
	void OnTabSelect(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	DWORD			 m_dwSelectOption;
	CDSManager	 m_DSMgr;
	CKSPManager	 m_KSPMgr;
	CISPSManager	 m_ISPSMgr;

	UIPlatform*      m_pUIPlatform;
};