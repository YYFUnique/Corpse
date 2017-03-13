#pragma once
#include "StdPtr.h"
#include <Winsvc.h>

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// �������������Ϣ
typedef struct tagSERVICEINFO {
	CString      strSvrName;
	CString      strDisplayName;
	CString      strPath;
	CString		  strDiscruption;
	DWORD    dwPID;
	DWORD    dwRunStatus;
	DWORD    dwStartType;
	DWORD    dwType;	
} SERVICEINFO, *LPSERVICEINFO;
typedef CYjList<SERVICEINFO, const SERVICEINFO&> CServiceList;

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
	DWORD dwPID;
}LS_SERVICE_CONFIG,*PLS_SERVICE_CONFIG;

//==================================================================
//�� �� ��: StopServiceByName
//��    ��: �رշ���
//�������: [in] lpServiceName, ��������
//              [in] bWait,   �Ƿ�ȴ�������ȫֹͣ
//�� �� ֵ: ����(BOOL), �رշ���ɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL StopServiceByName(LPCTSTR lpServiceName , BOOL bWait = FALSE);

DLL_API BOOL StopDependentsServiceByName(LPCTSTR lpServiceName , BOOL bWait);

//==================================================================
//��������InstallDriver
//��   �ܣ���װ��������
//�������: [in] lpszFilePath, �����ļ�·��
//						 lpszDrvName ������������
//�� �� ֵ: BOOL, ��������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL InstallDriver(LPCTSTR lpszFilePath, LPCTSTR lpszDrvName);

//==================================================================
//��������DelServiceByName
//���ܣ� ɾ������
//�������: [in] lpServiceName, ��������
//�� �� ֵ: ����(BOOL), ɾ������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL DelServiceByName(LPCTSTR lpServiceName);

//==================================================================
//�� �� ��: StartServiceByName
//��    ��: ��������
//�������: [in] lpServiceName, ��������
//                 [in] bWait,   �Ƿ�ȴ������������ɹ�״̬
//�� �� ֵ: ����(BOOL),�����������ɹ�����TRUE,ʧ�ܷ���FALSE
//==================================================================
DLL_API BOOL StartServiceByName(LPCTSTR lpServiceName,BOOL bWait = TRUE);

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
DLL_API BOOL RestartServiceByName(LPCTSTR lpServiceName,DWORD dwTimeout = 0);

//==================================================================
//�� �� ��: GetServiceCount
//��    ��: Ӧ�ò���������
//�������: ��
//�� �� ֵ: ����(DWORD), ���صõ���Ӧ�ò���������
//==================================================================       
DLL_API DWORD GetServiceCount();                             

//==================================================================
//�� �� ��: GetCoreServiceCount
//��    ��: ���Ĳ���������
//�������: ��
//�� �� ֵ: ����(DWORD), ���صõ��ĺ��Ĳ���������
//==================================================================
DLL_API DWORD GetCoreServiceCount();  

//==================================================================
//�� �� ��: IsServiceRunning
//��    ��: �ж�ָ�������Ƿ���������
//�������: [in] lpServiceName, ������
//�� �� ֵ: ����(BOOL), ����������з���TRUE,����FALSE
//==================================================================
DLL_API BOOL IsServiceRunning(LPCTSTR lpServiceName);  

//==================================================================
//�� �� ��: GetServiceInfo
//��    ��: ��ȡӦ�ò������Ϣ������������浽CServiceList����
//�������: [out] serList, ����Ӧ�ò������Ϣ������
//�� �� ֵ: ����(BOOL), �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================                                     
DLL_API BOOL GetServiceInfo(CServiceList& SrvList);  

//==================================================================
//�� �� ��: GetCoreServiceInfo
//��    ��: ��ȡ���Ĳ������Ϣ������������浽CServiceList����
//�������: [out] serList, ������Ĳ������Ϣ������
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================                            
DLL_API BOOL GetCoreServiceInfo(CServiceList& SrvList);

//==================================================================
//�� �� ��: GetServiceConfig
//��    ��: ��ȡָ�������������Ϣ
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//			[out] ServiceConfig, ���ط�������������Ϣ
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================    
DLL_API BOOL GetServiceConfig(LPCTSTR lpszServiceName , LS_SERVICE_CONFIG& ServiceConfig);

//==================================================================
//�� �� ��: GetServiceDependencies
//��    ��: ��ȡָ�������������
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//			      [out] strSrvDependencies, ���ط����������
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================    
DLL_API BOOL GetServiceDependencies(LPCTSTR lpszServiceName , CStdArray& strSrvDependencies);

//==================================================================
//�� �� ��: GetShareServiceDllPath
//��    ��: ��ȡ������̷���ʹ�õ�Dll·��
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//�� �� ֵ: �����ȡ�ɹ�����Dll·��,���򷵻ؿ��ַ���
//==================================================================
DLL_API CString GetShareServiceDllPath(LPCTSTR lpszServiceName);

//==================================================================
//�� �� ��: SetServiceStartTypeConfig
//��    ��: ���÷�����������
//�������: [in] lpszServiceName,ָ����Ҫ��ȡ������Ϣ�ķ�������
//			[in] dwStartType,ָ���������������
//�� �� ֵ: �����ȡ�ɹ�����TRUE,����FALSE
//==================================================================
DLL_API BOOL SetServiceStartTypeConfig(LPCTSTR lpszServiceName,DWORD dwStartType);

//==================================================================
//��������SetServiceConfig
//��    �ܣ����÷���������Ϣ
//���������[in]lpszServiceName,ָ����Ҫ����������Ϣ�ķ�������
//                   [in]dwServiceType��ָ����������                            (ȱʡ����Ϊ�����иò���������)
//                   [in]dwStartType��ָ��������������                        (ȱʡ����Ϊ�����иò���������)
//                   [in]dwErrorControl��ָ���������ȼ�                    (ȱʡ����Ϊ�����иò���������)
//                   [in]lpBinaryPathName��ָ�������Ӧ�ļ���·��   (ȱʡ����Ϊ�����иò���������)
//                   [in]lpDisplayName��������ʾ����                            (ȱʡ����Ϊ�����иò���������)
//                   [in]lpLoadOrderGroup������������                        (ȱʡ����Ϊ�����иò���������)
//                   [in]lpdwTagId�����������е�id                                 (ȱʡ����Ϊ�����иò���������)
//                   [in]lpDependencies������������                              (ȱʡ����Ϊ�����иò���������)
//                   [in]lpServiceStartName��ʹ�÷�����û���             (ȱʡ����Ϊ�����иò���������)
//                   [in]lpPassword��ʹ�÷�����û�����                         (ȱʡ����Ϊ�����иò���������)
//����ֵ��������óɹ�����TRUE�����򷵻�FALSE
//===================================================================
DLL_API BOOL SetServiceConfig(LPCTSTR lpszServiceName, DWORD dwServiceType = SERVICE_NO_CHANGE, 
					  DWORD dwStartType = SERVICE_NO_CHANGE,DWORD  dwErrorControl = SERVICE_NO_CHANGE,  
					  LPCTSTR lpBinaryPathName = NULL, LPCTSTR lpDisplayName = NULL,LPCTSTR lpLoadOrderGroup = NULL,
					  LPDWORD lpdwTagId = NULL, LPCTSTR lpDependencies = NULL, LPCTSTR lpServiceStartName = NULL, LPCTSTR lpPassword = NULL);

//==================================================================
//��������ServiceIsExist
//��   �� ���жϷ����Ƿ����
//�����������in��lpszServiceName,ָ����Ҫ�жϵķ�������
//����ֵ������÷�����ڷ���TRUE�����򷵻�FALSE
//==================================================================
DLL_API BOOL ServiceIsExist(LPCTSTR lpszServiceName);

//==================================================================
//��������GetServiceFilePath
//��   �� ����ȡ��������ļ�·��
//�����������in��schService:������
//					��out��lpszPathFile:���س����ļ�·��,������Ĭ�ϳ���MAX_PATH
//����ֵ���ɹ�����TRUE�����򷵻�FALSE
//==================================================================
DLL_API BOOL GetServiceFilePath(SC_HANDLE schService, LPTSTR lpszPathFile);

//==================================================================
//��������GetServiceFilePath
//��   �� ����ȡ��������ļ�·��
//�����������in��lpszServiceName:��������
//					��out��lpszPathFile:���س����ļ�·��,������Ĭ�ϳ���MAX_PATH
//����ֵ���ɹ�����TRUE�����򷵻�FALSE
//==================================================================
DLL_API BOOL GetServiceFilePath(LPCTSTR lpszServiceName, LPTSTR lpszPathFile);

//==================================================================
//��������GetServiceFilePathEx
//��   �� ����ȡ��������ļ�·���������κ����������в�����Dll·������exe·����
//�����������in��lpszServiceName:��������
//					��out��lpszPathFile:���س����ļ�·��,������Ĭ�ϳ���MAX_PATH
//����ֵ���ɹ�����TRUE�����򷵻�FALSE
//==================================================================
DLL_API BOOL GetServiceFilePathEx(LPCTSTR lpszServiceName,LPTSTR lpszPathFile);