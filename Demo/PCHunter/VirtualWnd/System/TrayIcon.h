#pragma once

#include "DllCore/Window/SystemTray.h"

class CTrayIcon : public CNotifyPump
{
public:
	CTrayIcon();
	~CTrayIcon();

public:
	void OnTrayMenu(CControlUI* pControl);
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP();
	void OnLoadItem(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);
protected:

	BOOL GetTrayAreaIcon(CTrayDataInfo& TrayDataInfo);
	BOOL GetOverflowAreaIcon(CTrayDataInfo& TrayDataInfo);
	//BOOL GetTrayIcon(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);
	//BOOL GetTrayIcon64(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);
private:
	BOOL					   m_bLoad;
	BOOL					   m_b64BitOS;
	CControlBuilder	   m_RootBuilder;
	CDialogBuilder	   m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};