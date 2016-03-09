#pragma once


#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#include <afxtempl.h>
// 保存服务的相关信息
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
//函 数 名: StopServiceByName
//功    能: 关闭服务
//输入参数: [in] lpServiceName, 服务名称
//返 回 值: 类型(BOOL), 关闭服务成功返回TRUE,否则返回FALSE
//==================================================================
LS_COMMON_API BOOL StopServiceByName(LPCTSTR lpServiceName);

//==================================================================
//函 数 名: StartServiceByName
//功    能: 启动服务
//输入参数: [in] lpServiceName, 服务名称
//返 回 值: 类型(BOOL),如果启动服务成功返回TRUE,失败返回FALSE
//==================================================================
LS_COMMON_API BOOL StartServiceByName(LPCTSTR lpServiceName);

//==================================================================
//函 数 名: RestartServiceByName
//功    能: 重启指定服务
//输入参数:  lpServiceName, 服务名称
//				   dwTimeout		等待服务状态更新为已启动后在返回的时间
//											默认不等待
//											-1，表示不等待
//返 回 值: 类型(BOOL),如果启动服务成功返回TRUE,失败返回FALSE
//       注：等待服务停止如果超过10s就不再等待服务停止，则退出循环查询服务状态
//==================================================================
LS_COMMON_API BOOL RestartServiceByName(LPCTSTR lpServiceName,DWORD dwTimeout = 0);

//==================================================================
//函 数 名: GetServiceCount
//功    能: 应用层服务的数量
//输入参数: 无
//返 回 值: 类型(DWORD), 返回得到的应用层服务的数量
//==================================================================       
LS_COMMON_API DWORD GetServiceCount();                             

//==================================================================
//函 数 名: GetCoreServiceCount
//功    能: 核心层服务的数量
//输入参数: 无
//返 回 值: 类型(DWORD), 返回得到的核心层服务的数量
//==================================================================
LS_COMMON_API DWORD GetCoreServiceCount();  

//==================================================================
//函 数 名: IsServiceOpen
//功    能: 判断指定服务是否开启
//输入参数: [in] lpServiceName, 服务名
//返 回 值: 类型(BOOL), 如果开启返回TRUE,否则FALSE
//==================================================================
LS_COMMON_API BOOL IsServiceOpen(LPCTSTR lpServiceName);  

//==================================================================
//函 数 名: GetServiceInfo
//功    能: 获取应用层服务信息，并将结果保存到CServiceList链表
//输入参数: [out] serList, 保存应用层服务信息的链表
//返 回 值: 类型(BOOL), 如果获取成功返回TRUE,否则FALSE
//==================================================================                                     
LS_COMMON_API BOOL GetServiceInfo(CServiceList& serList);  

//==================================================================
//函 数 名: GetCoreServiceInfo
//功    能: 获取核心层服务信息，并将结果保存到CServiceList链表
//输入参数: [out] serList, 保存核心层服务信息的链表
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================                            
LS_COMMON_API BOOL GetCoreServiceInfo(CServiceList& serList);

//==================================================================
//函 数 名: GetServiceConfig
//功    能: 获取指定服务的配置信息
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//			[out] ServiceConfig, 返回服务器的配置信息
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================    
LS_COMMON_API BOOL GetServiceConfig(LPCTSTR lpszServiceName , LS_SERVICE_CONFIG& ServiceConfig);

//==================================================================
//函 数 名: GetShareServiceDllPath
//功    能: 获取共享进程服务使用的Dll路径
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//返 回 值: 如果获取成功返回Dll路径,否则返回空字符串
//==================================================================
LS_COMMON_API CString GetShareServiceDllPath(LPCTSTR lpszServiceName);

//==================================================================
//函 数 名: SetServiceStartTypeConfig
//功    能: 设置服务启动类型
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//			[in] dwStartType,指定服务的启动类型
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================
LS_COMMON_API BOOL SetServiceStartTypeConfig(LPCTSTR lpszServiceName,DWORD dwStartType);
