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
	/************************************************************************/
	/* 函  数  名：EnumAllVolume                                                                   */
	/* 函数功能：枚举当前系统中所有的卷设备												   */
	/* 输入参数：strVolumeArray 返回当前系统中存在的卷设备						   */
	/* 返  回 值：成功返回TRUE，失败返回FALSE											   */
	/************************************************************************/
	static BOOL EnumAllVolume(CStdArray& strVolumeArray);

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
};