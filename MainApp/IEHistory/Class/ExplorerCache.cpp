#include "StdAfx.h"
#include "ExplorerCache.h"
#include "LsCommon/Reg.h"
#include "LsCommon/CommonFunc.h"

CExplorerCache::CExplorerCache()
{
	m_strDescribe = _T("资源管理器地址栏缓存");
}

CExplorerCache::~CExplorerCache()
{

}

BOOL CExplorerCache::GetListCtrlHeader(CStringArray& strHeader)
{
	strHeader.RemoveAll();
	strHeader.Add(_T("访问地址"));
	strHeader.Add(_T("上次访问时间"));

	return TRUE;
}

BOOL CExplorerCache::Lanuch(CItemInfoList& ItemInfoList)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{
		LPCTSTR lpszSubkey = _T("Software\\MicroSoft\\Windows\\CurrentVersion\\Explorer\\TypedPaths");

		RegOpenKeyEx(HKEY_CURRENT_USER,lpszSubkey,0,KEY_READ,&hKey);
		DWORD dwIndex  = 0;

		while(TRUE)
		{
			TCHAR szRunKey[MAX_PATH];
			DWORD dwSize = sizeof(szRunKey);
			TCHAR szRunValue[MAX_PATH];
			DWORD dwSizeOfByte = sizeof(szRunValue);

			DWORD dwRet = RegEnumValue(hKey,dwIndex++,szRunKey,&dwSize,NULL,NULL,(LPBYTE)szRunValue,&dwSizeOfByte);
			if (dwRet == ERROR_NO_MORE_ITEMS)
				break;

			if (_tcscmp(szRunKey,_T("MRUList")) == 0)
				continue;

			ITEMCOOKIES ItemCookies;
			ItemCookies.strTitle = szRunValue;
			ItemInfoList.AddTail(ItemCookies);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hKey != NULL)
		RegCloseKey(hKey);

	return bSuccess;
}
