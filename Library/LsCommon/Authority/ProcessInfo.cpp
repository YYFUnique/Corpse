#include "StdAfx.h"
#include <Psapi.h>
#include "ProcessInfo.h"
#include "../ErrorInfo.h"
#include "../Class/Process.h"

CProcessInfo::CProcessInfo(void)
{
	m_hProcess = NULL;
	m_dwProcessId = 0;
}

CProcessInfo::~CProcessInfo(void)
{
	CloseProcessHandle();
}

void CProcessInfo::CloseProcessHandle()
{
	if (m_hProcess)
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
	m_dwProcessId = 0;
}

BOOL CProcessInfo::TerminateProcess()
{
	if (m_dwProcessId <= 8)
	{
		SetErrorInfo(CUSTOM_ERROR , 0 , _T("终止进程失败:无效进程ID."));
		return FALSE;
	}

	HANDLE hProcess=OpenProcess(PROCESS_TERMINATE,0,m_dwProcessId);
	if (hProcess==NULL)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("终止进程时打开进程失败:"));
		return FALSE;
	}

	BOOL bSuccess=::TerminateProcess(hProcess,0);
	if (bSuccess==FALSE)
		SetErrorInfo(SYSTEM_ERROR,0,_T("终止进程失败"));

	CloseHandle(hProcess);
	return bSuccess;
}

DWORD CProcessInfo::GetProcessId()
{
	return m_dwProcessId;
}

BOOL CProcessInfo::SetProcessId(DWORD dwProcessId)
{
	CloseProcessHandle();

	m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE , dwProcessId);
	if (m_hProcess == NULL)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开进程失败"));
		return FALSE;
	}

	m_dwProcessId = dwProcessId;
	return TRUE;
}

BOOL CProcessInfo::GetProcessName(CString& strProcessName)
{
	TCHAR szProcessName[MAX_PATH];
	if (GetModuleBaseName(m_hProcess , NULL , szProcessName , _countof(szProcessName)) == 0)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取进程名失败"));
		return FALSE;
	}

	strProcessName = szProcessName;
	return TRUE;
}

BOOL CProcessInfo::GetUserName( CString& strProcessUserName )
{
	return GetProcessUserName(m_dwProcessId , strProcessUserName);
}