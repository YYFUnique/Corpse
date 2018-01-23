#include "StdAfx.h"
#include "Security.h"
#include "ErrorInfo.h"

BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable /*= TRUE*/)
{
	HANDLE hToken;
	BOOL bSuccess = FALSE;

	do 
	{
		//�򿪽������ƻ�
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪵�ǰ���̳��ִ���"));
			break;
		}

		//	��ý��̱���ΨһID
		LUID luid = {0};
		if (LookupPrivilegeValue(NULL, lpszPrivilegeName, &luid) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡ %s ��ʶʧ��"), lpszPrivilegeName);
			break;
		}

		TOKEN_PRIVILEGES Token;
		Token.PrivilegeCount = 1;
		Token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		Token.Privileges[0].Luid = luid;

		//����Ȩ��
		if (AdjustTokenPrivileges(hToken ,0, &Token, sizeof(TOKEN_PRIVILEGES), NULL, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��������Ȩ��ʧ��"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);
	
	if (hToken != NULL)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	return bSuccess;
}

//BOOL InjectProcess(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId, DWORD dwWaitTime)
//{
//	HANDLE hRemoteThread = NULL;
//	HANDLE hRemoteProcess = NULL;
//	LPVOID pRemoteAddr = NULL;
//	DWORD dwSize = 0;
//
//	BOOL bSuccess = FALSE;
//
//	do 
//	{
//		 hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId );
//		if (hRemoteProcess == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪽���[%d]ʧ��"), dwRemoteProcessId);
//			break;
//		}
//		dwSize = (_tcslen(lpszFilePath)+1) * sizeof(TCHAR);
//		//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ����ռ�
//		pRemoteAddr = (char *) VirtualAllocEx( hRemoteProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
//		if (pRemoteAddr == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ�̽����ڴ�ռ�ʧ��"));
//			break;
//		}
//
//		//ʹ��WriteProcessMemory������DLL��·����д�뵽Զ�̽��̵��ڴ�ռ�
//		if (WriteProcessMemory(hRemoteProcess, pRemoteAddr, (void *)lpszFilePath, dwSize, NULL) == FALSE)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("������д��Զ�̽���ʧ��"));
//			break;
//		}
//
//		//����LoadLibraryA����ڵ�ַ
//#ifdef _UNICODE
//		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
//#else
//		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
//#endif
//
//		//����Զ���߳�LoadLibrary��ͨ��Զ���̵߳��ô����µ��߳�
//		if( (hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, pRemoteAddr, 0, NULL) ) == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ���߳�ʧ��"));
//			break;
//		}
//
//		if (dwWaitTime != 0 )
//			WaitForSingleObject(hRemoteThread, dwWaitTime);
//		
//		bSuccess = TRUE;
//	} while (FALSE);
//
//	if (hRemoteThread != NULL)
//	{
//		CloseHandle(hRemoteThread);
//		hRemoteThread = NULL;
//	}
//	
//	if (hRemoteProcess != NULL)
//	{
//		VirtualFreeEx(hRemoteProcess, pRemoteAddr, dwSize, MEM_COMMIT);
//		CloseHandle(hRemoteProcess);
//		hRemoteProcess = NULL;
//	}
//
//	return bSuccess;
//}

BOOL UnLoadViewOfModule(DWORD dwProcessId, LPVOID lpBaseAddr)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	do 
	{
		HMODULE hModule = GetModuleHandle(_T("ntdll.dll"));
		if (hModule == NULL)
			hModule = LoadLibrary(_T("ntdll.dll"));

		if (hModule == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��ntdll���ʧ��"));
			break;
		}

		PFNNtUnmapViewOfSection pfnUnLoadModule = (PFNNtUnmapViewOfSection)GetProcAddress(hModule,"NtUnmapViewOfSection");
		if (pfnUnLoadModule == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��ȡ����[NtUnmapViewOfSection]��ַʧ��"));
			break;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessId);
		if (hProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪽���ʧ��"));
			break;
		}

		if (pfnUnLoadModule(hProcess, lpBaseAddr) != 0)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("ж��Զ��ָ��ģ��ʧ��"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hProcess != NULL)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	return bSuccess;
}