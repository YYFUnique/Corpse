#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "../Utils/StdPtr.h"

typedef struct _LS_OS_USER_INFO
{
	CString strUserName;
	CString strFullName;
	CString strComment;
	DWORD dwFlags;
	union
	{
		SID sidUserSid;
		BYTE UserSidBuffer[SECURITY_MAX_SID_SIZE];
	};
}LS_OS_USER_INFO,*PLS_OS_USER_INFO;

typedef CYjList<LS_OS_USER_INFO , const LS_OS_USER_INFO&> CLsOsUserInfoList;

//==================================================================
//�� �� ��: ConvertUserSidStringToName
//��    ��: ת���û�SID�ַ������û���
//�������: [in]  lpszUserSid, �û�SID
//			[out]  strUserName, �����û���
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL ConvertUserSidStringToName(LPCTSTR lpszUserSid , CString& strUserName);

//==================================================================
//�� �� ��: GetUsersBelongingToTheGroup
//��    ��: ��ȡ�û������û���
//�������: [in]  lpszUserName, �û�����
//			[out]  strGroupArray, �û�������
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL GetUsersBelongingToTheGroup(LPCTSTR lpszUserName , CStdArray& strGroupArray);

//==================================================================
//�� �� ��: IsUserInAdminGroup
//��    ��: �ж��û��Ƿ�����Administrators��
//�������: [in]  lpszUserName, �û�����
//�� �� ֵ: ���ڹ���Ա�鷵��TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL IsUserInAdminGroup(LPCTSTR lpszUserName);

//==================================================================
//�� �� ��: EnumOsUserInfo
//��    ��: ö�ٵ�ǰ����ϵͳ�û���Ϣ
//�������: [out]  OsUserInfoArray, �û���Ϣ����
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL EnumOsUserInfo(CLsOsUserInfoList& OsUserInfoArray);

//==================================================================
//�� �� ��: LsQueryUserToken
//��    ��: ��ȡָ���Ự���û�������Ϣ(��װWTSQueryUserToken����,�Ա���2000���ܹ���������)
//�������: [out]  OsUserInfoArray, �û���Ϣ����
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL LsQueryUserToken(DWORD dwSessionId , PHANDLE phHandle);