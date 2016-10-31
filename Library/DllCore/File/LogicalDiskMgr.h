#pragma once

#include <WinIoCtl.h>

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

class YCLIB_CORE_API CLogicalDiskMgr
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
protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);
private:
	HANDLE m_hVolume;
};