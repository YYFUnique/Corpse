#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//在当前进程为System权限时模拟为当前会话用户权限
class LS_COMMON_API CImpersonateSessionUser
{
public:
	CImpersonateSessionUser(void);

	~CImpersonateSessionUser(void);

	BOOL Impersonate();

	void RevertToSelf();

protected:
	BOOL m_bSystemUser;

	BOOL m_bNeedRevertToSelf;
};
