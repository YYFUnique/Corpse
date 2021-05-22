/*************************************************************
 Author		: David A. Jones
 File Name	: MemLeakDetect.h
 Date		: July 30, 2004
 Synopsis	:		 
			A trace memory feature for source code to trace and
			find memory related bugs. 

 Future		:
				1) Memory corruption
				2) Freeing memory without allocating
				3) Freeing memory twice
				4) Not Freeing memory at all
				5) over running memory boundardies

		July 2009: tim.s.stevens@bt.com (UNICODE/ANSI 32 bit only, more secure CRT with VS 2008).
		Feb  2011: Doug Rogers <rogers.doug@gmail.com>, igor.jambrek@gmail.com, OfekSH & tim. (Compiles as 64 & 32 bit).
		Based on http://www.codeproject.com/cpp/MemLeakDetect.asp

****************************************************************/
#pragma once
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC

#include <map>
#define _CRTBLD
#include <windows.h>
#include <..\crt\src\dbgint.h>
#include <ImageHlp.h>
#include <crtdbg.h>

#pragma comment( lib, "imagehlp.lib" )
using namespace std;
//using namespace std;
#define MLD_CUSTOMSTACKWALK			1
//
#define MLD_MAX_NAME_LENGTH			256
#define MLD_MAX_TRACEINFO					63
#define MLD_TRACEINFO_EMPTY				_T("")
#define MLD_TRACEINFO_NOSYMBOL		_T("?(?)")

#ifdef  MLD_CUSTOMSTACKWALK
#define MLD_STACKWALKER				symStackTrace2
#else
#define MLD_STACKWALKER				symStackTrace
#endif

#define AfxTrace MyTrace

typedef USHORT (WINAPI *CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);

#ifndef _WIN64
typedef DWORD ADDR;
typedef PIMAGEHLP_SYMBOL IMAGE_SYM;
typedef IMAGEHLP_LINE IMAGE_LN;
#else
typedef DWORD64 ADDR;
typedef PIMAGEHLP_SYMBOL64 IMAGE_SYM;
typedef IMAGEHLP_LINE64 IMAGE_LN;
#endif

class CMemLeakDetect
{
	public:

		typedef struct 	{
				ADDRESS				addrPC;
				ADDRESS				addrFrame;
			
		} STACKFRAMEENTRY;

		typedef struct tagAllocBlockInfo
		{
			//	Added constructor to zero memory - thanks to bugfix from OfekSH.
				tagAllocBlockInfo() { ZeroMemory(traceinfo, sizeof(traceinfo) ); }
				void*					address; 
				size_t					size;
				TCHAR					szFileName[MAX_PATH];
				DWORD				lineNumber;
				DWORD				occurance;
				STACKFRAMEENTRY		traceinfo[MLD_MAX_TRACEINFO];

		} AllocBlockInfo;

		//typedef int POSITION;
		typedef map<LPVOID, AllocBlockInfo>				KEYMAP;
		typedef map<LPVOID, AllocBlockInfo>::iterator	POSITION;
		typedef pair<LPVOID, AllocBlockInfo>			KEYVALUE;

		class CMapMem
		{
			public:

				KEYMAP			m_Map;
				POSITION		m_Pos;

				inline BOOL Lookup(LPVOID pAddr,  AllocBlockInfo& aInfo) { 

					m_Pos = m_Map.find(pAddr);
					//
					if (m_Pos == m_Map.end())
					{
						return FALSE;
					}
					//
					pAddr = m_Pos->first;
					aInfo = m_Pos->second;

					return TRUE;
				};

				inline POSITION end() { 

					return m_Map.end(); 
				};

				inline void RemoveKey(LPVOID pAddr) { 
					
					m_Map.erase(pAddr);
				};

				inline void RemoveAll() {
					m_Map.clear();
				};

				void SetAt(LPVOID pAddr, AllocBlockInfo& aInfo) {

					m_Map[pAddr] = aInfo;
				};

				inline POSITION GetStartPosition() { 
					POSITION pos = m_Map.begin(); 
					return pos;
				};

				inline void GetNextAssoc(POSITION& pos, LPVOID& rAddr, AllocBlockInfo& aInfo) {

					rAddr = pos->first;
					aInfo = pos->second;
					pos++;
				};

				void InitHashTable(int preAllocEntries, BOOL flag) 	{
					 preAllocEntries	= NULL;
					 flag				= NULL;
				};

		};

		CMemLeakDetect();
		~CMemLeakDetect();
		void Init();
		void End();
		void addMemoryTrace(void* addr,  size_t asize,  LPCTSTR lpszFileName, DWORD lnum);
		void redoMemoryTrace(void* addr,  void* oldaddr, size_t asize,  LPCTSTR lpszFileName, DWORD lnum);
		void removeMemoryTrace(void* addr, void* realdataptr);
		void cleanupMemoryTrace();
		void dumpMemoryTrace();
		//

		//CMap<LPVOID, LPVOID, AllocBlockInfo, AllocBlockInfo> m_AllocatedMemoryList;
		CMapMem		m_AllocatedMemoryList;
		DWORD			m_dwAllocTimes;
		BOOL				m_bLocked;
	//
	private:
		BOOL initSymInfo(LPCTSTR lpUserPath);
		BOOL cleanupSymInfo();
		void symbolPaths(LPTSTR lpszSymbolPaths);
		void symStackTrace(STACKFRAMEENTRY* pStacktrace);
		void symStackTrace2(STACKFRAMEENTRY* pStacktrace);
		BOOL symFunctionInfoFromAddresses(ADDR fnAddress, ADDR stackAddress, LPTSTR lpszSymbol, UINT BufSizeTCHARs);
		BOOL symSourceInfoFromAddress(ADDR address, LPTSTR lpszSourceInfo, UINT BufSizeTCHARs);
		BOOL symModuleNameFromAddress(ADDR address, LPTSTR lpszModule, UINT BufSizeTCHARs);

	private:
		HANDLE				m_hProcess;
		HMODULE			m_hkernel32;
		DWORD				m_dwsymBufSize;
		IMAGEHLP_SYMBOL*	m_pSymbol;
		CaptureStackBackTraceType m_func;
};
#else
class CMemLeakDetect
{

};
#endif _DEBUG