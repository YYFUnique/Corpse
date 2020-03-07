#include "StdAfx.h"
#include "Wrapper.h"

#define SET_ERROR_CODE(x) if (pdwWin32ErrorCode) { *pdwWin32ErrorCode = x; }

Wrapper::Wrapper()
{
	this->m_bIsInitialized = FALSE;
}

Wrapper::~Wrapper()
{

}

BOOL Wrapper::InitializeLibrary(LPCTSTR lpszFileName, BOOL bRetry /* = FALSE */, DWORD* pdwWin32ErrorCode /* = NULL */)
{
	if (this->IsInitialized() != FALSE)
	{
		SET_ERROR_CODE(ERROR_OBJECT_ALREADY_EXISTS);
		return FALSE;
	}

	if (this->LoadLibrary(lpszFileName, bRetry, pdwWin32ErrorCode) == FALSE)
	{
		return FALSE;
	}

	if (this->GetFunctionsAddress(TRUE, pdwWin32ErrorCode) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Wrapper::UnInitializeLibrary(DWORD* pdwWin32ErrorCode /* = NULL */)
{
	if (this->IsInitialized() == FALSE)
	{
		SET_ERROR_CODE(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	if (this->FreeLibrary(pdwWin32ErrorCode) == FALSE)
	{
		return FALSE;
	}

	BOOL bAddressFlag = FALSE;
	if (this->GetFunctionsAddress((bAddressFlag, pdwWin32ErrorCode) == FALSE))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Wrapper::IsInitialized() const
{
	return this->m_bIsInitialized;
}