#include "StdAfx.h"
#include "Hardware.h"
#include "../ErrorInfo.h"
#include <setupapi.h>
#include <devguid.h>
#include <cfgmgr32.h> 
#pragma comment(lib,"setupapi.lib")

BOOL IsClassHidden(GUID ClassGuid)
{
	BOOL bHidden = FALSE;
	HKEY hClassKey = NULL;

	if (hClassKey = SetupDiOpenClassRegKey(&ClassGuid,KEY_READ))
	{
		bHidden = (RegQueryValueEx(hClassKey,_T("NoDisplayClass"),NULL,NULL,NULL,NULL) == ERROR_SUCCESS);
		RegCloseKey(hClassKey);
	}                                 

	return bHidden;
}


BOOL GetDevInfo(GUID DevGuid,CDevInfoList& DevInfoList,BOOL bShow/* = TRUE*/)
{
	BOOL bSuccess = FALSE;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	do 
	{
		hDevInfo = SetupDiGetClassDevs(&DevGuid,NULL,NULL,DIGCF_PRESENT);
		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ�豸���ʧ��"));
			break;
		}

		BYTE lpDataBuffer[4*1024];
		LPTSTR pDataBuffer = (LPTSTR)lpDataBuffer;
		DWORD dwSize = sizeof(lpDataBuffer);
		DWORD dwRequiredSize = 0;

		SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)}; //�豸��Ϣ�ṹ��
		DWORD dwStatus;		//�豸״̬
		DWORD dwProblem;	//������
		for (int i=0;SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData);++i)
		{
			DEV_INFO DevInfo;
			DevInfo.DevInst = DeviceInfoData.DevInst;
			DevInfo.ClassGuid = DeviceInfoData.ClassGuid;

			if (CM_Get_DevNode_Status(&dwStatus, &dwProblem,DeviceInfoData.DevInst,0) != CR_SUCCESS)
				continue;

			if((dwStatus & DN_NO_SHOW_IN_DM) || IsClassHidden(DeviceInfoData.ClassGuid))
				continue;
			
			if (SetupDiGetDeviceRegistryProperty(&DevInfo.ClassGuid,&DeviceInfoData,SPDRP_CONFIGFLAGS,NULL,
				(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize) == TRUE)
				continue;

			if (SetupDiClassNameFromGuid(&DevInfo.ClassGuid,pDataBuffer,dwSize,&dwRequiredSize) == TRUE)
				DevInfo.strClassName = pDataBuffer;

			if (SetupDiGetDeviceInstanceId(hDevInfo,&DeviceInfoData,pDataBuffer,dwSize,&dwRequiredSize) == TRUE)
				DevInfo.strDevInstId = pDataBuffer;

			//��ȡ�豸����
			if (SetupDiGetDeviceRegistryProperty(hDevInfo,&DeviceInfoData,SPDRP_DEVICEDESC,
						NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize) == TRUE)
				DevInfo.strDevDesc = pDataBuffer;

			if (SetupDiGetDeviceRegistryProperty(hDevInfo,&DeviceInfoData,SPDRP_FRIENDLYNAME,
						NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize) == TRUE)
				DevInfo.strDevName = pDataBuffer;

			DevInfo.strServiceName = _T("");
			DevInfo.strDisplayName = _T("");
			DevInfo.dwFirstStartType = 0;
			DevInfo.dwBluetoothFlag = 0;

			DWORD dwStatus; //�豸״̬
			DWORD dwProblem; //������
			DWORD dwError = CM_Get_DevNode_Status(&dwStatus,&dwProblem,DeviceInfoData.DevInst,0);
			if(CR_SUCCESS != dwError)
			{
				if(DevInfo.ClassGuid==GUID_DEVCLASS_DISKDRIVE)
				{	
					DevInfo.dwStatus = -1;
					DevInfo.bHidden = FALSE;
					DevInfoList.AddTail(DevInfo);
				}
				continue;
			}

			BOOL bHiddenClass = IsClassHidden(DevInfo.ClassGuid); //�ж��Ƿ����ص��豸��
			if ((dwStatus&DN_NO_SHOW_IN_DM) || bHiddenClass) //�Ƿ������ص��豸�����豸��
				DevInfo.bHidden = TRUE;
			else
				DevInfo.bHidden = FALSE;

			//�豸������
			if ((dwStatus&DN_STARTED) && (dwProblem==0))
				DevInfo.dwStatus = DEV_ENABLE;
			//�豸�ѽ���
			else if ((dwStatus&DN_HAS_PROBLEM) && (dwProblem==CM_PROB_DISABLED))
				DevInfo.dwStatus = DEV_DISABLE;
			//�����豸
			else if ((dwStatus&DN_HAS_PROBLEM) && (dwProblem!=CM_PROB_DISABLED))
				DevInfo.dwStatus = DEV_PROBLEM;
			else
				DevInfo.dwStatus = -1;

			DevInfoList.AddTail(DevInfo);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	return bSuccess;
}

BOOL ScanfHardwareInfo()
{
	DEVINST   devInst; 
	CONFIGRET   status; 

	//�õ��豸�������ĸ���� 
	status   =   CM_Locate_DevNode(&devInst,NULL,CM_LOCATE_DEVNODE_NORMAL); 
	if (status!=CR_SUCCESS) 
	{ 
		printf( "CM_Locate_DevNode   failed:   %x\n ",status); 
		return   FALSE; 
	} 

	//CM_REENUMERATE_ASYNCHRONOUS �첽��ʽ����������Ӧ
	status   =   CM_Reenumerate_DevNode(devInst,CM_REENUMERATE_ASYNCHRONOUS); 
	if   (status   !=CR_SUCCESS) 
	{ 
		printf( "CM_Reenumerate_DevNode   failed:   %x\n ",status); 
		return   FALSE; 
	}

	return TRUE;
}