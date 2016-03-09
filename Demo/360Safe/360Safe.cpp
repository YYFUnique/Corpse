
#include "stdafx.h"
#include "360Safe.h"
#include "BaseDialog.h"
#include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")


using namespace DuiLib;

LONG WINAPI LsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	LPCTSTR lpszDumpFilePath = _T("C:\\360Safe.dmp");
	HANDLE lhDumpFile = CreateFile(lpszDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpWithPrivateReadWriteMemory , &loExceptionInfo, NULL, NULL);
	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL LsSetUnhandledExceptionFilter( __in LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter )
{
	if (lpTopLevelExceptionFilter == NULL)
		lpTopLevelExceptionFilter = LsUnhandledExceptionFilter;

	SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(_T("skin.zip"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

#ifndef _DEBUG
	LsSetUnhandledExceptionFilter(LsUnhandledExceptionFilter);
#endif

	BaseDialog* pFrame = new BaseDialog();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("360Safe"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	pFrame->CenterWindow();
	pFrame->ShowWindow(true);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}
