#include "StdAfx.h"
#include "DiskVolumeInfo.h"
#include "LogicalDiskMgr.h"

CDiskVolumeInfo::CDiskVolumeInfo()
{

}

CDiskVolumeInfo::~CDiskVolumeInfo()
{

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

BOOL CDiskVolumeInfo::EnumVolumeForDisk(DWORD dwPhysicalDiskNumber , ULONGLONG ullStartingOffset , ULONGLONG ullExtentLength , CStdArray& strVolumeArray)
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (EnumAllVolume(strVolumeArray) == FALSE)
			break;

		int nItemCount = (int)strVolumeArray.GetCount();
		for(int i=nItemCount-1;i>=0;i--)
		{
			const CString& strVolume = strVolumeArray.GetAt(i);

			CLogicalDiskMgr LogicalDisk;
			if (LogicalDisk.OpenDisk(strVolume) == FALSE)
			{
				strVolumeArray.RemoveAt(i);
				continue;
			}

			BYTE DiskInfoBuffer[sizeof(DWORD) + 100 * sizeof(DISK_EXTENT )];
			PVOLUME_DISK_EXTENTS pVolumeDiskExtents = (PVOLUME_DISK_EXTENTS)DiskInfoBuffer;
			pVolumeDiskExtents->NumberOfDiskExtents = 100;

			DWORD dwSize = _countof(DiskInfoBuffer);

			if (LogicalDisk.GetDiskExtents(pVolumeDiskExtents, dwSize) == FALSE)
			{
				strVolumeArray.RemoveAt(i);
				continue;
			}

			DWORD j;
			for(j=0;j<pVolumeDiskExtents->NumberOfDiskExtents;j++)
			{
				if (pVolumeDiskExtents->Extents[j].DiskNumber != dwPhysicalDiskNumber)
					continue;

				if (ullExtentLength == 0)
					break;

				if ((ULONGLONG)pVolumeDiskExtents->Extents[j].StartingOffset.QuadPart >= ullStartingOffset &&
					(ULONGLONG)pVolumeDiskExtents->Extents[j].StartingOffset.QuadPart < ullStartingOffset + ullExtentLength)
					break;

				if ((ULONGLONG)pVolumeDiskExtents->Extents[j].StartingOffset.QuadPart + pVolumeDiskExtents->Extents[j].ExtentLength.QuadPart > ullStartingOffset &&
					(ULONGLONG)pVolumeDiskExtents->Extents[j].StartingOffset.QuadPart + pVolumeDiskExtents->Extents[j].ExtentLength.QuadPart <= ullStartingOffset + ullExtentLength)
					break;
			}

			if (j == pVolumeDiskExtents->NumberOfDiskExtents)
				strVolumeArray.RemoveAt(i);
		}

		return TRUE;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}