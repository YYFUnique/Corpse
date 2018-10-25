#include "StdAfx.h"
#include "FileSystemFilter.h"
#include <fltUser.h>
#include <DllCore/Utils/OsInfo.h>
#include <DllCore/Utils/ErrorInfo.h>
#include <DllCore/Utils/Registry.h>
#include <DllCore/Utils/ServiceControl.h>
#include <DllCore/Log/LogHelper.h>

#ifdef _WIN64
#pragma comment(lib,"fltlib(x64).lib")
#else
#pragma comment(lib,"fltlib.lib")
#endif

#define		FS_FILTER_NAME								_T("MzfFileMon")
#define		FS_FILTER_DISPLAY_NAME					FS_FILTER_NAME
#define		FS_FILTER_DEFAULT_INSTANCE			_T("MzfFileFilter")
#define		FS_FILTER_LOAD_GROUP					_T("FileSystemFilter")
#define		FS_FILTER_COMMUNICATE_PORT		L"\\FsFilterPort"

CFileSystemFilter::CFileSystemFilter()
{

}

CFileSystemFilter::~CFileSystemFilter()
{

}

BOOL CFileSystemFilter::OpenDrv()
{
	if (IsOpen() == TRUE)
		return TRUE;

	if (OsIsWinXpOrLater() == FALSE)
	{
		SetErrorInfo(CUSTOM_ERROR, 0, _T("打开文件访问控制驱动失败:当前系统不支持."));
		return FALSE;
	}

	HRESULT hRet = FilterConnectCommunicationPort(FS_FILTER_COMMUNICATE_PORT, 0, NULL, 0, NULL, &m_hDrv);
	if (FAILED(hRet))
	{
		SetErrorInfo(COM_ERROR, hRet, _T("打开文件访问控制驱动失败"));
		return FALSE;
	}

	return TRUE;
}

LPCTSTR CFileSystemFilter::GetDrvFileName()
{
	return OsIsWow64Process() ? _T("MzfFileMon64.sys") : _T("MzfFileMon.sys");
}

BOOL CFileSystemFilter::StopDrvService()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (StopServiceByName(FS_FILTER_NAME) == FALSE)
			break;
		// 设置文件过滤驱动手动启动
		if (SetServiceStartTypeConfig(FS_FILTER_NAME, SERVICE_DEMAND_START) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}

BOOL CFileSystemFilter::DelDrvService()
{
	StopServiceByName(FS_FILTER_NAME);
	return DelServiceByName(FS_FILTER_NAME);
}

BOOL CFileSystemFilter::StartDrvService()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (ServiceIsExist(FS_FILTER_NAME))
		{
			LOG_PRINT(_T("VerifyFsFilterSvcInfo"));
			if (VerifyFsFilterSvcInfo() == FALSE)
				break;
		}
		else
		{
			LOG_PRINT(_T("CreateFsFilterSvc"));
			if (CreateFsFilterSvc() == FALSE)
				break;
		}

		LOG_PRINT(_T("InstallMiniFilter"));
		if (InstallMiniFilter() == FALSE)
			break;

		if (StartServiceByName(FS_FILTER_NAME) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CFileSystemFilter::DeviceIoControl(LPVOID lpInBuffer , DWORD nInBufferSize , LPVOID lpOutBuffer /* = NULL  */, DWORD nOutBufferSize /* = 0  */, LPDWORD lpBytesReturned /* = NULL */)
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (IsOpen() == FALSE)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("控制服务驱动未打开，请先打开驱动"));
			break;
		}

		DWORD dwByteReturn;
		HRESULT hRet = FilterSendMessage(m_hDrv,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize,&dwByteReturn);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR, hRet, _T("设置信息到驱动失败"));
			break;
		}
		else if (lpBytesReturned)
			*lpBytesReturned = dwByteReturn;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

//创建文件过滤驱动服务
BOOL CFileSystemFilter::CreateFsFilterSvc()
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCMag = NULL;
	SC_HANDLE hService = NULL;

	do 
	{
		if (OsIsWinXpOrLater() == FALSE)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("打开文件访问控制驱动失败:当前系统不支持."));
			return FALSE;
		}

		hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (hSCMag == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("安装文件过滤驱动时，打开服务控制管理器失败"));
			break;
		}

		LOG_PRINT(_T("CreateService"));
		hService = CreateService(hSCMag,
													FS_FILTER_NAME,
													FS_FILTER_DISPLAY_NAME,
													SERVICE_ALL_ACCESS,
													SERVICE_FILE_SYSTEM_DRIVER,
													SERVICE_SYSTEM_START,
													SERVICE_ERROR_NORMAL,
													GetDrvFilePath(), FS_FILTER_LOAD_GROUP,
													NULL, NULL, NULL, NULL);
		if (hService == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("安装文件过滤驱动时，创建服务失败"));
			break;
		}

		LOG_PRINT(_T("bRet = TRUE;"));
		bRet = TRUE;
	} while (0);

	if (hService)
		CloseServiceHandle(hService);

	if (hSCMag)
		CloseServiceHandle(hSCMag);

	return bRet;
}

BOOL CFileSystemFilter::VerifyFsFilterSvcInfo()
{
	BOOL bSuccess = FALSE;
	do 
	{
		LS_SERVICE_CONFIG ServiceConfig;
		if (GetServiceConfig(FS_FILTER_NAME, ServiceConfig) == FALSE)
			break;

		if (ServiceConfig.dwServiceType == SERVICE_FILE_SYSTEM_DRIVER && 
			ServiceConfig.dwStartType == SERVICE_SYSTEM_START && 
			ServiceConfig.dwErrorControl == SERVICE_ERROR_NORMAL &&
			ServiceConfig.strBinaryPathName == GetDrvFilePath() &&
			ServiceConfig.strDisplayName == FS_FILTER_DISPLAY_NAME &&
			ServiceConfig.strLoadOrderGroup.CompareNoCase(FS_FILTER_LOAD_GROUP) == 0)
		{
			bSuccess = TRUE;
			break;
		}

		bSuccess = SetServiceConfig(FS_FILTER_NAME , SERVICE_FILE_SYSTEM_DRIVER , SERVICE_SYSTEM_START ,
													SERVICE_ERROR_NORMAL , GetDrvFilePath() , FS_FILTER_DISPLAY_NAME , 
													FS_FILTER_LOAD_GROUP);

	} while (FALSE);

	return bSuccess;
}

BOOL CFileSystemFilter::InstallMiniFilter()
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{
		CString strFsFltFilterPath;
		strFsFltFilterPath.Format(_T("SYSTEM\\CurrentControlSet\\services\\%s\\Instances"), FS_FILTER_NAME);

		if (RegCreateKey(HKEY_LOCAL_MACHINE, strFsFltFilterPath, &hKey) != ERROR_SUCCESS)
		{
			SetErrorTitle(_T("安装文件过滤驱动，打开注册表键值失败"));
			break;
		}

		if (LsRegSetValue(hKey, _T("DefaultInstance"), FS_FILTER_DEFAULT_INSTANCE) == FALSE)
		{
			SetErrorTitle(_T("安装文件过滤驱动，设置注册表键值失败"));
			break;
		}

		RegCloseKey(hKey);
		hKey = NULL;

		TCHAR szRegPath[MAX_PATH];
		_stprintf_s(szRegPath,_countof(szRegPath),_T("SYSTEM\\CurrentControlSet\\services\\%s\\Instances\\%s"), FS_FILTER_NAME, FS_FILTER_DEFAULT_INSTANCE);

		if (RegCreateKey(HKEY_LOCAL_MACHINE, szRegPath, &hKey) != ERROR_SUCCESS)
		{
			SetErrorTitle(_T("安装文件过滤驱动，打开注册表键值失败"));
			break;
		}

		LPCTSTR lpszAltitude = _T("385285");
		DWORD dwFlag = 0;
		if ((LsRegSetValue(hKey, _T("Altitude"), lpszAltitude) && LsRegSetValue(hKey, _T("Flags"), dwFlag)) == FALSE)
		{
			SetErrorTitle(_T("安装文件过滤驱动，设置注册表键值失败"));
			break;
		}

		//安全模式键值
		_stprintf_s(szRegPath, _countof(szRegPath), _T("SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Minimal\\%s"), GetDrvFileName());
		SHSetValue(HKEY_LOCAL_MACHINE , szRegPath , _T(""), REG_SZ , _T("Driver") , 7 * sizeof(TCHAR));

		_stprintf_s(szRegPath, _countof(szRegPath), _T("SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Network\\%s"), GetDrvFileName());
		SHSetValue(HKEY_LOCAL_MACHINE , szRegPath , _T(""), REG_SZ , _T("Driver") , 7 * sizeof(TCHAR));

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

HRESULT CFileSystemFilter::UnloadFilterNotMandatory(LPCTSTR lpszFilterName)
{
	CStringW strFilterName(lpszFilterName);

	return FilterUnload(strFilterName);
}