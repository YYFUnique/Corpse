#include "StdAfx.h"
#include "IECookies.h"
#include "LsCommon/Reg.h"
#include "LsCommon/CommonFunc.h"
#include "LsCommon/TextTools.h"
#include "LsCommon/FileTools.h"
CIeCookies::CIeCookies()
{
	m_strDescribe = _T("浏览器Cookies");
}

CIeCookies::~CIeCookies()
{

}

BOOL CIeCookies::GetListCtrlHeader(CStringArray& strHeader)
{
	strHeader.RemoveAll();
	strHeader.Add(_T("标题"));
	strHeader.Add(_T("网址"));
	//strHeader.Add(_T("访问次数"));
	strHeader.Add(_T("上次访问时间"));

	return TRUE;
}

BOOL CIeCookies::Lanuch(CItemInfoList& ItemInfoList)
{
	BOOL bSuccess = FALSE;
	do 
	{
		GetIeCookies(ItemInfoList);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CIeCookies::GetIeCookies(CItemInfoList& ItemInfoList)
{
	CoInitialize(NULL);
	LPSHELLFOLDER lpShellFolder;
	LPITEMIDLIST lpItemIdList;
	LPENUMIDLIST lpEnumIdList;
	HRESULT hRes;

	lpShellFolder = GetSpecialFolder(NULL, CSIDL_COOKIES);
	if (lpShellFolder)
	{
		hRes = lpShellFolder->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,&lpEnumIdList);

		if (SUCCEEDED(hRes))
		{
			while (lpEnumIdList->Next(1, &lpItemIdList, NULL) == S_OK)
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