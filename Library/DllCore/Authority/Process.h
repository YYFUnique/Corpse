#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// ģ�⵱ǰ�û�Ȩ��
#define  RunFunctionAsCurrentUser() \
	CImpersonateSessionUser ImpersionateSessionUser;	\
	ImpersionateSessionUser.Impersonate()

// ģ��System�û�Ȩ��
#define  RunFunctionAsSystemUser() \
	CImpersonateSystemUser ImpersonateSystemUser; \
	ImpersonateSystemUser.Impersonate()

// ģ��TrustedInstall�û�Ȩ��
#define RunFunctionAsTrustedInstallUser() \
	CImpersonateTrustedInstallUser ImpersonateTrustedInstallUser;	\
	ImpersonateTrustedInstallUser.Impersonate()

/************************************************************************/
// �������ƣ�GetProcessUserName
// �������ܣ���ȡ�����û���
// ����������dwPID ����ID 
//					 strProcessName     �����û���
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
DLL_API BOOL GetProcessUserName(DWORD dwProcessId, CString& strProcessName);

/************************************************************************/
// �������ƣ�GetProcessCommandLine
// �������ܣ���ȡ���̵�������
// ����������dwPid ָ������
//					 strCmdLine     ָ�����̵�������
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
DLL_API BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine);

/************************************************************************/
// �������ƣ�IsRunAsAdmin
// �������ܣ��ж��Ƿ��Թ���ԱȨ������
// ������������
// ��  �� ֵ������Թ���ԱȨ������Ż�TRUE������Ż�FALSE
/************************************************************************/
DLL_API BOOL IsRunAsAdmin();

/************************************************************************/
// �������ƣ�IsRunAsSystem
// �������ܣ��ж��Ƿ��Է���Ȩ������
// ������������
// ��  �� ֵ������Է���Ȩ������Ż�TRUE������Ż�FALSE
/************************************************************************/
DLL_API BOOL IsRunAsSystem();

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

DLL_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);