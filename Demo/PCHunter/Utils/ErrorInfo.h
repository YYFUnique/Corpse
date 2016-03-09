#pragma once

// ԭ��LanGuardError�еĴ�����
#define ERROR_LG_SUCCESS				0				// �ɹ�
#define ERROR_LG_LOAD_PROFILE			1				// �������ļ��м��ع���ʧ��
#define ERROR_LG_SAVE_PROFILE			2				// ������������ļ�ʧ��
#define ERROR_LG_CHECK_PROFILE			3				// ���������ļ�����
#define ERROR_LG_INVALID_PARAMETER		4				// ��Ч�Ĺ������
#define ERROR_LG_SCM					5				// �򿪷��������ʧ��
#define ERROR_LG_SERVICE				6				// ��������ʧ��
#define ERROR_LG_MEMORY					7				// �����ڴ�ʧ��
#define ERROR_LG_INVALID_PATH_OR_FILE	8				// �Ƿ����ļ�·��
#define ERROR_LG_INVALID_HANDLE			9				// �Ƿ����������
#define ERROR_LG_STARTING				10				// ������������ʧ��
#define ERROR_LG_STOPPING				11				// ֹͣ��������ʧ��
#define ERROR_LG_REMOVING				12				// ɾ�������������ʧ��
#define ERROR_LG_IO						13				// ����������ʧ��
#define ERROR_LG_NO_INITIALIZED			14				// û�г�ʼ��
#define ERROR_LG_ALREADY_INITIALIZED	15				// �Ѿ���ʼ��
#define ERROR_LG_NULL_POINTER			16				// ָ���ǿ�ָ��
#define ERROR_LG_GET_LOCAL_ADDRESS		17				// ��ȡ������ַʧ��
#define ERROR_LG_GET_GATE_WAY			18				// ��ȡĬ������ʧ��
#define ERROR_LG_REMOVE_NET_RULE		19				// ɾ���������ʧ��
#define ERROR_LG_SET_NET_RULE			20				// �����������ʧ��

#ifndef NT_SUCCESS
#define NTSTATUS LONG
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
CString GetLastErrorInfo();

//==================================================================
//�� �� ��: ReleaseThreadErrorInfo
//��    ��: �ͷ��̴߳�����Ϣ�ڴ�
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
void ReleaseThreadErrorInfo();

//==================================================================
//�� �� ��: StartupProcessErrorInfo
//��    ��: ���ô�����Ϣ����·��
//�������: [in]  lpszLogFileName, ������Ϣ��־�ļ�·��
//�� �� ֵ: �޷���ֵ
//==================================================================
void StartupProcessErrorInfo(LPCTSTR lpszLogFileName=NULL);

//==================================================================
//�� �� ��: ReleaseProcessErrorInfo
//��    ��: �ͷŽ��̴�����Ϣ�����Դ
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
void ReleaseProcessErrorInfo();

//==================================================================
//�� �� ��: GetThreadErrorInfoString
//��    ��: ��ȡ�̴߳�����Ϣ�ַ���
//�������: ��
//�� �� ֵ: �����̴߳�����Ϣ�ַ���������
//==================================================================
CString& GetThreadErrorInfoString();

//==================================================================
//�� �� ��: IsSpecialError
//��    ��: �жϵ�ǰ�����Ƿ�Ϊָ���Ĵ���
//�������: [in]  dwErrorType, ��������,��Ϊ:
//			SYSTEM_ERROR, ϵͳ����
//			CUSTOM_ERROR, �Զ������
//			[in]  dwErrorCode, �������
//�� �� ֵ: ��ָ���Ĵ��󷵻�TRUE,����FALSE 
//==================================================================
BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

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
DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

DWORD RtlNtStatusToDosError(NTSTATUS NtStatus);

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
DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

//==================================================================
//�� �� ��: SetErrorTitle
//��    ��: ���ô�����Ϣ��ǰ�����ʾ��Ϣ
//�������: [in]  lpszTitle, ������Ϣǰ�����ʾ��Ϣ
//			(����:"���ļ�ʧ��:��һ����������ʹ�ô��ļ��������޷�����",
//			�������õľ���ǰ���"���ļ�ʧ��"ð��֮ǰ���ַ���)
//�� �� ֵ: ���ش������
//==================================================================
DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//==================================================================
//�� �� ��: ResetErrorInfo
//��    ��: ��յ�ǰ�Ĵ�����Ϣ
//�������: ��
//�� �� ֵ: ��
//==================================================================
void ResetErrorInfo();

//==================================================================
//�� �� ��: AppendErrorInfo
//��    ��: ׷�Ӵ�����Ϣ,ÿ�ε��ò����Ĵ������Իس����з�Ϊ�ָ�
//�������: �μ�SetErrorInfo����
//�� �� ֵ: ��
//==================================================================
void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//==================================================================
//�� �� ��: WriteErrorInfoToLogFile
//��    ��: �Ѵ�����Ϣд��ָ������־�ļ�
//�������: [in]  lpszLogInfo, д����־����Ϣ
//�� �� ֵ: д����־�ļ��ɹ�����TRUE, ���򷵻�FALSE
//==================================================================
BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo=NULL,...);

//==================================================================
//�� �� ��: GetLogFileName
//��    ��: ��ȡ��־�ļ�·��,(����C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\�����ն˰�ȫ����ϵͳ\Log\AssetMgr(2009-09-25).log)
//�������: ��
//�� �� ֵ: ���ػ�ȡ������־�ļ���·��
//==================================================================
CString GetLogFileName();

//==================================================================
//�� �� ��: GetAppDataFileName
//��    ��: ��ȡ��־�ļ�·��,(����C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\�����ն˰�ȫ����ϵͳ\"lpszDirName"\�ļ���."lpszExtendName"
//�������: 
//				lpszDirName			Ҫ��ȡ��lpszDirName�ļ���·��
//				lpszExtendName		��ȡlpszExtendName���͵��ļ�
//				lpszVersion				�������ϵͳ������汾��
//�� �� ֵ: ���ػ�ȡ������־�ļ���·��
//==================================================================
CString GetLogFilePath(LPCTSTR lpszDirName,LPCTSTR lpszExtendName,LPCTSTR lpszVersion = NULL);

//==================================================================
//�� �� ��: SetLogFileCodeType
//��    ��: ������־�ļ����뷽ʽ
//�������: [in]  dwCodeType
//�� �� ֵ: ���ñ��뷽ʽ�ɹ�����TRUE, �����־�ļ��Ѿ����򷵻�FALSE
//==================================================================
BOOL SetLogFileCodeType(DWORD dwCodeType);

extern DWORD g_dwLastError;

CString GetLogModuleFileName();