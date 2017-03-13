#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

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

DLL_API BOOL IsRunAsAdmin();

DLL_API BOOL IsRunAsSystem();

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

DLL_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);