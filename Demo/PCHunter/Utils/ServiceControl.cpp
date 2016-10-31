#include "stdafx.h"
#include "ServiceControl.h"
#include "ErrorInfo.h"

BOOL DelServiceByName(LPCTSTR lpServiceName)
{
	BOOL bRet=FALSE;	

	SC_HANDLE hSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_QUERY_LOCK_STATUS);
	if (hSCManager!=NULL) 
	{
		SC_HANDLE hSCService=OpenService(hSCManager,lpServiceName,DELETE);
		if (hSCService!=NULL)
		{
			bRet = DeleteService(hSCService);
			CloseServiceHandle(hSCService);
		}
		CloseServiceHandle(hSCManager);
	}

	return bRet;
}

BOOL StartServiceByName(LPCTSTR lpServiceName,BOOL bWait)
{
	BOOL bRet=FALSE;
	SC_HANDLE SCManager=0,SCService=0;

	do 
	{
		SCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);	
		if (SCManager == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("启动服务%s时打开服务控制管理器失败"),lpServiceName);
			break;
		}

		SCService=OpenService(SCManager,lpServiceName,SERVICE_QUERY_STATUS | SERVICE_START);
		if (SCService == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("启动服务%s时打开服务失败"),lpServiceName);
			break;
		}

		SERVICE_STATUS ServiceStatus;
		if (QueryServiceStatus(SCService,&ServiceStatus) == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("启动服务%s时获取服务状态失败"),lpServiceName);
			break;
		}

		if (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
		{
			bRet = TRUE;
			break;
		}
		else
		{
			if (ServiceStatus.dwCurrentState != SERVICE_START_PENDING)
				if (StartService(SCService,0,0)==0)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("启动服务%s失败"),lpServiceName);
					break;
				}

				if (bWait == FALSE)
				{
					bRet = TRUE;
					break;
				}

				//预先设置错误信息
				SetErrorInfo(CUSTOM_ERROR , 0 , _T("启动服务%s失败:启动超时。") , lpServiceName);

				SERVICE_STATUS ServiceStatus;
				for (int i=0;i<50;i++)
				{
					if (QueryServiceStatus(SCService,&ServiceStatus) == 0)
					{
						SetErrorInfo(SYSTEM_ERROR,0,_T("启动服务%s时获取服务状态失败"),lpServiceName);
						break;
					}

					if (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
					{
						bRet = TRUE;
						break;
					}

					Sleep(100);
				}
		}
	} while (0);

	if (SCService)
		CloseServiceHandle(SCService);

	if (SCManager)
		CloseServiceHandle(SCManager);

	return bRet;
}

BOOL RestartServiceByName(LPCTSTR lpServiceName,DWORD dwTimeout /* = 0*/)
{	
	BOOL bRet = FALSE;
	SC_HANDLE hSCMag = NULL;
	SC_HANDLE hSCSvr = NULL;
	DWORD dwWaitTime = 10*1000;
	do
	{
		hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
		if (hSCMag == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("重启服务时打开服务管理器失败"));
			break;
		}

		hSCSvr = OpenService(hSCMag, lpServiceName, SERVICE_QUERY_STATUS | SERVICE_START| SERVICE_STOP);
		if (hSCSvr == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("重启服务时打开服务%s失败"),lpServiceName);
			break;
		}

		SERVICE_STATUS  ssStatus;

		BOOL bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
		if (bSuccess == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("重启服务时查询服务%s状态失败"),lpServiceName);
			break;
		}

		//如果不是停止中和已停止则停止服务
		DWORD dwStartTime = GetTickCount();
		if (ssStatus.dwCurrentState != SERVICE_STOP_PENDING && ssStatus.dwCurrentState != SERVICE_STOPPED)
		{
			bSuccess = ControlService(hSCSvr,SERVICE_CONTROL_STOP,&ssStatus);
			if (bSuccess == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR , 0 , _T("重启服务时停止服务%s失败"),lpServiceName);
				break;
			}
		}

		do 
		{
			if (ssStatus.dwCurrentState == SERVICE_STOPPED)
				break;
			Sleep(50);
			if (dwTimeout !=INFINITE && GetTickCount()-dwStartTime > dwWaitTime)		
			{
				SetErrorInfo(CUSTOM_ERROR,0,_T("重启服务等待服务%s关闭时，等待时间超过了预设时间"),lpServiceName);
				break;
			}
			bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
			if (bSuccess == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("重启服务查询服务%s状态时失败"),lpServiceName);
				break;
			}
		} while (TRUE);


		if (StartService(hSCSvr,NULL,NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("重启服务时启动服务%s失败"),lpServiceName);
			break;
		}

		if (dwTimeout != INFINITE)
		{
			do 
			{
				bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
				if (bSuccess == FALSE)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("重启服务查询服务%s状态时失败"),lpServiceName);
					break;
				}

				if (ssStatus.dwCurrentState == SERVICE_RUNNING)
				{
					bRet = TRUE;
					break;
				}
				Sleep(50);
				if (dwTimeout !=INFINITE && GetTickCount()-dwStartTime>dwTimeout + dwWaitTime)	//如果等待时间已超过预设时间则退出
				{
					SetErrorInfo(CUSTOM_ERROR,0,_T("重启服务等待服务%s启动时，等待时间超过了预设时间"),lpServiceName);
					break;
				}
			} while (TRUE);
		}
		else
			bRet = TRUE;

	}while(FALSE);


	if (hSCSvr)
		CloseServiceHandle(hSCSvr);
	if (hSCMag)
		CloseServiceHandle(hSCMag);

	return bRet;
}

BOOL IsServiceRunning(LPCTSTR lpServiceName)
{
	SC_HANDLE hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (hSCMag == NULL)
		return FALSE;

	SC_HANDLE hSCSvr = OpenService(hSCMag, lpServiceName, SERVICE_QUERY_STATUS);
	if (hSCSvr == NULL)
	{
		CloseServiceHandle(hSCMag);
		return FALSE;
	}

	BOOL bRet = TRUE;
	do 
	{
		SERVICE_STATUS  ssStatus;

		BOOL bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
		if (!bSuccess)
		{
			bRet = FALSE;
			break;
		}

		if (ssStatus.dwCurrentState != SERVICE_RUNNING)
		{
			bRet = FALSE;
		}
	} while (0);

	CloseServiceHandle(hSCSvr);
	CloseServiceHandle(hSCMag);
	return bRet; 
}

BOOL ServiceIsExist(LPCTSTR lpszServiceName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCMag = NULL;
	SC_HANDLE hService = NULL;
	do 
	{
		hSCMag = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hSCMag == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("查看服务时是否存在时打开服务控制管理器失败"));
			break;
		}

		hService = OpenService(hSCMag,lpszServiceName,GENERIC_READ);
		if (hService == NULL)
		{
			if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
			{
				break;
			}
		}

		bRet = TRUE;
	} while (0);

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hSCMag)
	{
		CloseServiceHandle(hSCMag);
	}

	return bRet;
}

BOOL GetServiceFilePath(SC_HANDLE schService, LPTSTR lpszPathFile)
{
	BYTE ServiceInfo[8192];
	LPQUERY_SERVICE_CONFIG lpServiceConfig = (LPQUERY_SERVICE_CONFIG)ServiceInfo;

	BOOL bSuccess = FALSE;

	do 
	{
		DWORD dwBytesNeeded;
		if (QueryServiceConfig(schService, lpServiceConfig, sizeof(ServiceInfo), &dwBytesNeeded) == FALSE)
		{
			DWORD dwErrorCode = GetLastError();
			if (dwErrorCode != ERROR_INSUFFICIENT_BUFFER)
			{
				SetErrorInfo(SYSTEM_ERROR , dwErrorCode , _T("获取服务配置信息失败"));
				break;
			}

			LPQUERY_SERVICE_CONFIG lpServiceConfig = (LPQUERY_SERVICE_CONFIG) new BYTE[dwBytesNeeded];
			if (lpServiceConfig == NULL)
			{
				SetErrorInfo(CUSTOM_ERROR , 0 , _T("获取服务配置信息失败:分配内存(%u字节)失败.") , dwBytesNeeded);
				break;
			}

			if (QueryServiceConfig(schService, lpServiceConfig, dwBytesNeeded, &dwBytesNeeded) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取服务配置信息失败"));
				break;
			}
		}

		_tcscpy_s(lpszPathFile , MAX_PATH , lpServiceConfig->lpBinaryPathName);

		bSuccess = TRUE;
	} while (0);

	if ((BYTE*)lpServiceConfig != ServiceInfo && lpServiceConfig)
		delete[] lpServiceConfig;

	return bSuccess;
}

BOOL GetServiceFilePath(LPCTSTR lpszServiceName, LPTSTR lpszPathFile)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCMag = NULL;
	SC_HANDLE hService = NULL;
	do 
	{
		hSCMag = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hSCMag == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务程序路径时打开服务控制管理器失败"));
			break;
		}

		hService = OpenService(hSCMag,lpszServiceName,GENERIC_READ);
		if (hService == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务程序路径时获取指定服务句柄失败"));
			break;
		}

		bRet = GetServiceFilePath(hService,lpszPathFile);
	} while (0);

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hSCMag)
	{
		CloseServiceHandle(hSCMag);
	}

	return bRet;
}

BOOL StopServiceByName(LPCTSTR lpServiceName , BOOL bWait)
{
	BOOL bRet=FALSE;	
	SC_HANDLE hSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_QUERY_LOCK_STATUS);
	if (hSCManager!=NULL) 
	{
		SC_HANDLE hSCService=OpenService(hSCManager,lpServiceName,SERVICE_QUERY_STATUS|SERVICE_STOP);
		if (hSCService!=NULL)
		{
			do 
			{
				SERVICE_STATUS ServiceStatus;
				if (QueryServiceStatus(hSCService,&ServiceStatus)==0)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("关闭%s服务时获取服务状态失败"),lpServiceName);
					break;
				}
				if (ServiceStatus.dwCurrentState==SERVICE_STOPPED)
				{
					bRet = TRUE;
					break;
				}	
				SERVICE_STATUS status;
				bRet = ControlService(hSCService,SERVICE_CONTROL_STOP,&status);
				if (bRet == FALSE)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("停止服务%s失败"),lpServiceName);
					break;
				}

				if (bWait)
				{
					bRet = FALSE;
					SetErrorInfo(CUSTOM_ERROR , 0 , _T("停止服务%s失败:停止超时。") , lpServiceName);

					SERVICE_STATUS ServiceStatus;
					for (int i=0;i<50;i++)
					{
						if (QueryServiceStatus(hSCService,&ServiceStatus) == 0)
						{
							SetErrorInfo(SYSTEM_ERROR,0,_T("停止服务%s时获取服务状态失败"),lpServiceName);
							break;
						}

						if (ServiceStatus.dwCurrentState == SERVICE_STOPPED)
						{
							bRet = TRUE;
							break;
						}

						Sleep(100);
					}
				}
			} while (0);
			CloseServiceHandle(hSCService);
		}
		CloseServiceHandle(hSCManager);
	}
	return bRet;
}

BOOL StopDependentsServiceByName(LPCTSTR lpServiceName , BOOL bWait)
{
	BOOL bRet=FALSE;	
	SC_HANDLE hSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_QUERY_LOCK_STATUS);
	if (hSCManager!=NULL) 
	{
		SC_HANDLE hSCService=OpenService(hSCManager,lpServiceName,SERVICE_ENUMERATE_DEPENDENTS|SERVICE_QUERY_STATUS|SERVICE_STOP);
		if (hSCService!=NULL)
		{
			do 
			{
				SERVICE_STATUS ServiceStatus;
				if (QueryServiceStatus(hSCService,&ServiceStatus)==0)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("关闭%s服务时获取服务状态失败"),lpServiceName);
					break;
				}
				if (ServiceStatus.dwCurrentState==SERVICE_STOPPED)
				{
					bRet = TRUE;
					break;
				}	
				SERVICE_STATUS status;
				bRet = ControlService(hSCService,SERVICE_CONTROL_STOP,&status);
				if (bRet == FALSE && GetLastError() == ERROR_DEPENDENT_SERVICES_RUNNING)
				{
					BYTE lpData[1024];
					DWORD dwSize = _countof(lpData);
					DWORD dwCount = 0;
					LPENUM_SERVICE_STATUS   lpDependencies = (LPENUM_SERVICE_STATUS)lpData;
					EnumDependentServices(hSCService,SERVICE_ACTIVE,lpDependencies,dwSize,&dwSize,&dwCount);

					for (DWORD dwItem=0;dwItem<dwCount;++dwItem)
					{
						SERVICE_STATUS ServiceStatus = lpDependencies[dwItem].ServiceStatus;
						if (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
							StopDependentsServiceByName(lpDependencies[dwItem].lpServiceName,bWait);
					}
					bRet=ControlService(hSCService,SERVICE_CONTROL_STOP,&status);
				}
				else if (bRet == FALSE)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("停止服务%s失败"),lpServiceName);
					break;
				}

				if (bWait)
				{
					bRet = FALSE;
					SetErrorInfo(CUSTOM_ERROR , 0 , _T("停止服务%s失败:停止超时。") , lpServiceName);

					SERVICE_STATUS ServiceStatus;
					for (int i=0;i<50;i++)
					{
						if (QueryServiceStatus(hSCService,&ServiceStatus) == 0)
						{
							SetErrorInfo(SYSTEM_ERROR,0,_T("停止服务%s时获取服务状态失败"),lpServiceName);
							break;
						}

						if (ServiceStatus.dwCurrentState == SERVICE_STOPPED)
						{
							bRet = TRUE;
							break;
						}

						Sleep(100);
					}
				}
			} while (0);
			CloseServiceHandle(hSCService);
		}
		CloseServiceHandle(hSCManager);
	}
	return bRet;
}