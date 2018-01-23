#include "StdAfx.h"
#include "Security.h"
#include "ErrorInfo.h"

BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable /*= TRUE*/)
{
	HANDLE hToken;
	BOOL bSuccess = FALSE;

	do 
	{
		//打开进程令牌环
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开当前进程出现错误"));
			break;
		}

		//	获得进程本地唯一ID
		LUID luid = {0};
		if (LookupPrivilegeValue(NULL, lpszPrivilegeName, &luid) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("获取 %s 标识失败"), lpszPrivilegeName);
			break;
		}

		TOKEN_PRIVILEGES Token;
		Token.PrivilegeCount = 1;
		Token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		Token.Privileges[0].Luid = luid;

		//调整权限
		if (AdjustTokenPrivileges(hToken ,0, &Token, sizeof(TOKEN_PRIVILEGES), NULL, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("调整进程权限失败"));
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
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开进程[%d]失败"), dwRemoteProcessId);
//			break;
//		}
//		dwSize = (_tcslen(lpszFilePath)+1) * sizeof(TCHAR);
//		//使用VirtualAllocEx函数在远程进程的内存地址空间分配DLL文件名空间
//		pRemoteAddr = (char *) VirtualAllocEx( hRemoteProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
//		if (pRemoteAddr == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("分配远程进程内存空间失败"));
//			break;
//		}
//
//		//使用WriteProcessMemory函数将DLL的路径名写入到远程进程的内存空间
//		if (WriteProcessMemory(hRemoteProcess, pRemoteAddr, (void *)lpszFilePath, dwSize, NULL) == FALSE)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("将数据写入远程进程失败"));
//			break;
//		}
//
//		//计算LoadLibraryA的入口地址
//#ifdef _UNICODE
//		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
//#else
//		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
//#endif
//
//		//启动远程线程LoadLibrary，通过远程线程调用创建新的线程
//		if( (hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, pRemoteAddr, 0, NULL) ) == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("创建远程线程失败"));
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
			SetErrorInfo(CUSTOM_ERROR, 0, _T("打开ntdll句柄失败"));
			break;
		}

		PFNNtUnmapViewOfSection pfnUnLoadModule = (PFNNtUnmapViewOfSection)GetProcAddress(hModule,"NtUnmapViewOfSection");
		if (pfnUnLoadModule == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("获取函数[NtUnmapViewOfSection]地址失败"));
			break;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessId);
		if (hProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开进程失败"));
			break;
		}

		if (pfnUnLoadModule(hProcess, lpBaseAddr) != 0)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("卸载远程指定模块失败"));
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