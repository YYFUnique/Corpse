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
	HANDLE Detach();
	BOOL OpenDisk(LPCTSTR lpszDiskVolumePath);
	BOOL GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents,DWORD& dwLen);

	BOOL LockVolume();
	BOOL UnlockVolume();
	BOOL DismountVolume();

	DWORD GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath);
	DWORD GetSystemDiskIndex();

	/************************************************************************/
	/* 函数名称：GetDriveProperty																   */
	/* 函数说明：根据输入参数不同，可以获取磁盘不同信息							   */
	/* 输入参数：HANDLE 为打开的磁盘句柄											           */
	/*					 pDevDesc::BusType 保存了磁盘接口方式							   */
	/* 返 回 值 ：返回TRUE表示成功，返回FALSE表示失败					               */
	/************************************************************************/
	static BOOL GetDriveProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
	static BOOL GetPhysicalDiskSize(HANDLE hPhysical, PHYSICAL_DISK_SIZE& PhysicalDiskSize);

protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);

private:
	HANDLE m_hVolume;
};