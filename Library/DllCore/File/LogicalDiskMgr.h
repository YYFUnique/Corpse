#pragma once

#include <WinIoCtl.h>

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

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
protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);
private:
	HANDLE m_hVolume;
};