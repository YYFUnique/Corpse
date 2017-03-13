#include "StdAfx.h"
#include "ImpersonateSessionUser.h"
#include "Process.h"
#include "../Utils/ErrorInfo.h"
#include "../User/UserInfo.h"

#include <Userenv.h>
#include <WtsApi32.h>
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "WtsApi32.lib")

CImpersonateSessionUser::CImpersonateSessionUser(void)
{
	m_bSystemUser = IsRunAsSystem();
	m_bNeedRevertToSelf = FALSE;
	m_hUserRegkey = NULL;
	m_hUserToken = NULL;
}

CImpersonateSessionUser::~CImpersonateSessionUser(void)
{
	RevertToSelf();

	if (m_hUserRegkey)
		UnloadUserProfile(m_hUserToken , m_hUserRegkey);
	if (m_hUserToken)
		CloseHandle(m_hUserToken);
}

BOOL CImpersonateSessionUser::Impersonate()
{
	if (m_bSystemUser == FALSE)
		return TRUE;

	HANDLE hToken = GetUserToken();
	if (hToken == NULL)
		return FALSE;

	m_bNeedRevertToSelf = ImpersonateLoggedOnUser(hToken);
	if (m_bNeedRevertToSelf == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("模拟用户权限失败"));
	}

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

HANDLE CImpersonateSessionUser::GetUserToken()
{
	if (m_bSystemUser == FALSE || m_hUserToken)
		return m_hUserToken;

	DWORD dwCurrentProcessId = GetCurrentProcessId();
	DWORD dwSessionId;
	if (ProcessIdToSessionId(dwCurrentProcessId , &dwSessionId) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("模拟用户权限时获取当前进程会话ID失败"));
		return NULL;
	}

	HANDLE hToken = NULL;
	if (LsQueryUserToken(dwSessionId , &hToken) == FALSE)
	{
		SetErrorTitle(_T("模拟用户权限时获取会话令牌失败"));
		return NULL;
	}

	m_hUserToken = hToken;

	ImpersonateLoggedOnUser(hToken);
	TCHAR szUserName[MAX_PATH];
	DWORD dwUserNameLen = MAX_PATH;
	if (GetUserName(szUserName, &dwUserNameLen))
		m_strUserName = szUserName;
	::RevertToSelf();

	return m_hUserToken;
}

HKEY CImpersonateSessionUser::GetUserRegKey()
{
	if (m_bSystemUser == FALSE)
		return HKEY_CURRENT_USER;
	else if (m_hUserRegkey)
		return m_hUserRegkey;

	HANDLE hToken = GetUserToken();
	if (hToken == NULL)
		return NULL;

	PROFILEINFO ProFileinfo;
	memset(&ProFileinfo, 0, sizeof(PROFILEINFO));
	ProFileinfo.dwSize = sizeof(PROFILEINFOA);
	ProFileinfo.lpUserName = (LPTSTR)(LPCTSTR)m_strUserName;
	ProFileinfo.dwFlags = 1;

	if (LoadUserProfile(hToken, &ProFileinfo) == FALSE)
		return NULL;
	
	m_hUserRegkey = (HKEY)ProFileinfo.hProfile;
	return m_hUserRegkey;
}