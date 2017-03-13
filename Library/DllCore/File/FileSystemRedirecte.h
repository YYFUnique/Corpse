#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef BOOL (WINAPI* pfnWow64DisableWow64FsRedirection)(LPVOID *); 
typedef BOOL (WINAPI* pfnWow64RevertWow64FsRedirection)(LPVOID); 

typedef enum tagOSType
{
	OSTYPENULL = 0,	//进程类型未定义
	OSTYPEx86	   = 1,	//x86类型
	OSTYPEx64	   = 2,	//x64类型
}OSType;

class DLL_API CFileSystemRedirection
{
public:
	CFileSystemRedirection(BOOL bDisableRedirection);
	~CFileSystemRedirection();
	BOOL Init();
	BOOL DisableFileSystemRedirection();
	BOOL RevertFileSystemRedirection();

protected:
	BOOL  m_bDisableRedirection;
	PVOID m_pOldValue;

	static OSType m_OSType; 
	static BOOL m_bInited;
	static pfnWow64DisableWow64FsRedirection pWow64DisableWow64FsRedirection;
	static pfnWow64RevertWow64FsRedirection pWow64RevertWow64FsRedirection;  
};