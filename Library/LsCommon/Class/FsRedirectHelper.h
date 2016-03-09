#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CFsRedirectHelper
{
public:
	CFsRedirectHelper(BOOL bSet);
	~CFsRedirectHelper();
	BOOL Init();
	BOOL DisableFsRedirect();
	void RevertFsRedirect();

	BOOL m_bSet;
	PVOID m_pOldValue;

protected:
	typedef int (__stdcall * Wow64DisableWow64FsRedirection)(LPVOID *); 
	typedef int (__stdcall * Wow64RevertWow64FsRedirection)(LPVOID); 

	static char bIs64; 
	static BOOL bInited;
	static Wow64DisableWow64FsRedirection f_Wow64DisableWow64FsRedirection;
	static Wow64RevertWow64FsRedirection f_Wow64RevertWow64FsRedirection;  
};