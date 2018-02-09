#include "StdAfx.h"
#include "Service.h"
#include "DebugHelper.h"
#include "DllCore/Utils/ErrorInfo.h"

//������ע����е�·��
#define SRV_REG_PATH						_T("SYSTEM\\CurrentControlSet\\Services\\Deamon")
//SvcHost��������ע����ֵ
#define SVCHOST_REG_PATH			_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost")
//��������������
#define SRV_BINARY							_T("%SystemRoot%\\System32\\svchost.exe -k Deamon")

#define SRV_NAME							_T("Deamon")

BOOL InstallService(LPCSTR lpszSrvName)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	HKEY hKeyName = NULL;
	SC_HANDLE hScm = NULL, schService = NULL;

	CString strSrvName = lpszSrvName;

	DWORD dwRet = RegOpenKey(HKEY_LOCAL_MACHINE, SVCHOST_REG_PATH, &hKey);
	dwRet = RegSetValueEx(hKey, SRV_NAME, 0, REG_MULTI_SZ, (BYTE*)(LPCTSTR)strSrvName, _tcslen(strSrvName)*sizeof(TCHAR));
	RegCloseKey(hKey);
	hKey = NULL;

	do 
	{
		//��װ����
		hScm = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hScm == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("OpenSCManager ��ʧ��.\r\n"));
			break;
		}

		schService = CreateService(hScm, strSrvName, NULL, 
														SERVICE_ALL_ACCESS & ~SC_MANAGER_MODIFY_BOOT_CONFIG,
														SERVICE_WIN32_SHARE_PROCESS,
														SERVICE_AUTO_START,     //starttype
														SERVICE_ERROR_NORMAL,     //errorcontroltype
														SRV_BINARY,     //service'sbinary
														NULL,               //noloadorderinggroup
														NULL,               //notagidentifier
														NULL,               //nodependencies
														NULL,               //LocalSystemaccount
														NULL);             //nopassword
		if (schService == NULL)
		{
			if (GetLastError() == ERROR_SERVICE_EXISTS)
			{
				schService = OpenService(hScm, strSrvName, SERVICE_START | SERVICE_QUERY_CONFIG | SERVICE_CHANGE_CONFIG);
				if (schService == NULL)
					schService = OpenService(hScm, strSrvName, SERVICE_START);
			}

			if (schService == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR, 0, _T("CreateService ����[%s]ʧ��.\r\n"), strSrvName);
				break;
			}

			DWORD dwSize = 0;
			QueryServiceConfig(schService, NULL, 0, &dwSize);
			if (dwSize != 0)
			{
				QUERY_SERVICE_CONFIG* pSrvConfig = (QUERY_SERVICE_CONFIG*) new BYTE[dwSize];
				if (pSrvConfig != NULL)
				{
					if (QueryServiceConfig(schService, pSrvConfig, dwSize, &dwSize))
					{
						if (pSrvConfig->dwStartType != SERVICE_AUTO_START)
							ChangeServiceConfig(schService, SERVICE_NO_CHANGE,SERVICE_AUTO_START,SERVICE_NO_CHANGE,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					}
					delete[] pSrvConfig;
				}
			}
		}

		dwRet = RegOpenKey(HKEY_LOCAL_MACHINE, SRV_REG_PATH, &hKey);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("�򿪷����ֵ·��ʧ��"));
			break;
		}

		dwRet = RegCreateKey(hKey, _T("Parameters"), &hKeyName);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("���������ֵ[Parameters]ʧ��"));
			break;
		}

		TCHAR szFilePath[] = _T("%SystemRoot%\\System32\\Deamon.dll");
		dwRet = RegSetValueEx(hKeyName, _T("ServiceDll"), 0, REG_EXPAND_SZ, (BYTE*)szFilePath, sizeof(szFilePath));
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("���÷������[ServiceDll]ʧ��"));
			break;
		}

		if (StartService(schService, 0, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��������[%s]ʧ��"), strSrvName);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		LOG_PRINT(DBG_TRACE_FLAG_ERROR, GetThreadErrorInfoString());

	if (hKeyName != NULL)
	{
		RegCloseKey(hKeyName);
		hKeyName = NULL;
	}
	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}
	if (schService != NULL)
	{
		CloseServiceHandle(schService);
		schService = NULL;
	}
	if (hScm != NULL)
	{
		CloseServiceHandle(hScm);
		hScm = NULL;
	}

	return bSuccess;
}

BOOL UninstallService(LPCSTR lpszSrvName)
{
	SC_HANDLE hScm = NULL;
	SC_HANDLE schService;

	BOOL bSuccess = FALSE;

	do 
	{
		hScm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (hScm == NULL)
			break;

		CString strSrvName = lpszSrvName;
		schService = OpenService(hScm, strSrvName, DELETE);
		if (schService == NULL)
			break;

		if (DeleteService(schService) == FALSE)
			break;

		bSuccess = TRUE;

	} while (FALSE);

	if (schService != NULL)
	{
		CloseServiceHandle(schService);
		schService = NULL;
	}
	if (hScm != NULL)
	{
		CloseServiceHandle(hScm);
		hScm = NULL;
	}

	return bSuccess;
}