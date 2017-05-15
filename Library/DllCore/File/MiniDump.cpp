#include "StdAfx.h"
#include "MiniDump.h"
#include "../Utils/FileTools.h"
#pragma comment(lib, "DbgHelp.lib")

typedef BOOL (WINAPI *pfnMiniDumpWriteDump)(__in  HANDLE hProcess,
																					__in  DWORD ProcessId,
																					__in  HANDLE hFile,
																					__in  MINIDUMP_TYPE DumpType,
																					__in  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
																					__in  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
																					__in  PMINIDUMP_CALLBACK_INFORMATION CallbackParam);


MINIDUMP_TYPE CMiniDump::m_sDumpType = MiniDumpNormal;

CMiniDump::CMiniDump()
{

}

CMiniDump::~CMiniDump()
{

}

void CMiniDump::InitDumpDebugInfo(MINIDUMP_TYPE MiniDumpType /* = MiniDumpNormal */)
{
#ifndef _DEBUG
	SetUnhandledExceptionFilter(&CMiniDump::TopLevelExceptionFilter);
	m_sDumpType = MiniDumpType;
#endif
}

LONG CMiniDump::TopLevelExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo)
{
	DWORD dwRet = 0;

	pfnMiniDumpWriteDump pMiniDumpWriteDump = NULL;

	HMODULE hDbgModule = NULL;
	HANDLE hDumpFile = INVALID_HANDLE_VALUE;
	do 
	{
		hDbgModule = LoadLibrary(_T("DbgHelp.dll"));
		// ��windows 2000 ��֧�ָú��� ����˶�̬����
		if (hDbgModule == NULL)	
			break;

		pMiniDumpWriteDump = (pfnMiniDumpWriteDump)GetProcAddress(hDbgModule, "MiniDumpWriteDump");
		if (pMiniDumpWriteDump == NULL)
			break;

		//����Dump�ļ�·��
		CString strDumpFilePath = GetProgramDataFilePath(_T("MiniDump"), _T("dmp"), GetModuleFileVersion());

		hDumpFile = CreateFile(strDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
		if (hDumpFile == INVALID_HANDLE_VALUE)
			break;

		MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
		loExceptionInfo.ExceptionPointers = pExceptionInfo;
		loExceptionInfo.ThreadId = GetCurrentThreadId();
		loExceptionInfo.ClientPointers = TRUE;

		pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, m_sDumpType , &loExceptionInfo, NULL, NULL);

		TCHAR szDumpFileDirPath[MAX_PATH];

		_tcscpy_s(szDumpFileDirPath,MAX_PATH,strDumpFilePath);
		PathRemoveFileSpec(szDumpFileDirPath);

		//ɾ�������ļ�
		//DeleteFileByTime(szDumpFileDirPath,GetLogModuleFileName(),_T(".dmp"),3);

	} while (FALSE);

	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDumpFile);
		hDumpFile = INVALID_HANDLE_VALUE;
	}

	if (hDbgModule)
	{
		FreeLibrary(hDbgModule);
		hDbgModule = NULL;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
