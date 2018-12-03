#pragma once
#include "Utils/ControlBuilder.h"
#include "DuiLib/Core/UIDlgBuilder.h"

//typedef struct tagRUNNING_APP_INFO
//{
//	HWND		hMainWnd;
//	BOOL		bRunning;
//	CString		strTitle;
//	CString		strClassName;
//}RUNNING_APP_INFO;
//
//typedef CDuiList<RUNNING_APP_INFO, const RUNNING_APP_INFO&> CRunAppInfoList;

typedef enum tagAUTORUN_TYPE
{
	AUTORUN_TYPE_RDP				=	0,	// 远程桌面监视程序
	AUTORUN_TYPE_INIT				=	1,	// UserInit 登录应用程序
	AUTORUN_TYPE_SHELL			=	2,	// Window 资源管理器
	AUTORUN_TYPE_APP				=	3,	// 64位应用64位系统开机启动
	AUTORUN_TYPE_APP64			=	4,	// 32位应用64位系统开机启动
	AUTORUN_TYPE_STARTUP		=	5,	// 开始菜单中的启动脚本
	AUTORUN_TYPE_USERRUN	=	6,	// 个人用户的启用项
}AUTORUN_TYPE;

typedef struct tagAUTORUN_APP_INFO
{
	CString strItemName;
	CString	 strAppPath;
	AUTORUN_TYPE AutoRunType;
}AUTORUN_APP_INFO,*PAUTORUN_APP_INFO;

typedef CDuiList<AUTORUN_APP_INFO, const AUTORUN_APP_INFO&> CAutoRunAppInfoList;

class CAutoStart : public CNotifyPump
{
public:
	CAutoStart();
	~CAutoStart();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);
	void OnAppMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnMenu(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
protected:
	void AppendStartMenuApps(CAutoRunAppInfoList& AutoRunList);
	void AppendUserRunApps(CAutoRunAppInfoList& AutoRunList);
	void AppendRunApps(BOOL bApp64, CAutoRunAppInfoList& AutoRunList);
	BOOL EnumAutoRunApp(CAutoRunAppInfoList& AutoRunAppInfoList);
private:
	BOOL m_bLoad;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};