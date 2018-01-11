#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "../Utils/StdPtr.h"

class DLL_API CDiskVolumeInfo
{
public:
	CDiskVolumeInfo();
	~CDiskVolumeInfo();

public:

	BOOL OpenVolume(LPCTSTR lpszDiskVolumePath);

	/************************************************************************/
	/* 函  数  名：GetDiskExtents	                                                                   */
	/* 函数功能：获取卷对应的磁盘信息															   */
	/* 输入参数：lpVolumeDiskExtents 卷对应的磁盘信息								   */
	/*					 dwLen 指针内容长度															   */
	/* 返  回 值：成功返回TRUE，失败返回FALSE											   */
	/************************************************************************/
	BOOL GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents,DWORD& dwLen);

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

public:
	/************************************************************************/
	/* 函  数  名：EnumAllVolume                                                                   */
	/* 函数功能：枚举当前系统中所有的卷设备												   */
	/* 输入参数：strVolumeArray 返回当前系统中存在的卷设备						   */
	/* 返  回 值：成功返回TRUE，失败返回FALSE											   */
	/************************************************************************/
	static BOOL EnumAllVolume(CStdArray& strVolumeArray);
protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);

protected:
	HANDLE m_hVolume;
};