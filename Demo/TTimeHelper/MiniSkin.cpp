#include "StdAfx.h"
#include "MiniSkin.h"

#include "DllCore/Utils/OsInfo.h"
#include "DllCore/Security/RemoteThreadInject.h"
#include "Wow64Ext/Wow64InjectDll.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Utils/ErrorInfo.h"

const UINT WM_TASKBARCREATED			= RegisterWindowMessage(_T("TaskbarCreated"));
const UINT WM_TOGGLEDESKTOP			= RegisterWindowMessage(TOGGLEDESKTOP);

CMiniSkin::CMiniSkin()
{
	m_pFloatWindow = NULL;
	m_pTimerHelper = NULL;
	m_pDesktopHelper = NULL;
}

CMiniSkin::~CMiniSkin()
{
	if (m_pFloatWindow != NULL)
		delete m_pFloatWindow;
	if (m_pTimerHelper != NULL)
		delete m_pTimerHelper;

	if (m_pDesktopHelper != NULL)
		m_pDesktopHelper->Release();
}

void CMiniSkin::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CMiniSkin::GetWindowClassName() const
{
	return _T("MiniSkin");
}

CDuiString CMiniSkin::GetSkinFile()
{
	return _T("MiniSkin.xml");
}

CDuiString CMiniSkin::GetZIPFileName() const
{
	return _T("TTimeHelper.zip");
}

CDuiString CMiniSkin::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("TTimeHelper");
#else
	return _T("");
#endif
}

UILIB_RESOURCETYPE CMiniSkin::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

void CMiniSkin::InitWindow()
{
	//m_pFloatWindow = new CFloatWindow;
	//m_pFloatWindow->StickWndToDesktop(m_hWnd);

	// 窗口注入，捕获显示桌面消息
	m_pDesktopHelper = ToggleDesktopHelper::GetInstance();
	if (m_pDesktopHelper != NULL)
	{
		BOOL bRet = m_pDesktopHelper->IsShellDesktopRunning();
		if (bRet == FALSE)
			InjectPluginToDesktop();

		m_pDesktopHelper->AddRef();
	}
}

void CMiniSkin::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnWindowInit(msg);
}

void CMiniSkin::OnWindowInit(TNotifyUI& msg)
{
	HWND hTimerHelper = FindWindow(_T("TTimeHelper"), NULL);
	if (m_pTimerHelper != NULL && IsWindow(hTimerHelper))
	{
		m_pTimerHelper->ShowWindow(true, false);
		return;
	}

	CTTimeHelper* pTimerHelper = new CTTimeHelper;
	pTimerHelper->ShowWindow(true, false);
}

BOOL CMiniSkin::InjectPluginToDesktop()
{
	BOOL bSuccess = FALSE;
	do 
	{	
		HWND hProgman = FindWindow(_T("Progman"), _T("Program Manager"));
		if (IsWindow(hProgman) == FALSE)
			break;

		DWORD dwExplorerPid = 0;
		GetWindowThreadProcessId(hProgman, &dwExplorerPid);

		LPCTSTR lpszPluginPath[] = {_T("Plugin\\IShellDesktopX64.dll"), _T("Plugin\\IShellDesktop.dll")};

		TCHAR szDirectoryPath[MAX_PATH];
		GetModuleFileName(NULL, szDirectoryPath, _countof(szDirectoryPath));
		PathRemoveFileSpec(szDirectoryPath);

		if (Is64BitPorcess(dwExplorerPid))
		{
			TCHAR szFilePath[MAX_PATH];
			PathCombine(szFilePath, szDirectoryPath, lpszPluginPath[0]);
			bSuccess = Wow64InjectX64(dwExplorerPid, szFilePath);
		}
		else
		{
			TCHAR szFilePath[MAX_PATH];
			PathCombine(szFilePath, szDirectoryPath, lpszPluginPath[1]);
			CRemoteThreadInject* pRemoteInject = new CRemoteThreadInject;
			if (pRemoteInject)
			{
				bSuccess = pRemoteInject->InjectDll(dwExplorerPid, szFilePath);
				delete pRemoteInject;
			}
		}
	} while (FALSE);

	return bSuccess;
}

HRESULT CMiniSkin::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 桌面资源管理器崩溃重启后产生该消息
	if (uMsg == WM_TASKBARCREATED)
	{
		m_pDesktopHelper = ToggleDesktopHelper::GetInstance();
		if (m_pDesktopHelper != NULL)
		{
			BOOL bRet = m_pDesktopHelper->IsShellDesktopRunning();
			if (bRet == FALSE)
				InjectPluginToDesktop();
			
			m_pDesktopHelper->AddRef();
		}
	}

	if (uMsg == WM_TOGGLEDESKTOP)
	{
		if (lParam == DTRF_RAISE)
		{
			SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		}
		else if (lParam == DTRF_LOWER)
		{
			DWORD dwStyle = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
			if (dwStyle & WS_EX_TOPMOST)
			{
				SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

				HWND hWnd = (HWND)GetForegroundWindow();
				if (hWnd == m_hWnd)
				{
					do 
					{
						hWnd = GetWindow(hWnd, GW_HWNDNEXT);
						BOOL bRealWnd = ::IsWindowVisible(hWnd);
						if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd) && ::IsWindowEnabled(hWnd) &&
							(GetWindowLong(hWnd, GWL_HWNDPARENT) == 0))
							break;
					} while (TRUE);
				}
				BringWindowToTop(hWnd);
			}
		}
		bHandled = TRUE;
		return FALSE;
	}

	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}