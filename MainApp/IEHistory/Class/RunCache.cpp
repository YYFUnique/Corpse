#include "StdAfx.h"
#include "RunCache.h"
#include "LsCommon/Reg.h"
#include "LsCommon/CommonFunc.h"

CRunCache::CRunCache()
{
	m_strDescribe = _T("'运行'对话框命令行缓存");
}

CRunCache::~CRunCache()
{

}

BOOL CRunCache::GetListCtrlHeader(CStringArray& strHeader)
{
	strHeader.RemoveAll();
	strHeader.Add(_T("命令行命令"));

	return TRUE;
}

BOOL CRunCache::Lanuch(CItemInfoList& ItemInfoList)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{
		LPCTSTR lpszSubkey = _T("Software\\MicroSoft\\Windows\\CurrentVersion\\Explorer\\RunMRU");
		
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
			ItemCookies.strTitle.TrimRight(_T("\\1"));
			ItemInfoList.AddTail(ItemCookies);
		}

		bSuccess = TRUE;
	} while (FALSE);
	
	if (hKey != NULL)
		RegCloseKey(hKey);

	return bSuccess;
}
