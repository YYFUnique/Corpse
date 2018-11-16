#pragma once
#include "Utils/ControlBuilder.h"
#include "DuiLib/Core/UIDlgBuilder.h"

typedef struct tagRUNNING_APP_INFO
{
	HWND		hMainWnd;
	BOOL		bRunning;
	CString		strTitle;
	CString		strClassName;
}RUNNING_APP_INFO;

typedef CDuiList<RUNNING_APP_INFO, const RUNNING_APP_INFO&> CRunAppInfoList;

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
	void OnTimerEx(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
protected:
	static BOOL CALLBACK  EnumWindowsProc(HWND hWnd,LPARAM lParam);
	void GetProcessPathByHwnd(HWND hWnd,CDuiString& strProcesspath);
	bool DosPathToNtPath(LPCTSTR lpszDosPath, CDuiString& strNtPath);
	BOOL GetRunAppsInfo(CRunAppInfoList& RunApps);
	void CreateNewAppItem(CListUI* pList, const TListInfoUI* pListInfo, const RUNNING_APP_INFO& RunAppInfo);
private:
	BOOL m_bLoad;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};