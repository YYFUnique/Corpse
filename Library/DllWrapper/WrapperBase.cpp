#include "StdAfx.h"
#include "WrapperBase.h"

// 10 Times for retry
#define RETRY_COUNT	10	
//10m sec for retry
#define RETRY_INTERVAL	10

#define SET_ERROR_CODE(x) if (pdwWin32ErrorCode) { *pdwWin32ErrorCode = x; }

Base::Base()
{
	this->m_hModule = NULL;
}

Base::~Base()
{
	if (this->IsLoadedLibrary() == TRUE)
	{
		DWORD dwWin32ErrorCode;
		if (this->FreeLibrary(&dwWin32ErrorCode) == FALSE)
		{

		}
	}
}

BOOL Base::LoadLibrary(LPCTSTR lpszFileName, BOOL bRetryFlag /*= FLASE*/, DWORD* pdwWin32ErrorCode /*= NULL*/)
{
	if (lpszFileName == NULL)
	{
		SET_ERROR_CODE(ERROR_BAD_ARGUMENTS);
		return FALSE;
	}

	if (this->m_hModule)
	{
		SET_ERROR_CODE(ERROR_OBJECT_ALREADY_EXISTS);
		return FALSE;
	}

	HMODULE hModule = ::LoadLibrary(lpszFileName);
	if (hModule == NULL)
	{
		DWORD dwLastError = ::GetLastError();
		if (dwLastError != ERROR_DLL_INIT_FAILED)
		{
			SET_ERROR_CODE(dwLastError);
			return FALSE;
		}

		if (bRetryFlag == FALSE)
		{
			SET_ERROR_CODE(dwLastError);
			return FALSE;
		}

		if (RetryLoadLibrary(hModule, lpszFileName, pdwWin32ErrorCode) == FALSE)
			return FALSE;
	}

	this->m_hModule = hModule;
	SET_ERROR_CODE(ERROR_SUCCESS);
	return TRUE;
}

BOOL Base::FreeLibrary(DWORD* pdwWin32ErrorCode /* = NULL */)
{
	if (this->m_hModule == NULL)
	{
		SET_ERROR_CODE(ERROR_INVALID_HANDLE);
		return FALSE;
	}
	
	if (::FreeLibrary(this->m_hModule) == FALSE)
	{
		SET_ERROR_CODE(GetLastError());
		return FALSE;
	}

	this->m_hModule = NULL;
	SET_ERROR_CODE(ERROR_SUCCESS);
	return TRUE;
}

BOOL Base::GetProcAddress(FARPROC* lpProcAddr, LPCSTR lpszFuncName, DWORD* pdwWin32ErrorCode /* = NULL */)
{
	if (lpProcAddr == NULL || lpszFuncName == NULL)
	{
		SET_ERROR_CODE(ERROR_BAD_ARGUMENTS);
		return FALSE;
	}

	if (this->IsLoadedLibrary() == FALSE)
	{
		SET_ERROR_CODE(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	*lpProcAddr = ::GetProcAddress(this->m_hModule, (LPSTR)lpszFuncName);
	if (*lpProcAddr == NULL)
	{
		SET_ERROR_CODE(GetLastError());
		return FALSE;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return TRUE;
}

BOOL Base::GetProcAddress(FARPROC* lpProcAddr, LPCWSTR lpwszFuncName, DWORD* pdwWin32ErrorCode /* = NULL */)
{
	USES_CONVERSION;
	if (lpwszFuncName == NULL)
	{
		SET_ERROR_CODE(ERROR_BAD_ARGUMENTS);
		return FALSE;
	}

	return this->GetProcAddress(lpProcAddr, CW2A(lpwszFuncName), pdwWin32ErrorCode);
}

BOOL Base::IsLoadedLibrary() const
{
	if (this->m_hModule)
		return TRUE;
	
	return FALSE;
}

BOOL Base::RetryLoadLibrary(HMODULE& hModule, LPCTSTR lpszFileName, DWORD* pdwWin32ErrorCode /* = NULL */)
{
	if (lpszFileName == NULL)
	{
		SET_ERROR_CODE(ERROR_BAD_ARGUMENTS);
		return FALSE;
	}

	int nCnt;
	BOOL bRtnStatus = FALSE;
	for (nCnt = 0; nCnt < RETRY_COUNT; ++nCnt)
	{
		hModule = ::LoadLibrary(lpszFileName);
		if (hModule != NULL)
		{
			bRtnStatus = TRUE;
			break;
		}

		DWORD dwLastError = ::GetLastError();
		if (dwLastError != ERROR_DLL_INIT_FAILED)
		{
			SET_ERROR_CODE(dwLastError);
			break;
		}

		::Sleep(RETRY_INTERVAL);
	}

	if (bRtnStatus == TRUE)
	{
		SET_ERROR_CODE(ERROR_SUCCESS);
	}

	return bRtnStatus;
}