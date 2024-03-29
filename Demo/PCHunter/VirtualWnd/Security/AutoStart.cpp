#include "StdAfx.h"
#include "AutoStart.h"
#include "DllCore/Utils/Registry.h"
#include "DllCore/Utils/FileTools.h"
#include "DllCore/File/FileSystemRedirecte.h"
#include <shlobj.h>

#define AUTO_RUN_PATH_RDP				_T("HKLM\\System\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\StartupPrograms")
#define AUTO_RUN_PATH_INIT				_T("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Userinit")
#define AUTO_RUN_PATH_SHELL			_T("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Shell")
#define AUTO_RUN_PATH_APP				_T("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")			
#define AUTO_RUN_PATH_APP64			_T("HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run")
#define AUTO_RUN_PATH_STARTUP	_T("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup")
#define AUTO_RUN_PATH_USER			_T("HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run")

CAutoStart::CAutoStart()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

CAutoStart::~CAutoStart()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CAutoStart, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CAutoStart::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CAutoStart::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("App"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(APPLICATION_MENU);
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

void CAutoStart::OnRefresh(TNotifyUI& msg)
{
	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pPaintManager->FindControl(_T("AutoStart"));
	if (pTreeView == NULL)
		return;	

	CAutoRunAppInfoList AutoRunAppInfoList;
	if (EnumAutoRunApp(AutoRunAppInfoList) == FALSE)
		return;

	pTreeView->RemoveAll();

	LPCTSTR lpszStartTypePath[] = {AUTO_RUN_PATH_RDP, AUTO_RUN_PATH_INIT, AUTO_RUN_PATH_SHELL,
														AUTO_RUN_PATH_APP, AUTO_RUN_PATH_APP64, AUTO_RUN_PATH_STARTUP,
														AUTO_RUN_PATH_USER};

	CTreeNodeUI* pTreeNode[7] = {0};

	POSITION pos = AutoRunAppInfoList.GetHeadPosition();
	while(pos)
	{
		const AUTORUN_APP_INFO& AutoRunAppInfo = AutoRunAppInfoList.GetNext(pos);

		int nType = AutoRunAppInfo.AutoRunType;
		// 设置开机启动项来自何处
		if (pTreeNode[nType] == NULL)
		{
 			pTreeNode[nType] = new CTreeNodeUI;
 			pTreeView->Add(pTreeNode[nType]);
			
			pTreeNode[nType]->SetItemText(lpszStartTypePath[nType]);
			pTreeNode[nType]->GetItemButton()->SetFont(0);
			pTreeNode[nType]->SetFixedHeight(33);

			//pTreeNode[nType]->SetText(lpszStartTypePath[nType]);
			//pTreeNode[nType]->SetFixedHeight(29);
			//pTreeNode[nType]->SetItemFont(0);
		}

		// 设置启动项名称
		CTreeNodeUI* pSubNode = new CTreeNodeUI;
		pTreeNode[nType]->Add(pSubNode);

		pTreeNode[nType]->SetChildPadding(10);

		pSubNode->SetItemText(AutoRunAppInfo.strItemName);
		pSubNode->SetFixedHeight(27);
		pSubNode->SetItemFont(0);
	}
}

void CAutoStart::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	OnRefresh(msg);
}

void CAutoStart::OnAppMenu(CControlUI* pControl)
{
	CListUI* pAppList = (CListUI*)m_pPaintManager->FindControl(_T("App"));
	if (pAppList->GetCurSel() == INVALID_ITEM_INDEX)
		return;

	CListTextElementUI* pItem = (CListTextElementUI*)pAppList->GetItemAt(pAppList->GetCurSel());
	HWND hWnd = (HWND)pItem->GetTag();
	if (IsWindow(hWnd) == FALSE)
		return;

	CDuiString strItemName = pControl->GetName();
}

BOOL CAutoStart::EnumAutoRunApp(CAutoRunAppInfoList& AutoStartList)
{
	// 获取RDP监视器
	AUTORUN_APP_INFO AutoRunApp;
	LPCTSTR lpszSubPath = _T("System\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd");
	LsRegQueryValue(HKEY_LOCAL_MACHINE, lpszSubPath, _T("StartupPrograms"), AutoRunApp.strItemName);
	if (AutoRunApp.strItemName.IsEmpty() == FALSE)
	{

		AutoRunApp.AutoRunType = AUTORUN_TYPE_RDP;
		AutoStartList.AddTail(AutoRunApp);
	}

	// 获取Userinit 登录初始应用
	lpszSubPath = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");
	LsRegQueryValue(HKEY_LOCAL_MACHINE, lpszSubPath, _T("Userinit"), AutoRunApp.strItemName);
	if (AutoRunApp.strItemName.IsEmpty() == FALSE)
	{
		AutoRunApp.AutoRunType = AUTORUN_TYPE_INIT;
		AutoStartList.AddTail(AutoRunApp);
	}

	// 获取资源管理Shell
	LsRegQueryValue(HKEY_LOCAL_MACHINE, lpszSubPath, _T("Shell"), AutoRunApp.strItemName);
	if (AutoRunApp.strItemName.IsEmpty() == FALSE)
	{
		AutoRunApp.AutoRunType = AUTORUN_TYPE_SHELL;
		AutoStartList.AddTail(AutoRunApp);
	}

	// 获取开机自启动应用
	AppendRunApps(FALSE, AutoStartList);

	AppendRunApps(TRUE, AutoStartList);
	
	// 获取开始菜单启动脚本应用
	AppendStartMenuApps(AutoStartList);

	// 获取当前用户注册表自启动项
	AppendUserRunApps(AutoStartList);

	return AutoStartList.IsEmpty() == FALSE;
}

void CAutoStart::AppendStartMenuApps(CAutoRunAppInfoList& AutoRunList)
{
	const static int nStartupFolder[] = {CSIDL_COMMON_STARTUP , CSIDL_STARTUP};

	for(int i=0;i<_countof(nStartupFolder);i++)
	{
		TCHAR szStartupFolderPath[MAX_PATH];
		if (SHGetSpecialFolderPath(NULL , szStartupFolderPath , nStartupFolder[i] , FALSE) == FALSE)
			continue;

		TCHAR szFilePath[MAX_PATH];
		PathCombine(szFilePath , szStartupFolderPath , _T("*.*"));

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(szFilePath , &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;
				else if (_tcsicmp(FindFileData.cFileName , _T("desktop.ini")) == 0)
					continue;

				PathCombine(szFilePath , szStartupFolderPath , FindFileData.cFileName);

				AUTORUN_APP_INFO  AutoRunInfo;
				AutoRunInfo.AutoRunType = AUTORUN_TYPE_STARTUP;

				LPCTSTR lpszExtName = PathFindExtension(FindFileData.cFileName);
				if (lpszExtName)
				{
					if (_tcsicmp(lpszExtName , _T(".lnk")) == 0)
					{
						GetFilePathByLink(szFilePath , AutoRunInfo.strAppPath);
						lpszExtName = PathFindExtension(AutoRunInfo.strAppPath);
					}
				}

				if (AutoRunInfo.strItemName.IsEmpty())
				{
					//PathRemoveExtension(FindFileData.cFileName);
					AutoRunInfo.strItemName = FindFileData.cFileName;
				}

				/*AutoRunInfo.strAppPath = szFilePath;*/
				AutoRunList.AddTail(AutoRunInfo);

			} while (FindNextFile(hFind , &FindFileData));
			FindClose(hFind);
		}
	}
}

void CAutoStart::AppendRunApps(BOOL bApp64, CAutoRunAppInfoList& AutoRunList)
{
	LPCTSTR lpszSubPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	HKEY hKey = NULL;
	do 
	{
		DWORD dwSamDesired = KEY_READ;
		if (bApp64 == FALSE)
			dwSamDesired |= KEY_WOW64_64KEY;

		RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszSubPath, 0, dwSamDesired, &hKey);
		if (hKey == NULL)
			break;

		DWORD dwIndex = 0;
		TCHAR szKey[MAX_PATH];
		TCHAR szValue[MAX_PATH];
		while(TRUE)
		{
			DWORD dwKey = _countof(szKey);
			DWORD dwValue = _countof(szValue);

			AUTORUN_APP_INFO AutoRunAppInfo;
			DWORD dwRet = RegEnumValue(hKey, dwIndex++, szKey, &dwKey, NULL, NULL, (LPBYTE)szValue, &dwValue);
			if (dwRet == ERROR_NO_MORE_ITEMS)
				break;

			AutoRunAppInfo.AutoRunType = bApp64 ? AUTORUN_TYPE_APP64 : AUTORUN_TYPE_APP;
			szKey[dwKey] = _T('\0');
			szValue[dwValue] = _T('\0');
			AutoRunAppInfo.strItemName = szKey;
			AutoRunAppInfo.strAppPath = szValue;

			AutoRunList.AddTail(AutoRunAppInfo);
		}
	} while (FALSE);

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}
}

void CAutoStart::AppendUserRunApps(CAutoRunAppInfoList& AutoRunList)
{
	LPCTSTR lpszSubPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");

	HKEY hKey = NULL;
	do 
	{
		RegOpenKeyEx(HKEY_CURRENT_USER, lpszSubPath, 0, KEY_READ, &hKey);
		if (hKey == NULL)
			break;

		DWORD dwIndex = 0;
		TCHAR szKey[MAX_PATH];
		TCHAR szValue[MAX_PATH];
		while(TRUE)
		{
			DWORD dwKey = _countof(szKey);
			DWORD dwValue = _countof(szValue);

			AUTORUN_APP_INFO AutoRunAppInfo;
			DWORD dwRet = RegEnumValue(hKey, dwIndex++, szKey, &dwKey, NULL, NULL, (LPBYTE)szValue, &dwValue);
			if (dwRet == ERROR_NO_MORE_ITEMS)
				break;
			
			AutoRunAppInfo.AutoRunType = AUTORUN_TYPE_USERRUN;
			szKey[dwKey] = _T('\0');
			szValue[dwValue] = _T('\0');
			AutoRunAppInfo.strItemName = szKey;
			AutoRunAppInfo.strAppPath = szValue;

			AutoRunList.AddTail(AutoRunAppInfo);
		}
	} while (FALSE);

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}
}