#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//在当前进程为System权限时模拟为当前会话用户权限
class DLL_API CImpersonateSessionUser
{
public:
	CImpersonateSessionUser(void);

	~CImpersonateSessionUser(void);

	BOOL Impersonate();

	void RevertToSelf();

	HANDLE GetUserToken();

	//获取用户注册表句柄
	HKEY GetUserRegKey();

protected:
	BOOL m_bSystemUser;

	BOOL m_bNeedRevertToSelf;

	HANDLE m_hUserToken;

	HKEY m_hUserRegkey;

	CString m_strUserName;
};
