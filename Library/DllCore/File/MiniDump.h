#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <Dbghelp.h>

class DLL_API CMiniDump
{
public:
	CMiniDump();
	~CMiniDump();
public:
	static void InitDumpDebugInfo(MINIDUMP_TYPE MiniDumpType = MiniDumpNormal);
protected:
	static LONG WINAPI TopLevelExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo);
private:
	static MINIDUMP_TYPE m_sDumpType;
};