#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CRemoteThreadInject
{
public:
    CRemoteThreadInject();
    ~CRemoteThreadInject();

public:
    BOOL Attach(LPCTSTR lpszExeName, LPCTSTR lpszDllName, DWORD dwWaitTime = INFINITE);
    BOOL Detach(LPCTSTR lpszExeName, DWORD dwWaitTime = INFINITE);

protected:
	DWORD GetProcessId(LPCTSTR pszProcessName);
	BOOL InjectDll(DWORD dwProcessId, LPCTSTR lpszDllName, DWORD dwWaitTime);
	BOOL RelaseDll(DWORD dwProcessId, DWORD dwWaitTime);

private:
    HMODULE	m_hMod;
    HANDLE		m_hInjecthread;
};

