#include "StdAfx.h"
#include "ImpersonateSystemUser.h"
#include "Process.h"
#include "../Utils/Security.h"
#include "../Utils/ErrorInfo.h"

#include <Tlhelp32.h>

CImpersonateSystemUser::CImpersonateSystemUser(void)
{
	m_bSystemUser = IsRunAsSystem();
	m_bNeedRevertToSelf = FALSE;
}

CImpersonateSystemUser::~CImpersonateSystemUser(void)
{
	RevertToSelf();
}

BOOL CImpersonateSystemUser::Impersonate()
{
	if (m_bSystemUser == TRUE)
		return TRUE;

	EnablePrivilege(SE_DEBUG_NAME);
	DWORD dwWinlogonProcessId = FindWinlogonProcess();
	if (dwWinlogonProcessId == 0)
		return FALSE;

	HANDLE hSystemToken = DuplicateProcessToken(dwWinlogonProcessId);
	if (hSystemToken == 0)
		return FALSE;

	m_bNeedRevertToSelf = ImpersonateLoggedOnUser(hSystemToken);
	if (m_bNeedRevertToSelf == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("模拟用户权限失败"));
	}

	CloseHandle(hSystemToken);
	return m_bNeedRevertToSelf;
}

void CImpersonateSystemUser::RevertToSelf()
{
	if (m_bNeedRevertToSelf)
	{
		::RevertToSelf();
		m_bNeedRevertToSelf = FALSE;
	}
}

DWORD CImpersonateSystemUser::FindWinlogonProcess()
{
	HANDLE hSnap;
	DWORD dwWinlogonProcessId = 0;
	do
	{
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("查找Winlogon进程时枚举进程失败"));   
			break;
		}

		PROCESSENTRY32 ProcessEntry;
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnap, &ProcessEntry) == FALSE)
			break;

		do
		{
			if (_tcsicmp(ProcessEntry.szExeFile , _T("Winlogon.exe")))
				continue;

			CString strUserName;
			if (GetProcessUserName(ProcessEntry.th32ProcessID , strUserName) == FALSE)
				continue;

			if (strUserName.CompareNoCase(_T("System")) == 0)
			{
				dwWinlogonProcessId = ProcessEntry.th32ProcessID;
				break;
			}
		}while(Process32Next(hSnap, &ProcessEntry) == TRUE);
	}while(0);
	
	if (hSnap != INVALID_HANDLE_VALUE)
		CloseHandle(hSnap);

	if (dwWinlogonProcessId == 0)
		SetErrorInfo(CUSTOM_ERROR , 0 , _T("查找Winlogon进程失败:进程未找到."));

	return dwWinlogonProcessId;
}

HANDLE CImpersonateSystemUser::DuplicateProcessToken(DWORD dwProcessId)
{
	HANDLE hProcess = NULL , hToken = NULL , hDuplicateToken = NULL;

	do 
	{
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION , 0 , dwProcessId);
		if (hProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开进程失败"));
			break;
		}

		if (OpenProcessToken(hProcess , TOKEN_DUPLICATE , &hToken) == 0)
		{
			if (OpenProcessToken(hProcess,READ_CONTROL | WRITE_DAC,&hToken)==0)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统进程Token失败"));
				break;
			}

			//获取该令牌的完全访问权限
			if (ModifyObjectSecurityToAccessAll(hToken)==FALSE)
			{
				SetErrorTitle(_T("设置Token对象权限失败"));
				break;
			}

			CloseHandle(hToken);
			hToken = NULL;
			if (OpenProcessToken(hProcess,TOKEN_DUPLICATE,&hToken)==0)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统进程Token失败"));
				break;
			}
		}

		if (DuplicateTokenEx(hToken , TOKEN_ALL_ACCESS , 0 , SecurityImpersonation , TokenPrimary , &hDuplicateToken)==0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("复制系统进程Token失败"));
			break;
		}

	} while (FALSE);	

	if (hToken)
		CloseHandle(hToken);

	if (hProcess)
		CloseHandle(hProcess);

	return hDuplicateToken;
}