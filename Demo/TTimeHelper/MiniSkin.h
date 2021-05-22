#pragma once
#include "TTimeHelper.h"
#include "DllCore/Window/FloatWindow.h"
#include "DllCore/Desktop/ToggleDesktopHelper.h"

class CMiniSkin : public WindowImplBase
{
public:
	CMiniSkin();
	~CMiniSkin();
public:
	void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;
	void InitWindow();

	void Notify(TNotifyUI& msg);
protected:
	void OnTimer(TNotifyUI& msg);
	BOOL InjectPluginToDesktop();
	void OnWindowInit(TNotifyUI& msg);
	HRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	//HWND m_hParent;
	CFloatWindow* m_pFloatWindow;
	CTTimeHelper* m_pTimerHelper;
	ToggleDesktopHelper* m_pDesktopHelper;	//×ÀÃæ²å¼þÖúÊÖ
};