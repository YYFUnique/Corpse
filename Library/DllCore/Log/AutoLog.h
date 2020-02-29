#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CAutoLog
{
public:
	CAutoLog(LPCTSTR lpszModuleName, LPCTSTR lpszFuncName);
	~CAutoLog();
protected:
	CString m_strModuleName;
	CString m_strFuncName;
};