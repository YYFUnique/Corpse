#include "StdAfx.h"
#include "IECookies.h"
#include "LsCommon/Reg.h"
#include "LsCommon/CommonFunc.h"
#include "LsCommon/TextTools.h"
#include "LsCommon/FileTools.h"
CIeCookies::CIeCookies()
{
	m_strDescribe = _T("�����Cookies");
}

CIeCookies::~CIeCookies()
{

}

BOOL CIeCookies::GetListCtrlHeader(CStringArray& strHeader)
{
	strHeader.RemoveAll();
	strHeader.Add(_T("����"));
	strHeader.Add(_T("��ַ"));
	//strHeader.Add(_T("���ʴ���"));
	strHeader.Add(_T("�ϴη���ʱ��"));

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