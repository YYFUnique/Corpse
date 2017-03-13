#include "stdafx.h"
#include "ErrorInfo.h"
#include "ServiceControl.h"
#include "TextTools.h"
#include <Winsvc.h>
 
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
				bRet=ControlService(hSCService,SERVICE_CONTROL_STOP,&status);
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
				bRet=ControlService(hSCService,SERVICE_CONTROL_STOP,&status);
				if (bRet == FALSE && GetLastError() == ERROR_DEPENDENT_SERVICES_RUNNING)
				{
					BYTE lpData[1024];
					DWORD dwSize = _countof(lpData);
					DWORD dwCount = 0;
					LPENUM_SERVICE_STATUS   lpDependencies = (LPENUM_SERVICE_STATUS)lpData;
					EnumDependentServices(hSCService,SERVICE_ACTIVE,lpDependencies,dwSize,&dwSize,&dwCount);
					
					//CStringArray strDependentServices;
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

BOOL InstallDriver(LPCTSTR lpszFilePath, LPCTSTR lpszDrvName)
{
	BOOL bSuccess = FALSE;

	SC_HANDLE  SCService = NULL;
	SC_HANDLE SCManager = NULL;
	do 
	{
		SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

		if (SCManager == NULL) 
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开服务管理器失败"));
			break;
		}

		//Create the driver "service"
		SCService = CreateService(SCManager,			  // SCManager database
													lpszDrvName,            // nombre del servicio
													lpszDrvName,            // nombre a mostrar
													SERVICE_ALL_ACCESS,    // acceso total
													SERVICE_KERNEL_DRIVER, // driver del kernel
													// 										 SERVICE_DEMAND_START,  // comienzo bajo demanda
													SERVICE_DEMAND_START,
													SERVICE_ERROR_NORMAL,  // control de errores normal
													lpszFilePath,	          // path del driver
													NULL,                  // no pertenece a un grupo
													NULL,                  // sin tag
													NULL,                  // sin dependencias
													NULL,                  // cuenta local del sistema
													NULL                   // sin password
													);

		//if i cant create, first i check if the driver already was loaded.
		if (SCService == NULL) 
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("创建驱动服务失败"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (SCService != NULL)
	{
		CloseServiceHandle(SCService);
		SCService=NULL;
	}	

	if (SCManager != NULL)
	{
		CloseServiceHandle(SCManager);
		SCManager = NULL;
	}

	return bSuccess;
}

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
		
		if (ssStatus.dwCurrentState != SERVICE_RUNNING && ssStatus.dwCurrentState != SERVICE_START_PENDING)
		{
			bRet = FALSE;
		}
	} while (0);

	CloseServiceHandle(hSCSvr);
	CloseServiceHandle(hSCMag);
	return bRet; 
}

DWORD GetServiceCount()
{
    DWORD dwBytesNeeded=0, cbBufSize=0, dwReslut=0, dwResume=0; 
    LPENUM_SERVICE_STATUS_PROCESS lpEss;
	SC_HANDLE hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (hSCMag == NULL)
    	return -1;
	
	// 第一次调用，以确定缓冲区大小
	if ( !EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL) )
	{		
		lpEss = (LPENUM_SERVICE_STATUS_PROCESS)VirtualAlloc(NULL, dwBytesNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	
	// 枚举服务，得到服务的进程ID，服务名
	EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (LPBYTE)lpEss, dwBytesNeeded, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL);

	CloseServiceHandle(hSCMag);
	VirtualFree(lpEss, 0, MEM_FREE);
	
	return dwReslut;
}

DWORD GetCoreServiceCount()
{
    DWORD dwBytesNeeded=0, cbBufSize=0, dwReslut=0, dwResume=0; 
    LPENUM_SERVICE_STATUS_PROCESS lpEss;
	SC_HANDLE hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (hSCMag == NULL)
		return -1;
	
	// 第一次调用，以确定缓冲区大小
	if ( !EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL) )
	{		
		lpEss = (LPENUM_SERVICE_STATUS_PROCESS) VirtualAlloc(NULL, dwBytesNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	
	// 枚举服务，得到服务的进程ID，服务名
	EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER, SERVICE_STATE_ALL, (LPBYTE)lpEss, dwBytesNeeded, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL);
	
	CloseServiceHandle(hSCMag);
	VirtualFree(lpEss, 0, MEM_FREE);
	
	return dwReslut;
}

BOOL GetServiceInfo(CServiceList& serList)
{
	serList.RemoveAll();
	SERVICEINFO SvrInfo;
	LPENUM_SERVICE_STATUS_PROCESS lpEss;
	LPQUERY_SERVICE_CONFIG lpSC;
	DWORD dwBytesNeeded=0, cbBufSize=0, dwReslut=0, dwResume=0; 
	
	LPSERVICE_DESCRIPTION lpsd;
	DWORD cbBufSize2=0, dwNeeded=0;
	
	SC_HANDLE hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (hSCMag == NULL)
		return FALSE;
	
	// 第一次调用，以确定缓冲区大小
	if ( !EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL) )
	{		
		lpEss = (LPENUM_SERVICE_STATUS_PROCESS) VirtualAlloc(NULL, dwBytesNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	
	// 枚举服务，得到服务的进程ID，服务名
	EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (LPBYTE)lpEss, dwBytesNeeded, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL);
	
	for (int i=0; i<(int)dwReslut; i++)
	{
		SC_HANDLE hSCSvr = OpenService( hSCMag, lpEss[i].lpServiceName, SERVICE_QUERY_CONFIG); 	
		if (hSCSvr == NULL)
			return FALSE;

		TCHAR szBuffer[1024]={0};
		QueryServiceConfig( hSCSvr,(LPQUERY_SERVICE_CONFIG)szBuffer, 1024, &dwBytesNeeded);
		lpSC = (LPQUERY_SERVICE_CONFIG)szBuffer;

		TCHAR szBuffer2[1024]={0};
		QueryServiceConfig2( hSCSvr, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) szBuffer2, 1024, &dwBytesNeeded);
		lpsd = (LPSERVICE_DESCRIPTION)szBuffer2;

// 		// 第一次调用，获取缓冲区大小
// 		if( !QueryServiceConfig( hSCSvr, NULL, 0, &dwBytesNeeded) )
// 		{
// 			if( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
// 			{
// 				cbBufSize = dwBytesNeeded;
// 				lpSC = (LPQUERY_SERVICE_CONFIG) VirtualAlloc(NULL, cbBufSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
// 			}
// 		}	
// 		
// 		// 根据服务名，查询对应服务的状态，这些状态包括：
// 		// 运行状态，类型，启动类型，文件路径
// 		QueryServiceConfig( hSCSvr, lpSC, cbBufSize, &dwBytesNeeded);
// 		
// 		if( !QueryServiceConfig2( hSCSvr, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwNeeded) )
// 		{
// 			if( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
// 			{
// 				cbBufSize2 = dwNeeded;
// 				lpsd = (LPSERVICE_DESCRIPTION) VirtualAlloc(NULL, cbBufSize2, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
// 			}
// 		}	
// 		
// 		QueryServiceConfig2( hSCSvr, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) lpsd, cbBufSize2, &dwNeeded);
		
		SvrInfo.strSvrName = CString (lpEss[i].lpServiceName);
		SvrInfo.strDisplayName = CString (lpEss[i].lpDisplayName);
		SvrInfo.strPath = CString (lpSC->lpBinaryPathName);
		SvrInfo.strDiscruption = CString (lpsd->lpDescription);
		
		SvrInfo.dwPID = lpEss[i].ServiceStatusProcess.dwProcessId;
		SvrInfo.dwRunStatus = lpEss[i].ServiceStatusProcess.dwCurrentState;
		SvrInfo.dwStartType = lpSC->dwStartType;
		SvrInfo.dwType = lpEss[i].ServiceStatusProcess.dwServiceType;
		
        serList.AddTail(SvrInfo);
		
		VirtualFree(lpsd, 0, MEM_FREE);
		VirtualFree(lpSC, 0, MEM_FREE);
		CloseServiceHandle(hSCSvr);
	}
	
	VirtualFree(lpEss, 0, MEM_FREE);
	CloseServiceHandle(hSCMag);
	
	return TRUE;
}

BOOL GetCoreServiceInfo(CServiceList& serList)
{
	serList.RemoveAll();
	SERVICEINFO SvrInfo;
	LPENUM_SERVICE_STATUS_PROCESS lpEss;
	LPQUERY_SERVICE_CONFIG lpSC;
	DWORD dwBytesNeeded=0, cbBufSize=0, dwReslut=0, dwResume=0; 
	
	LPSERVICE_DESCRIPTION lpsd;
	DWORD cbBufSize2=0, dwNeeded=0;
	
	SC_HANDLE hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (hSCMag == NULL)
		return FALSE;
	
	// 第一次调用，以确定缓冲区大小
	if ( !EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL) )
	{		
		lpEss = (LPENUM_SERVICE_STATUS_PROCESS) VirtualAlloc(NULL, dwBytesNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	
	// 枚举服务，得到服务的进程ID，服务名
	EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER, SERVICE_STATE_ALL, (LPBYTE)lpEss, dwBytesNeeded, 
		&dwBytesNeeded, &dwReslut, &dwResume, NULL);
	
	for (int i=0; i<(int)dwReslut; i++)
	{
		SC_HANDLE hSCSvr = OpenService( hSCMag, lpEss[i].lpServiceName, SERVICE_QUERY_CONFIG); 	
		if (hSCSvr == NULL)
			return FALSE;

		TCHAR szBuffer[1024]={0};
		QueryServiceConfig( hSCSvr,(LPQUERY_SERVICE_CONFIG)szBuffer, 1024, &dwBytesNeeded);
		lpSC = (LPQUERY_SERVICE_CONFIG)szBuffer;

		TCHAR szBuffer2[1024]={0};
		QueryServiceConfig2( hSCSvr, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) szBuffer2, 1024, &dwBytesNeeded);
		lpsd = (LPSERVICE_DESCRIPTION)szBuffer2;

 		SvrInfo.strSvrName = CString (lpEss[i].lpServiceName);
		SvrInfo.strDisplayName = CString (lpEss[i].lpDisplayName);
		SvrInfo.strPath = CString (lpSC->lpBinaryPathName);
		SvrInfo.strDiscruption = CString (lpsd->lpDescription);
		
		SvrInfo.dwPID = lpEss[i].ServiceStatusProcess.dwProcessId;
		SvrInfo.dwRunStatus = lpEss[i].ServiceStatusProcess.dwCurrentState;
		SvrInfo.dwStartType = lpSC->dwStartType;
		SvrInfo.dwType = lpEss[i].ServiceStatusProcess.dwServiceType;
		
        serList.AddTail(SvrInfo);
		
		VirtualFree(lpsd, 0, MEM_FREE);
		VirtualFree(lpSC, 0, MEM_FREE);
		CloseServiceHandle(hSCSvr);
	}
	
	VirtualFree(lpEss, 0, MEM_FREE);
	CloseServiceHandle(hSCMag);
	
	return TRUE;
}

BOOL GetServiceConfig(LPCTSTR lpszServiceName,LS_SERVICE_CONFIG& ServiceConfig)
{
	SC_HANDLE hSCManager = 0;
	SC_HANDLE hService = 0;
	LPQUERY_SERVICE_CONFIG lpQueryServiceConfig = NULL;
	LPSERVICE_STATUS_PROCESS lpServiceStatusProcess = NULL;
	BOOL bSuccess = FALSE;

	do
	{
		hSCManager = OpenSCManager(NULL,NULL,SERVICE_QUERY_CONFIG);
		if (hSCManager == NULL) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时打开服务控制管理器失败"),NULL);
			break;
		}

		SC_HANDLE hService = OpenService(hSCManager,lpszServiceName,SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS);
		if (hService == NULL)
		{ 
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时打开服务(%s)失败"),lpszServiceName);
			break;
		}

		DWORD dwBytesNeeded,dwBufSize;
		if (QueryServiceConfig(hService,NULL,0,&dwBytesNeeded) == FALSE)
		{
			if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
			{
				dwBufSize = dwBytesNeeded;
				lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED,dwBufSize);
				if (lpQueryServiceConfig == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时动态申请内存失败"),NULL);
					break;
				}
			}
			else
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时查询服务(%s)信息失败"),lpszServiceName);
				break;
			}
		}

		if (QueryServiceConfig(hService,lpQueryServiceConfig,dwBufSize,&dwBytesNeeded) == FALSE) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时查询服务(%s)信息失败"),lpszServiceName);
			break;
		}

		dwBytesNeeded = dwBufSize = 0;
		if (QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,NULL,0,&dwBytesNeeded) == FALSE)
		{
			if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
			{
				dwBufSize = dwBytesNeeded;
				lpServiceStatusProcess = (LPSERVICE_STATUS_PROCESS)LocalAlloc(LMEM_FIXED,dwBufSize);
				if (lpServiceStatusProcess == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务状态信息时动态申请内存失败"),NULL);
					break;
				}
			}
			else
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务状态信息时查询服务(%s)信息失败"),lpszServiceName);
				break;
			}
		}

		if (QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(BYTE*)lpServiceStatusProcess,dwBufSize,&dwBytesNeeded) == FALSE) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务状态信息时查询服务(%s)信息失败"),lpszServiceName);
			break;
		}

		ServiceConfig.dwErrorControl = lpQueryServiceConfig->dwErrorControl;
		ServiceConfig.dwServiceType = lpQueryServiceConfig->dwServiceType;
		ServiceConfig.dwStartType = lpQueryServiceConfig->dwStartType;
		ServiceConfig.dwTagId = lpQueryServiceConfig->dwTagId;
		ServiceConfig.strBinaryPathName = lpQueryServiceConfig->lpBinaryPathName;
		ServiceConfig.strDisplayName = lpQueryServiceConfig->lpDisplayName;
		ServiceConfig.strLoadOrderGroup = lpQueryServiceConfig->lpLoadOrderGroup;
		ServiceConfig.strDependencies = lpQueryServiceConfig->lpDependencies;
		ServiceConfig.strServiceStartName = lpQueryServiceConfig->lpServiceStartName;
		ServiceConfig.dwCurrentState = lpServiceStatusProcess->dwCurrentState;
		ServiceConfig.dwPID = lpServiceStatusProcess->dwProcessId;

		bSuccess = TRUE;
	}
	while(0);

	if (hService)
		CloseServiceHandle(hService);

	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (lpQueryServiceConfig)
		LocalFree(lpQueryServiceConfig);

	if (lpServiceStatusProcess)
		LocalFree(lpServiceStatusProcess);

	return bSuccess;
}


BOOL GetServiceDependencies(LPCTSTR lpszServiceName , CStdArray& strSrvDependencies)
{
	SC_HANDLE hSCManager = 0;
	SC_HANDLE hService = 0;
	LPQUERY_SERVICE_CONFIG lpQueryServiceConfig = NULL;
	LPSERVICE_STATUS_PROCESS lpServiceStatusProcess = NULL;
	BOOL bSuccess = FALSE;

	do
	{
		hSCManager = OpenSCManager(NULL,NULL,SERVICE_QUERY_CONFIG);
		if (hSCManager == NULL) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时打开服务控制管理器失败"),NULL);
			break;
		}

		hService = OpenService(hSCManager,lpszServiceName,SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS);
		if (hService == NULL)
		{ 
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时打开服务(%s)失败"),lpszServiceName);
			break;
		}

		DWORD dwBytesNeeded,dwBufSize;
		if (QueryServiceConfig(hService,NULL,0,&dwBytesNeeded) == FALSE)
		{
			if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
			{
				dwBufSize = dwBytesNeeded;
				lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED,dwBufSize);
				if (lpQueryServiceConfig == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时动态申请内存失败"),NULL);
					break;
				}
			}
			else
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时查询服务(%s)信息失败"),lpszServiceName);
				break;
			}
		}

		if (QueryServiceConfig(hService,lpQueryServiceConfig,dwBufSize,&dwBytesNeeded) == FALSE) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时查询服务(%s)信息失败"),lpszServiceName);
			break;
		}

		AnalyseDbNullTerminatedText(lpQueryServiceConfig->lpDependencies,strSrvDependencies);

		bSuccess = TRUE;
	}
	while(0);

	if (hService)
		CloseServiceHandle(hService);

	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (lpQueryServiceConfig)
		LocalFree(lpQueryServiceConfig);

	if (lpServiceStatusProcess)
		LocalFree(lpServiceStatusProcess);

	return bSuccess;
}


BOOL SetServiceStartTypeConfig(LPCTSTR lpszServiceName,DWORD dwStartType)
{
	SC_HANDLE hSCManager = 0;
	SC_HANDLE hService = 0;
	LPQUERY_SERVICE_CONFIG lpQueryServiceConfig = NULL;
	BOOL bSuccess = FALSE;

	do
	{
		hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hSCManager == NULL) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时打开服务控制管理器失败"),NULL);
			break;
		}

		SC_HANDLE hService = OpenService(hSCManager,lpszServiceName,SERVICE_ALL_ACCESS);
		if (hService == NULL)
		{ 
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时打开服务(%s)失败"),lpszServiceName);
			break;
		}

		DWORD dwBytesNeeded,dwBufSize;
		if (QueryServiceConfig(hService,NULL,0,&dwBytesNeeded) == FALSE)
		{
			if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
			{
				dwBufSize = dwBytesNeeded;
				lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED,dwBufSize);
				if (lpQueryServiceConfig == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时动态申请内存失败"),NULL);
					break;
				}
			}
			else
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时查询服务(%s)信息失败"),lpszServiceName);
				break;
			}
		}

		if (QueryServiceConfig(hService,lpQueryServiceConfig,dwBufSize,&dwBytesNeeded) == FALSE) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取服务配置信息时查询服务(%s)信息失败"),lpszServiceName);
			break;
		}

		if(ChangeServiceConfig(hService,SERVICE_NO_CHANGE,dwStartType,SERVICE_NO_CHANGE,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("设置服务(%s)启动类型配置信息时败"),lpszServiceName);
			CString str = GetThreadErrorInfoString();
			break;
		}

		bSuccess = TRUE;
	}
	while(0);

	if (hService)
		CloseServiceHandle(hService);

	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (lpQueryServiceConfig)
		LocalFree(lpQueryServiceConfig);

	return bSuccess;
}

CString GetShareServiceDllPath(LPCTSTR lpszServiceName)
{
	HKEY hKey = 0;
	TCHAR szDllPath[MAX_PATH] = {0};
	DWORD dwErrorCode;
	do 
	{
		CString strKeyPath;
		strKeyPath.Format(_T("SYSTEM\\CurrentControlSet\\Services\\%s\\Parameters"),lpszServiceName);
		dwErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,(LPCTSTR)strKeyPath,0,KEY_READ,&hKey);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwErrorCode,_T("在获取共享进程服务动态链接库路径时，打开注册表失败"));
			break;
		}
		
		TCHAR szEnvirStr[MAX_PATH] = {0};
		DWORD dwBytesReturned = sizeof(szEnvirStr);
		dwErrorCode = RegQueryValueEx(hKey,_T("ServiceDll"),NULL,NULL,(LPBYTE)szEnvirStr,&dwBytesReturned);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,dwErrorCode,_T("在获取共享进程服务动态链接库路径时，读取注册表失败"));
			break;
		}
		
		if (ExpandEnvironmentStrings(szEnvirStr,szDllPath,MAX_PATH) == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("在获取共享进程服务动态链接库路径时，展开环境变量失败"));
			break;
		}
	}while(0);

	if (hKey)
		RegCloseKey(hKey);

	return szDllPath;
}

BOOL SetServiceConfig(LPCTSTR lpszServiceName, DWORD dwServiceType, DWORD dwStartType, DWORD  dwErrorControl,  
									  LPCTSTR lpBinaryPathName, LPCTSTR lpDisplayName, LPCTSTR lpLoadOrderGroup, LPDWORD lpdwTagId, 
									  LPCTSTR lpDependencies, LPCTSTR lpServiceStartName, LPCTSTR lpPassword)
{

	BOOL bRet = FALSE;
	SC_HANDLE hSCManager = 0;
	SC_HANDLE hService = 0;
	do
	{
		hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hSCManager == NULL) 
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("设置服务配置信息时打开服务控制管理器失败"),NULL);
			break;
		}

		SC_HANDLE hService = OpenService(hSCManager,lpszServiceName,SERVICE_CHANGE_CONFIG);
		if (hService == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("设置服务配置信息时打开服务(%s)失败"),lpszServiceName);
			break;
		}

		if(ChangeServiceConfig(hService,dwServiceType,dwStartType,dwErrorControl,lpBinaryPathName,
			lpLoadOrderGroup,lpdwTagId,lpDependencies,lpServiceStartName,lpPassword,lpDisplayName)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("设置服务(%s)配置信息失败"),lpszServiceName);
			break;
		}

		bRet = TRUE;
	}
	while(0);

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
	}


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

BOOL GetServiceFilePathEx(LPCTSTR lpszServiceName,LPTSTR lpszPathFile)
{
	CString strServiceFilePath = GetShareServiceDllPath(lpszServiceName);
	if (strServiceFilePath.IsEmpty() == TRUE)
	{
		TCHAR szServiceFilePath[MAX_PATH];
		if (GetServiceFilePath(lpszServiceName,szServiceFilePath) == TRUE)
		{
			CString strServicePath(szServiceFilePath);
			strServicePath.TrimLeft(_T("\""));

			int nIndex = strServicePath.Find(_T(".exe"));

			if (nIndex == -1)
			{
				nIndex = strServicePath.Find(_T(".EXE"));
			}
			strServiceFilePath = strServicePath.Left(nIndex+4);
		}
	}

	_tcscpy_s(lpszPathFile,MAX_PATH,strServiceFilePath);

	return TRUE;
}