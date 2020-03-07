#include "StdAfx.h"
#include "DemoWrapper.h"
#include "DllCore/Utils/ErrorInfo.h"

BOOL DemoWrapperLib::Initialize(LPCTSTR lpszModuleName)
{
	if (_tcslen(lpszModuleName) == 0)
	{
		SetErrorInfo(SYSTEM_ERROR, 0, _T("DemoWrapperLib::Initialize£¨£© - lpszModuleName is empty."));
		return FALSE;
	}
	
	if (PathFileExists(lpszModuleName) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR, 0, _T("DemoWrapperLib::Initialize£¨£© - file [%s] doesn't exist."), lpszModuleName);
		return FALSE;
	}

	DWORD dwErrorCode = ERROR_SUCCESS;
	BOOL bRet = this->InitializeLibrary(lpszModuleName, FALSE, &dwErrorCode);
	if (bRet == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR, dwErrorCode, _T("DemoWrapperLib::Initialize£¨£© - InitializeLibrary failed."));
	}

	return bRet;
}

BOOL DemoWrapperLib::Uninitialize()
{
	DWORD dwErrorCode = ERROR_SUCCESS;
	BOOL bRet = this->UnInitializeLibrary(&dwErrorCode);
	if (bRet == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR, dwErrorCode, _T("DemoWrapperLib::Initialize£¨£© - InitializeLibrary failed."));
	}
	return bRet;
}