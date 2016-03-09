#include "stdafx.h"
#include "DebugInfo.h"
#include "ErrorInfo.h"
#include <shlwapi.h>
#include "tlhelp32.h"

#include "FileTools.h"
//����Ϊ����ģʽ��Ϣ������
//--------------------------------------------------------------------
/*
����˵��:���ڴ˴���Ϊ�˽����Ϊ�ⲿ�������乹�캯������֮ǰ��CPP��
��ĳЩȫ�ֱ���δ��ʼ�������µ��÷Ƿ��Ĵ���,�������ⲿ����֮ǰ����
��ʼ����CPP�е�ȫ�ֱ���
*/
#ifdef LS_STATIC_LIB_CALL
#pragma warning(disable:4073)
#pragma init_seg(lib)
#endif

#define  MAX_ADDRESS_LENGTH  32
#define  MAX_NAME_LENGTH		 1024


//#ifdef NDEBUG
//BOOL m_bDebugMode=DEBUG_MODE_NONE;
//#else
//BOOL m_bDebugMode=DEBUG_MODE_NORMAL;
//#endif

void Output(LPCSTR lpszFile,int nLine,LPCTSTR lpszFormat,...)
{
	va_list paralist;
	va_start(paralist, lpszFormat); 

	OutputEx(_T(""),lpszFile,nLine,lpszFormat,paralist);
}


void OutputEx(LPCTSTR lpszModuleName,LPCSTR lpszFile,int nLine,LPCTSTR lpszFormat,...)
{
	va_list paralist;
	va_start(paralist, lpszFormat); 

	CString strFileName(PathFindFileNameA(lpszFile));

	if (lpszFormat)
		OutputV(_T(""),strFileName,nLine,lpszFormat,paralist);
	else
		OutputV(_T(""),strFileName,nLine,GetThreadErrorInfoString(),paralist);
}

void OutputV(LPCTSTR lpszModuleName,LPCTSTR lpszFileName,int nLine,LPCTSTR lpszMsg,va_list argList)
{
	SYSTEMTIME CurrentTime;
	GetLocalTime(&CurrentTime);
	
	CString strDebugInfo(lpszMsg);
	strDebugInfo.TrimRight(_T("\n\r"));
	CString strFormatMsg;
	strFormatMsg.FormatV(strDebugInfo,argList);
	strFormatMsg.TrimRight(_T("\n\r"));

	CString strOutputString;
	strOutputString.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%03d (%s[%s:%d])%s\r\n"),CurrentTime.wYear,CurrentTime.wMonth,
						   CurrentTime.wDay,CurrentTime.wHour,CurrentTime.wMinute,CurrentTime.wSecond,CurrentTime.wMilliseconds,lpszModuleName,
						   lpszFileName,nLine,strFormatMsg);

	OutputDebugString(strOutputString);
}

BOOL LsSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	if (lpTopLevelExceptionFilter == NULL)
		lpTopLevelExceptionFilter = LsUnhandledExceptionFilter;

	SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
	return TRUE;
}

LONG WINAPI LsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	CCallstackInfo CallstackInfoList;
	if (GetCallstackInfo(ExceptionInfo->ContextRecord,CallstackInfoList) == TRUE)
	{
		POSITION pos = CallstackInfoList.GetHeadPosition();
		while(pos)
		{
			CALLSTACKINFO& CallstackInfo = CallstackInfoList.GetNext(pos);
			CString strCallstackInfo;
			strCallstackInfo.Format(_T("%s::%s[%d]"),CallstackInfo.strModuleName,CallstackInfo.strFileName,CallstackInfo.dwLine);
			WriteErrorInfoToLogFile(strCallstackInfo);
		}
	}

	CString strDumpFilePath = GetFilePath(_T("MiniDump"),_T(".dmp"));
	HANDLE lhDumpFile = CreateFile(strDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpWithPrivateReadWriteMemory , &loExceptionInfo, NULL, NULL);
	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL GetCallstackInfo(const CONTEXT* pContext,CCallstackInfo& CallstackInfoList)
{
	BOOL bSuccess = FALSE;
	do 
	{
		HANDLE hProcess = GetCurrentProcess(); 
		SymInitialize(hProcess, NULL, TRUE);  

		STACKFRAME64 StackFrame;  
		ZeroMemory(&StackFrame,sizeof(STACKFRAME64));  
		DWORD dwImageType = IMAGE_FILE_MACHINE_AMD64;

#ifdef _M_IX86 
		dwImageType = IMAGE_FILE_MACHINE_AMD64;  
		StackFrame.AddrPC.Offset = pContext->Eip;  
		StackFrame.AddrStack.Offset = pContext->Esp;  
		StackFrame.AddrFrame.Offset = pContext->Ebp;  
#elif _M_X64  
		dwImageType = IMAGE_FILE_MACHINE_AMD64;  
		StackFrame.AddrPC.Offset = pContext->Rip;  
		StackFrame.AddrFrame.Offset = pContext->Rsp;  
	    StackFrame.AddrStack.Offset = pContext->Rsp;  
#elif _M_IA64  
	    dwImageType = IMAGE_FILE_MACHINE_IA64;  
		StackFrame.AddrPC.Offset = pContext->StIIP;  
		StackFrame.AddrFrame.Offset = pContext->IntSp;  
		StackFrame.AddrBStore.Offset = pContext->RsBSP;  
		StackFrame.AddrBStore.Mode = AddrModeFlat;  
		StackFrame.AddrStack.Offset = pContext->IntSp;  
#endif
		//3��ƽ̨���в���
		StackFrame.AddrPC.Mode = AddrModeFlat;  
		StackFrame.AddrFrame.Mode = AddrModeFlat;  
		StackFrame.AddrStack.Mode = AddrModeFlat;  

		HANDLE hThread = GetCurrentThread();
		while(TRUE)
		{
			if (StackWalk64(dwImageType,hProcess,hThread,&StackFrame,&pContext,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL) == FALSE)
				break;
			if (StackFrame.AddrFrame.Offset == 0)
				break;
			CALLSTACKINFO CallstackInfo;
			BYTE SymbolBuffer[sizeof(IMAGEHLP_SYMBOL64)+MAX_NAME_LENGTH];
			IMAGEHLP_SYMBOL64* pSymbol = (IMAGEHLP_SYMBOL64*)SymbolBuffer;
			ZeroMemory(pSymbol,sizeof(IMAGEHLP_SYMBOL64)+MAX_NAME_LENGTH);
			
			pSymbol->SizeOfStruct = sizeof(SymbolBuffer);
			pSymbol->MaxNameLength = MAX_NAME_LENGTH;
			DWORD SymDisplacement = 0;
			//�õ���������
			if (SymGetSymFromAddr64(hProcess,StackFrame.AddrPC.Offset,NULL,pSymbol))
				CallstackInfo.strMethodName = pSymbol->Name;

			//�õ��ļ��������ڴ�����
			IMAGEHLP_LINE64 LineInfo;
			ZeroMemory(&LineInfo,sizeof(IMAGEHLP_LINE64));
			LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			DWORD dwLineDisplacement = 0;
			if (SymGetLineFromAddr64(hProcess,StackFrame.AddrPC.Offset,&dwLineDisplacement,&LineInfo))
			{
				CallstackInfo.strFileName = LineInfo.FileName;
				CallstackInfo.dwLine = LineInfo.LineNumber;
			}

			//�õ�ģ������
			IMAGEHLP_MODULE64 ModuleInfo;
			ZeroMemory(&ModuleInfo,sizeof(IMAGEHLP_MODULE64));
			ModuleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
			if (SymGetModuleInfo64(hProcess,StackFrame.AddrPC.Offset,&ModuleInfo))
				CallstackInfo.strModuleName = ModuleInfo.ModuleName;
			
			CallstackInfoList.AddTail(CallstackInfo);
		}
		SymCleanup(hProcess);

		bSuccess = CallstackInfoList.IsEmpty() == FALSE;
	} while (FALSE);
	
	return bSuccess;
}