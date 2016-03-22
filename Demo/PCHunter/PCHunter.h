#pragma once

#include "Task.h"
#include "wndtask.h"
#include "MessageTip.h"

class CPCHunter : public WindowImplBase
{
public:
	CPCHunter();
	~CPCHunter();

//override
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	CPaintManagerUI* GetMainWndPaintManager();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	DWORD GetBkColor();
	void SetBkColor(DWORD crColor);
	CDuiString GetBkImage();
	void SetBkImage(LPCTSTR lpszBkImage);

	void SetListTrans(int nValue);
	void SetHeadTrans(LPCTSTR lpszBkImage,int nValue);
	HWND GetTrayWndHwnd();
	HWND FindNotifyIconOverflowWindow();
	void GetTrayWndPosition(CMenuWnd* pMenu,POINT& pt);

	CControlUI* GetViewObject(CTabLayoutUI* pTabLayout,int nIndex);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnF5Down(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
	LRESULT OnMenuClick(WPARAM wParam, LPARAM lParam);
protected:
	void OnClick(TNotifyUI& msg);
	void OnTabSelect(TNotifyUI& msg);
	void OnValueChanged(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	
	CVirtualWnd* FindObjectByName(LPCTSTR lpszVirtualWndName);

	void OnRemoteDesktop(CListTextElementUI* pItem);
	void OnPing(CListTextElementUI* pItem);
	void OnHostScanMenu(CControlUI* pControl);
	void OnRangeMenu(CControlUI* pControl);
protected:
	int m_nHeadTrans;
	CDuiString m_strBkImage;
	UINT _m;
	CControlUI* m_pLastPage;
	CControlUI* m_pCurrentPage;

	CTaskMgr	m_TaskMgr;
	CNetMgr		m_NetMgr;
	CStdPtrArray	m_SubNotifys;

	CUITrayIcon m_Tray;
	CMessageTip* m_pMsgTip;
	CDropTargetEx* m_pDropTarget;
};