#include "StdAfx.h"
#include "IEAddrCache.h"
#include "LsCommon/Reg.h"
#include "LsCommon/CommonFunc.h"
#include "LsCommon/FileTools.h"

CIeAddrCache::CIeAddrCache()
{
	m_strDescribe = _T("�������ַ������");
}

CIeAddrCache::~CIeAddrCache()
{

}

BOOL CIeAddrCache::GetListCtrlHeader(CStringArray& strHeader)
{
	strHeader.RemoveAll();
	strHeader.Add(_T("����"));
	strHeader.Add(_T("��ַ"));
	//strHeader.Add(_T("���ʴ���"));
	strHeader.Add(_T("�ϴη���ʱ��"));

	return TRUE;
}

BOOL CIeAddrCache::Lanuch(CItemInfoList& ItemInfoList)
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

		GetIeAddrCache(szHistoryPath,ItemInfoList);

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}

BOOL CIeAddrCache::GetIeAddrCache(LPCTSTR lpszIeAddrCache,CItemInfoList& ItemInfoList)
{
	CoInitialize(NULL);
	LPSHELLFOLDER lpShellFolder;
	LPITEMIDLIST lpItemIdList;
	LPENUMIDLIST lpEnumIdList;
	HRESULT hRes;

	lpShellFolder = GetSpecialFolder(NULL,lpszIeAddrCache);

	if (lpShellFolder)
	{
		hRes = lpShellFolder->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,&lpEnumIdList);

		if (SUCCEEDED(hRes))
		{
			ULONG uRet;
			while (lpEnumIdList->Next(1, &lpItemIdList, &uRet) == S_OK)
			{
				SHFILEINFO ShFileInfo;
				DWORD_PTR dwRet = SHGetFileInfo((LPCTSTR)lpItemIdList,-1,&ShFileInfo,sizeof(SHFILEINFO),
					SHGFI_PIDL|SHGFI_DISPLAYNAME);

				ITEMINFO ItemInfo;
				ItemInfo.strTitle = ShFileInfo.szDisplayName;
				ItemInfoList.AddTail(ItemInfo);
			}
			lpEnumIdList->Release();
		}
		lpShellFolder->Release();
	}

	CoUninitialize();

	return TRUE;
}
