#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CProcessInfo
{
public:
	//���캯��
	CProcessInfo(void);

	//��������
	~CProcessInfo(void);

	//���ý���ID
	BOOL OpenProcess(DWORD dwProcessId);

	//��ȡ������
	BOOL GetProcessName(CString& strProcessName);

	//��ȡ�����û���
	BOOL GetUserName(CString& strProcessUserName);

	//��ֹ����
	BOOL TerminateProcess();

	//��ȡ���̾����Ϣ
	HANDLE GetProcess();

	//�����Ƿ��Թ���ԱȨ������,Ĭ���ǻ�ȡ��������Ϣ
	BOOL IsProcessRunAsAdmin();

	//�жϽ����Ƿ���SYSTEMȨ������
	BOOL IsProcessRunAsSystem();
protected:

	//�رս��̾��
	void ReleaseHandle();

	DWORD m_dwProcessId;

	HANDLE m_hProcess;
};
