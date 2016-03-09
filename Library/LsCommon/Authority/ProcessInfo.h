#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CProcessInfo
{
public:
	//构造函数
	CProcessInfo(void);

	//析构函数
	~CProcessInfo(void);

	//获取进程ID
	DWORD GetProcessId();

	//设置进程ID
	BOOL SetProcessId(DWORD dwProcessId);

	//获取进程名
	BOOL GetProcessName(CString& strProcessName);

	//获取进程用户名
	BOOL GetUserName(CString& strProcessUserName);

	//终止进程
	BOOL TerminateProcess();

protected:

	//关闭进程句柄
	void CloseProcessHandle();

	DWORD m_dwProcessId;

	HANDLE m_hProcess;
};
