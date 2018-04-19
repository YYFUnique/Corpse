#pragma once

class CPCHunter : public WindowImplBase
{
public:
	CPCHunter();
	~CPCHunter();

//override
public:
	void OnFinalMessage( HWND hWnd );
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	CPaintManagerUI* GetMainWndPaintManager();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


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
	DUI_DECLARE_MESSAGE_MAP()
	void OnClick(TNotifyUI& msg);
	void OnTabSelect(TNotifyUI& msg);
	void OnValueChanged(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	
protected:
	//主机扫描相关菜单
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

	CTaskMgr		m_TaskMgr;
	CNetworkMgr	m_NetworkMgr;
	CHardMgr		m_HardMgr;

	CUITrayIcon m_Tray;
	//CMessageTip* m_pMsgTip;
	//CUIDropTarget* m_pDropTarget;
	//IMAGE3D::PARAM3DTRANSFORM _3dParam;
	//IMAGE3D::C3DTransform image3d;
};