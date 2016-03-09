#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

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

//���Ҫ�жϵ��ַ����ǿ�
#ifdef _DEBUG
#define ASSERT_STR_NOT_NULL(lpszstr)  if((lpszstr) == NULL || _tcslen(lpszstr) == 0){ASSERT(FALSE);}
#else
#define ASSERT_STR_NOT_NULL(lpszstr) 
#endif

//==================================================================
//�� �� ��: GetLastErrorInfo
//��    ��: ��ȡ��������Ϣ
//�������: ��
//�� �� ֵ: ���ش�����Ϣ�������ַ���
//==================================================================
LS_COMMON_API CString GetLastErrorInfo();

//==================================================================
//�� �� ��: ReleaseThreadErrorInfo
//��    ��: �ͷ��̴߳�����Ϣ�ڴ�
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void ReleaseThreadErrorInfo();

//==================================================================
//�� �� ��: StartupProcessErrorInfo
//��    ��: ���ô�����Ϣ����·��
//�������: [in]  lpszLogFileName, ������Ϣ��־�ļ�·��
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void StartupProcessErrorInfo(LPCTSTR lpszLogFileName=NULL);

//==================================================================
//�� �� ��: ReleaseProcessErrorInfo
//��    ��: �ͷŽ��̴�����Ϣ�����Դ
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void ReleaseProcessErrorInfo();

//==================================================================
//�� �� ��: GetThreadErrorInfoString
//��    ��: ��ȡ�̴߳�����Ϣ�ַ���
//�������: ��
//�� �� ֵ: �����̴߳�����Ϣ�ַ���������
//==================================================================
LS_COMMON_API CString& GetThreadErrorInfoString();

//==================================================================
//�� �� ��: IsSpecialError
//��    ��: �жϵ�ǰ�����Ƿ�Ϊָ���Ĵ���
//�������: [in]  dwErrorType, ��������,��Ϊ:
//			SYSTEM_ERROR, ϵͳ����
//			CUSTOM_ERROR, �Զ������
//			[in]  dwErrorCode, �������
//�� �� ֵ: ��ָ���Ĵ��󷵻�TRUE,����FALSE 
//==================================================================
LS_COMMON_API BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

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
LS_COMMON_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

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
LS_COMMON_API DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

//==================================================================
//�� �� ��: SetErrorTitle
//��    ��: ���ô�����Ϣ��ǰ�����ʾ��Ϣ
//�������: [in]  lpszTitle, ������Ϣǰ�����ʾ��Ϣ
//			(����:"���ļ�ʧ��:��һ����������ʹ�ô��ļ��������޷�����",
//			�������õľ���ǰ���"���ļ�ʧ��"ð��֮ǰ���ַ���)
//�� �� ֵ: ���ش������
//==================================================================
LS_COMMON_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//==================================================================
//�� �� ��: ResetErrorInfo
//��    ��: ��յ�ǰ�Ĵ�����Ϣ
//�������: ��
//�� �� ֵ: ��
//==================================================================
LS_COMMON_API void ResetErrorInfo();

//==================================================================
//�� �� ��: AppendErrorInfo
//��    ��: ׷�Ӵ�����Ϣ,ÿ�ε��ò����Ĵ������Իس����з�Ϊ�ָ�
//�������: �μ�SetErrorInfo����
//�� �� ֵ: ��
//==================================================================
LS_COMMON_API void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//==================================================================
//�� �� ��: WriteErrorInfoToLogFile
//��    ��: �Ѵ�����Ϣд��ָ������־�ļ�
//�������: [in]  lpszLogInfo, д����־����Ϣ
//�� �� ֵ: д����־�ļ��ɹ�����TRUE, ���򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo=NULL,...);

//==================================================================
//�� �� ��: GetLogFileName
//��    ��: ��ȡ��־�ļ�·��,(����C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\�����ն˰�ȫ����ϵͳ\Log\AssetMgr(2009-09-25).log)
//�������: ��
//�� �� ֵ: ���ػ�ȡ������־�ļ���·��
//==================================================================
LS_COMMON_API CString GetLogFileName();

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
LS_COMMON_API CString GetLogFilePath(LPCTSTR lpszDirName,LPCTSTR lpszExtendName,LPCTSTR lpszVersion = NULL);

//==================================================================
//�� �� ��: SetLogFileCodeType
//��    ��: ������־�ļ����뷽ʽ
//�������: [in]  dwCodeType
//�� �� ֵ: ���ñ��뷽ʽ�ɹ�����TRUE, �����־�ļ��Ѿ����򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL SetLogFileCodeType(DWORD dwCodeType);

//==================================================================
//�� �� ��: LanGuardSetLastError
//��    ��: LanGuardError�еĴ�����
//�������: [in]  dwError,�������
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void LanGuardSetLastError(DWORD dwError);

//==================================================================
//�� �� ��: LanGuardGetLastError
//��    ��: ��ȡLangard�����������
//�������: ��
//�� �� ֵ: �������Ĵ������
//==================================================================
LS_COMMON_API DWORD LanGuardGetLastError();

//==================================================================
//�� �� ��: LanGuardGetLastErrorString
//��    ��: ��ȡLangard�������������Ӧ�������ַ���
//�������: ��
//�� �� ֵ: ���ش������������ַ���
//==================================================================
LS_COMMON_API LPCTSTR LanGuardGetLastErrorString();

extern DWORD g_dwLastError;

LS_COMMON_API CString GetLogModuleFileName();