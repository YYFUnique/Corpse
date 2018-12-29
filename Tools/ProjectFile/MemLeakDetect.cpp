/*************************************************************
 Author		: David A. Jones
 File Name	: MemLeakDetect.h
 Date		: July 30, 2004
 Synopsis		 
			A trace memory feature for source code to trace and
			find memory related bugs. 


****************************************************************/
// See MemLeakDetect.h for full history.
// Based on http://www.codeproject.com/cpp/MemLeakDetect.asp
#include "stdafx.h"
#include "MemLeakDetect.h"
#include "DllCore/Utils/FileTools.h"

#ifdef _DEBUG
#include <tchar.h>
#include <fstream>
#include <time.h>
#include <atlstr.h>

#include <Psapi.h>					// Only needed for GetModuleBaseName().
#pragma comment(lib, "Psapi.lib")	// Only needed for GetModuleBaseName().
#pragma comment(lib, "DbgHelp.lib")
//#pragma warning(disable:4312)	// 'type cast' : conversion from 'long' to 'void *' of greater size
//#pragma warning(disable:4313)
//#pragma warning(disable:4267)
#pragma warning(disable:4100)	// Unreferenced formal parameter.

static CMemLeakDetect*	g_pMemTrace			= NULL;
static _CRT_ALLOC_HOOK	pfnOldCrtAllocHook	= NULL;

static int catchMemoryAllocHook(int	allocType, 
						 void	*userData, 
						 size_t size, 
						 int	blockType, 
						 long	requestNumber, 
		  const unsigned char	*filename, // Can't be UNICODE
						 int	lineNumber) ;

static int MyTrace(LPCTSTR lpszFormat, ...);

static int MyTrace(LPCTSTR lpszFormat, ...)
{
 	va_list args;
	va_start(args, lpszFormat);
	TCHAR buffer[1024];
	_vstprintf_s(buffer, _countof(buffer), lpszFormat, args);

#ifndef UNICODE
	return _CrtDbgReport(_CRT_WARN,NULL,NULL,NULL,buffer);
#else
	return _CrtDbgReportW(_CRT_WARN,NULL,NULL,NULL,buffer);
#endif
}

static int catchMemoryAllocHook(int	allocType, 
						 void	*userData, 
						 size_t size, 
						 int	blockType, 
						 long	requestNumber, 
		  const unsigned char	*filename,  // Can't be UNICODE
						 int	lineNumber)
{
	_CrtMemBlockHeader *pCrtHead;
	long prevRequestNumber;
#ifdef UNICODE
	wchar_t Wname[1024] ;
	Wname[0] = L'\0' ;
#endif

	// internal C library internal allocations
	if ( blockType == _CRT_BLOCK )
		return TRUE;

	// check if someone has turned off mem tracing
	if  ((( _CRTDBG_ALLOC_MEM_DF & _crtDbgFlag) == 0) && 
		(( allocType			== _HOOK_ALLOC)		|| 
			( allocType			== _HOOK_REALLOC)))
	{
		if (pfnOldCrtAllocHook)
		{
			pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}

	// protect if mem trace is not initialized
	if (g_pMemTrace == NULL)
	{
		if (pfnOldCrtAllocHook)
		{
			pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}

	// protect internal mem trace allocs
	if (g_pMemTrace->m_bLocked != FALSE)
	{
		if (pfnOldCrtAllocHook)
		{
			pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}
	// lock the function
	g_pMemTrace->m_bLocked = TRUE;
	//
#ifdef UNICODE
	int len ;
	if (NULL != filename)
	{
		len = (int)strlen((char *)filename) + 1 ;
		MultiByteToWideChar(CP_ACP, 0, (char *)filename, len, Wname, len) ;
	}
	else
		len = 0 ;
#else
	#define Wname (char*)filename
#endif
	if (allocType == _HOOK_ALLOC)
	{
		g_pMemTrace->addMemoryTrace((void *) requestNumber, size, Wname, lineNumber);
	}
	else
	if (allocType == _HOOK_REALLOC)
	{
		if (_CrtIsValidHeapPointer(userData))
		{
			pCrtHead = pHdr(userData);
			prevRequestNumber = pCrtHead->lRequest;
			//
			if (pCrtHead->nBlockUse == _IGNORE_BLOCK)
			{
				if (pfnOldCrtAllocHook)
				{
					pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
				}
				goto END;
			}
	   		g_pMemTrace->redoMemoryTrace((void *) requestNumber, (void *) prevRequestNumber, size, Wname, lineNumber);
		}
	}
	else
	if (allocType == _HOOK_FREE)
	{
		if (_CrtIsValidHeapPointer(userData))
		{
			pCrtHead = pHdr(userData);
			requestNumber = pCrtHead->lRequest;
			//
			if (pCrtHead->nBlockUse == _IGNORE_BLOCK)
			{
				if (pfnOldCrtAllocHook)
				{
					pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
				}
				goto END;
			}
	   		g_pMemTrace->removeMemoryTrace((void *) requestNumber, userData);
		}
	}
END:
	// unlock the function
	g_pMemTrace->m_bLocked = FALSE;
	return TRUE;
}

void CMemLeakDetect::addMemoryTrace(void* addr, size_t asize,  LPCTSTR lpszFileName, DWORD lnum)
{
	AllocBlockInfo ainfo;
	//
	if (m_AllocatedMemoryList.Lookup(addr, ainfo))
	{
		// already allocated
		AfxTrace(_T("ERROR!CMemLeakDetect::addMemoryTrace() Address(0x%p) already allocated\n"), addr);
		return;
	}
	//
	ainfo.address			= addr;
	ainfo.lineNumber	= lnum;
	ainfo.size					= asize;
	ainfo.occurance		= ++m_dwAllocTimes;
	MLD_STACKWALKER(&ainfo.traceinfo[0]);
	//
	if (lpszFileName)
		_tcsncpy_s(&ainfo.szFileName[0], MLD_MAX_NAME_LENGTH, lpszFileName, MLD_MAX_NAME_LENGTH);

	else
	  ainfo.szFileName[0] = 0;
	//
	m_AllocatedMemoryList.SetAt(addr, ainfo);
}

void CMemLeakDetect::redoMemoryTrace(void* addr,  void* oldaddr, size_t asize, LPCTSTR lpszFileName, DWORD lnum)
{
	AllocBlockInfo ainfo;

	if (m_AllocatedMemoryList.Lookup(oldaddr,(AllocBlockInfo &) ainfo))
	{
		m_AllocatedMemoryList.RemoveKey(oldaddr);
	}
	else
	{
		AfxTrace(_T("ERROR!CMemLeakDetect::redoMemoryTrace() didnt find Address(0x%08X) to free\n"), oldaddr);
	}
	//
	ainfo.address		= addr;
	ainfo.lineNumber	= lnum;
	ainfo.size			= asize;
	ainfo.occurance		= m_dwAllocTimes++;
	MLD_STACKWALKER(&ainfo.traceinfo[0]);
	//
	if (lpszFileName)
		_tcsncpy_s(&ainfo.szFileName[0], MLD_MAX_NAME_LENGTH, lpszFileName, MLD_MAX_NAME_LENGTH);
	else
	  ainfo.szFileName[0] = 0;

	m_AllocatedMemoryList.SetAt(addr, ainfo);
}

void CMemLeakDetect::removeMemoryTrace(void* addr, void* realdataptr)
{
	AllocBlockInfo ainfo;
	//
	if (m_AllocatedMemoryList.Lookup(addr,(AllocBlockInfo &) ainfo))
	{
		m_AllocatedMemoryList.RemoveKey(addr);
	}
	else
	{
	   //freeing unallocated memory
		AfxTrace(_T("ERROR!CMemLeakDetect::removeMemoryTrace() didnt find Address(0x%08X) to free\n"), addr);
	}
}

void CMemLeakDetect::cleanupMemoryTrace()
{
	m_AllocatedMemoryList.RemoveAll();
}

void CMemLeakDetect::dumpMemoryTrace()
{
	POSITION			pos;
	LPVOID				addr;
	AllocBlockInfo		ainfo;
	TCHAR					szDataBuf[MAX_PATH];
	TCHAR					szFileName[MAX_PATH];

	TCHAR					symInfo[MLD_MAX_NAME_LENGTH];
	TCHAR					srcInfo[MLD_MAX_NAME_LENGTH];
	size_t					dwTotalSize						= 0;
	int						nLeakDetected			= 0;
	STACKFRAMEENTRY*	p							= 0;
	CString strMemLeakDetected;
	ofstream myfile;

	struct tm timeinfo;
	__time64_t long_time;
	_time64(&long_time);
	// Convert to local time.
	_localtime64_s(&timeinfo, &long_time);

	TCHAR szTempPath[MAX_PATH];
	TCHAR szProcName[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, szTempPath);
	GetModuleBaseName(GetCurrentProcess(), NULL, szProcName, _countof(szProcName));

	_stprintf_s(szFileName, _countof(szFileName), _T("%smldetector-(%s)_"), szTempPath, szProcName); 
	_tcsftime(szDataBuf, _countof(szDataBuf), _T("%b%d-%Y__%H-%M-%S.log"), &timeinfo);

	_tcscat_s(szFileName, _countof(szFileName), szDataBuf);

	myfile.open(szFileName); 

	//DeleteOldTempFiles(szTempPath, _T("mldetector-(*.log"), 7);
	//DeleteFileByTime(szTempPath, _T("mldetector-(*.log"), 7);
	DeleteFileByTime(szTempPath, _T("mldetector-(*.log"), 7);

	//
	_tcscpy_s(symInfo, MLD_MAX_NAME_LENGTH, MLD_TRACEINFO_NOSYMBOL);
	_tcscpy_s(srcInfo, MLD_MAX_NAME_LENGTH, MLD_TRACEINFO_NOSYMBOL);
	//
	pos = m_AllocatedMemoryList.GetStartPosition();
	//

	while (pos != m_AllocatedMemoryList.end())
	{
		nLeakDetected++;
		_stprintf_s(szDataBuf, _countof(szDataBuf), _T("\n=========Memory Leak has been Detected(%d)=========\n"), nLeakDetected);
		AfxTrace(szDataBuf);

		strMemLeakDetected = szDataBuf;
		myfile << strMemLeakDetected;

		//
		m_AllocatedMemoryList.GetNextAssoc(pos, (LPVOID &) addr, (AllocBlockInfo&) ainfo);

		if (ainfo.szFileName[0] != NULL)
		{
			_stprintf_s(szDataBuf, _countof(szDataBuf), _T("Memory Leak <0x%p> Bytes(%d) occurance(%d) %s(%d)\n"), 
					ainfo.address, ainfo.size, ainfo.occurance, ainfo.szFileName, ainfo.lineNumber);
		}
		else
		{
			_stprintf_s(szDataBuf, _countof(szDataBuf), _T("Memory Leak <0x%p> Bytes(%d) occurance(%d)\n"), 
					ainfo.address, ainfo.size, ainfo.occurance);
		}
		//
		AfxTrace(szDataBuf);

		strMemLeakDetected = szDataBuf;
		myfile << strMemLeakDetected;

		//
		p = &ainfo.traceinfo[0];
		while(p[0].addrPC.Offset)
		{
			symFunctionInfoFromAddresses(p[0].addrPC.Offset, p[0].addrFrame.Offset, symInfo, _countof(symInfo));
			symSourceInfoFromAddress(p[0].addrPC.Offset, srcInfo, _countof(srcInfo));
			//_stprintf_s(szDataBuf, _countof(szDataBuf), _T("%s : %s()\n"), srcInfo, symInfo);
			//AfxTrace(szDataBuf);
			strMemLeakDetected.Format(_T("%s : %s()\n"), srcInfo, symInfo);
			//strMemLeakDetected = szDataBuf;
			myfile << strMemLeakDetected;

			p++;
		}
		dwTotalSize += ainfo.size;
	}
	_stprintf_s(szDataBuf, _countof(szDataBuf), _T("\n======================================================\n"));
	AfxTrace(szDataBuf);

	strMemLeakDetected = szDataBuf;
	myfile << strMemLeakDetected;

	if (dwTotalSize == 0) 
		_stprintf_s(szDataBuf, _countof(szDataBuf), _T("None Memory Leaks of %d Allocations.\n\n"), m_dwAllocTimes);
	else
		_stprintf_s(szDataBuf, _countof(szDataBuf), _T("Total %d Count Memory Leaks: %d bytes of %d Alocations.\n\n"), nLeakDetected, dwTotalSize, m_dwAllocTimes);

	AfxTrace(szDataBuf);

	strMemLeakDetected = szDataBuf;
	myfile << strMemLeakDetected;

#ifdef UNICODE
	const TCHAR *umb = _T("Unicode");
#else
	const TCHAR *umb = _T("Multibyte");
#endif
#ifdef _WIN64
	const TCHAR *w64 = _T("64 bit");
#else
	const TCHAR *w64 = _T("32 bit");
#endif
#ifdef NDEBUG
	const TCHAR *dbg = _T("release build.");
#else
	const TCHAR *dbg = _T("debug build.");
#endif
	
	_stprintf_s(szDataBuf, _countof(szDataBuf), _T("%s %s %s\n"), umb, w64, dbg);
	AfxTrace(szDataBuf);

	strMemLeakDetected = szDataBuf;
	myfile << strMemLeakDetected;

	myfile.close();
}

void CMemLeakDetect::Init()
{
	if (m_hkernel32 == NULL)
		m_hkernel32 = LoadLibrary(_T("kernel32.dll"));
	
	m_func = (CaptureStackBackTraceType)(GetProcAddress(m_hkernel32, "RtlCaptureStackBackTrace"));
	m_dwsymBufSize		= (MLD_MAX_NAME_LENGTH + sizeof(IMAGEHLP_SYMBOL));
	m_hProcess			= GetCurrentProcess();
	m_pSymbol				= (IMAGE_SYM)GlobalAlloc( GMEM_FIXED, m_dwsymBufSize);

	m_AllocatedMemoryList.InitHashTable(10211, TRUE);
	initSymInfo( NULL );
	m_bLocked				= FALSE;
	g_pMemTrace			= this;
	pfnOldCrtAllocHook	= _CrtSetAllocHook( catchMemoryAllocHook ); 
}

void CMemLeakDetect::End()
{
	m_bLocked				= TRUE;
	_CrtSetAllocHook(pfnOldCrtAllocHook);
	dumpMemoryTrace();
	cleanupMemoryTrace();
	cleanupSymInfo();
	GlobalFree(m_pSymbol);
	g_pMemTrace				= NULL;
	FreeLibrary(m_hkernel32);
}

CMemLeakDetect::CMemLeakDetect()
{
	m_dwAllocTimes = 0;
	m_hkernel32 = NULL;
	Init();
}

CMemLeakDetect::~CMemLeakDetect()
{
	End();
}

// PRIVATE STUFF
void CMemLeakDetect::symbolPaths(LPTSTR lpszSymbolPath)
{
	TCHAR lpszPath[MLD_MAX_NAME_LENGTH];

   // Creating the default path where the dgbhelp.dll is located
   // ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;"
	_tcscpy_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T(".;..\\;..\\..\\"));

	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariable(_T("_NT_SYMBOL_PATH"), lpszPath, MLD_MAX_NAME_LENGTH ))
	{
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T(";"));
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, lpszPath );
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariable( _T("_NT_ALTERNATE_SYMBOL_PATH"), lpszPath, MLD_MAX_NAME_LENGTH ))
	{
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T(";"));
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, lpszPath );
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariable( _T("SYSTEMROOT"), lpszPath, MLD_MAX_NAME_LENGTH ) )
	{
	    _tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T(";"));
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, lpszPath);
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T(";"));

		// SYSTEMROOT\System32
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, lpszPath );
		_tcscat_s( lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T("\\System32"));
	}
}

BOOL CMemLeakDetect::cleanupSymInfo()
{
	return SymCleanup( GetCurrentProcess() );
}

// Initializes the symbol files
BOOL CMemLeakDetect::initSymInfo(LPCTSTR lpszUserSymbolPath )
{
	TCHAR   lpszSymbolPath[MLD_MAX_NAME_LENGTH];
    DWORD   symOptions = SymGetOptions();

	symOptions |= SYMOPT_LOAD_LINES; 
	symOptions &= ~SYMOPT_UNDNAME;
	SymSetOptions( symOptions );

    // Get the search path for the symbol files
	symbolPaths( lpszSymbolPath);
	//
	if (lpszUserSymbolPath)
	{
		_tcscat_s(lpszSymbolPath, MLD_MAX_NAME_LENGTH, _T(";"));
		_tcscat_s(lpszSymbolPath, MLD_MAX_NAME_LENGTH, lpszUserSymbolPath);
	}

	CStringA strSymbolPath(lpszSymbolPath);

	return SymInitialize(GetCurrentProcess(), strSymbolPath, TRUE);
}

//
// This code is still under investigation
// I have to test this code and make sure it is compatible
// with the other stack walker!
//
void CMemLeakDetect::symStackTrace2(STACKFRAMEENTRY* pStacktrace )
{
	long			StackIndex				= 0;

	ADDR			block[63];
	memset(block,0,sizeof(block));

	USHORT frames = (m_func)(3,59,(void**)block,NULL);

	for (int i = 0; i < frames ; i++)
	{
		ADDR			InstructionPtr = (ADDR)block[i];

		pStacktrace[StackIndex].addrPC.Offset		= InstructionPtr;
		pStacktrace[StackIndex].addrPC.Segment	= NULL;
		pStacktrace[StackIndex].addrPC.Mode		= AddrModeFlat;
		//
		StackIndex++;
	}
	pStacktrace[StackIndex].addrPC.Offset = 0;
	pStacktrace[StackIndex].addrPC.Segment = 0;
}

BOOL CMemLeakDetect::symFunctionInfoFromAddresses(ADDR fnAddress, ADDR stackAddress, LPTSTR lpszSymbol, UINT BufSizeTCHARs)
{
	ADDR             dwDisp	= 0;

	::ZeroMemory(m_pSymbol, m_dwsymBufSize);
	m_pSymbol->SizeOfStruct		= sizeof(IMAGEHLP_LINE64);
	m_pSymbol->MaxNameLength	= m_dwsymBufSize - sizeof(IMAGEHLP_SYMBOL);

    // Set the default to unknown
	_tcscpy_s(lpszSymbol, BufSizeTCHARs, MLD_TRACEINFO_NOSYMBOL);

	// Get symbol info for IP
	if (SymGetSymFromAddr(m_hProcess, (ADDR)fnAddress, &dwDisp, m_pSymbol))
	{
		CString strFunlName(m_pSymbol->Name,m_pSymbol->MaxNameLength);
		if (m_pSymbol->MaxNameLength > BufSizeTCHARs)
		{
			_stprintf_s(lpszSymbol, BufSizeTCHARs, _T("0x%08X"), fnAddress);
			return FALSE;
		}
		
		_tcscpy_s(lpszSymbol, BufSizeTCHARs, strFunlName);
		return TRUE;
	}
	//create the symbol using the address because we have no symbol
	_stprintf_s(lpszSymbol, BufSizeTCHARs, _T("0x%08X"), fnAddress);
	return FALSE;
}

BOOL CMemLeakDetect::symSourceInfoFromAddress(ADDR address, LPTSTR lpszSourceInfo, UINT BufSizeTCHARs)
{
	BOOL           bRet = FALSE;
	IMAGE_LN	LineInfo;
	DWORD       dwDisp;
	TCHAR         szModuleInfo[MAX_PATH] = {0};

	_tcscpy_s(szModuleInfo, _countof(szModuleInfo), MLD_TRACEINFO_NOSYMBOL);

	memset(&LineInfo, NULL, sizeof( IMAGEHLP_LINE));
	LineInfo.SizeOfStruct = sizeof( IMAGEHLP_LINE);

	if (SymGetLineFromAddr(m_hProcess, address, &dwDisp, &LineInfo))
	{
		CString strFileName(LineInfo.FileName);

		//_stprintf_s(lpszSourceInfo, BufSizeTCHARs, _T("%s(%d): 0x%08X"), strFileName, LineInfo.LineNumber, address );
		_stprintf_s(lpszSourceInfo, BufSizeTCHARs, _T("%s(%d)"), strFileName, LineInfo.LineNumber);
		bRet = TRUE;
	}
	else
	{
        // Using the "modulename!address" format
	  	symModuleNameFromAddress(address, szModuleInfo, _countof(szModuleInfo));
		
		//LPCTSTR lpszFormat = _T("%s.dll::0x%08X");
		LPCTSTR lpszFormat = _T("%s.dll");
		if (szModuleInfo[0] == _T('?') || szModuleInfo[0] == _T('\0'))
			lpszFormat = _T("0x%p");

		_stprintf_s(lpszSourceInfo, BufSizeTCHARs, lpszFormat, szModuleInfo);
		
		bRet = FALSE;
	}
	//
	return bRet;
}

BOOL CMemLeakDetect::symModuleNameFromAddress(ADDR address, LPTSTR lpszModule, UINT BufSizeTCHARs)
{
	BOOL              bRet = FALSE;
	IMAGEHLP_MODULE   ModuleInfo;

	::ZeroMemory(&ModuleInfo, sizeof(IMAGEHLP_MODULE));
	ModuleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
	
	if (SymGetModuleInfo(m_hProcess, (ADDR)address, &ModuleInfo))
	{
		// Note. IMAGEHLP_MODULE::ModuleName seems to be hardcoded as 32 char/wchar_t (VS2008).
		CString strModuleName(ModuleInfo.ModuleName);

		_tcscpy_s(lpszModule, BufSizeTCHARs, strModuleName);
		bRet = TRUE;
	}
	else
	{
		_tcscpy_s(lpszModule, BufSizeTCHARs, MLD_TRACEINFO_NOSYMBOL);
	}
	
	return bRet;
}
#endif	//_DEBUG
