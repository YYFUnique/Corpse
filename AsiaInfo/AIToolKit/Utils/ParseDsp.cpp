#include "StdAfx.h"
#include "ParseDsp.h"

CParseDsp::CParseDsp()
{

}

CParseDsp::~CParseDsp()
{

}

BOOL CParseDsp::Init(LPCTSTR lpszDepluaContent)
{
	BOOL bSuccess = FALSE;
	do 
	{
		LPCTSTR lpszReturnObject = _T("return");
		if (_tcsnicmp(lpszDepluaContent, lpszReturnObject, _tcslen(lpszReturnObject)) != 0)
			break;

		CString strDepluaContent(lpszDepluaContent);
		strDepluaContent.Replace(_T("\""), _T(""));

		TCHAR szDeplua[MAX_PATH];
		TCHAR szType[MAX_PATH];
		_stscanf_s(strDepluaContent, _T("return {%[^=]={type=%[^}]}"), 
																			szDeplua, _countof(szDeplua),
																			szType, _countof(szType));

		m_strDepluaFileName = szDeplua;
		m_strDepluaFileType = szType;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

LPCTSTR CParseDsp::GetFileName() const
{
	return m_strDepluaFileName;
}

LPCTSTR CParseDsp::GetLuaFileType() const
{
	return m_strDepluaFileType;
}

BOOL CParseDsp::IsPackageHasDep(LPCTSTR lpszPackage, CString& strPackageDeplua)
{
	BOOL bSuccess = FALSE;
	HZIP hPackage = NULL;
	do 
	{
		if (PathFileExists(lpszPackage) == FALSE)
			break;

		hPackage = OpenZip(lpszPackage, NULL);
		if (hPackage == NULL)
			break;

		int nIndex = 0;
		ZIPENTRY ZipEntry;
		ZRESULT zRet = FindZipItem(hPackage, _T("dep/"), false, &nIndex, &ZipEntry);
		if (zRet != ZR_OK || nIndex == -1)
			break;
		
		if (GetZipItem(hPackage, ZipEntry.index+1, &ZipEntry) != ZR_OK)
			break;

		BYTE bPackageDeplua[MAX_PATH] = {0};
		if (UnzipItem(hPackage, ZipEntry.index, bPackageDeplua, _countof(bPackageDeplua)) != ZR_OK)
			break;

		strPackageDeplua = (LPSTR)bPackageDeplua;
		
		bSuccess = TRUE;
	} while (FALSE);

	if (hPackage != NULL)
	{
		CloseZip(hPackage);
		hPackage = NULL;
	}

	return bSuccess;
}

BOOL CParseDsp::UnPackageFile(LPCTSTR lpszUnpackageDir, LPCTSTR lpszPackage)
{
	BOOL bSuccess = FALSE;
	HZIP hPackage = NULL;
	do 
	{
		if (PathFileExists(lpszPackage) == FALSE)
			break;

		hPackage = OpenZip(lpszPackage, NULL);
		if (hPackage == NULL)
			break;

		
		ZIPENTRY ZipEntry;
		ZRESULT zRet = GetZipItem(hPackage, -1, &ZipEntry);
		if (zRet != ZR_OK)
			break;

		SetUnzipBaseDir(hPackage, lpszUnpackageDir);

		int nIndex = ZipEntry.index;
		for (int n=0; n < nIndex; ++n)
		{
			GetZipItem(hPackage, n, &ZipEntry);
			UnzipItem(hPackage, n, ZipEntry.name);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hPackage != NULL)
	{
		CloseZip(hPackage);
		hPackage = NULL;
	}

	return bSuccess;
}

BOOL CParseDsp::PackageFile(LPCTSTR lpszPackageDir, LPCTSTR lpszPackageName)
{
	BOOL bSuccess = FALSE;
	HZIP hPackage = NULL;
	do 
	{
		if (PathFileExists(lpszPackageDir) == FALSE)
			break;

		hPackage = CreateZip(lpszPackageName, NULL);
		if (hPackage == NULL)
			break;

		bSuccess = PackageFileInDirectory(hPackage, lpszPackageDir, _T(""));
	} while (FALSE);

	if (hPackage != NULL)
	{
		CloseZip(hPackage);
		hPackage = NULL;
	}

	return bSuccess;
}

BOOL CParseDsp::PackageFileInDirectory(HZIP hPackage, LPCTSTR lpszPackageDir, LPCTSTR lpszBaseDir)
{
	WIN32_FIND_DATA Win32Data;
	TCHAR szSearchPath[MAX_PATH];
	PathCombine(szSearchPath, lpszPackageDir, _T("*.*"));
	HANDLE hFile = FindFirstFile(szSearchPath, &Win32Data);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do 
	{
		if (_tcsicmp(Win32Data.cFileName, _T(".")) ==0 || _tcsicmp(Win32Data.cFileName, _T("..")) ==0 )
			continue;

		TCHAR szFilePath[MAX_PATH];
		PathCombine(szFilePath, lpszPackageDir, Win32Data.cFileName);

		TCHAR szPackageItemName[MAX_PATH];
		PathCombine(szPackageItemName, lpszBaseDir, Win32Data.cFileName);

		if (Win32Data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			PackageFileInDirectory(hPackage, szFilePath, szPackageItemName);
		else
			ZipAdd(hPackage, szPackageItemName, szFilePath);
			
	} while (FindNextFile(hFile, &Win32Data));

	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}