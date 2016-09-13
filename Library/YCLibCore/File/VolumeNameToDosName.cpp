#include "StdAfx.h"
#include "VolumeNameToDosName.h"
#include "../Utils/ErrorInfo.h"

CVolumeNameToDosName::CVolumeNameToDosName(void)
{
}

void CVolumeNameToDosName::UpdateVolumeNameInfo()
{
	DWORD dwLogicalDrives = GetLogicalDrives();
	for(int i=0;i<26;i++)
	{
		if (((1 << i) & dwLogicalDrives) == 0)
			continue;

		TCHAR szDriveLetter[] = _T("A:");
		szDriveLetter[0] += i;

		TCHAR szDeviceName[MAX_PATH];
		if (QueryDosDevice(szDriveLetter , szDeviceName , _countof(szDeviceName)) == FALSE)
			continue;

		BOOL bAddNewItem = TRUE;

		POSITION pos = m_VolumeNameList.GetHeadPosition();
		while(pos)
		{
			VOLUME_NAME_INFO& VolumeNameInfo = m_VolumeNameList.GetNext(pos);
			if (VolumeNameInfo.strVolumeName.CompareNoCase(szDeviceName) == 0)
			{
				VolumeNameInfo.szDriveLetter = 'A' + i;
				bAddNewItem = FALSE;
				break;
			}
		}

		if (bAddNewItem)
		{
			VOLUME_NAME_INFO VolumeNameInfo;
			VolumeNameInfo.szDriveLetter = 'A' + i;
			VolumeNameInfo.strVolumeName = szDeviceName;
			m_VolumeNameList.AddTail(VolumeNameInfo);
		}
	}
}

BOOL CVolumeNameToDosName::ConvertVolumeNameToDosName(LPCTSTR lpszVolumeNamePath , CString& strDosName)
{
	BOOL bSuccess = FALSE;
	CString strDosPath;

	//检查是否为网络格式
	if (_tcsnicmp(lpszVolumeNamePath , _T("\\Device\\LanManRedirector") , 24) == 0)
	{
		strDosPath = lpszVolumeNamePath;
		strDosPath.Delete(1 ,23);
		strDosName = strDosPath;
		return TRUE;
	}
	else if (_tcsnicmp(lpszVolumeNamePath , _T("\\Device\\Mup") , 11) == 0)
	{
		strDosPath = lpszVolumeNamePath;
		strDosPath.Delete(1 ,10);
		strDosName = strDosPath;
		return TRUE;
	}

	POSITION pos = m_VolumeNameList.GetHeadPosition();
	while(pos)
	{
		const VOLUME_NAME_INFO& VolumeNameInfo = m_VolumeNameList.GetNext(pos);
		DWORD dwVolumeNameLength = VolumeNameInfo.strVolumeName.GetLength();
		if (_tcslen(lpszVolumeNamePath) < dwVolumeNameLength)
			continue;
		else if (_tcsnicmp(lpszVolumeNamePath , VolumeNameInfo.strVolumeName , dwVolumeNameLength))
			continue;
		else if (lpszVolumeNamePath[dwVolumeNameLength] != 0 && lpszVolumeNamePath[dwVolumeNameLength] != '\\')
			continue;

		strDosPath.Format(_T("%c:%s") , VolumeNameInfo.szDriveLetter , lpszVolumeNamePath + dwVolumeNameLength);
		strDosName = strDosPath;

		bSuccess = TRUE;
		break;
	}		

	return bSuccess;
}

BOOL CVolumeNameToDosName::ConvertUnknownVolumeNameToDosName(LPCTSTR lpszVolumeNamePath , CString& strDosName)
{
	if (_tcsnicmp(lpszVolumeNamePath , _T("\\Device\\HarddiskVolume") , 22))
		return FALSE;
	lpszVolumeNamePath = _tcschr(lpszVolumeNamePath + 22, '\\');
	if (lpszVolumeNamePath == NULL)
		lpszVolumeNamePath = _T("");

	DWORD dwLogicalDrives = GetLogicalDrives();
	for (int i=2;i<26;i++)
	{
		if ((1 << i) & dwLogicalDrives)
			continue;

		CString strDosPath;
		strDosPath.Format(_T("%c:%s") , 'A'+i , lpszVolumeNamePath);
		strDosName = strDosPath;
		return TRUE;
	}

	return FALSE;
}