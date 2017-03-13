#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//�ڵ�ǰ����ΪSystemȨ��ʱģ��Ϊ��ǰ�Ự�û�Ȩ��
class DLL_API CImpersonateSessionUser
{
public:
	CImpersonateSessionUser(void);

	~CImpersonateSessionUser(void);

	BOOL Impersonate();

	void RevertToSelf();

	HANDLE GetUserToken();

	//��ȡ�û�ע�����
	HKEY GetUserRegKey();

protected:
	BOOL m_bSystemUser;

	BOOL m_bNeedRevertToSelf;

	HANDLE m_hUserToken;

	HKEY m_hUserRegkey;

	CString m_strUserName;
};
