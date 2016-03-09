#include "StdAfx.h"
#include <WtsApi32.h>
#include "ImpersonateSessionUser.h"
#include "../ErrorInfo.h"
#include "../Class/Process.h"

#pragma comment(lib , "WtsApi32.lib")

CImpersonateSessionUser::CImpersonateSessionUser(void)
{
	m_bSystemUser = IsSystemUserProcess();
	m_bNeedRevertToSelf = FALSE;
}

CImpersonateSessionUser::~CImpersonateSessionUser(void)
{
	RevertToSelf();
}

BOOL CImpersonateSessionUser::Impersonate()
{
	if (m_bSystemUser == FALSE)
		return TRUE;

	DWORD dwCurrentProcessId = GetCurrentProcessId();
	DWORD dwSessionId;
	if (ProcessIdToSessionId(dwCurrentProcessId , &dwSessionId) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("ģ���û�Ȩ��ʱ��ȡ��ǰ���̻ỰIDʧ��"));
		return FALSE;
	}

	HANDLE hToken = NULL;
	if (WTSQueryUserToken(dwSessionId , &hToken) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("ģ���û�Ȩ��ʱ��ȡ�Ự����ʧ��"));
		return FALSE;
	}

	m_bNeedRevertToSelf = ImpersonateLoggedOnUser(hToken);
	if (m_bNeedRevertToSelf == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("ģ���û�Ȩ��ʧ��"));
	}

	CloseHandle(hToken);
	return m_bNeedRevertToSelf;
}

void CImpersonateSessionUser::RevertToSelf()
{
	if (m_bNeedRevertToSelf)
	{
		::RevertToSelf();
		m_bNeedRevertToSelf = FALSE;
	}
}