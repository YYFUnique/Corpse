#include "StdAfx.h"
#include "LogicalDiskMgr.h"
#include "../Utils/ErrorInfo.h"

CLogicalDiskMgr::CLogicalDiskMgr()
{
	m_hVolume = INVALID_HANDLE_VALUE;
}

CLogicalDiskMgr::~CLogicalDiskMgr()
{
	if (m_hVolume != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hVolume);
		m_hVolume = INVALID_HANDLE_VALUE;
	}
}

BOOL CLogicalDiskMgr::OpenDisk(LPCTSTR lpszDiskVolumePath)
{
	BOOL bSuccess = FALSE;
	do 
	{
		TCHAR szVolumeName[MAX_PATH];
		_stprintf_s(szVolumeName, _countof(szVolumeName), _T("\\\\.\\%c:"), lpszDiskVolumePath[0]);

		m_hVolume = CreateFile(szVolumeName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
		if (m_hVolume == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("打开%s失败"), szVolumeName);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

HANDLE CLogicalDiskMgr::Detach()
{
	HANDLE hLogicalDisk = m_hVolume;
	m_hVolume = INVALID_HANDLE_VALUE;

	return hLogicalDisk;
}

BOOL CLogicalDiskMgr::GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents, DWORD& dwLen)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_hVolume == INVALID_HANDLE_VALUE)
			break;

		BOOL bRet = ::DeviceIoControl(m_hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0,
																lpVolumeDiskExtents,dwLen, &dwLen, NULL);
		if (bRet == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("获取设备扩展信息失败"));
			break;
		}
		
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CLogicalDiskMgr::GetDriveProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	BOOL bSuccess = FALSE;
	ASSERT(hDevice);

	do 
	{
		if (hDevice == INVALID_HANDLE_VALUE)
			break;

		// 查询输入参数
		STORAGE_PROPERTY_QUERY Query;    
		// IOCTL输出数据长度
		DWORD dwOutBytes;               

		// 指定查询方式
		Query.PropertyId  = StorageDeviceProperty;
		Query.QueryType = PropertyStandardQuery;

		// 用IOCTL_STORAGE_QUERY_PROPERTY取设备属性信息
		bSuccess = ::DeviceIoControl(hDevice,																// 设备句柄
													IOCTL_STORAGE_QUERY_PROPERTY,						// 获取设备属性信息
													&Query, sizeof(STORAGE_PROPERTY_QUERY),		// 输入数据缓冲区
													pDevDesc, pDevDesc->Size,									// 输出数据缓冲区
													&dwOutBytes,															// 输出数据长度
													(LPOVERLAPPED)NULL);											// 用同步I/O    

	} while (FALSE);

	return bSuccess;
}

BOOL CLogicalDiskMgr::GetPhysicalDiskSize(HANDLE hPhysical, PHYSICAL_DISK_SIZE& PhysicalDiskSize)
{
	BOOL bSuccess = FALSE;	
	DWORD dwBytesReturned;

	ASSERT(hPhysical);
	do 
	{
		if (hPhysical == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开磁盘设备失败"));
			break;
		}

		DISK_GEOMETRY_EX  GeoEx;
		if (::DeviceIoControl(hPhysical, 
				IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, 
				&GeoEx, sizeof(GeoEx), &dwBytesReturned, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取可移动设备物理总大小失败"));
			break;
		}

		if (GeoEx.Geometry.BytesPerSector == 0)
			GeoEx.Geometry.BytesPerSector = 512;

		PhysicalDiskSize.dwBytesPerSector = GeoEx.Geometry.BytesPerSector;
		PhysicalDiskSize.DiskSectors.QuadPart = GeoEx.DiskSize.QuadPart / GeoEx.Geometry.BytesPerSector;

		bSuccess = TRUE;

	} while (FALSE);

	return bSuccess;
}

DWORD CLogicalDiskMgr::GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath)
{
	DWORD dwHardDiskIndex = -1;
	do 
	{
		//如果打开卷设备失败，直接返回-1
		if (OpenDisk(lpszDiskVolumePath) == FALSE)
			break;

		BYTE lpData[1024];
		PVOLUME_DISK_EXTENTS lpVolumeDiskExtents = (PVOLUME_DISK_EXTENTS)lpData;
		DWORD dwSize = _countof(lpData);
		if (GetDiskExtents(lpVolumeDiskExtents,dwSize) == FALSE)
			break;

		if (lpVolumeDiskExtents->NumberOfDiskExtents != 0)
			dwHardDiskIndex = lpVolumeDiskExtents->Extents[0].DiskNumber;

	} while (FALSE);
	
	return dwHardDiskIndex;
}

DWORD CLogicalDiskMgr::GetSystemDiskIndex()
{
	DWORD dwIndex = -1;
	do 
	{
		TCHAR szWindows[MAX_PATH];
		if (GetSystemWindowsDirectory(szWindows, sizeof(szWindows)) == 0)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("获取Windows目录失败"));
			break;
		}

		if (PathStripToRoot(szWindows) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("获取Windows目录所在更目录失败"));
			break;
		}

		dwIndex = GetHardDiskIndexFromVolume(szWindows);
		if (dwIndex == -1)
		{
			SetErrorTitle(_T("获取Windows所在硬盘序号失败"));
			break;
		}

	} while (FALSE);

	return dwIndex;
}

BOOL CLogicalDiskMgr::DeviceIoControl(DWORD dwIoControlCode)
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwReturnBytes = 0;
	return ::DeviceIoControl(m_hVolume,dwIoControlCode,NULL,0,NULL,0,&dwReturnBytes,NULL);
}

BOOL CLogicalDiskMgr::LockVolume()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(FSCTL_LOCK_VOLUME);
}

BOOL CLogicalDiskMgr::UnlockVolume()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(FSCTL_UNLOCK_VOLUME);
}

BOOL CLogicalDiskMgr::DismountVolume()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(FSCTL_DISMOUNT_VOLUME);
}