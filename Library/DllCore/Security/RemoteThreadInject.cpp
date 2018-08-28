#include "stdafx.h"
#include "RemoteThreadInject.h"
#include "../Utils/Security.h"
#include "../Utils/ErrorInfo.h"
#include <Tlhelp32.h>

CRemoteThreadInject::CRemoteThreadInject(void)
{
	m_hMod = NULL;
	m_hInjecthread = NULL;
}

CRemoteThreadInject::~CRemoteThreadInject(void)
{
}

DWORD CRemoteThreadInject::GetProcessId(LPCTSTR lpszProcessName)
{
    HANDLE hProcess = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (INVALID_HANDLE_VALUE == hProcess)
		return 0;

    DWORD dwProcessId = 0;

    PROCESSENTRY32 process32 = {0};
    process32.dwSize = sizeof(PROCESSENTRY32);

    BOOL bRetProcess = FALSE;
    bRetProcess = ::Process32First(hProcess, &process32);

    do
    {
        if (_tcscmp(lpszProcessName, process32.szExeFile) == 0)
        {
            dwProcessId = process32.th32ProcessID;
            break;
        }

        bRetProcess = ::Process32Next(hProcess, &process32);
    }while (bRetProcess);

    ::CloseHandle(hProcess);

    return dwProcessId;
}

BOOL CRemoteThreadInject::InjectDll(DWORD dwProcessId, LPCTSTR lpszFilePath, DWORD dwWaitTime = INFINITE)
{
	HANDLE hRemoteThread = NULL;
	HANDLE hRemoteProcess = NULL;
	LPVOID   pRemoteAddr = NULL;
	DWORD dwSize = 0;

	BOOL bSuccess = FALSE;

	do 
	{
		EnablePrivilege(SE_DEBUG_NAME, TRUE);

		hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if (hRemoteProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开进程[%d]失败"), dwProcessId);
			break;
		}
		dwSize = (_tcslen(lpszFilePath)+1) * sizeof(TCHAR);
		//使用VirtualAllocEx函数在远程进程的内存地址空间分配DLL文件名空间
		pRemoteAddr = (char *) VirtualAllocEx( hRemoteProcess, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (pRemoteAddr == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("分配远程进程内存空间失败"));
			break;
		}

		//使用WriteProcessMemory函数将DLL的路径名写入到远程进程的内存空间
		if (WriteProcessMemory(hRemoteProcess, pRemoteAddr, (void *)lpszFilePath, dwSize, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("将数据写入远程进程失败"));
			break;
		}

		//计算LoadLibraryA的入口地址
#ifdef _UNICODE
		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
#else
		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
#endif

		//启动远程线程LoadLibrary，通过远程线程调用创建新的线程
		m_hInjecthread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, pRemoteAddr, 0, NULL);
		if (m_hInjecthread == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("创建远程线程失败"));
			break;
		}

		if (dwWaitTime != 0 )
			WaitForSingleObject(hRemoteThread, dwWaitTime);

		DWORD dwExitCode;
		GetExitCodeThread(m_hInjecthread, &dwExitCode);
		m_hMod = (HMODULE)dwExitCode;

		bSuccess = TRUE;
	} while (FALSE);

	if (hRemoteThread != NULL)
	{
		CloseHandle(hRemoteThread);
		hRemoteThread = NULL;
	}

	if (hRemoteProcess != NULL)
	{
		VirtualFreeEx(hRemoteProcess, pRemoteAddr, dwSize, MEM_COMMIT);
		CloseHandle(hRemoteProcess);
		hRemoteProcess = NULL;
	}

	return bSuccess;
}

BOOL CRemoteThreadInject::RelaseDll(DWORD dwProcessId, DWORD dwWaitTime = INFINITE)
{
	BOOL bSuccess = FALSE;
	HANDLE hRemoteProcess = NULL;
	do 
	{
		if (m_hMod == NULL)
			break;

		EnablePrivilege(SE_DEBUG_NAME, TRUE);

		hRemoteProcess  = ::OpenProcess(  PROCESS_ALL_ACCESS, FALSE, dwProcessId); 
		if (hRemoteProcess == NULL)
			break;

		//	计算LoadLibraryA的入口地址
#ifdef _UNICODE
	   PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
#else
	   PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
#endif

	   //	启动远程线程LoadLibrary，通过远程线程调用创建新的线程
	   m_hInjecthread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, m_hMod, 0, NULL);
	   if (m_hInjecthread == NULL)
	   {
		   SetErrorInfo(SYSTEM_ERROR, 0, _T("创建远程线程失败"));
		   break;
	   }

	   //	5. 等待线程结束返回
	   WaitForSingleObject(m_hInjecthread, dwWaitTime);

	   //	6. 获取线程退出码,即LoadLibrary的返回值，即dll的首地址
	   DWORD dwExitCode;
	   GetExitCodeThread(m_hInjecthread, &dwExitCode);

	   m_hMod = NULL;

	   bSuccess = TRUE;
	} while (FALSE);

	if (hRemoteProcess != NULL)
	{
	   CloseHandle(hRemoteProcess);
	   hRemoteProcess = NULL;
	}

    return bSuccess;
}

BOOL CRemoteThreadInject::Attach(LPCTSTR lpszExeName, LPCTSTR lpszDllName, DWORD dwWaitTime /*= INFINITE*/)
{
    DWORD dwProcessId = GetProcessId(lpszExeName);

    return InjectDll(dwProcessId, lpszDllName, dwWaitTime);
}

BOOL CRemoteThreadInject::Detach(LPCTSTR lpszExeName, DWORD dwWaitTime /*= INFINITE*/)
{
    DWORD dwProcessId = GetProcessId(lpszExeName);

    return RelaseDll(dwProcessId, dwWaitTime);
}
