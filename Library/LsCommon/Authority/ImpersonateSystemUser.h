#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//在当前进程为普通用户权限时模拟System权限
class LS_COMMON_API CImpersonateSystemUser
{
public:
	CImpersonateSystemUser(void);

	~CImpersonateSystemUser(void);

	BOOL Impersonate();

	void RevertToSelf();

	static DWORD FindWinlogonProcess();

	static HANDLE DuplicateProcessToken(DWORD dwProcessId);

protected:
	BOOL m_bSystemUser;

	BOOL m_bNeedRevertToSelf;
};
