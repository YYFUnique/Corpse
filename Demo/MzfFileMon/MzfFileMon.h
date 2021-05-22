#pragma once

#include "DllCore/Window/FloatWindow.h"

class CMzfFileMon : public WindowImplBase
{
public:
	CMzfFileMon();
	~CMzfFileMon();
public:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void InitWindow();
	void PinToDesktop();
	virtual void OnFinalMessage(HWND hWnd);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	LRESULT OnMenuClick(WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnClick(TNotifyUI& msg);
		void OnTabSelect(TNotifyUI& msg);
		void OnValueChanged(TNotifyUI& msg);
		void OnSelectChanged(TNotifyUI& msg);
protected:
	CControlUI* GetViewObject(CTabLayoutUI* pTabLayout,int nIndex);
private:
	CFileProcMgr m_FileProcMgr;
	CFloatWindow* m_pFloatWindow;
};