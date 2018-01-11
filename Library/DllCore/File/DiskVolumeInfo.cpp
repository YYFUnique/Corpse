#include "StdAfx.h"
#include "DiskVolumeInfo.h"
#include "LogicalDiskMgr.h"
#include "../Utils/ErrorInfo.h"

CDiskVolumeInfo::CDiskVolumeInfo()
{

}

CDiskVolumeInfo::~CDiskVolumeInfo()
{

}

BOOL CDiskVolumeInfo::OpenVolume(LPCTSTR lpszDiskVolumePath)
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

BOOL CDiskVolumeInfo::GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents, DWORD& dwLen)
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

BOOL CDiskVolumeInfo::LockVolume()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(FSCTL_LOCK_VOLUME);
}

BOOL CDiskVolumeInfo::UnlockVolume()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(FSCTL_UNLOCK_VOLUME);
}

BOOL CDiskVolumeInfo::DismountVolume()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(FSCTL_DISMOUNT_VOLUME);
}

BOOL CDiskVolumeInfo::DeviceIoControl(DWORD dwIoControlCode)
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwReturnBytes = 0;
	return ::DeviceIoControl(m_hVolume, dwIoControlCode, NULL, 0, NULL, 0, &dwReturnBytes, NULL);
}

BOOL CDiskVolumeInfo::EnumAllVolume(CStdArray& strVolumeArray)
{
	BOOL bSuccess = FALSE;
	HANDLE hVolume = INVALID_HANDLE_VALUE;
	TCHAR szVolumeName[MAX_PATH];
	hVolume = FindFirstVolume(szVolumeName,_countof(szVolumeName));
	if (hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{
		TCHAR szVolumePath[MAX_PATH];
		DWORD dwLen = _countof(szVolumePath);
		if (GetVolumePathNamesForVolumeName(szVolumeName,szVolumePath,_countof(szVolumePath),&dwLen) != FALSE &&
			_tcslen(szVolumePath) != 0)
			strVolumeArray.Add(szVolumePath);

		bSuccess = FindNextVolume(hVolume,szVolumeName,_countof(szVolumeName));
	}while(bSuccess);

	return strVolumeArray.IsEmpty() == FALSE;
}