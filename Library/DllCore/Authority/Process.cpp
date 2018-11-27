#include "StdAfx.h"
#include "Process.h"
#include "ProcessInfo.h"
#include "NtDll.h"
#include <Psapi.h>
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Utils/OsInfo.h"
#include "DllCore/Utils/FileTools.h"

#include <Aclapi.h>
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Psapi.lib")

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);

BOOL GetProcessUserName(DWORD dwProcessId, CString& strProcessName)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	PTOKEN_USER pTokenUser=NULL; 
	do 
	{
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
		if (hProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开进程%d句柄失败"),dwProcessId);
			break;
		}

		if (OpenProcessToken(hProcess,TOKEN_QUERY,&hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程%dToken查询权限失败"),dwProcessId);
			break;
		}

		TCHAR szUserName[256]={0}; 
		TCHAR szDomain[256]={0}; 
		DWORD dwDomainSize=256; 
		DWORD dwNameSize=256; 
		DWORD dwSize = 0;

		SID_NAME_USE    SNU; 

		BOOL bRet = GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize);
		if (bRet == FALSE)
		{
			pTokenUser = (PTOKEN_USER)new BYTE[dwSize];
			if (pTokenUser == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程Token信息分配内存失败"));
				break;
			}

			bRet = GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize);
		}

		if (bRet != TRUE)
			break;

		if (LookupAccountSid(NULL,pTokenUser->User.Sid,szUserName,&dwNameSize,szDomain,&dwDomainSize,&SNU) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程%d用户名失败"),dwProcessId);
			break;
		}
		strProcessName = szUserName;

		bSuccess = TRUE;
	} while (FALSE);

	if (pTokenUser != NULL)
	{	
		delete[] pTokenUser;
		pTokenUser = NULL;
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	if (hProcess != NULL)
		CloseHandle(hProcess);
	return bSuccess;
}

BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine)
{
	PROCNTQSIP NtQueryInformationProcess;
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	do 
	{
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if (hProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开进程%d句柄失败"),dwPid);
			break;
		}

		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
																GetModuleHandle(_T("ntdll")),
																"NtQueryInformationProcess"
																);

		status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, (PVOID)&pbi,
																	sizeof(PROCESS_BASIC_INFORMATION), NULL);

		if (status)
			break;

		if (ReadProcessMemory(hProcess, pbi.PebBaseAddress,&Peb, sizeof(PEB), &dwDummy) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 ,_T("读取进程PEB基地址失败"));
			break;
		}

		if (ReadProcessMemory(hProcess, Peb.ProcessParameters,&ProcParam, sizeof(PROCESS_PARAMETERS),&dwDummy) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("获取进程启动参数失败"));
			break;
		}

		lpAddress = ProcParam.CommandLine.Buffer;
		dwSize = ProcParam.CommandLine.Length;

		BYTE* bCmdLine = new BYTE[dwSize];
		if (bCmdLine == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程的CMD命令行失败，分配内存失败"));
			break;
		}

		BOOL 	bSuccess = FALSE;
		do 
		{
			bSuccess = ReadProcessMemory(hProcess, lpAddress, bCmdLine, dwSize, &dwDummy);
			if (bSuccess == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR, 0, _T("获取进程CMD命令行失败"));
				break;
			}

			CStringW strCmdLineW((LPWSTR)bCmdLine,dwSize/sizeof(TCHAR));
			strCmdLine = strCmdLineW;

		} while (FALSE);

		delete[] bCmdLine;

		bRet = TRUE;
	} while (FALSE);

	if (hProcess != NULL)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	return bRet;
}

BOOL IsRunAsAdmin()
{
	CProcessInfo ProcessRunAsAdmin;
	DWORD dwProcessId = GetCurrentProcessId();
	if (ProcessRunAsAdmin.OpenProcess(dwProcessId) == FALSE)
		return FALSE;

	return ProcessRunAsAdmin.IsProcessRunAsAdmin();
}

BOOL IsRunAsSystem()
{
	CProcessInfo ProcessRunAsSystem;
	DWORD dwProcessId = GetCurrentProcessId();
	if (ProcessRunAsSystem.OpenProcess(dwProcessId) == FALSE)
		return FALSE;

	return ProcessRunAsSystem.IsProcessRunAsSystem();
}

//BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject)
//{
//	DWORD dwLengthNeeded=0;PSECURITY_DESCRIPTOR pSecurityInfo=0;
//	GetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION,0,0,&dwLengthNeeded);
//	if (dwLengthNeeded)
//	{
//		pSecurityInfo=(PSECURITY_DESCRIPTOR)new BYTE[dwLengthNeeded];
//		GetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION,pSecurityInfo,dwLengthNeeded,&dwLengthNeeded);
//
//		BOOL bDaclPresent,bDaclDefaulted;PACL pDacl;
//		GetSecurityDescriptorDacl(pSecurityInfo,&bDaclPresent,&pDacl,&bDaclDefaulted);
//
//		TCHAR szUserName[MAX_PATH];DWORD dwBufferSize=MAX_PATH;
//		GetUserName(szUserName,&dwBufferSize);
//
//		EXPLICIT_ACCESS ea;
//		BuildExplicitAccessWithName(&ea,szUserName,TOKEN_ALL_ACCESS,GRANT_ACCESS,FALSE);
//
//		PACL pNewDacl;
//		SetEntriesInAcl(1,&ea,pDacl,&pNewDacl);
//		LocalFree(pDacl);
//
//		DWORD dwAbsoluteSDSize=0,dwAbsDaclSize=0,dwSaclSize=0,dwOwnerSize=0,dwPrimaryGroupSize=0;
//		MakeAbsoluteSD(pSecurityInfo,0,&dwAbsoluteSDSize,0,&dwAbsDaclSize,0,&dwSaclSize,0,&dwOwnerSize,0,&dwPrimaryGroupSize);
//
//		PSECURITY_DESCRIPTOR pAbsoluteSD=0;
//		PACL pAbsDacl=0,pSacl=0;PSID pOwner=0,pPrimaryGroup=0;
//		if (dwAbsoluteSDSize)
//			pAbsoluteSD=LocalAlloc(LPTR,dwAbsoluteSDSize);
//		if (dwAbsDaclSize)
//			pAbsDacl=(PACL)LocalAlloc(LPTR,dwAbsDaclSize);
//		if (dwSaclSize)
//			pSacl=(PACL)LocalAlloc(LPTR,dwSaclSize);
//		if (dwOwnerSize)
//			pOwner=(PSID)LocalAlloc(LPTR,dwOwnerSize);
//		if (dwPrimaryGroupSize)
//			pPrimaryGroup=(PSID)LocalAlloc(LPTR,dwPrimaryGroupSize);
//		MakeAbsoluteSD(pSecurityInfo,pAbsoluteSD,&dwAbsoluteSDSize,pAbsDacl,&dwAbsDaclSize,pSacl,&dwSaclSize,pOwner,&dwOwnerSize,pPrimaryGroup,&dwPrimaryGroupSize);
//		SetSecurityDescriptorDacl(pAbsoluteSD,bDaclPresent,pNewDacl,bDaclDefaulted);
//		SetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION, pAbsoluteSD);
//
//		if (pAbsoluteSD)
//			LocalFree(pAbsoluteSD);
//		if (pAbsDacl)
//			LocalFree(pAbsDacl);
//		if (pSacl)
//			LocalFree(pSacl);
//		if (pOwner)
//			LocalFree(pOwner);
//		if (pPrimaryGroup)
//			LocalFree(pPrimaryGroup);
//	}
//
//	if (pSecurityInfo)
//		delete[] pSecurityInfo;
//	return TRUE;
//}

//BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess)
//{
//	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
//
//	if (NULL == hProcess)
//		hProcess = OpenProcess( PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,TRUE, dwPID);
//
//	if(NULL == hProcess)
//		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE, dwPID);
//
//	if(hProcess == NULL)
//		return FALSE;
//
//	return TRUE;
//}

BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath)
{
	TCHAR		szImagePath[MAX_PATH];
	HANDLE	hProcess = NULL;

	BOOL bSuccess = FALSE;
	do 
	{
		strFullPath.Empty();
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
		if (hProcess == NULL)
		{
			hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwPID);
			if (hProcess == NULL)
				break;
		}

		if (GetProcessImageFileName(hProcess, szImagePath, MAX_PATH) == FALSE)
			break;

		if (DosPathToNtPath(szImagePath, strFullPath) == FALSE)
			break;

		bSuccess = TRUE;

	} while (FALSE);

	if (hProcess != NULL)
		CloseHandle(hProcess);

	return TRUE;
}

BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath)
{
	TCHAR			szDriveStr[500];
	TCHAR			szDrive[3];
	TCHAR			szDevName[100];
	INT				cchDevName;

	//ASSERT_STR_NOT_NULL(lpszDosPath);
	//获取本地磁盘字符串
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (int i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = _T('\0');
			if (!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(lpszDosPath, szDevName, cchDevName) == 0)//命中
			{
				strNtPath.Format(_T("%s%s"),szDrive,lpszDosPath+cchDevName);
				return TRUE;
			}
		}
	}
	strNtPath = lpszDosPath;
	return FALSE;
}

BOOL TerminateProcessByProcessId(DWORD dwProcessId)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	BOOL bSuccess = FALSE;
	do 
	{
		hProcess = OpenProcess(PROCESS_TERMINATE , FALSE, dwProcessId);
		if (hProcess == NULL)
			break;

		if (TerminateProcess(hProcess,0) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("终止进程%d失败"),dwProcessId);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hProcess != INVALID_HANDLE_VALUE)
		CloseHandle(hProcess);

	return bSuccess;
}

BOOL GetProcessCurrentDirctory(DWORD dwPid,CString& strProcessCurrentDirctory)
{
	PROCNTQSIP NtQueryInformationProcess;
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	TCHAR szCurrentDirctory[MAX_PATH] = {0};

	do 
	{
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION , FALSE, dwPid);
		if (hProcess == NULL)
			break;

		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
																					GetModuleHandle(_T("ntdll")),
																					"NtQueryInformationProcess"
																					);

		status = NtQueryInformationProcess( hProcess, ProcessBasicInformation,(PVOID)&pbi,
																	sizeof(PROCESS_BASIC_INFORMATION), NULL);

		if (status)
			break;

		if (ReadProcessMemory( hProcess,
			pbi.PebBaseAddress,&Peb,
			sizeof(PEB),&dwDummy) == FALSE)
			break;

		if (ReadProcessMemory( hProcess,
			Peb.ProcessParameters,&ProcParam,
			sizeof(PROCESS_PARAMETERS),&dwDummy) == FALSE)
			break;

		lpAddress = ProcParam.CurrentDirectory.Buffer;
		dwSize = ProcParam.CurrentDirectory.Length;

		if (_countof(szCurrentDirctory)<dwSize)
			break;

		if (ReadProcessMemory( hProcess,lpAddress,szCurrentDirctory,dwSize,&dwDummy)== FALSE)
			break;

		strProcessCurrentDirctory = szCurrentDirctory;
		bRet = TRUE;
	} while (FALSE);

	if (hProcess)
		CloseHandle (hProcess);
	return bRet;
}

BOOL GetParentProcessID(DWORD dwPid,DWORD& dwParentProcessID)
{
	PROCNTQSIP NtQueryInformationProcess;
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	BOOL                      bRet = FALSE;

	if (dwPid == 0)
	{
		SetErrorInfo(CUSTOM_ERROR , 0 , _T("没有找到该进程的父进程！"));
		return FALSE;
	}

	do 
	{
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if (hProcess == NULL)
			break;

		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
																						GetModuleHandle(_T("ntdll")),
																						"NtQueryInformationProcess"
																						);

		status = NtQueryInformationProcess( hProcess, ProcessBasicInformation,(PVOID)&pbi,
																		sizeof(PROCESS_BASIC_INFORMATION), NULL);

		if (status)
			break;

		dwParentProcessID = pbi.InheritedFromUniqueProcessId;
		bRet = TRUE;
	} while (FALSE);

	if (hProcess)
		CloseHandle (hProcess);

	return bRet;
}

BOOL GetPriorityClass(DWORD dwPid,CString& strPricrityDescribe)
{
	strPricrityDescribe.Empty();
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if (hProcess == NULL)
		return FALSE;

	DWORD dwPriorityClass = GetPriorityClass(hProcess);
	if (dwPriorityClass == 0)
		return FALSE;

	switch(dwPriorityClass)
	{
	case REALTIME_PRIORITY_CLASS:
		strPricrityDescribe = _T("实时");
		break;
	case HIGH_PRIORITY_CLASS:
		strPricrityDescribe = _T("高");
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		strPricrityDescribe = _T("高于标准");
		break;
	case NORMAL_PRIORITY_CLASS:
		strPricrityDescribe = _T("普通");
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		strPricrityDescribe = _T("低于标准");
		break;		
	case IDLE_PRIORITY_CLASS:
		strPricrityDescribe = _T("低");
		break;
	default:
		strPricrityDescribe = _T("未知");
	}

	CloseHandle(hProcess);
	return strPricrityDescribe.IsEmpty() == FALSE;
}

BOOL EnumProcessInfo(CProcessItemInfoList& ProcessItemInfoList)
{
	ProcessItemInfoList.RemoveAll();
	BOOL bSuccess = FALSE;
	LPBYTE lpBuffData = NULL;
	do 
	{
		DWORD dwSize = 0;
		NTSTATUS Ntstaus = NtQuerySystemInformation(SystemProcessInformation, lpBuffData, 0, &dwSize);
		if (Ntstaus == STATUS_INFO_LENGTH_MISMATCH )
		{
			lpBuffData = new BYTE[dwSize+1];
			if (lpBuffData == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("分配内存失败"));
				break;
			}

			Ntstaus = NtQuerySystemInformation(SystemProcessInformation,lpBuffData, dwSize+1, &dwSize);
			if (NT_SUCCESS(Ntstaus) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统进程信息失败"));
				break;
			}
		}

		if (NT_SUCCESS(Ntstaus))
		{
			PLS_SYSTEM_PROCESSES_INFO pLsSystemProcessInfo = (PLS_SYSTEM_PROCESSES_INFO)lpBuffData;
			while(pLsSystemProcessInfo != NULL)
			{
				PROCESS_ITEM_INFO ProcessItemInfo;
				ProcessItemInfo.strProcessName = CString(pLsSystemProcessInfo->ProcessName.Buffer,pLsSystemProcessInfo->ProcessName.Length);
				ProcessItemInfo.dwProcessId = pLsSystemProcessInfo->ProcessId;
				GetProcessFullPath(pLsSystemProcessInfo->ProcessId,ProcessItemInfo.strProcessPath);
				if (ProcessItemInfo.dwProcessId == 0)
				{
					ProcessItemInfo.strProcessName = _T("System Idle Process");
					ProcessItemInfo.strUserName = _T("SYSTEM");
				}
				else if (ProcessItemInfo.dwProcessId == 4)
				{
					ProcessItemInfo.strProcessPath = GetSystemDirectory(_T("ntkrnlpa.exe"));
					ProcessItemInfo.strUserName = _T("SYSTEM");
				}

				ProcessItemInfo.dwThread = pLsSystemProcessInfo->ThreadCount;
				ProcessItemInfo.dwHandle = pLsSystemProcessInfo->HandleCount;
				ProcessItemInfo.dwMemUse = pLsSystemProcessInfo->dwWorkingSetSize;

				ProcessItemInfo.ullProcessUseTime = pLsSystemProcessInfo->KernelTime + pLsSystemProcessInfo->UserTime;

				GetProcessUserName(pLsSystemProcessInfo->ProcessId, ProcessItemInfo.strUserName);
				ProcessItemInfo.dwConsole = pLsSystemProcessInfo->dwSessionId;

				ProcessItemInfoList.AddTail(ProcessItemInfo);

				if (pLsSystemProcessInfo->NextEntryDelta == NULL)
					pLsSystemProcessInfo = NULL;
				else
					pLsSystemProcessInfo =(PLS_SYSTEM_PROCESSES_INFO)((DWORD)pLsSystemProcessInfo + pLsSystemProcessInfo->NextEntryDelta);
			}
		}

		bSuccess = TRUE;
	} while (FALSE);	

	if (lpBuffData != NULL)
	{
		delete[] lpBuffData;
		lpBuffData = NULL;
	}

	return bSuccess;
}