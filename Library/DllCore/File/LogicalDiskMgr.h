#pragma once

#include <WinIoCtl.h>

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//获取磁盘大小信息
typedef struct _PHYSICAL_DISK_SIZE
{
	ULARGE_INTEGER DiskSectors;		//磁盘扇区数
	DWORD dwBytesPerSector;			//每扇区字节数
}PHYSICAL_DISK_SIZE,*PPHYSICAL_DISK_SIZE;

class DLL_API CLogicalDiskMgr
{
public:
	CLogicalDiskMgr();
	~CLogicalDiskMgr();

public:
	
	BOOL OpenDisk(LPCTSTR lpszDiskVolumePath);
	BOOL GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents,DWORD& dwLen);

	/************************************************************************/
	/* 函数名称：Detach																				   */
	/* 函数说明：解除对象与设备的关联关系													   */
	/* 返 回 值 ：返回打开设备的设备句柄											               */
	/************************************************************************/
	HANDLE Detach();

	/************************************************************************/
	/* 函数名称：LockVolume																		   */
	/* 函数说明：锁定卷设备																			   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	BOOL LockVolume();

	/************************************************************************/
	/* 函数名称：UnlockVolume																	   */
	/* 函数说明：解除卷设备锁定状态																   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	BOOL UnlockVolume();

	/************************************************************************/
	/* 函数名称：DismountVolume																   */
	/* 函数说明：卸载卷设备																			   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	BOOL DismountVolume();
	
	/************************************************************************/
	/* 函数名称：GetSystemDiskIndex															   */
	/* 函数说明：获取系统盘所在磁盘序号														   */
	/* 返 回 值 ：返回磁盘序号															               */
	/************************************************************************/
	DWORD GetSystemDiskIndex();

	/************************************************************************/
	/* 函数名称：GetHardDiskIndexFromVolume											   */
	/* 函数说明：根据卷标获取当前卷所在磁盘												   */
	/* 输入参数：lpszDiskVolumePath	卷设备卷标								           */
	/* 返 回 值 ：返回磁盘序号															               */
	/************************************************************************/
	DWORD GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath);

public:

	/************************************************************************/
	/* 函数名称：GetDriveProperty																   */
	/* 函数说明：根据输入参数不同，可以获取磁盘不同信息							   */
	/* 输入参数：HANDLE 为打开的磁盘句柄											           */
	/*					 pDevDesc::BusType 保存了磁盘接口方式							   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	static BOOL GetDriveProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);

	/************************************************************************/
	/* 函数名称：GetPhysicalDiskSize															   */
	/* 函数说明：获取指定句柄设备磁盘大小信息											   */
	/* 输入参数：dwPhysicalIndex 要打开的磁盘序号							           */
	/*					 PhysicalDiskSize 磁盘扇区信息											   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	static BOOL GetPhysicalDiskSize(DWORD dwPhysicalIndex, PHYSICAL_DISK_SIZE& PhysicalDiskSize);

	/************************************************************************/
	/* 函数名称：DeleteDiskVolumeLnk														   */
	/* 函数说明：解除指定磁盘上所有卷的设备符号链接									   */
	/* 输入参数：dwDiskIndex 指定磁盘设备的序号								           */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	static BOOL DeleteDiskVolumeLnk(DWORD dwDiskIndex);

	//获取指定存储设备的描述信息
	static BOOL GetStorageDeviceProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
	//获取指定序号存储设备的总线类型
	static BOOL GetStorageDeviceBusType(DWORD dwDiskIndex, STORAGE_BUS_TYPE& StorageBusType);
	
	static BOOL GetDiskSerialNumber(DWORD dwDiskIndex, CString& strSerialNumber);
protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);

private:
	HANDLE m_hVolume;
};