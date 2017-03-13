#include "StdAfx.h"
#include "Process.h"
#include "ProcessInfo.h"
#include "NtDll.h"
#include "../Utils/ErrorInfo.h"

#include <Aclapi.h>
#pragma comment(lib,"Advapi32.lib")

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);

BOOL GetProcessUserName(DWORD dwProcessId, CString& strProcessName)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	PTOKEN_USER pTokenUser=NULL; 
	do 
	{
		if (GetProcessHandle(dwProcessId, hProcess) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("�򿪽���%d���ʧ��"),dwProcessId);
			break;
		}

		if (OpenProcessToken(hProcess,TOKEN_QUERY,&hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ����%dToken��ѯȨ��ʧ��"),dwProcessId);
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
				SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ����Token��Ϣ�����ڴ�ʧ��"));
				break;
			}

			bRet = GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize);
		}

		if (bRet != TRUE)
			break;

		if (LookupAccountSid(NULL,pTokenUser->User.Sid,szUserName,&dwNameSize,szDomain,&dwDomainSize,&SNU) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ����%d�û���ʧ��"),dwProcessId);
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

	GetProcessHandle(dwPid,hProcess);
	if (hProcess == NULL)
		return FALSE;

	do 
	{
		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
			GetModuleHandle(_T("ntdll")),
			"NtQueryInformationProcess"
			);

		status = NtQueryInformationProcess(hProcess,
																	ProcessBasicInformation,(PVOID)&pbi,
																	sizeof(PROCESS_BASIC_INFORMATION),
																	NULL);

		if (status)
			break;

		if (ReadProcessMemory(hProcess, pbi.PebBaseAddress,&Peb, sizeof(PEB), &dwDummy) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 ,_T("��ȡ����PEB����ַʧ��"));
			break;
		}

		if (ReadProcessMemory(hProcess, Peb.ProcessParameters,&ProcParam, sizeof(PROCESS_PARAMETERS),&dwDummy) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡ������������ʧ��"));
			break;
		}

		lpAddress = ProcParam.CommandLine.Buffer;
		dwSize = ProcParam.CommandLine.Length;

		BYTE* bCmdLine = new BYTE[dwSize];
		if (bCmdLine == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ���̵�CMD������ʧ�ܣ������ڴ�ʧ��"));
			break;
		}

		BOOL 	bSuccess = FALSE;
		do 
		{
			bSuccess = ReadProcessMemory(hProcess, lpAddress, bCmdLine, dwSize, &dwDummy);
			if (bSuccess == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡ����CMD������ʧ��"));
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

BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject)
{
	DWORD dwLengthNeeded=0;PSECURITY_DESCRIPTOR pSecurityInfo=0;
	GetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION,0,0,&dwLengthNeeded);
	if (dwLengthNeeded)
	{
		pSecurityInfo=(PSECURITY_DESCRIPTOR)new BYTE[dwLengthNeeded];
		GetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION,pSecurityInfo,dwLengthNeeded,&dwLengthNeeded);

		BOOL bDaclPresent,bDaclDefaulted;PACL pDacl;
		GetSecurityDescriptorDacl(pSecurityInfo,&bDaclPresent,&pDacl,&bDaclDefaulted);

		TCHAR szUserName[MAX_PATH];DWORD dwBufferSize=MAX_PATH;
		GetUserName(szUserName,&dwBufferSize);

		EXPLICIT_ACCESS ea;
		BuildExplicitAccessWithName(&ea,szUserName,TOKEN_ALL_ACCESS,GRANT_ACCESS,FALSE);

		PACL pNewDacl;
		SetEntriesInAcl(1,&ea,pDacl,&pNewDacl);
		LocalFree(pDacl);

		DWORD dwAbsoluteSDSize=0,dwAbsDaclSize=0,dwSaclSize=0,dwOwnerSize=0,dwPrimaryGroupSize=0;
		MakeAbsoluteSD(pSecurityInfo,0,&dwAbsoluteSDSize,0,&dwAbsDaclSize,0,&dwSaclSize,0,&dwOwnerSize,0,&dwPrimaryGroupSize);

		PSECURITY_DESCRIPTOR pAbsoluteSD=0;
		PACL pAbsDacl=0,pSacl=0;PSID pOwner=0,pPrimaryGroup=0;
		if (dwAbsoluteSDSize)
			pAbsoluteSD=LocalAlloc(LPTR,dwAbsoluteSDSize);
		if (dwAbsDaclSize)
			pAbsDacl=(PACL)LocalAlloc(LPTR,dwAbsDaclSize);
		if (dwSaclSize)
			pSacl=(PACL)LocalAlloc(LPTR,dwSaclSize);
		if (dwOwnerSize)
			pOwner=(PSID)LocalAlloc(LPTR,dwOwnerSize);
		if (dwPrimaryGroupSize)
			pPrimaryGroup=(PSID)LocalAlloc(LPTR,dwPrimaryGroupSize);
		MakeAbsoluteSD(pSecurityInfo,pAbsoluteSD,&dwAbsoluteSDSize,pAbsDacl,&dwAbsDaclSize,pSacl,&dwSaclSize,pOwner,&dwOwnerSize,pPrimaryGroup,&dwPrimaryGroupSize);
		SetSecurityDescriptorDacl(pAbsoluteSD,bDaclPresent,pNewDacl,bDaclDefaulted);
		SetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION, pAbsoluteSD);

		if (pAbsoluteSD)
			LocalFree(pAbsoluteSD);
		if (pAbsDacl)
			LocalFree(pAbsDacl);
		if (pSacl)
			LocalFree(pSacl);
		if (pOwner)
			LocalFree(pOwner);
		if (pPrimaryGroup)
			LocalFree(pPrimaryGroup);
	}

	if (pSecurityInfo)
		delete[] pSecurityInfo;
	return TRUE;
}

BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess)
{
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);

	if (NULL == hProcess)
		hProcess = OpenProcess( PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,TRUE, dwPID);

	if(NULL == hProcess)
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE, dwPID);

	if(hProcess == NULL)
		return FALSE;

	return TRUE;
}