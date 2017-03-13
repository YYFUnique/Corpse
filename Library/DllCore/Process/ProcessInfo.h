#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CProcessInfo
{
public:
	//构造函数
	CProcessInfo(void);

	//析构函数
	~CProcessInfo(void);

	//设置进程ID
	BOOL OpenProcess(DWORD dwProcessId);

	//获取进程名
	BOOL GetProcessName(CString& strProcessName);

	//获取进程用户名
	BOOL GetUserName(CString& strProcessUserName);

	//终止进程
	BOOL TerminateProcess();

	//获取进程句柄信息
	HANDLE GetProcess();

	//进程是否以管理员权限运行,默认是获取本进程信息
	BOOL IsProcessRunAsAdmin();

	//判断进程是否以SYSTEM权限运行
	BOOL IsProcessRunAsSystem();
protected:

	//关闭进程句柄
	void ReleaseHandle();

	DWORD m_dwProcessId;

	HANDLE m_hProcess;
};
