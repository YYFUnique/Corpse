#pragma once
#include "Utils/ControlBuilder.h"
#include "DuiLib/Core/UIDlgBuilder.h"

class CApplication : public CNotifyPump
{
public:
	CApplication();
	~CApplication();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);
	void OnAppMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnMenu(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
protected:
	static BOOL CALLBACK  EnumWindowsProc(HWND hWnd,LPARAM lParam);
	void GetProcessPathByHwnd(HWND hWnd,CDuiString& strProcesspath);
	bool DosPathToNtPath(LPCTSTR lpszDosPath, CDuiString& strNtPath);
	void GetAppInfoByHwnd(HWND hWnd);
	void FillAppInfoIntoList(HWND hWnd);
private:
	BOOL m_bLoad;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};