#include "StdAfx.h"
#include "IECache.h"
#include "LsCommon/Reg.h"
#include "LsCommon/CommonFunc.h"

CIeCache::CIeCache()
{
	m_strDescribe = _T("IE浏览器缓存");
}

CIeCache::~CIeCache()
{

}

BOOL CIeCache::GetListCtrlHeader(CStringArray& strHeader)
{
	strHeader.RemoveAll();
	strHeader.Add(_T("标题"));
	strHeader.Add(_T("网址"));
	//strHeader.Add(_T("访问次数"));
	strHeader.Add(_T("上次访问时间"));

	return TRUE;
}

BOOL CIeCache::Lanuch(CItemInfoList& ItemInfoList)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CReg RegHistory;
		LPCTSTR lpszSubkey = _T("Software\\MicroSoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders");
		if (RegHistory.OpenRegKey(lpszSubkey,FALSE,HKEY_CURRENT_USER) != TRUE)
			break;

		TCHAR szHistoryEnvironmentPath[MAX_PATH];
		DWORD dwSize = sizeof(szHistoryEnvironmentPath);
		if (RegHistory.ReadRegKey(_T("History"),szHistoryEnvironmentPath,dwSize) == FALSE)
			break;

		TCHAR szHistoryPath[MAX_PATH];
		if (ExpandEnvironmentStrings(szHistoryEnvironmentPath,szHistoryPath,sizeof(szHistoryPath)) == 0)
			break;

		AdjustProcessAuthority(SE_DEBUG_NAME);

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}
