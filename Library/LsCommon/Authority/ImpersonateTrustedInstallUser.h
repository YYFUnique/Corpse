#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//获取TrustedInstall用户的文件管理权限
class LS_COMMON_API CImpersonateTrustedInstallUser
{
public:
	CImpersonateTrustedInstallUser(void);

	~CImpersonateTrustedInstallUser(void);

	BOOL Impersonate(LPCTSTR lpszFilePath,LPCTSTR lpszGroupName);

	BOOL ChangeFileName(LPCTSTR lpszFileName);

	void RevertToSelf();

protected:
	BOOL m_bSystemUser;

	BOOL m_bNeedRevertToSelf;
};
