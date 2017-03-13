#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//�ڵ�ǰ����Ϊ��ͨ�û�Ȩ��ʱģ��SystemȨ��
class DLL_API CImpersonateSystemUser
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
