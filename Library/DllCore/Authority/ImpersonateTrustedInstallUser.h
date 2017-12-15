#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//��ȡTrustedInstall�û����ļ�����Ȩ��
class DLL_API CImpersonateTrustedInstallUser
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
