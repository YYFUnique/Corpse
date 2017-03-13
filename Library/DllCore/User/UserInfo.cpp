#include "StdAfx.h"
#include <Sddl.h>
#include <Lm.h>
#include "UserInfo.h"
#include "../Utils/ErrorInfo.h"

#pragma comment(lib , "Netapi32.lib")

typedef BOOL (WINAPI *LPWTS_QUERY_USER_TOKEN)(DWORD dwSessionId,	PHANDLE phHandle);

BOOL ConvertUserSidStringToName(LPCTSTR lpszUserSid , CString& strUserName)
{
	BOOL bSuccess = FALSE;
	PSID lpSid = NULL;

	do 
	{
		if (ConvertStringSidToSid(lpszUserSid , &lpSid) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("转换用户SID字符串失败"));
			break;
		}

		TCHAR szUserName[4096],szDomain[4096];
		DWORD dwUserNameLen = _countof(szUserName);
		DWORD dwDomainLen = _countof(szDomain);
		SID_NAME_USE SNU;
		if (LookupAccountSid(NULL , lpSid , szUserName , &dwUserNameLen , szDomain , &dwDomainLen , &SNU) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("转换用户SID到用户名失败"));
			break;
		}

		strUserName = szUserName;

		bSuccess = TRUE;
	} while (0);

	if (lpSid)
		LocalFree(lpSid);

	return bSuccess;
}

BOOL GetUsersBelongingToTheGroup(LPCTSTR lpszUserName , CStdArray& strGroupArray)
{
	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;

	NET_API_STATUS nStatus=NetUserGetLocalGroups(NULL,CStringW(lpszUserName),0,0,(LPBYTE*)&pBuf,dwPrefMaxLen,
		&dwEntriesRead,&dwTotalEntries);
	if (nStatus!=NERR_Success || pBuf==NULL)
	{
		SetErrorInfo(SYSTEM_ERROR , nStatus , _T("枚举用户所在组信息失败"));
		return FALSE;
	}

	strGroupArray.RemoveAll();

	LPLOCALGROUP_USERS_INFO_0 pCurrentBuf=pBuf;
	for (DWORD i = 0; i < dwEntriesRead; i++)
	{
		strGroupArray.Add(CString(pCurrentBuf->lgrui0_name));
		pCurrentBuf++;
	}

	NetApiBufferFree(pBuf);

	return TRUE;
}

BOOL IsUserInAdminGroup(LPCTSTR lpszUserName)
{
	CStdArray strGroupArray;
	if (GetUsersBelongingToTheGroup(lpszUserName , strGroupArray) == FALSE)
		return FALSE;

	int nCount = strGroupArray.GetCount();
	for (int n=0; n<nCount;++n)
	{
		if (strGroupArray.GetAt(n).CompareNoCase(lpszUserName) == 0)
			return TRUE;
	}

	return FALSE;
}

BOOL EnumOsUserInfo(CLsOsUserInfoList& OsUserInfo)
{
	OsUserInfo.RemoveAll();

	LPUSER_INFO_20 pUserInfoBuf = NULL;
	//因NetUserEnum传入level=23时返回124,因此按照level=20进行枚举
	DWORD dwEntriesRead=0,dwTotalEntries=0;
	NET_API_STATUS nStatus = NetUserEnum(NULL , 20 , FILTER_NORMAL_ACCOUNT , (LPBYTE*)&pUserInfoBuf , MAX_PREFERRED_LENGTH , &dwEntriesRead , &dwTotalEntries , 0);
	if (nStatus != NERR_Success)
	{
		SetErrorInfo(SYSTEM_ERROR , nStatus , _T("枚举用户失败"));
		return FALSE;
	}

	if (pUserInfoBuf == NULL)
		return FALSE;
	for(DWORD i=0;i<dwEntriesRead;i++)
	{
		LS_OS_USER_INFO OsUserinfo;
		OsUserinfo.strUserName = pUserInfoBuf[i].usri20_name;
		OsUserinfo.strFullName = pUserInfoBuf[i].usri20_full_name;
		OsUserinfo.strComment = pUserInfoBuf[i].usri20_comment;
		OsUserinfo.dwFlags = pUserInfoBuf[i].usri20_flags;

		memset(OsUserinfo.UserSidBuffer , 0 , sizeof(OsUserinfo.UserSidBuffer));
		LPUSER_INFO_23 pUserInfo23Buf = NULL;
		if (NetUserGetInfo(NULL,pUserInfoBuf[i].usri20_name,23,(LPBYTE *)&pUserInfo23Buf)==NERR_Success)
		{
			ASSERT(sizeof(OsUserinfo.UserSidBuffer) >= GetLengthSid(pUserInfo23Buf->usri23_user_sid));
			CopySid(sizeof(OsUserinfo.UserSidBuffer) , &OsUserinfo.sidUserSid , pUserInfo23Buf->usri23_user_sid);
			NetApiBufferFree(pUserInfo23Buf);
		}

		OsUserInfo.AddTail(OsUserinfo);
	}

	if (pUserInfoBuf)
		NetApiBufferFree(pUserInfoBuf);
	
	return TRUE;
}

BOOL LsQueryUserToken(DWORD dwSessionId , PHANDLE phHandle)
{
	HMODULE hLib = NULL;
	static LPWTS_QUERY_USER_TOKEN WtsQueryUserToken = NULL;
	if (WtsQueryUserToken == 0)
	{
		hLib = LoadLibrary(_T("Wtsapi32.dll"));//获取函数地之后不释放动态库
		if (hLib != NULL)
			WtsQueryUserToken = (LPWTS_QUERY_USER_TOKEN)GetProcAddress(hLib, "WTSQueryUserToken");
	}

	if (WtsQueryUserToken == NULL)
	{
		if (hLib)
			FreeLibrary(hLib);
		SetErrorInfo(CUSTOM_ERROR , 0 , _T("获取用户令牌信息失败:当前系统不支持。"));
		return FALSE;
	}

	if (WtsQueryUserToken(dwSessionId, phHandle) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取用户令牌信息失败"));
		return FALSE;
	}

	return TRUE;
}
