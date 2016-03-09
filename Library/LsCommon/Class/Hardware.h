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
	DEVINST DevInst;			//设备实例
	DWORD dwStatus;				//当前状态
	DWORD dwFirstStartType;		//最初的服务启动类型
	BOOL bHidden;				//该设备是否隐藏
	DWORD dwBluetoothFlag;		//蓝牙驱动设备标识0:非蓝牙1：微软默认2：第三方蓝牙
	GUID ClassGuid;				//设备类GUID
	CString strDevInstId;		//设备实例ID
	CString strParentInstId;	//设备父节点实例ID
	CString strClassName;		//设备类名
	CString strDevDesc;			//设备描述
	CString strDevName;			//设备名
	CString strDevEnumerator;   //设备枚举名(总线类型)
	CString strServiceName;		//服务名
	CString strDisplayName;		//服务显示名称
}DEV_INFO,*PDEV_INFO;

typedef CList<DEV_INFO,DEV_INFO&> CDevInfoList;

#define DEV_DISABLE				0 //设备处于禁用状态
#define DEV_ENABLE				1 //设备处于启用状态
#define DEV_PROBLEM				2 //设备存在问题

LS_COMMON_API BOOL IsClassHidden(GUID ClassGuid);

LS_COMMON_API BOOL GetDevInfo(GUID DevGuid,CDevInfoList& DevInfoList,BOOL bShow = TRUE);

LS_COMMON_API BOOL ScanfHardwareInfo();