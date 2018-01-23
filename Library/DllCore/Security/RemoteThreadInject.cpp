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
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪽���[%d]ʧ��"), dwProcessId);
			break;
		}
		dwSize = (_tcslen(lpszFilePath)+1) * sizeof(TCHAR);
		//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ����ռ�
		pRemoteAddr = (char *) VirtualAllocEx( hRemoteProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (pRemoteAddr == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ�̽����ڴ�ռ�ʧ��"));
			break;
		}

		//ʹ��WriteProcessMemory������DLL��·����д�뵽Զ�̽��̵��ڴ�ռ�
		if (WriteProcessMemory(hRemoteProcess, pRemoteAddr, (void *)lpszFilePath, dwSize, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("������д��Զ�̽���ʧ��"));
			break;
		}

		//����LoadLibraryA����ڵ�ַ
#ifdef _UNICODE
		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
#else
		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
#endif

		//����Զ���߳�LoadLibrary��ͨ��Զ���̵߳��ô����µ��߳�
		m_hInjecthread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, pRemoteAddr, 0, NULL);
		if (m_hInjecthread == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ���߳�ʧ��"));
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

		//	����LoadLibraryA����ڵ�ַ
#ifdef _UNICODE
	   PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
#else
	   PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
#endif

	   //	����Զ���߳�LoadLibrary��ͨ��Զ���̵߳��ô����µ��߳�
	   m_hInjecthread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, m_hMod, 0, NULL);
	   if (m_hInjecthread == NULL)
	   {
		   SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ���߳�ʧ��"));
		   break;
	   }

	   //	5. �ȴ��߳̽�������
	   WaitForSingleObject(m_hInjecthread, dwWaitTime);

	   //	6. ��ȡ�߳��˳���,��LoadLibrary�ķ���ֵ����dll���׵�ַ
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
