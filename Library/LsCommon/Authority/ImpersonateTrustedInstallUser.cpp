#include "StdAfx.h"
#include "ImpersonateTrustedInstallUser.h"
#include "../Class/Process.h"
#include "..\ErrorInfo.h"
#include <Aclapi.h>
#include <Shlwapi.h>

CImpersonateTrustedInstallUser::CImpersonateTrustedInstallUser(void)
{

	m_bNeedRevertToSelf = FALSE;
}

CImpersonateTrustedInstallUser::~CImpersonateTrustedInstallUser(void)
{
	RevertToSelf();
}

BOOL CImpersonateTrustedInstallUser::Impersonate(LPCTSTR lpszFilePath,LPCTSTR lpszGroupName)
{
	BOOL bSuccess = FALSE;
	PACL pNewDacl = NULL,pOldDacl = NULL;
	do 
	{
		DWORD dwRet = GetNamedSecurityInfo((LPTSTR)lpszFilePath,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,&pOldDacl,NULL,NULL);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取%s访问控制列表失败"),lpszFilePath);
			WriteErrorInfoToLogFile(GetThreadErrorInfoString());
			break;
		}

		EXPLICIT_ACCESS ea;
		::BuildExplicitAccessWithName(&ea,(LPTSTR)lpszGroupName,GENERIC_ALL,GRANT_ACCESS,SUB_CONTAINERS_AND_OBJECTS_INHERIT);
		dwRet = SetEntriesInAcl(1,&ea,pOldDacl,&pNewDacl);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("创建新的ACL对象失败"));
			break;
		}
	
		dwRet = SetNamedSecurityInfo((LPTSTR)lpszFilePath,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,pNewDacl,NULL);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("设置文件安全对象列表失败"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);
	if (pNewDacl)
		LocalFree(pNewDacl);

	return bSuccess;
}

BOOL CImpersonateTrustedInstallUser::ChangeFileName(LPCTSTR lpszFileName)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (PathFileExists(lpszFileName))
		{
			if (Impersonate(lpszFileName,_T("Everyone")) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("修改文件[%s]所有者权限失败"),lpszFileName);
				WriteErrorInfoToLogFile(GetThreadErrorInfoString());
				break;
			}
			CString strBakFileName = lpszFileName;
			strBakFileName += _T(".lsbak");
			if (MoveFile(lpszFileName,strBakFileName) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("文件重命名失败"));
				break;
			}
		}

		CString strBootLoaderFile;
		TCHAR szBootBakFile[MAX_PATH];
		_tcscpy_s(szBootBakFile,_countof(szBootBakFile),lpszFileName);
		PathRemoveFileSpec(szBootBakFile);
		strBootLoaderFile.Format(_T("%s\\boot\\winload.exe"),szBootBakFile);
		OutputDebugString(strBootLoaderFile);
		if (PathFileExists(strBootLoaderFile))
		{
			if (Impersonate(strBootLoaderFile,_T("Everyone")) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("修改文件[%s]所有者权限失败"),strBootLoaderFile);
				WriteErrorInfoToLogFile(GetThreadErrorInfoString());
				break;
			}
			CString strBootBakFile = strBootLoaderFile;
			strBootBakFile += _T(".lsbak");
			if (MoveFile(strBootLoaderFile,strBootBakFile) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("文件重命名失败"));
				break;
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CImpersonateTrustedInstallUser::RevertToSelf()
{
	
}