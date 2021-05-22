#pragma once

class DRV_API CFileSystemFilter : public CDrvObject
{
public:
	CFileSystemFilter();
	~CFileSystemFilter();

public:
	BOOL OpenDrv();
	BOOL StartDrvService();
	BOOL StopDrvService();
	BOOL DelDrvService();
	virtual LPCTSTR GetDrvFileName();
	virtual BOOL DeviceIoControl(LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer = NULL, DWORD nOutBufferSize = 0, LPDWORD lpBytesReturned = NULL);
public:

	// 获取是否允许运行USB存储设备上的程序及脚本
	BOOL GetEnableUsbDiskExecute();
	// 设置是否允许运行USB存储设备上的程序及脚本
	BOOL SetEnableUsbDiskExecute(BOOL bEnable);
	// 查询过滤驱动是否可以停止
	BOOL GetCanStopFsDrv();
	// 设置过滤驱动是否可以停止
	BOOL SetFsFltFilterCanStop(BOOL bCanStop /*= TRUE*/);
	// 获取过滤驱动版本
	DWORD GetFsFltVersion();
	// 判断过滤驱动是否是期望版本
	BOOL FsFltVersionCorrectly();
public:	
	BOOL InstallMiniFilter();
	BOOL CreateFsFilterSvc();
	//核实服务信息，如路径，服务类型，启动类型
	BOOL VerifyFsFilterSvcInfo();
};