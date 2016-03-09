#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMONEX_API
#elif defined LS_COMMONEX_EXPORTS
#define LS_COMMONEX_API __declspec(dllexport)
#else
#define LS_COMMONEX_API 
#endif

//������Ϣ�̱߳��ش洢
typedef enum
{
	CUSTOM_ERROR = 1,			//�Զ������
	SYSTEM_ERROR,					//ϵͳ����
	WINSOCK_ERROR,				//WINSOCK����
	KERNEL_ERROR,					//NTSTATUS���͵Ĵ���
	COM_ERROR,						//COM���ص�LRESULT���󣬿���ʹ��HRESULT_CODEȡ�ô����
}LS_ERROR_TYPE,*PLS_ERROR_TYPE;

typedef struct _ERROR_INFO
{
	DWORD dwErrorType;
	DWORD dwErrorCode;
	CString szErrorInfo;
}ERROR_INFO,*PERROR_INFO;

//==================================================================
//�� �� ��: GetLastErrorInfo
//��    ��: ��ȡ��������Ϣ
//�������: ��
//�� �� ֵ: ���ش�����Ϣ�������ַ���
//==================================================================
LS_COMMONEX_API CString GetLastErrorInfo();

//==================================================================
//�� �� ��: ReleaseThreadErrorInfo
//��    ��: �ͷ��̴߳�����Ϣ�ڴ�
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMONEX_API void ReleaseThreadErrorInfo();

//==================================================================
//�� �� ��: StartupProcessErrorInfo
//��    ��: ���ô�����Ϣ����·��
//�������: [in]  lpszLogFileName, ������Ϣ��־�ļ�·��
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMONEX_API void StartupProcessErrorInfo(LPCTSTR lpszLogFileName=NULL);

//==================================================================
//�� �� ��: ReleaseProcessErrorInfo
//��    ��: �ͷŽ��̴�����Ϣ�����Դ
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMONEX_API void ReleaseProcessErrorInfo();

//==================================================================
//�� �� ��: GetThreadErrorInfoString
//��    ��: ��ȡ�̴߳�����Ϣ�ַ���
//�������: ��
//�� �� ֵ: �����̴߳�����Ϣ�ַ���������
//==================================================================
LS_COMMONEX_API CString& GetThreadErrorInfoString();

//==================================================================
//�� �� ��: SetErrorInfo
//��    ��: ���ô�����Ϣ
//�������: [in]  dwErrorType, ��������,��Ϊ:
//			SYSTEM_ERROR, ϵͳ����
//			CUSTOM_ERROR, �Զ������
//			WINSOCK_ERROR,�����е�һЩ����
//			KERNEL_ERROR,	NTϵͳ�еĴ���ʹ�øô����ʱ�򣬱��봫��NTϵͳ�Ĵ����
//			[in]  dwErrorCode, �������,�ò������Ϊ0,���Զ���ȡһ���������
//			[in]  lpszErrorTitle, ������Ϣtitle�����ַ���
//			[in]  ..., �����б�
//�� �� ֵ: ���ش������
//==================================================================
LS_COMMONEX_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//==================================================================
//�� �� ��: SetErrorInfo
//��    ��: ���ô�����Ϣ
//�������: [in]  dwErrorType, ��������,��Ϊ:
//			SYSTEM_ERROR, ϵͳ����
//			CUSTOM_ERROR, �Զ������
//			[in]  dwErrorCode, �������
//			[in]  lpszErrorTitle, ������Ϣtitle�����ַ���
//			[in]  argList, �����б�
//�� �� ֵ: ���ش������
//==================================================================
LS_COMMONEX_API DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

//==================================================================
//�� �� ��: SetErrorTitle
//��    ��: ���ô�����Ϣ��ǰ�����ʾ��Ϣ
//�������: [in]  lpszTitle, ������Ϣǰ�����ʾ��Ϣ
//			(����:"���ļ�ʧ��:��һ����������ʹ�ô��ļ��������޷�����",
//			�������õľ���ǰ���"���ļ�ʧ��"ð��֮ǰ���ַ���)
//�� �� ֵ: ���ش������
//==================================================================
LS_COMMONEX_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);
