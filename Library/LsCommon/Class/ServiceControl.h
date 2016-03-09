#pragma once


#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#include <afxtempl.h>
// �������������Ϣ
typedef struct tagSERVICEINFO {
	CString  strSvrName;
	CString  strDisplayName;
	CString  strPath;
	CString  strDiscruption;
	DWORD    dwPID;
	DWORD    dwRunStatus;
	DWORD    dwStartType;
	DWORD    dwType;	
} SERVICEINFO, *LPSERVICEINFO;
typedef CList<SERVICEINFO, SERVICEINFO> CServiceList;

typedef struct _LS_SERVICE_CONFIG
{
	DWORD  dwServiceType;
	DWORD  dwStartType;
	DWORD  dwErrorControl;
	CString strBinaryPathName;
	CString strLoadOrderGroup;
	DWORD  dwTagId;
	CString strDependencies;
	CString strServiceStartName;
	CString strDisplayName;
	DWORD dwCurrentState;
}LS_SERVICE_CONFIG,*PLS_SERVICE_CONFIG;

//==================================================================
//�� �� ��: StopServiceByName
//��    ��: �رշ���
//�������: [in] lpServiceName, ��������
//�� �� ֵ: ����(BOOL), �رշ���ɹ�����TRUE,���򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL StopServiceByName(LPCTSTR lpServiceName);

//==================================================================
//�� �� ��: StartServiceByName
//��    ��: ��������
//�������: [in] lpServiceName, ��������
//�� �� ֵ: ����(BOOL),�����������ɹ�����TRUE,ʧ�ܷ���FALSE
//==================================================================
LS_COMMON_API BOOL StartServiceByName(LPCTSTR lpServiceName);

//==================================================================
//�� �� ��: RestartServiceByName
//��    ��: ����ָ������
//�������:  lpServiceName, ��������
//				   dwTimeout		�ȴ�����״̬����Ϊ���������ڷ��ص�ʱ��
//											Ĭ�ϲ��ȴ�
//											-1����ʾ���ȴ�
//�� �� ֵ: ����(BOOL),�����������ɹ�����TRUE,ʧ�ܷ���FALSE
//       ע���ȴ�����ֹͣ�������10s�Ͳ��ٵȴ�����ֹͣ�����˳�ѭ����ѯ����״̬
//==================================================================
LS_COMMON_API BOOL RestartServiceByName(LPCTSTR lpServiceName,DWORD dwTimeout = 0);

//==================================================================
//�� �� ��: GetServiceCount
//��    ��: Ӧ�ò���������
//�������: ��
//�� �� ֵ: ����(DWORD), ���صõ���Ӧ�ò���������
//==================================================================       
LS_COMMON_API DWORD GetServiceCount();                             

//==================================================================
//�� �� ��: GetCoreServiceCount
//��    ��: ���Ĳ���������
//�������: ��
//�� �� ֵ: ����(DWORD), ���صõ��ĺ��Ĳ���������
//==================================================================
LS_COMMON_API DWORD GetCoreServiceCount();  

//==================================================================
//�� �� ��: IsServiceOpen
//��    ��: �ж�ָ�������Ƿ���
//�������: [in] lpServiceName, ������
//�� �� ֵ: ����(BOOL), �����������TRUE,����FALSE
//==================================================================
LS_COMMON_API BOOL IsServiceOpen(LPCTSTR lpServiceName);  

//==================================================================
//�� �� ��: GetServiceInfo
//��    ��: ��ȡӦ�ò������Ϣ������������浽CServiceList����
//�������: [out] serList, ����Ӧ�ò������Ϣ������
//�� �� ֵ: ����(BOOL), �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================                                     
LS_COMMON_API BOOL GetServiceInfo(CServiceList& serList);  

//==================================================================
//�� �� ��: GetCoreServiceInfo
//��    ��: ��ȡ���Ĳ������Ϣ������������浽CServiceList����
//�������: [out] serList, ������Ĳ������Ϣ������
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================                            
LS_COMMON_API BOOL GetCoreServiceInfo(CServiceList& serList);

//==================================================================
//�� �� ��: GetServiceConfig
//��    ��: ��ȡָ�������������Ϣ
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//			[out] ServiceConfig, ���ط�������������Ϣ
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================    
LS_COMMON_API BOOL GetServiceConfig(LPCTSTR lpszServiceName , LS_SERVICE_CONFIG& ServiceConfig);

//==================================================================
//�� �� ��: GetShareServiceDllPath
//��    ��: ��ȡ������̷���ʹ�õ�Dll·��
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//�� �� ֵ: �����ȡ�ɹ�����Dll·��,���򷵻ؿ��ַ���
//==================================================================
LS_COMMON_API CString GetShareServiceDllPath(LPCTSTR lpszServiceName);

//==================================================================
//�� �� ��: SetServiceStartTypeConfig
//��    ��: ���÷�����������
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//			[in] dwStartType,ָ���������������
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================
LS_COMMON_API BOOL SetServiceStartTypeConfig(LPCTSTR lpszServiceName,DWORD dwStartType);
