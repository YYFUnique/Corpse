#include "StdAfx.h"
#include "AppMonitor.h"
#include <WtsApi32.h>
#include <UserEnv.h>
#include <Windows.h>
#include "DebugHelper.h"

#pragma comment(lib,"WtsApi32.lib")
#pragma comment(lib,"Userenv.lib")

#define MIN_POLL_TIME					1000						// 线程轮询时间为 1 秒
#define INVALID_SESSION_ID			((DWORD)-1)			// 无效会话ID
#define INVALID_PROCESS_ID			((DWORD)-1)			// 无效进程ID

CAppMonitorInfo CAppMonitor::m_AppMonitorList;

CAppMonitor* CAppMonitor::GetInstance()
{
	static CAppMonitor sAppMonitor;
	return &sAppMonitor;
}

CAppMonitor::CAppMonitor()
	: m_dwAcceptCodes(0)
	, m_hService(NULL)
	, m_dwPollTime(MIN_POLL_TIME)
{
	InitializeCriticalSection(&m_csMonitor);
}

CAppMonitor::~CAppMonitor()
{
	StopMonitor();

	if (m_hThreadMonitor != NULL)
	{
		TerminateThread(m_hThreadMonitor, 0);
		CloseHandle(m_hThreadMonitor);
	}

	if (m_hEventExit != NULL)
		CloseHandle(m_hEventExit);
	DeleteCriticalSection(&m_csMonitor);
}

BOOL CAppMonitor::Run(LPWSTR lpszSrvName)
{
	m_hService = RegisterServiceCtrlHandlerExW(lpszSrvName, (LPHANDLER_FUNCTION_EX)ServiceHandler, (LPVOID)this);
	if (m_hService == NULL)
		return FALSE;

	SetSCManager(SERVICE_START_PENDING, 0, 1);

	SetSCManager(SERVICE_RUNNING, 0, 0);

	StartMonitor();

	return TRUE;
}

BOOL CAppMonitor::AddMonitorAppInfo(APPMONITOR_INFO& AppInfo)
{
	BOOL bSuccess = TRUE;
	do 
	{
		EnterCriticalSection(&m_csMonitor);
		POSITION pos = m_AppMonitorList.GetHeadPosition();

		while(pos)
		{
			const APPMONITOR_INFO& AppMonitorInfo = m_AppMonitorList.GetNext(pos);
			if (AppMonitorInfo.strFilePath.CompareNoCase(AppInfo.strFilePath) == 0)
			{
				bSuccess = FALSE;
				break;
			}

			if (AppMonitorInfo.strProcessName.CompareNoCase(AppInfo.strProcessName) == 0)
			{
				bSuccess = FALSE;
				break;
			}

			//当前链表中没有找到需要保护的进程，需要后面添加
			bSuccess = TRUE;
		}
		OutputDebugString(_T("2222222222222"));

		if (bSuccess)
			m_AppMonitorList.AddTail(AppInfo);

		LeaveCriticalSection(&m_csMonitor);
	} while (FALSE);
	
	return bSuccess;
}

BOOL CAppMonitor::DeleteMonitorAppInfo(const APPMONITOR_INFO& AppInfo)
{
	BOOL bSuccess = FALSE;
	do 
	{
		EnterCriticalSection(&m_csMonitor);
		POSITION pos = m_AppMonitorList.GetHeadPosition();
		while(pos)
		{
			POSITION posDelete = pos;
			const APPMONITOR_INFO& AppMonitorInfo = m_AppMonitorList.GetNext(pos);
			//需要删除的保护对象
			if (AppMonitorInfo.strFilePath.CompareNoCase(AppInfo.strFilePath) == 0 ||
				AppMonitorInfo.strProcessName.CompareNoCase(AppInfo.strProcessName) == 0)
			{
				m_AppMonitorList.RemoveAt(posDelete);
				break;
			}
		}
		LeaveCriticalSection(&m_csMonitor);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CAppMonitor::SetAcceptCodes(DWORD dwAcceptCode)
{
	m_dwAcceptCodes = dwAcceptCode;
}

DWORD CAppMonitor::GetAcceptCodes()
{
	return m_dwAcceptCodes;
}

void CAppMonitor::OnSessionChange(DWORD dwEventType, LPVOID lpEventData)
{
	PWTSSESSION_NOTIFICATION pwn = (PWTSSESSION_NOTIFICATION)lpEventData;

	switch (dwEventType)
	{
		case WTS_SESSION_LOGON:			// 添加新的会话 ID 到会话信息中
				LOG_PRINT(DBG_TRACE_FLAG_INFO, _T("Deamon.dll!CAppMonitor::OnSessionChange[WTS_SESSION_LOGON]"));
			break;
		case WTS_SESSION_LOGOFF:		// 删除已退出的会话 ID
				LOG_PRINT(DBG_TRACE_FLAG_INFO, _T("Deamon.dll!CAppMonitor::OnSessionChange[WTS_SESSION_LOGOFF]"));
			break;

		//  case WTS_SESSION_LOCK:
		//	case WTS_SESSION_UNLOCK:
		//	case WTS_REMOTE_CONNECT:
		//	case WTS_REMOTE_DISCONNECT:
		//	case WTS_CONSOLE_CONNECT:
		//	case WTS_CONSOLE_DISCONNECT:
		//	case WTS_SESSION_REMOTE_CONTROL:

	default: 
		break;
	}
}

BOOL CAppMonitor::SetSCManager(DWORD dwStatus, DWORD dwExitCode, DWORD dwProgress)
{
	SERVICE_STATUS SrvStatus;
	SrvStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	SrvStatus.dwCurrentState = m_dwCurrentState = dwStatus;
	SrvStatus.dwControlsAccepted = m_dwAcceptCodes;
	SrvStatus.dwWin32ExitCode = dwExitCode;
	SrvStatus.dwServiceSpecificExitCode = 0;
	SrvStatus.dwCheckPoint = dwProgress;
	SrvStatus.dwWaitHint = 3000;

	return ::SetServiceStatus(m_hService, &SrvStatus);
}

DWORD CAppMonitor::GetPollTime()
{
	return m_dwPollTime;
}

void CAppMonitor::SetPollTime(DWORD dwPollTime)
{
	m_dwPollTime = dwPollTime > MIN_POLL_TIME ? dwPollTime : MIN_POLL_TIME;
}

BOOL CAppMonitor::StopMonitor()
{
	if (m_hThreadMonitor == NULL || m_hEventExit == NULL)
		return FALSE;

	// 通知监视线程退出
	SetEvent(m_hEventExit);

	if (WaitForSingleObject(m_hThreadMonitor, m_dwPollTime*2) != WAIT_OBJECT_0)
		TerminateThread(m_hThreadMonitor, 0);

	CloseHandle(m_hThreadMonitor);
	m_hThreadMonitor = NULL;

	CloseHandle(m_hEventExit);
	m_hEventExit = NULL;

	return TRUE;
}

BOOL CAppMonitor::StartMonitor()
{
	BOOL bRet = FALSE;

	if (m_hEventExit != NULL)
		StopMonitor();

	m_hEventExit = CreateEvent(NULL, FALSE, FALSE, _T("MonitorEvent"));
	if (m_hEventExit != NULL)
	{
		DWORD dwTid = 0;

		m_hThreadMonitor = CreateThread(NULL, 0,  (LPTHREAD_START_ROUTINE)ThreadMonitor,  (LPVOID)this, CREATE_SUSPENDED, &dwTid);
		if (m_hThreadMonitor != NULL)
		{
			ResetEvent(m_hEventExit);
			ResumeThread(m_hThreadMonitor);
		}
	}

	return bRet;
}

DWORD CAppMonitor::ServiceHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID  lpContext)
{
	CAppMonitor* pAppMonitor = (CAppMonitor *)lpContext;

	if (pAppMonitor == NULL)
		return ERROR_CALL_NOT_IMPLEMENTED;

	switch(dwControl)
	{
	case SERVICE_CONTROL_STOP:
		pAppMonitor->StopMonitor();

		pAppMonitor->SetSCManager(SERVICE_STOP_PENDING, 0, 1);		

		Sleep(10);
		pAppMonitor->SetSCManager(SERVICE_STOPPED, 0, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		pAppMonitor->SetSCManager(SERVICE_PAUSE_PENDING, 0, 1);
		pAppMonitor->SetSCManager(SERVICE_PAUSED, 0, 0);
		break;
	case SERVICE_CONTROL_CONTINUE:
		pAppMonitor->SetSCManager(SERVICE_CONTINUE_PENDING, 0, 1);
		pAppMonitor->SetSCManager(SERVICE_RUNNING, 0, 0);
		break;
	case SERVICE_CONTROL_INTERROGATE:
		pAppMonitor->SetSCManager(pAppMonitor->m_dwCurrentState, 0, 0);
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		pAppMonitor->StopMonitor();
		pAppMonitor->SetSCManager(SERVICE_STOPPED, 0, 0);
		break;
	case SERVICE_CONTROL_SESSIONCHANGE:
		pAppMonitor->SetSCManager(SERVICE_CONTROL_SESSIONCHANGE, 0, 0);
		pAppMonitor->OnSessionChange(dwEventType, lpEventData);

		/*if (!pAppMonitor->m_bTermServiceRunning)
		{
			return ERROR_CALL_NOT_IMPLEMENTED;
		}*/

		break;
	}

	return NO_ERROR;
}

DWORD CAppMonitor::ThreadMonitor(LPVOID lpParam)
{
	DWORD dwRet = TRUE;
	CAppMonitor * pAppMonitor = (CAppMonitor *)lpParam;

	if (pAppMonitor == NULL)
		return FALSE;

	while(WaitForSingleObject(pAppMonitor->m_hEventExit, pAppMonitor->m_dwPollTime) != WAIT_OBJECT_0)
	{
		// 检测应用程序是否存在，并替换其是否存活的标识
		if (pAppMonitor->IsAppRunning() == FALSE)
		{
			// 启动应用程序
			if (WaitForSingleObject(pAppMonitor->m_hEventExit, 50) != WAIT_OBJECT_0)
				pAppMonitor->StartApps();
		}		
	}	

	return dwRet;
}

// 检测应用程序是否在运行, 如果未运行，则返回 FALSE, 否则返回 TRUE
BOOL CAppMonitor::IsAppRunning()
{
	BOOL bSuccess = FALSE;
	PWTS_SESSION_INFO wsi = NULL;
	PWTS_PROCESS_INFO wpi = NULL;

	do 
	{
		m_AppSessionList.RemoveAll();

		DWORD dwCount = 0;
		DWORD dwLen = 0;
		if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, 0, 1, &wpi, &dwCount) == FALSE)
			break;

		if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &wsi, &dwLen) == FALSE)
			break;

		for (UINT m=0; m<dwLen; ++m)
		{
			if (wsi[m].State == WTSListen || wsi[m].State == WTSDisconnected)
				continue;

			if (wsi[m].SessionId == 0)
				continue;

			EnterCriticalSection(&m_csMonitor);
			POSITION pos = m_AppMonitorList.GetHeadPosition();
			CString strTipInfo;
			strTipInfo.Format(_T("Count:%d.\r\n"),m_AppMonitorList.GetCount());
			OutputDebugString(strTipInfo);
			while(pos)
			{
				BOOL bFlag = FALSE;
				const APPMONITOR_INFO& AppMonitorInfo = m_AppMonitorList.GetNext(pos);
				for (UINT n=0; n<dwCount; ++n)
				{
					if (wpi[n].SessionId == 0 || wpi[n].SessionId != wsi[m].SessionId)
						continue;

					if (AppMonitorInfo.strProcessName.CompareNoCase(wpi[n].pProcessName) == 0)
					{
						bFlag = TRUE;
						break;
					}
				}

				if (bFlag == FALSE)
				{
					//添加到需要启动的链表中
					SESSION_PROCESS_INFO AppSessionInfo;
					AppSessionInfo.dwSessionId = wsi[m].SessionId;
					AppSessionInfo.strFilePath = AppMonitorInfo.strFilePath;
					m_AppSessionList.AddTail(AppSessionInfo);
				}
			}
			LeaveCriticalSection(&m_csMonitor);
		}
	
	} while (FALSE);

	if (wpi != NULL)
	{
		WTSFreeMemory(wpi);
		wpi = NULL;
	}

	if (wsi != NULL)
	{
		WTSFreeMemory(wsi);
		wsi = NULL;
	}

	return m_AppSessionList.IsEmpty();
}

BOOL CAppMonitor::StartApps()
{
	EnterCriticalSection(&m_csMonitor);
	POSITION pos = m_AppSessionList.GetHeadPosition();
	while(pos)
	{
		const SESSION_PROCESS_INFO& AppSessionInfo = m_AppSessionList.GetNext(pos);

		HANDLE hToken = NULL;
		if (QueryUserToken(AppSessionInfo.dwSessionId, &hToken) == FALSE)
		{
			LOG_PRINT(DBG_TRACE_FLAG_INFO,_T("CAppMonitor::StartApps(QueryUserToken Faild).\r\n"));
			continue;
		}
		
		if (ImpersonateLoggedOnUser(hToken) != FALSE)
		{
			LPVOID lpEnviromentStrings = NULL;
			if (CreateEnvironmentBlock(&lpEnviromentStrings, hToken, FALSE) != FALSE)
			{
				STARTUPINFO si = {sizeof(si)};
				PROCESS_INFORMATION pi = {0};
				PROFILEINFO pfi = {sizeof(pfi)};

				si.wShowWindow = SW_SHOW;
				si.lpDesktop = TEXT("winsta0\\default");
				LoadUserProfile(hToken, &pfi);

				BOOL bRet = CreateProcessAsUser(hToken, AppSessionInfo.strFilePath, 
																			NULL, NULL, NULL, FALSE, 
																			NORMAL_PRIORITY_CLASS|CREATE_UNICODE_ENVIRONMENT, 
																			lpEnviromentStrings, NULL, &si, &pi);
				if (bRet != FALSE)
				{
					SetThreadToken(&pi.hThread, hToken);
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}

				UnloadUserProfile(hToken, pfi.hProfile);
				DestroyEnvironmentBlock(lpEnviromentStrings);
			}

			RevertToSelf();		
			CloseHandle(hToken);
		}
	}
	LeaveCriticalSection(&m_csMonitor);

	return TRUE;
}

DWORD CAppMonitor::GetActiveConsoleSessionId()
{
	/*DWORD dwRet = INVALID_SESSION_ID;
	LPWTS_GET_ACTIVE_CONSOLE_SESSION_ID WtsGetActiveConsoleSessionId = NULL;
	HMODULE hLib = LoadLibrary("Kernel32.dll");

	if (hLib != NULL)	// 因windows 2000 不支持 WTSGetActiveConsoleSessionId 函数 ，因此动态调用
	{
		WtsGetActiveConsoleSessionId = (LPWTS_GET_ACTIVE_CONSOLE_SESSION_ID)GetProcAddress(hLib, 
			"WTSGetActiveConsoleSessionId");
		if (WtsGetActiveConsoleSessionId != NULL)
		{
			dwRet = WtsGetActiveConsoleSessionId();
		}
		FreeLibrary(hLib);
	}	*/
	return WTSGetActiveConsoleSessionId();
}

// 取得给定会话 ID 的用户令牌
BOOL CAppMonitor::QueryUserToken(DWORD dwSessionId, PHANDLE phToken)
{
	BOOL bRet = WTSQueryUserToken(dwSessionId, phToken);
	//if (bRet == FALSE)
	//	bRet = GetTokenBySessionId(dwSessionId, phToken);

	return bRet;
}

// 取得给定会话 ID 的用户令牌
/*BOOL CAppMonitor::GetTokenBySessionId(DWORD dwSessionId, PHANDLE phToken)
{
	BOOL bRet = FALSE;
	DWORD lpdwProcesses[1024] = {0};
	DWORD cbNeeded = 0;
	DWORD dwProcesses = 0;
	DWORD i = 0;
	DWORD dwSid = -1;

	if (phToken == NULL){
		return bRet;

	// 得到所有进程 PID 
	if (EnumProcesses(lpdwProcesses, sizeof(lpdwProcesses), &cbNeeded))
	{
		dwProcesses = cbNeeded / sizeof(DWORD);

		for (i=0; !bRet && i<dwProcesses; i++)
		{
			if (lpdwProcesses[i] == 0 || lpdwProcesses[i] == 4 || lpdwProcesses[i] == 8)
				continue;

			// 取给定 PID 进程的会话 ID
			if (ProcessIdToSessionId(lpdwProcesses[i], &dwSid) == FALSE)
				continue;

			if (dwSid == dwSessionId)			
			{
				CString strFilePath;
				WowGetProcessFileNameById(lpdwProcesses[i], strFilePath);
				strFilePath.MakeLower();

				TCHAR szWinDir[MAX_PATH]={0};
				GetWindowsDirectory(szWinDir,MAX_PATH);
				PathAppend(szWinDir,_T("explorer.exe"));
				CharLower(szWinDir);

				if (_tcsicmp(strFilePath,szWinDir)==0)
				{
					// 取应用程序的令牌 
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,lpdwProcesses[i]);
					if (hProcess != NULL)
					{	
						TCHAR szUserName[4096];
						GetProcessUserInfo(hProcess, szUserName);
						if(_stricmp(szUserName, "SYSTEM") !=0 && 
							_stricmp(szUserName, "NETWORK SERVICE") !=0 && 
							_stricmp(szUserName, "LOCAL SERVICE") !=0 )
							bRet = OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, phToken);
						CloseHandle(hProcess);
					}
				}	
			}
		}			
	}

	return bRet;
}*/