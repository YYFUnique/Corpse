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

// 保存服务的相关信息
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
//函 数 名: StopServiceByName
//功    能: 关闭服务
//输入参数: [in] lpServiceName, 服务名称
//              [in] bWait,   是否等待服务完全停止
//返 回 值: 类型(BOOL), 关闭服务成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL StopServiceByName(LPCTSTR lpServiceName , BOOL bWait = FALSE);

DLL_API BOOL StopDependentsServiceByName(LPCTSTR lpServiceName , BOOL bWait);

//==================================================================
//函数名：InstallDriver
//功   能：安装驱动服务
//输入参数: [in] lpszFilePath, 驱动文件路径
//						 lpszDrvName 驱动服务名称
//返 回 值: BOOL, 创建服务成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL InstallDriver(LPCTSTR lpszFilePath, LPCTSTR lpszDrvName);

//==================================================================
//函数名：DelServiceByName
//功能： 删除服务
//输入参数: [in] lpServiceName, 服务名称
//返 回 值: 类型(BOOL), 删除服务成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL DelServiceByName(LPCTSTR lpServiceName);

//==================================================================
//函 数 名: StartServiceByName
//功    能: 启动服务
//输入参数: [in] lpServiceName, 服务名称
//                 [in] bWait,   是否等待服务启动至成功状态
//返 回 值: 类型(BOOL),如果启动服务成功返回TRUE,失败返回FALSE
//==================================================================
DLL_API BOOL StartServiceByName(LPCTSTR lpServiceName,BOOL bWait = TRUE);

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
DLL_API BOOL RestartServiceByName(LPCTSTR lpServiceName,DWORD dwTimeout = 0);

//==================================================================
//函 数 名: GetServiceCount
//功    能: 应用层服务的数量
//输入参数: 无
//返 回 值: 类型(DWORD), 返回得到的应用层服务的数量
//==================================================================       
DLL_API DWORD GetServiceCount();                             

//==================================================================
//函 数 名: GetCoreServiceCount
//功    能: 核心层服务的数量
//输入参数: 无
//返 回 值: 类型(DWORD), 返回得到的核心层服务的数量
//==================================================================
DLL_API DWORD GetCoreServiceCount();  

//==================================================================
//函 数 名: IsServiceRunning
//功    能: 判断指定服务是否正在运行
//输入参数: [in] lpServiceName, 服务名
//返 回 值: 类型(BOOL), 如果正在运行返回TRUE,否则FALSE
//==================================================================
DLL_API BOOL IsServiceRunning(LPCTSTR lpServiceName);  

//==================================================================
//函 数 名: GetServiceInfo
//功    能: 获取应用层服务信息，并将结果保存到CServiceList链表
//输入参数: [out] serList, 保存应用层服务信息的链表
//返 回 值: 类型(BOOL), 如果获取成功返回TRUE,否则FALSE
//==================================================================                                     
DLL_API BOOL GetServiceInfo(CServiceList& SrvList);  

//==================================================================
//函 数 名: GetCoreServiceInfo
//功    能: 获取核心层服务信息，并将结果保存到CServiceList链表
//输入参数: [out] serList, 保存核心层服务信息的链表
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================                            
DLL_API BOOL GetCoreServiceInfo(CServiceList& SrvList);

//==================================================================
//函 数 名: GetServiceConfig
//功    能: 获取指定服务的配置信息
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//			[out] ServiceConfig, 返回服务器的配置信息
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================    
DLL_API BOOL GetServiceConfig(LPCTSTR lpszServiceName , LS_SERVICE_CONFIG& ServiceConfig);

//==================================================================
//函 数 名: GetServiceDependencies
//功    能: 获取指定服务的依赖项
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//			      [out] strSrvDependencies, 返回服务的依赖项
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================    
DLL_API BOOL GetServiceDependencies(LPCTSTR lpszServiceName , CStdArray& strSrvDependencies);

//==================================================================
//函 数 名: GetShareServiceDllPath
//功    能: 获取共享进程服务使用的Dll路径
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//返 回 值: 如果获取成功返回Dll路径,否则返回空字符串
//==================================================================
DLL_API CString GetShareServiceDllPath(LPCTSTR lpszServiceName);

//==================================================================
//函 数 名: SetServiceStartTypeConfig
//功    能: 设置服务启动类型
//输入参数: [in] lpszServiceName,指定需要获取配置信息的服务名称
//			[in] dwStartType,指定服务的启动类型
//返 回 值: 如果获取成功返回TRUE,否则FALSE
//==================================================================
DLL_API BOOL SetServiceStartTypeConfig(LPCTSTR lpszServiceName,DWORD dwStartType);

//==================================================================
//函数名：SetServiceConfig
//功    能：设置服务配置信息
//输入参数：[in]lpszServiceName,指定需要设置配置信息的服务名称
//                   [in]dwServiceType，指定服务类型                            (缺省参数为不进行该参数的设置)
//                   [in]dwStartType，指定服务启动类型                        (缺省参数为不进行该参数的设置)
//                   [in]dwErrorControl，指定服务错误等级                    (缺省参数为不进行该参数的设置)
//                   [in]lpBinaryPathName，指定服务对应文件的路径   (缺省参数为不进行该参数的设置)
//                   [in]lpDisplayName，服务显示名称                            (缺省参数为不进行该参数的设置)
//                   [in]lpLoadOrderGroup，服务组名称                        (缺省参数为不进行该参数的设置)
//                   [in]lpdwTagId，服务在组中的id                                 (缺省参数为不进行该参数的设置)
//                   [in]lpDependencies，独立名称组                              (缺省参数为不进行该参数的设置)
//                   [in]lpServiceStartName，使用服务的用户名             (缺省参数为不进行该参数的设置)
//                   [in]lpPassword，使用服务的用户密码                         (缺省参数为不进行该参数的设置)
//返回值：如果设置成功返回TRUE，否则返回FALSE
//===================================================================
DLL_API BOOL SetServiceConfig(LPCTSTR lpszServiceName, DWORD dwServiceType = SERVICE_NO_CHANGE, 
					  DWORD dwStartType = SERVICE_NO_CHANGE,DWORD  dwErrorControl = SERVICE_NO_CHANGE,  
					  LPCTSTR lpBinaryPathName = NULL, LPCTSTR lpDisplayName = NULL,LPCTSTR lpLoadOrderGroup = NULL,
					  LPDWORD lpdwTagId = NULL, LPCTSTR lpDependencies = NULL, LPCTSTR lpServiceStartName = NULL, LPCTSTR lpPassword = NULL);

//==================================================================
//函数名：ServiceIsExist
//功   能 ：判断服务是否存在
//输入参数：【in】lpszServiceName,指定需要判断的服务名称
//返回值：如果该服务存在返回TRUE，否则返回FALSE
//==================================================================
DLL_API BOOL ServiceIsExist(LPCTSTR lpszServiceName);

//==================================================================
//函数名：GetServiceFilePath
//功   能 ：获取服务程序文件路径
//输入参数：【in】schService:服务句柄
//					【out】lpszPathFile:返回程序文件路径,缓冲区默认长度MAX_PATH
//返回值：成功返回TRUE，否则返回FALSE
//==================================================================
DLL_API BOOL GetServiceFilePath(SC_HANDLE schService, LPTSTR lpszPathFile);

//==================================================================
//函数名：GetServiceFilePath
//功   能 ：获取服务程序文件路径
//输入参数：【in】lpszServiceName:服务名称
//					【out】lpszPathFile:返回程序文件路径,缓冲区默认长度MAX_PATH
//返回值：成功返回TRUE，否则返回FALSE
//==================================================================
DLL_API BOOL GetServiceFilePath(LPCTSTR lpszServiceName, LPTSTR lpszPathFile);

//==================================================================
//函数名：GetServiceFilePathEx
//功   能 ：获取服务程序文件路径（不带任何命令行运行参数的Dll路径或者exe路径）
//输入参数：【in】lpszServiceName:服务名称
//					【out】lpszPathFile:返回程序文件路径,缓冲区默认长度MAX_PATH
//返回值：成功返回TRUE，否则返回FALSE
//==================================================================
DLL_API BOOL GetServiceFilePathEx(LPCTSTR lpszServiceName,LPTSTR lpszPathFile);