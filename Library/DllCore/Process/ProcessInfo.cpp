#include "StdAfx.h"
#include "ProcessInfo.h"
#include "Process.h"
#include "../Utils/ErrorInfo.h"

#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

CProcessInfo::CProcessInfo(void)
{
	m_hProcess = NULL;
	m_dwProcessId = 0;
}

CProcessInfo::~CProcessInfo(void)
{
	ReleaseHandle();
}

void CProcessInfo::ReleaseHandle()
{
	if (m_hProcess != NULL)
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
	m_dwProcessId = 0;
}

BOOL CProcessInfo::TerminateProcess()
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	do 
	{
		if (m_dwProcessId <= 8)
		{
			SetErrorInfo(CUSTOM_ERROR , 0 , _T("��ֹ����ʧ��:��Ч����ID."));
			break;
		}

		hProcess = ::OpenProcess(PROCESS_TERMINATE, 0, m_dwProcessId);
		if (hProcess==NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ֹ����[%d]ʱ�򿪽���ʧ��"), m_dwProcessId);
			break;
		}

		if (::TerminateProcess(hProcess, 0) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ֹ����[%d]ʧ��"), m_dwProcessId);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hProcess != NULL)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	return bSuccess;
}

BOOL CProcessInfo::OpenProcess(DWORD dwProcessId)
{
	ReleaseHandle();

	m_hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE , dwProcessId);
	if (m_hProcess == NULL)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("�򿪽���ʧ��"));
		return FALSE;
	}

	m_dwProcessId = dwProcessId;
	return TRUE;
}

HANDLE CProcessInfo::GetProcess()
{
	return m_hProcess;
}

BOOL CProcessInfo::IsProcessRunAsAdmin()
{
	BOOL bElevated = FALSE;  
	HANDLE hToken = NULL;	
	do 
	{
		if (m_hProcess == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��ȡ���̾��ʧ��"));
			break;
		}

		if (OpenProcessToken(m_hProcess, TOKEN_QUERY, &hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪽��̻�ȡ���̾��ʧ��"));
			break;
		}

		TOKEN_ELEVATION tokenEle;
		DWORD dwRetLen = 0;  

		// Retrieve token elevation information
		if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen) == FALSE) 
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡ����Token��Ϣʧ��"));
			break;
		}

		//�����Ƿ��Թ���ԱȨ�����б�ʶ
		bElevated = tokenEle.TokenIsElevated;  

	} while (FALSE);
	
	if (hToken != NULL)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	return bElevated;  
}

BOOL CProcessInfo::IsProcessRunAsSystem()
{
	BOOL bRunAsSystemUser = FALSE;

	CString strProcessUserName;
	if (GetUserName(strProcessUserName) == FALSE)
	{
		if (strProcessUserName.CompareNoCase(_T("system")) == 0)
			bRunAsSystemUser = TRUE;
	}

	return bRunAsSystemUser;
}

BOOL CProcessInfo::GetProcessName(CString& strProcessName)
{
	TCHAR szProcessName[MAX_PATH];

	do 
	{
		if (m_hProcess == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("δ��ȡ�����̾�������ȡ���̾��������"));
			break;
		}

		if (GetModuleBaseName(m_hProcess , NULL , szProcessName , _countof(szProcessName)) == 0)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ȡ������ʧ��"));
			break;
		}

		strProcessName = szProcessName;
	} while (FALSE);
	
	return strProcessName.IsEmpty() == FALSE;
}

BOOL CProcessInfo::GetUserName(CString& strProcessUserName)
{
	return GetProcessUserName(m_dwProcessId , strProcessUserName);
}