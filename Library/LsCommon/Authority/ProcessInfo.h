#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CProcessInfo
{
public:
	//���캯��
	CProcessInfo(void);

	//��������
	~CProcessInfo(void);

	//��ȡ����ID
	DWORD GetProcessId();

	//���ý���ID
	BOOL SetProcessId(DWORD dwProcessId);

	//��ȡ������
	BOOL GetProcessName(CString& strProcessName);

	//��ȡ�����û���
	BOOL GetUserName(CString& strProcessUserName);

	//��ֹ����
	BOOL TerminateProcess();

protected:

	//�رս��̾��
	void CloseProcessHandle();

	DWORD m_dwProcessId;

	HANDLE m_hProcess;
};
