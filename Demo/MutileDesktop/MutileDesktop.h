#pragma once

typedef enum tagDesktopType
{
	DESKTOP_TYPE_UNDEFINED	= -1,		//未定义桌面
	DESKTOP_TYPE_SYSTEM			= 0,		//普通系统桌面
	DESKTOP_TYPE_SECURITY		= 1,		//自定义安全桌面
}DESKTOP_TYPE;

class CMutileDesktop : public WindowImplBase
{
public:
	CMutileDesktop();
	~CMutileDesktop();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandle);
	
	static BOOL CreateDesktop(DESKTOP_TYPE DesktopType);
	static BOOL SwitchDesktop(DESKTOP_TYPE DesktopType);
protected:
	void OnClick(TNotifyUI& msg);
	static UINT workThread(LPARAM lParam);
protected:
	static HDESK m_hDesktop[2];
	static BOOL m_bInit;
	ATOM m_HotKey;
	static DESKTOP_TYPE m_CurrentDesktopType;
};