#include "StdAfx.h"
#include "Wow64InjectDll.h"

#include "wow64ext.h"
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

typedef struct tagClientId
{
	DWORD64 UniqueProcess;
	DWORD64 UniqueThread;
} CLIENT_ID;

typedef struct _UNICODE_STRING {
	USHORT    Length;     //UNICODE占用的内存字节数，个数*2；
	USHORT      MaximumLength; 
	DWORD64   Buffer;     //注意这里指针的问题
} UNICODE_STRING ,*PUNICODE_STRING;

BYTE ShellCode[] = {
			0x48, 0x89, 0x4c, 0x24, 0x08,                               // mov       qword ptr [rsp+8],rcx 
			0x57,                                                       // push      rdi
			0x48, 0x83, 0xec, 0x20,                                     // sub       rsp,20h
			0x48, 0x8b, 0xfc,                                           // mov       rdi,rsp
			0xb9, 0x08, 0x00, 0x00, 0x00,                               // mov       ecx,8
			0xb8, 0xcc, 0xcc, 0xcc, 0xcc,                               // mov       eac,0CCCCCCCCh
			0xf3, 0xab,                                                 // rep stos  dword ptr [rdi]
			0x48, 0x8b, 0x4c, 0x24, 0x30,                               // mov       rcx,qword ptr [__formal]
			0x49, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       r9,0  //PVOID*  BaseAddr opt
			0x49, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       r8,0  //PUNICODE_STRING Name
			0x48, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rdx,0
			0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rcx,0
			0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rax,0 
			0xff, 0xd0,                                                 // call      rax   LdrLoadDll
			0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rcx,0
			0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rax,0
			0xff, 0xd0                                                  // call      rax
};


BOOL Wow64InjectX64(DWORD dwProcessid, LPCTSTR lpszFilePath, DWORD dwWaitTime /*= INFINITE*/)
{
	HANDLE hProcess = NULL;
	BOOL bSuccess = FALSE;

	DWORD64 hRemoteThread = NULL;
	DWORD64 pRemoteAddr = NULL;
	DWORD64  pShellCodeAddr = NULL;
	BYTE * pbData = NULL;
	do 
	{
		if (PathFileExists(lpszFilePath) == FALSE)
			break;

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessid);
		if (NULL == hProcess)
			break;

		size_t dwFilePathLen = (size_t)::_tcslen(lpszFilePath);
		size_t dwSize = (dwFilePathLen+1) * sizeof(WCHAR) + sizeof(UNICODE_STRING) + sizeof(DWORD64);
		pRemoteAddr = (DWORD64)VirtualAllocEx64(hProcess,NULL,dwSize,MEM_COMMIT,PAGE_READWRITE);
		pShellCodeAddr = (DWORD64)VirtualAllocEx64(hProcess,NULL,sizeof(ShellCode),MEM_COMMIT,PAGE_EXECUTE_READWRITE);

		if ((pRemoteAddr == NULL) || (pShellCodeAddr == NULL))
			break;

		pbData = new BYTE[dwSize];
		memset(pbData, 0, dwSize);

		PUNICODE_STRING pRtlString = (PUNICODE_STRING)(pbData + sizeof(DWORD64));
		pRtlString->Length = dwFilePathLen * sizeof(WCHAR);
		pRtlString->MaximumLength = pRtlString->Length;
		//wcscpy_s((WCHAR*)(pRtlString+1), pRtlString->Length, CStringW(lpszFilePath));
		// 第二个参数为字符个数，非字节数
		wcscpy_s((WCHAR*)(pRtlString+1), dwFilePathLen+1, CStringW(lpszFilePath));

		pRtlString->Buffer = (DWORD64)((BYTE*)pRemoteAddr + sizeof(DWORD64) + sizeof(UNICODE_STRING));

		DWORD64 ntdll64 = GetModuleHandle64(L"ntdll.dll");
		DWORD64 ntdll_LdrLoadDll = GetProcAddress64(ntdll64,"LdrLoadDll");
		DWORD64 ntdll_RtlCreateUserThread = GetProcAddress64(ntdll64,"RtlCreateUserThread");
		DWORD64 ntdll_RtlExitThread = GetProcAddress64(ntdll64,"RtlExitUserThread");
		if (NULL == ntdll_LdrLoadDll || NULL==ntdll_RtlCreateUserThread || NULL==ntdll_RtlExitThread)
			break;

		//r9
		memcpy(ShellCode+32, &pRemoteAddr, sizeof(DWORD64));

		//r8
		DWORD64 ptr = pRemoteAddr + sizeof(DWORD64);
		memcpy(ShellCode+42, &ptr, sizeof(PUNICODE_STRING));

		//LdrLoaddll
		memcpy(ShellCode+72, &ntdll_LdrLoadDll, sizeof(DWORD64));

		//RtlExitUserThread
		memcpy(ShellCode+94, &ntdll_RtlExitThread, sizeof(DWORD64));

		if (WriteProcessMemory64(hProcess, pRemoteAddr, pbData, dwSize, NULL) == FALSE ||
			WriteProcessMemory64(hProcess, pShellCodeAddr, ShellCode, sizeof(ShellCode), NULL) == FALSE)
			break;
		
		CLIENT_ID ClientId;
		X64Call(ntdll_RtlCreateUserThread, 10,
																(DWORD64)hProcess,                    // ProcessHandle
																(DWORD64)NULL,							// SecurityDescriptor
																(DWORD64)FALSE,							// CreateSuspended
																(DWORD64)0,									// StackZeroBits
																(DWORD64)NULL,							// StackReserved
																(DWORD64)NULL,							// StackCommit
																pShellCodeAddr,								// StartAddress
																(DWORD64)NULL,							// StartParameter
																(DWORD64)&hRemoteThread,      // ThreadHandle
																(DWORD64)&ClientId);					// ClientID
		if (INVALID_HANDLE_VALUE == (HANDLE)hRemoteThread)
			break;

		WaitForSingleObject((HANDLE)hRemoteThread, dwWaitTime);

		bSuccess = TRUE;
	} while (FALSE);
	
	if (pRemoteAddr != NULL)
		VirtualFreeEx64(hProcess, pRemoteAddr, 0, MEM_RELEASE);
	if (pShellCodeAddr != NULL)
		VirtualFreeEx64(hProcess, pShellCodeAddr, 0, MEM_RELEASE);

	if (pbData != NULL)
	{
		delete[] pbData;
		pbData = NULL;
	}

	if (hRemoteThread != NULL)
		CloseHandle((HANDLE)hRemoteThread);

	if (hProcess != NULL)
		CloseHandle(hProcess);

	return bSuccess;
}