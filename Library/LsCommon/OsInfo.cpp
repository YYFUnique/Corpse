#include "stdafx.h"
#include "OsInfo.h"
#include "RegClass.h"
#include "ErrorInfo.h"
#include "CommonFunc.h"
#include <comutil.h>
#include <Wbemidl.h>

#pragma comment(lib,"Wbemuuid.lib")
#pragma comment(lib,"comsuppw.lib")

BOOL IsWow64Process()
{
	static BOOL bWow64Process = -1;

	if (bWow64Process==TRUE || bWow64Process==FALSE)
		return bWow64Process;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess , PBOOL Wow64Process);
	HMODULE hModule = GetModuleHandle(_T("Kernel32.dll"));
	if (hModule == NULL)
		return FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hModule,"IsWow64Process");

	if (fnIsWow64Process==NULL)
		return FALSE;			//win2000

	bWow64Process=FALSE;
	if (fnIsWow64Process(GetCurrentProcess(),&bWow64Process)==FALSE)
		return FALSE;

	return bWow64Process;
}

CString GetSystemName()
{
	CString strSubKey(_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"));
	CRegClass Reg;
	Reg.OpenKey(strSubKey);
	CString strProductName;
	if (Reg.ReadKey(_T("ProductName"),strProductName) == FALSE)
		return _T("");
	else
		return strProductName;
}

CString GetSystemDirectoryEx(LPCTSTR lpszFileName)
{
	TCHAR szSystemPath[MAX_PATH];
	GetSystemDirectory(szSystemPath,sizeof(szSystemPath));
	if (lpszFileName[0] != NULL)
		PathCombine(szSystemPath,szSystemPath,lpszFileName);

	return szSystemPath;
}

CString GetWindowsDirectoryEx()
{
	TCHAR szWindowsDirectory[MAX_PATH];
	GetSystemWindowsDirectory(szWindowsDirectory,sizeof(szWindowsDirectory));
	return szWindowsDirectory;
}

BOOL OsIsServer()
{
	OSVERSIONINFOEX OsVertionInfo;
	memset(&OsVertionInfo,0,sizeof(OsVertionInfo));
	OsVertionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((LPOSVERSIONINFO)&OsVertionInfo))
		return FALSE;

	if (OsVertionInfo.wProductType==VER_NT_WORKSTATION)
		return FALSE;

	return TRUE;
}

CString GetComputerName()
{
	TCHAR szName[MAX_PATH];
	DWORD dwSize = sizeof(szName);
	if (GetComputerName(szName , &dwSize) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("获取计算机名称失败"));
		return _T("");
	}

	return szName;
}

CString GetUserName()
{
	TCHAR szName[MAX_PATH];
	DWORD dwSize = sizeof(szName);
	if (GetUserName(szName , &dwSize) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("获取当前用户名失败"));
		return _T("");
	}

	return szName;
}

DWORD AfxGetScreenSize()
{
	DWORD dwSize = 0;
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	dwSize = MAKELONG(cy,cx);
	return dwSize;
}

CString GetScreenSize(DWORD dwScreen,LPCTSTR lpszSymbol /*= _T("x")*/)
{
	CString strScreenSize;
	strScreenSize.Format(_T("%d%s%d"),HIWORD(dwScreen),lpszSymbol,LOWORD(dwScreen));

	return strScreenSize;
}

CString GetCpuInfo()
{
	CString strCpuInfo(_T(""));
	CRegClass Reg;
	Reg.OpenKey(_T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"));
	CString strProductName;
	if (Reg.ReadKey(_T("ProcessorNameString"),strCpuInfo) == FALSE)
		return _T("");

	DWORD dwProcessorNumber = GetCurrentProcessorNumber();
	if (dwProcessorNumber>1)
		strCpuInfo.AppendFormat(_T("(%d核)"),dwProcessorNumber);
	
	return strCpuInfo;
}

BOOL AfxGetTotalMem(PDWORD pdwTotalMem , BOOL bGetInstall/* = FALSE*/ , PDWORD pdwInstallMem /* = 0*/)
{
	MEMORYSTATUSEX MemStatus;
	memset(&MemStatus,0,sizeof(MEMORYSTATUSEX));
	MemStatus.dwLength=sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&MemStatus);
	*pdwTotalMem = (DWORD)(MemStatus.ullTotalPhys/(1024*1024));

	return TRUE;
}

DWORD AfxGetTotalMem()
{
	DWORD dwTotalMem = 0;
	AfxGetTotalMem(&dwTotalMem);

	return dwTotalMem;
}

CAMERA_INFO GetCameraInfo()
{
	return NoCamera;
}

CString GetCameraDescribeInfo(int CameraInfo)
{
	CString strCameraInfo;
	switch(CameraInfo)
	{
		case InstallCamera:
			strCameraInfo = _T("有");
			break;
		case NoCameraDriver:
			strCameraInfo = _T("未安装驱动");
			break;
		default:
			strCameraInfo = _T("无");
	}
	return strCameraInfo;
}

DWORD GetServerPack()
{
	return 1;
}

BOOL RestartOperatSystem()
{
	if (AdjustProcessAuthority(SE_SHUTDOWN_NAME) == FALSE)	//设置进程权限
	{
		SetErrorTitle(_T("设置进程关机权限失败"));
		return FALSE;
	}

	if (ExitWindowsEx(EWX_REBOOT|EWX_FORCE,SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("设置操作系统重启失败"));
		return FALSE;
	}

	return TRUE;
}

BOOL OsIsVistaOrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	memset(&osvi,0,sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	osvi.dwMajorVersion = 6;

	VER_SET_CONDITION(dwlConditionMask,VER_MAJORVERSION,VER_GREATER_EQUAL);
	
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION,dwlConditionMask);
}

//得到wmi 中  Win32_OperatingSystem 表中字段
//输入 wszColumnName 字段名，得到相应字段值 OsType
BOOL GetWin32_OperatingSystemByWMI(LPCTSTR lpszColumnName,CString& OsType)
{
	BOOL bSuccess = FALSE;
	//连接到Server
	IWbemLocator *pLoc = NULL;
	IWbemServices *pSvc = NULL ;
	IEnumWbemClassObject *pEnumerator = NULL ;
	do 
	{
		HRESULT hRes = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator,(LPVOID *) &pLoc);
		if( FAILED(hRes)) 
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:创建实例失败!"));
			break;
		}

		hRes = pLoc->ConnectServer(_bstr_t("root\\cimv2"),NULL,NULL,0,NULL,0,0,&pSvc);
		if( FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:连接到Server失败!"));
			break;
		}

		//设置WMI连接的安全等级。
		hRes = CoSetProxyBlanket(pSvc,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,NULL,RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);
		if(FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:设置WMI连接的安全等级失败!"));
			break;
		}

		//BSTR bszWQL = SysAllocString ( L"WQL" );
		//BSTR bszQuery = SysAllocString (L"Select * from Win32_OperatingSystem");
		BSTR bszWQL = (bstr_t)(L"WQL");
		BSTR bszQuery = (bstr_t)(L"Select * From Win32_OperatingSystem");
		if((bszWQL==NULL) || (bszQuery==NULL))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:为查询变量分配内存失败!"));
			break;
		}

		hRes = pSvc->ExecQuery(bszWQL, bszQuery,WBEM_RETURN_IMMEDIATELY, NULL, &pEnumerator);

		if (FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:查询操作系统版本失败!"));
			break;
		}

		ULONG uReturn = 0;
		IWbemClassObject * m_pWbemClass[1] = {NULL};
		hRes = pEnumerator->Next( WBEM_INFINITE, 1, m_pWbemClass, &uReturn);
		if(FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:获取操作系统版本信息失败!"));
			break;
		}

		VARIANT Caption;
		CStringW strColumnName(lpszColumnName);
		hRes = m_pWbemClass[0]->Get(strColumnName,0, &Caption, 0, 0);
		if (FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取操作系统类型失败:查询操作系统版本描述信息失败!"));
			break;
		}
		
		OsType = (LPCTSTR)Caption.pbstrVal;
		bSuccess = TRUE;
	} while (FALSE);
	
	if (pEnumerator != NULL)
		pEnumerator->Release();

	if (pSvc != NULL)
		pSvc->Release();

	if (pLoc != NULL)
		pLoc->Release();

	CoUninitialize();
	return bSuccess;
}

LS_COMMON_API BOOL GetLogicalDriveStrings(CStringArray& strDriverStrings)
{
	TCHAR szLogicalDriveString[MAX_PATH*4];
	if (GetLogicalDriveStrings(MAX_PATH*4,szLogicalDriveString) == 0)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("获取可用磁盘失败"));
		return FALSE;
	}

	LPCTSTR lpszLogicalString = szLogicalDriveString;
	while(lpszLogicalString != NULL)
	{
		strDriverStrings.Add(lpszLogicalString);
		lpszLogicalString += _tcslen(lpszLogicalString) + 1;
	}
	return TRUE;
}