#include "StdAfx.h"
#include "Security.h"
#include "ErrorInfo.h"
#include <AclAPI.h>

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

BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject)
{
	BOOL bSuccess = FALSE;
	DWORD dwLen = 0;
	PSECURITY_DESCRIPTOR pSecurityInfo = NULL;
	do 
	{
		if (hObject == NULL)
			break;

		// 获取安全描述符长度
		if (GetKernelObjectSecurity(hObject, DACL_SECURITY_INFORMATION, 0, 0, &dwLen) == FALSE)
			break;

		// 获取安全描述内容
		pSecurityInfo = (PSECURITY_DESCRIPTOR)new BYTE[dwLen];
		if (GetKernelObjectSecurity(hObject, DACL_SECURITY_INFORMATION, pSecurityInfo, dwLen,&dwLen) == FALSE)
			break;

		BOOL bDaclPresent,bDaclDefaulted;
		PACL pDacl = NULL;
		GetSecurityDescriptorDacl(pSecurityInfo, &bDaclPresent, &pDacl, &bDaclDefaulted);

		TCHAR szUserName[MAX_PATH];DWORD dwBufferSize=MAX_PATH;
		GetUserName(szUserName, &dwBufferSize);

		EXPLICIT_ACCESS ea;
		BuildExplicitAccessWithName(&ea,szUserName,TOKEN_ALL_ACCESS,GRANT_ACCESS,FALSE);

		PACL pNewDacl;
		SetEntriesInAcl(1,&ea,pDacl,&pNewDacl);
		LocalFree(pDacl);

		DWORD dwAbsoluteSDSize=0,dwAbsDaclSize=0,dwSaclSize=0,dwOwnerSize=0,dwPrimaryGroupSize=0;
		MakeAbsoluteSD(pSecurityInfo,0,&dwAbsoluteSDSize,0,&dwAbsDaclSize,0,&dwSaclSize,0,&dwOwnerSize,0,&dwPrimaryGroupSize);

		PSECURITY_DESCRIPTOR pAbsoluteSD=0;
		PACL pAbsDacl=0,pSacl=0;PSID pOwner=0,pPrimaryGroup=0;
		if (dwAbsoluteSDSize)
			pAbsoluteSD=LocalAlloc(LPTR,dwAbsoluteSDSize);
		if (dwAbsDaclSize)
			pAbsDacl=(PACL)LocalAlloc(LPTR,dwAbsDaclSize);
		if (dwSaclSize)
			pSacl=(PACL)LocalAlloc(LPTR,dwSaclSize);
		if (dwOwnerSize)
			pOwner=(PSID)LocalAlloc(LPTR,dwOwnerSize);
		if (dwPrimaryGroupSize)
			pPrimaryGroup=(PSID)LocalAlloc(LPTR,dwPrimaryGroupSize);
		MakeAbsoluteSD(pSecurityInfo,pAbsoluteSD,&dwAbsoluteSDSize,pAbsDacl,&dwAbsDaclSize,pSacl,&dwSaclSize,pOwner,&dwOwnerSize,pPrimaryGroup,&dwPrimaryGroupSize);
		SetSecurityDescriptorDacl(pAbsoluteSD,bDaclPresent,pNewDacl,bDaclDefaulted);
		SetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION, pAbsoluteSD);

		if (pAbsoluteSD)
			LocalFree(pAbsoluteSD);
		if (pAbsDacl)
			LocalFree(pAbsDacl);
		if (pSacl)
			LocalFree(pSacl);
		if (pOwner)
			LocalFree(pOwner);
		if (pPrimaryGroup)
			LocalFree(pPrimaryGroup);

		bSuccess = TRUE;
	} while (FALSE);
	
	if (pSecurityInfo)
		delete[] pSecurityInfo;

	return bSuccess;
}

BOOL ModifyAccessToEveryOne(LPCTSTR lpszRegPath)
{
	BOOL bSuccess = FALSE;

	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pOldDacl = NULL;
	PACL pNewDacl = NULL;

	do
	{
		//获取主键的DACL     
		DWORD dwRet = GetNamedSecurityInfo((LPTSTR)lpszRegPath, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,     
																			NULL, NULL, &pOldDacl, NULL, &pSD);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("获取主键DACL失败"));
			break;
		}     

		//创建一个ACE，允许Everyone完全控制对象，并允许子对象继承此权限
		EXPLICIT_ACCESS ea;     
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));     
		BuildExplicitAccessWithName(&ea, _T("Everyone"), KEY_ALL_ACCESS, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);     

		//将新的ACE加入DACL     
		dwRet = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl);     
		if (dwRet !=ERROR_SUCCESS)     
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("将新创建的ACE加入DACL失败"));
			break;
		}     

		//更新主键的DACL     
		dwRet = SetNamedSecurityInfo((LPTSTR)lpszRegPath, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,     
			NULL, NULL, pNewDacl, NULL);     
		if (dwRet != ERROR_SUCCESS)     
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("更新主键的DACL失败"));
			break;
		}

		bSuccess = TRUE;
	}while(FALSE);

	if (pSD)
		LocalFree(pSD);
	if (pOldDacl)
		LocalFree(pOldDacl);
	if (pNewDacl)
		LocalFree(pNewDacl);

	return bSuccess;
}