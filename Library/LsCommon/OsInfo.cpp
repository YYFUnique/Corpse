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
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("��ȡ���������ʧ��"));
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
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("��ȡ��ǰ�û���ʧ��"));
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
		strCpuInfo.AppendFormat(_T("(%d��)"),dwProcessorNumber);
	
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
			strCameraInfo = _T("��");
			break;
		case NoCameraDriver:
			strCameraInfo = _T("δ��װ����");
			break;
		default:
			strCameraInfo = _T("��");
	}
	return strCameraInfo;
}

DWORD GetServerPack()
{
	return 1;
}

BOOL RestartOperatSystem()
{
	if (AdjustProcessAuthority(SE_SHUTDOWN_NAME) == FALSE)	//���ý���Ȩ��
	{
		SetErrorTitle(_T("���ý��̹ػ�Ȩ��ʧ��"));
		return FALSE;
	}

	if (ExitWindowsEx(EWX_REBOOT|EWX_FORCE,SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("���ò���ϵͳ����ʧ��"));
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

//�õ�wmi ��  Win32_OperatingSystem �����ֶ�
//���� wszColumnName �ֶ������õ���Ӧ�ֶ�ֵ OsType
BOOL GetWin32_OperatingSystemByWMI(LPCTSTR lpszColumnName,CString& OsType)
{
	BOOL bSuccess = FALSE;
	//���ӵ�Server
	IWbemLocator *pLoc = NULL;
	IWbemServices *pSvc = NULL ;
	IEnumWbemClassObject *pEnumerator = NULL ;
	do 
	{
		HRESULT hRes = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator,(LPVOID *) &pLoc);
		if( FAILED(hRes)) 
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:����ʵ��ʧ��!"));
			break;
		}

		hRes = pLoc->ConnectServer(_bstr_t("root\\cimv2"),NULL,NULL,0,NULL,0,0,&pSvc);
		if( FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:���ӵ�Serverʧ��!"));
			break;
		}

		//����WMI���ӵİ�ȫ�ȼ���
		hRes = CoSetProxyBlanket(pSvc,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,NULL,RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);
		if(FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:����WMI���ӵİ�ȫ�ȼ�ʧ��!"));
			break;
		}

		//BSTR bszWQL = SysAllocString ( L"WQL" );
		//BSTR bszQuery = SysAllocString (L"Select * from Win32_OperatingSystem");
		BSTR bszWQL = (bstr_t)(L"WQL");
		BSTR bszQuery = (bstr_t)(L"Select * From Win32_OperatingSystem");
		if((bszWQL==NULL) || (bszQuery==NULL))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:Ϊ��ѯ���������ڴ�ʧ��!"));
			break;
		}

		hRes = pSvc->ExecQuery(bszWQL, bszQuery,WBEM_RETURN_IMMEDIATELY, NULL, &pEnumerator);

		if (FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:��ѯ����ϵͳ�汾ʧ��!"));
			break;
		}

		ULONG uReturn = 0;
		IWbemClassObject * m_pWbemClass[1] = {NULL};
		hRes = pEnumerator->Next( WBEM_INFINITE, 1, m_pWbemClass, &uReturn);
		if(FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:��ȡ����ϵͳ�汾��Ϣʧ��!"));
			break;
		}

		VARIANT Caption;
		CStringW strColumnName(lpszColumnName);
		hRes = m_pWbemClass[0]->Get(strColumnName,0, &Caption, 0, 0);
		if (FAILED(hRes))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("��ȡ����ϵͳ����ʧ��:��ѯ����ϵͳ�汾������Ϣʧ��!"));
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
		SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ���ô���ʧ��"));
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