#pragma once

class DemoWrapperLib : public Wrapper
{
public:
	//===========================================
	//			function variable
	//===========================================
	BOOL (WINAPI *TMPSMPCL_Init)(__out HANDLE* handle);
public:
	BOOL Initialize(LPCTSTR lpszModuleName);
	BOOL Uninitialize(void);
	DECLARE_TMCOMMON_LIBRARY_WRAPPER(DemoWrapperLib)
};

IMPL_INLINE_TMCOMMON_LIBRARY_WRAPPER(DemoWrapperLib)
	PROC_TMCOMMON_LIBRARY_WRAPPER(TMPSMPCL_Init, "TMPSMPCL_Init")
END_TMCOMMON_LIBRARY_WRAPPER()