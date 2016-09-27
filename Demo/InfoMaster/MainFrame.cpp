#include "stdafx.h"
#include "InfoMaster.h"
#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp.lib" )

LONG WINAPI LsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	//CCallstackInfo CallstackInfoList;
	//if (GetCallstackInfo(ExceptionInfo->ContextRecord,CallstackInfoList) == TRUE)
	//{
	//	POSITION pos = CallstackInfoList.GetHeadPosition();
	//	while(pos)
	//	{
	//		CALLSTACKINFO& CallstackInfo = CallstackInfoList.GetNext(pos);
	//		CString strCallstackInfo;
	//		strCallstackInfo.Format(_T("%s::%s[%d]"),CallstackInfo.strModuleName,CallstackInfo.strFileName,CallstackInfo.dwLine);
	//		WriteErrorInfoToLogFile(strCallstackInfo);
	//	}
	//}

	CString strDumpFilePath = _T("C:\\infomaster.dmp");
	HANDLE lhDumpFile = CreateFile(strDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpWithPrivateReadWriteMemory , &loExceptionInfo, NULL, NULL);
	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL LsSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	if (lpTopLevelExceptionFilter == NULL)
		lpTopLevelExceptionFilter = LsUnhandledExceptionFilter;

	SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
	return TRUE;
}

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	LsSetUnhandledExceptionFilter(NULL);

	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;

	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CInfoMaster* pInfoMaster = new CInfoMaster();
		if (pInfoMaster == NULL) 
			break;

		pInfoMaster->Create(NULL, _T("辰锐终端安全配置工具"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pInfoMaster->CenterWindow();
		pInfoMaster->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();
	} while (FALSE);
	
	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return TRUE;
}