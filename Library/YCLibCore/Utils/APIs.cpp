#include "StdAfx.h"
#include "APIs.h"
#include "OsInfo.h"

LRESULT GetUserPicturePathV2(LPCTSTR lpszUserName, LPTSTR pwszPicPath, UINT picPathLen)
{
	HINSTANCE hInst = LoadLibrary(_T("shell32.dll"));   
	if (hInst)   
	{
		typedef HRESULT (WINAPI* FUNC)(LPCWSTR, DWORD, LPWSTR ,UINT);
		FUNC GetPicturePath;
		GetPicturePath = (FUNC)GetProcAddress(hInst,(char*)261);
		if (GetPicturePath)
			return GetPicturePath(lpszUserName ,0 , pwszPicPath , picPathLen);
	}

	return ERROR_INVALID_PARAMETER;
}

LRESULT GetUserPicturePathV1(LPCTSTR lpszUserName, LPTSTR pwszPicPath)
{
	HINSTANCE hInst = LoadLibrary(_T("shell32.dll"));   
	if (hInst)   
	{
		typedef HRESULT (WINAPI* FUNC)(LPCWSTR, DWORD, LPWSTR );
		FUNC GetPicturePath;
		GetPicturePath = (FUNC)GetProcAddress(hInst,(char*)233);
		if (GetPicturePath)
			return GetPicturePath(lpszUserName ,0 , pwszPicPath);
	}

	return ERROR_INVALID_PARAMETER;
}

LRESULT GetUserPicturePath(LPCTSTR lpszUserName, LPTSTR pwszPicPath, UINT picPathLen)
{
	LRESULT lRet = ERROR_SUCCESS;
	//Vista及以后系统
	if (OsIsVistaOrLater())
		lRet = GetUserPicturePathV2(lpszUserName ,pwszPicPath , picPathLen);
	else
		lRet = GetUserPicturePathV1(lpszUserName , pwszPicPath);

	return lRet;
}