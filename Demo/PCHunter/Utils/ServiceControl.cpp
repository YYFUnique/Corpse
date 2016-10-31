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
			SetErrorInfo(SYSTEM_ERROR,0,_T("��������%sʱ�򿪷�����ƹ�����ʧ��"),lpServiceName);
			break;
		}

		SCService=OpenService(SCManager,lpServiceName,SERVICE_QUERY_STATUS | SERVICE_START);
		if (SCService == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��������%sʱ�򿪷���ʧ��"),lpServiceName);
			break;
		}

		SERVICE_STATUS ServiceStatus;
		if (QueryServiceStatus(SCService,&ServiceStatus) == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��������%sʱ��ȡ����״̬ʧ��"),lpServiceName);
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
					SetErrorInfo(SYSTEM_ERROR,0,_T("��������%sʧ��"),lpServiceName);
					break;
				}

				if (bWait == FALSE)
				{
					bRet = TRUE;
					break;
				}

				//Ԥ�����ô�����Ϣ
				SetErrorInfo(CUSTOM_ERROR , 0 , _T("��������%sʧ��:������ʱ��") , lpServiceName);

				SERVICE_STATUS ServiceStatus;
				for (int i=0;i<50;i++)
				{
					if (QueryServiceStatus(SCService,&ServiceStatus) == 0)
					{
						SetErrorInfo(SYSTEM_ERROR,0,_T("��������%sʱ��ȡ����״̬ʧ��"),lpServiceName);
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
			SetErrorInfo(SYSTEM_ERROR,0,_T("��������ʱ�򿪷��������ʧ��"));
			break;
		}

		hSCSvr = OpenService(hSCMag, lpServiceName, SERVICE_QUERY_STATUS | SERVICE_START| SERVICE_STOP);
		if (hSCSvr == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��������ʱ�򿪷���%sʧ��"),lpServiceName);
			break;
		}

		SERVICE_STATUS  ssStatus;

		BOOL bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
		if (bSuccess == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��������ʱ��ѯ����%s״̬ʧ��"),lpServiceName);
			break;
		}

		//�������ֹͣ�к���ֹͣ��ֹͣ����
		DWORD dwStartTime = GetTickCount();
		if (ssStatus.dwCurrentState != SERVICE_STOP_PENDING && ssStatus.dwCurrentState != SERVICE_STOPPED)
		{
			bSuccess = ControlService(hSCSvr,SERVICE_CONTROL_STOP,&ssStatus);
			if (bSuccess == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR , 0 , _T("��������ʱֹͣ����%sʧ��"),lpServiceName);
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
				SetErrorInfo(CUSTOM_ERROR,0,_T("��������ȴ�����%s�ر�ʱ���ȴ�ʱ�䳬����Ԥ��ʱ��"),lpServiceName);
				break;
			}
			bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
			if (bSuccess == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("���������ѯ����%s״̬ʱʧ��"),lpServiceName);
				break;
			}
		} while (TRUE);


		if (StartService(hSCSvr,NULL,NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("��������ʱ��������%sʧ��"),lpServiceName);
			break;
		}

		if (dwTimeout != INFINITE)
		{
			do 
			{
				bSuccess = QueryServiceStatus(hSCSvr, &ssStatus);
				if (bSuccess == FALSE)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("���������ѯ����%s״̬ʱʧ��"),lpServiceName);
					break;
				}

				if (ssStatus.dwCurrentState == SERVICE_RUNNING)
				{
					bRet = TRUE;
					break;
				}
				Sleep(50);
				if (dwTimeout !=INFINITE && GetTickCount()-dwStartTime>dwTimeout + dwWaitTime)	//����ȴ�ʱ���ѳ���Ԥ��ʱ�����˳�
				{
					SetErrorInfo(CUSTOM_ERROR,0,_T("��������ȴ�����%s����ʱ���ȴ�ʱ�䳬����Ԥ��ʱ��"),lpServiceName);
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
			SetErrorInfo(SYSTEM_ERROR,0,_T("�鿴����ʱ�Ƿ����ʱ�򿪷�����ƹ�����ʧ��"));
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
				SetErrorInfo(SYSTEM_ERROR , dwErrorCode , _T("��ȡ����������Ϣʧ��"));
				break;
			}

			LPQUERY_SERVICE_CONFIG lpServiceConfig = (LPQUERY_SERVICE_CONFIG) new BYTE[dwBytesNeeded];
			if (lpServiceConfig == NULL)
			{
				SetErrorInfo(CUSTOM_ERROR , 0 , _T("��ȡ����������Ϣʧ��:�����ڴ�(%u�ֽ�)ʧ��.") , dwBytesNeeded);
				break;
			}

			if (QueryServiceConfig(schService, lpServiceConfig, dwBytesNeeded, &dwBytesNeeded) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ȡ����������Ϣʧ��"));
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
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ�������·��ʱ�򿪷�����ƹ�����ʧ��"));
			break;
		}

		hService = OpenService(hSCMag,lpszServiceName,GENERIC_READ);
		if (hService == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ�������·��ʱ��ȡָ��������ʧ��"));
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
					SetErrorInfo(SYSTEM_ERROR,0,_T("�ر�%s����ʱ��ȡ����״̬ʧ��"),lpServiceName);
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
					SetErrorInfo(SYSTEM_ERROR,0,_T("ֹͣ����%sʧ��"),lpServiceName);
					break;
				}

				if (bWait)
				{
					bRet = FALSE;
					SetErrorInfo(CUSTOM_ERROR , 0 , _T("ֹͣ����%sʧ��:ֹͣ��ʱ��") , lpServiceName);

					SERVICE_STATUS ServiceStatus;
					for (int i=0;i<50;i++)
					{
						if (QueryServiceStatus(hSCService,&ServiceStatus) == 0)
						{
							SetErrorInfo(SYSTEM_ERROR,0,_T("ֹͣ����%sʱ��ȡ����״̬ʧ��"),lpServiceName);
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
					SetErrorInfo(SYSTEM_ERROR,0,_T("�ر�%s����ʱ��ȡ����״̬ʧ��"),lpServiceName);
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
					SetErrorInfo(SYSTEM_ERROR,0,_T("ֹͣ����%sʧ��"),lpServiceName);
					break;
				}

				if (bWait)
				{
					bRet = FALSE;
					SetErrorInfo(CUSTOM_ERROR , 0 , _T("ֹͣ����%sʧ��:ֹͣ��ʱ��") , lpServiceName);

					SERVICE_STATUS ServiceStatus;
					for (int i=0;i<50;i++)
					{
						if (QueryServiceStatus(hSCService,&ServiceStatus) == 0)
						{
							SetErrorInfo(SYSTEM_ERROR,0,_T("ֹͣ����%sʱ��ȡ����״̬ʧ��"),lpServiceName);
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