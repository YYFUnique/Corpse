#pragma once

#ifdef LS_STATIC_LIB_CALL
#define	LS_COMMON_API
#elif defined LS_EXPORTS
#define	LS_COMMON_API  __declspec(dllexport)
#else
#define	LS_COMMON_API  __declspec(dllimport)
#endif
#include <cfgmgr32.h>

typedef struct _DEV_INFO
{
	DEVINST DevInst;			//�豸ʵ��
	DWORD dwStatus;				//��ǰ״̬
	DWORD dwFirstStartType;		//����ķ�����������
	BOOL bHidden;				//���豸�Ƿ�����
	DWORD dwBluetoothFlag;		//���������豸��ʶ0:������1��΢��Ĭ��2������������
	GUID ClassGuid;				//�豸��GUID
	CString strDevInstId;		//�豸ʵ��ID
	CString strParentInstId;	//�豸���ڵ�ʵ��ID
	CString strClassName;		//�豸����
	CString strDevDesc;			//�豸����
	CString strDevName;			//�豸��
	CString strDevEnumerator;   //�豸ö����(��������)
	CString strServiceName;		//������
	CString strDisplayName;		//������ʾ����
}DEV_INFO,*PDEV_INFO;

typedef CList<DEV_INFO,DEV_INFO&> CDevInfoList;

#define DEV_DISABLE				0 //�豸���ڽ���״̬
#define DEV_ENABLE				1 //�豸��������״̬
#define DEV_PROBLEM				2 //�豸��������

LS_COMMON_API BOOL IsClassHidden(GUID ClassGuid);

LS_COMMON_API BOOL GetDevInfo(GUID DevGuid,CDevInfoList& DevInfoList,BOOL bShow = TRUE);

LS_COMMON_API BOOL ScanfHardwareInfo();