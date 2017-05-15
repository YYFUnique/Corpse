#pragma once

class CInstaller : public WindowImplBase
{
public:
	CInstaller();
	~CInstaller();
public:
	void OnFinalMessage( HWND hWnd );
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;
	//virtual LPCTSTR GetResourceID() const;
	virtual void Notify(TNotifyUI& msg);
	void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

protected:

	void OnClick(TNotifyUI& msg);
	void OnInitDialog(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnMenuItemClick(CControlUI* pControl);
private:
	CMenuUI* pMenu;

	int m_nZStep;
	int m_nYStep;
	int m_nFrameIndex;
	HBITMAP m_hBitmapBefore;
	HBITMAP m_hBitmapAfter;

	DWORD m_dwTime;
};
