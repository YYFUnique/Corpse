#pragma once
#include "../Utils/StdPtr.h"
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
	/************************************************************************/
	/* 函数名称：OpenDisk																			   */
	/* 函数说明：使用磁盘序号打开磁盘															   */
	/* 返 回 值 ：打开磁盘成功返回TRUE，否则返回FALSE					               */
	/************************************************************************/
	BOOL OpenDisk(UINT nPhycialDiskIndex);

	/************************************************************************/
	/* 函数名称：OpenDisk																			   */
	/* 函数说明：使用通过SetupAPI获取的磁盘名称来打开磁盘						   */
	/* 返 回 值 ：打开磁盘成功返回TRUE，否则返回FALSE					               */
	/************************************************************************/
	BOOL OpenDisk(LPCTSTR lpszDiskVolumePath);

	/************************************************************************/
	/* 函数名称：Detach																				   */
	/* 函数说明：解除对象与设备的关联关系													   */
	/* 返 回 值 ：返回打开设备的设备句柄											               */
	/************************************************************************/
	HANDLE Detach();
	
	/************************************************************************/
	/* 函数名称：GetDriveProperty																   */
	/* 函数说明：根据输入参数不同，可以获取磁盘不同信息							   */
	/* 输入参数：HANDLE 为打开的磁盘句柄											           */
	/*					 pDevDesc::BusType 保存了磁盘接口方式							   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	BOOL GetDriveProperty(PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);

	/************************************************************************/
	/* 函数名称：FlushDiskDev																		   */
	/* 函数说明：刷新磁盘管理器，资源管理器重新加载磁盘							   */
	/* 输入参数：无									 											           */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	BOOL FlushDiskDev();
public:

	/************************************************************************/
	/* 函数名称：GetSystemDiskIndex															   */
	/* 函数说明：获取系统盘所在磁盘序号														   */
	/* 返 回 值 ：返回磁盘序号															               */
	/************************************************************************/
	static DWORD GetSystemDiskIndex();

	/************************************************************************/
	/* 函数名称：GetHardDiskIndexFromVolume											   */
	/* 函数说明：根据卷标获取当前卷所在磁盘												   */
	/* 输入参数：lpszDiskVolumePath	卷设备卷标								           */
	/* 返 回 值 ：返回磁盘序号															               */
	/************************************************************************/
	static DWORD GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath);

	/************************************************************************/
	/* 函数名称：DeleteDiskVolumeLnk														   */
	/* 函数说明：解除指定磁盘上所有卷的设备符号链接									   */
	/* 输入参数：dwDiskIndex 指定磁盘设备的序号								           */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	static BOOL DeleteDiskVolumeLnk(DWORD dwDiskIndex);

	/************************************************************************/
	/* 函数名称：GetPhysicalDiskSize															   */
	/* 函数说明：获取指定句柄设备磁盘大小信息											   */
	/* 输入参数：dwPhysicalIndex 要打开的磁盘序号							           */
	/*					 PhysicalDiskSize 磁盘扇区信息											   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	static BOOL GetPhysicalDiskSize(DWORD dwPhysicalIndex, PHYSICAL_DISK_SIZE& PhysicalDiskSize);

	//获取指定存储设备的描述信息
	static BOOL GetStorageDeviceProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
	//获取指定序号存储设备的总线类型
	static BOOL GetStorageDeviceBusType(DWORD dwDiskIndex, STORAGE_BUS_TYPE& StorageBusType);
	
	static BOOL GetDiskSerialNumber(DWORD dwDiskIndex, CString& strSerialNumber);

	/************************************************************************/
	/* 函  数  名：EnumVolumeForDisk                                                           */
	/* 函数功能：枚举指定磁盘中所有的卷设备												   */
	/* 输入参数：dwPhysicalDiskNumber  指定磁盘序号								   */
	/*					 ullStartingOffset 卷在磁盘中的开始偏移								   */
	/*					 ullExtentLength	卷的扩展长度											   */
	/*					 strVolumeArray 返回指定磁盘中存在的卷设备						   */
	/* 返  回 值：成功返回TRUE，失败返回FALSE											   */
	/************************************************************************/
	static BOOL EnumVolumeForDisk(DWORD dwPhysicalDiskNumber , ULONGLONG ullStartingOffset , ULONGLONG ullExtentLength , CStdArray& strVolumeArray);

private:
	HANDLE m_hStorageDevice;
};