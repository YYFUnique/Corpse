#ifndef ERROR_INFO_H
#define ERROR_INFO_H


#ifdef LS_COMMON_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API 
#endif

typedef enum
{
	CUSTOM_ERROR = 1,				//�Զ������
	SYSTEM_ERROR,					//ϵͳ����
	WINSOCK_ERROR,					//WINSOCK����
}LS_ERROR_TYPE,*PLS_ERROR_TYPE;

#define	ERROR_FLAG_ERROR_INFO			1			//������Ϣ
#define	ERROR_FLAG_DEBUG_INFO			2			//������Ϣ
#define	ERROR_FLAG_LOG_FILE_INFO		4			//��־��Ϣ
#define	ERROR_FLAG_ALL					0xffffffff	//���з�ʽ

typedef enum
{
	LS_DEBUG_MODE_NONE,	//�ǵ���ģʽ
	LS_DEBUG_MODE_NORMAL,	//��ͨ����ģʽ
}LS_DEBUG_MODE,*PLS_DEBUG_MODE;

typedef struct _LS_ERROR_INFO
{
	DWORD dwErrorType;
	DWORD dwErrorCode;
	CString szErrorInfo;
}LS_ERROR_INFO,*PLS_ERROR_INFO;

LS_COMMON_API CString GetLastErrorInfo();

LS_COMMON_API void ReleaseThreadErrorInfo();

LS_COMMON_API void StartupProcessErrorInfo();

LS_COMMON_API void ReleaseProcessErrorInfo();

LS_COMMON_API CString& GetThreadErrorInfoString();

LS_COMMON_API BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

LS_COMMON_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

LS_COMMON_API DWORD SetErrorInfoV(LS_ERROR_INFO& LsErrorInfo , DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

LS_COMMON_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//��յ�ǰ������Ϣ
LS_COMMON_API void ResetErrorInfo();

//׷�Ӵ�����Ϣ
LS_COMMON_API void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//���õ���ģʽ
LS_COMMON_API void SetDebugMode(LS_DEBUG_MODE dwDebugMode);

//��ȡĬ����־�ļ�·��
LS_COMMON_API CString GetDefaultLogFileName();

LS_COMMON_API BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo = NULL ,...);

#define OutputStringToDebuger(lpszDebugTitle,...) OutputStringToDebugerEx(__FILE__ , __LINE__ , __FUNCTION__ , lpszDebugTitle , ## __VA_ARGS__)

LS_COMMON_API BOOL OutputStringToDebugerEx(LPCSTR lpszFileName , DWORD dwLine , LPCSTR lpszFunctionName , LPCTSTR lpszDebugInfo = NULL ,...);

#endif