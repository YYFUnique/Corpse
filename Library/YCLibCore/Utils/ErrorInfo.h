#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

#include <atlstr.h>

#ifndef NT_SUCCESS
#define NTSTATUS LONG
#endif
//

typedef ULONG (WINAPI *LPFN_RtlDosErrorFromNtStatus)(IN NTSTATUS Status);

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
	CString   strErrorInfo;
}ERROR_INFO,*PERROR_INFO;

//==================================================================
//�� �� ��: GetLastErrorInfo
//��    ��: ��ȡ��������Ϣ
//�������: ��
//�� �� ֵ: ���ش�����Ϣ�������ַ���
//==================================================================
YCLIB_CORE_API CString GetLastErrorInfo();

//==================================================================
//�� �� ��: ReleaseThreadErrorInfo
//��    ��: �ͷ��̴߳�����Ϣ�ڴ�
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
YCLIB_CORE_API void ReleaseThreadErrorInfo();

//==================================================================
//�� �� ��: ReleaseProcessErrorInfo
//��    ��: �ͷŽ��̴�����Ϣ�����Դ
//�������: ��
//�� �� ֵ: �޷���ֵ
//==================================================================
YCLIB_CORE_API void ReleaseProcessErrorInfo();

//==================================================================
//�� �� ��: GetThreadErrorInfoString
//��    ��: ��ȡ�̴߳�����Ϣ�ַ���
//�������: ��
//�� �� ֵ: �����̴߳�����Ϣ�ַ���������
//==================================================================
YCLIB_CORE_API CString& GetThreadErrorInfoString();

//==================================================================
//�� �� ��: IsSpecialError
//��    ��: �жϵ�ǰ�����Ƿ�Ϊָ���Ĵ���
//�������: [in]  dwErrorType, ��������,��Ϊ:
//			SYSTEM_ERROR, ϵͳ����
//			CUSTOM_ERROR, �Զ������
//			[in]  dwErrorCode, �������
//�� �� ֵ: ��ָ���Ĵ��󷵻�TRUE,����FALSE 
//==================================================================
YCLIB_CORE_API BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

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
YCLIB_CORE_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

YCLIB_CORE_API DWORD RtlNtStatusToDosError(NTSTATUS NtStatus);

//==================================================================
//�� �� ��: SetErrorInfo
//��    ��: ���ô�����Ϣ
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
YCLIB_CORE_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//==================================================================
//�� �� ��: ResetErrorInfo
//��    ��: ��յ�ǰ�Ĵ�����Ϣ
//�������: ��
//�� �� ֵ: ��
//==================================================================
YCLIB_CORE_API void ResetErrorInfo();

//==================================================================
//�� �� ��: AppendErrorInfo
//��    ��: ׷�Ӵ�����Ϣ,ÿ�ε��ò����Ĵ������Իس����з�Ϊ�ָ�
//�������: �μ�SetErrorInfo����
//�� �� ֵ: ��
//==================================================================
YCLIB_CORE_API void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

CString GetLogModuleFileName();