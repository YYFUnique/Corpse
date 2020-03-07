#pragma once

class DLL_WRAPPER_API Base
{
public:
	Base();
	virtual ~Base();
public:
	operator HMODULE() const {return (this->m_hModule);}
public:
	BOOL LoadLibrary(LPCTSTR lpszFileName, BOOL bRetryFlag = FALSE, DWORD* pdwWin32ErrorCode = NULL);
	BOOL FreeLibrary(DWORD* pdwWin32ErrorCode = NULL);
	BOOL GetProcAddress(FARPROC* lpProcAddr, LPCSTR lpszFuncName, DWORD* pdwWin32ErrorCode = NULL);
	BOOL GetProcAddress(FARPROC* lpProcAddr, LPCWSTR lpwszFuncName, DWORD* pdwWin32ErrorCode = NULL);
	BOOL IsLoadedLibrary() const;
private:
	BOOL RetryLoadLibrary(HMODULE& hModule, LPCTSTR lpszFileName, DWORD* pdwWin32ErrorCode = NULL);
protected:
	HMODULE m_hModule;
};