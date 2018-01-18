#include "StdAfx.h"
#include "LogicalDiskMgr.h"
#include "DiskVolumeInfo.h"
#include "../Utils/ErrorInfo.h"
#include "../Utils/StdPtr.h"

#pragma comment(lib,"Crypt32.lib")

CLogicalDiskMgr::CLogicalDiskMgr()
{
	m_hStorageDevice = INVALID_HANDLE_VALUE;
}

CLogicalDiskMgr::~CLogicalDiskMgr()
{
	if (m_hStorageDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hStorageDevice);
		m_hStorageDevice = INVALID_HANDLE_VALUE;
	}
}

//Device Name=\\?\USBSTOR#Disk&Ven_ADATA&Prod_USB_Flash_Drive&Rev_1.00#1108171000000095&0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}
BOOL CLogicalDiskMgr::OpenDisk(LPCTSTR lpszDiskPath)
{
	BOOL bSuccess = FALSE;
	do 
	{
		m_hStorageDevice = CreateFile(lpszDiskPath, NULL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING,0,NULL);
		if (m_hStorageDevice == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("��%sʧ��"), lpszDiskPath);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CLogicalDiskMgr::OpenDisk(UINT nPhycialDiskIndex)
{
	BOOL bSuccess = FALSE;
	do 
	{
		TCHAR szPhysicalDiskName[MAX_PATH];
		_stprintf_s(szPhysicalDiskName, _countof(szPhysicalDiskName), _T("\\\\.\\PhysicalDrive%d"), nPhycialDiskIndex);

		m_hStorageDevice = CreateFile(szPhysicalDiskName, GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
		if (m_hStorageDevice == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("��%sʧ��"), szPhysicalDiskName);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

HANDLE CLogicalDiskMgr::Detach()
{
	HANDLE hLogicalDisk = m_hStorageDevice;
	m_hStorageDevice = INVALID_HANDLE_VALUE;

	return hLogicalDisk;
}

BOOL CLogicalDiskMgr::GetDriveProperty(PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (m_hStorageDevice == INVALID_HANDLE_VALUE)
			break;

		// ��ѯ�������
		STORAGE_PROPERTY_QUERY Query;    
		// IOCTL������ݳ���
		DWORD dwOutBytes;               

		// ָ����ѯ��ʽ
		Query.PropertyId  = StorageDeviceProperty;
		Query.QueryType = PropertyStandardQuery;

		// ��IOCTL_STORAGE_QUERY_PROPERTYȡ�豸������Ϣ
		bSuccess = ::DeviceIoControl(m_hStorageDevice,																// �豸���
													IOCTL_STORAGE_QUERY_PROPERTY,						// ��ȡ�豸������Ϣ
													&Query, sizeof(STORAGE_PROPERTY_QUERY),		// �������ݻ�����
													pDevDesc, pDevDesc->Size,									// ������ݻ�����
													&dwOutBytes,															// ������ݳ���
													(LPOVERLAPPED)NULL);											// ��ͬ��I/O    

	} while (FALSE);

	return bSuccess;
}

BOOL CLogicalDiskMgr::GetPhysicalDiskSize(DWORD dwPhysicalIndex, PHYSICAL_DISK_SIZE& PhysicalDiskSize)
{
	BOOL bSuccess = FALSE;	
	HANDLE hPhysical = INVALID_HANDLE_VALUE;

	do 
	{
		TCHAR szPhysicalDrive[MAX_PATH];
		_stprintf_s(szPhysicalDrive, _countof(szPhysicalDrive), _T("\\\\.\\PHYSICALDRIVE%d"), dwPhysicalIndex);
		hPhysical = CreateFile(szPhysicalDrive, GENERIC_READ|GENERIC_WRITE, 
											FILE_SHARE_READ|FILE_SHARE_WRITE,
											NULL, OPEN_EXISTING, 0, NULL);
		if (hPhysical == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪴����豸ʧ��"));
			break;
		}

		DWORD dwBytesReturned;
		DISK_GEOMETRY_EX  DiskGeoEx;
		if (::DeviceIoControl(hPhysical, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, 
											&DiskGeoEx, sizeof(DISK_GEOMETRY_EX), &dwBytesReturned, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ���ƶ��豸�����ܴ�Сʧ��"));
			break;
		}

		if (DiskGeoEx.Geometry.BytesPerSector == 0)
			DiskGeoEx.Geometry.BytesPerSector = 512;

		PhysicalDiskSize.dwBytesPerSector = DiskGeoEx.Geometry.BytesPerSector;
		PhysicalDiskSize.DiskSectors.QuadPart = DiskGeoEx.DiskSize.QuadPart / DiskGeoEx.Geometry.BytesPerSector;

		bSuccess = TRUE;

	} while (FALSE);

	if (hPhysical != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hPhysical);
		hPhysical = INVALID_HANDLE_VALUE;
	}

	return bSuccess;
}

BOOL CLogicalDiskMgr::EnumVolumeForDisk(DWORD dwPhysicalDiskNumber , ULONGLONG ullStartingOffset , ULONGLONG ullExtentLength , CStdArray& strVolumeArray)
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (CDiskVolumeInfo::EnumAllVolume(strVolumeArray) == FALSE)
			break;

		int nItemCount = (int)strVolumeArray.GetCount();
		for(int i=nItemCount-1;i>=0;i--)
		{
			const CString& strVolume = strVolumeArray.GetAt(i);

			CDiskVolumeInfo DiskVolume;
			if (DiskVolume.OpenVolume(strVolume) == FALSE)
			{
				strVolumeArray.RemoveAt(i);
				continue;
			}

			BYTE DiskInfoBuffer[sizeof(DWORD) + 100 * sizeof(DISK_EXTENT )];
			PVOLUME_DISK_EXTENTS pVolumeDiskExtents = (PVOLUME_DISK_EXTENTS)DiskInfoBuffer;
			pVolumeDiskExtents->NumberOfDiskExtents = 100;

			DWORD dwSize = _countof(DiskInfoBuffer);

			if (DiskVolume.GetDiskExtents(pVolumeDiskExtents, dwSize) == FALSE)
			{
				strVolumeArray.RemoveAt(i);
				continue;
			}

			DWORD dwIndex = 0;
			for (dwIndex=0; dwIndex<pVolumeDiskExtents->NumberOfDiskExtents; ++dwIndex)
			{
				if (pVolumeDiskExtents->Extents[dwIndex].DiskNumber != dwPhysicalDiskNumber)
					continue;

				if (ullExtentLength == 0)
					break;

				if ((ULONGLONG)pVolumeDiskExtents->Extents[dwIndex].StartingOffset.QuadPart >= ullStartingOffset &&
					(ULONGLONG)pVolumeDiskExtents->Extents[dwIndex].StartingOffset.QuadPart < ullStartingOffset + ullExtentLength)
					break;

				if ((ULONGLONG)pVolumeDiskExtents->Extents[dwIndex].StartingOffset.QuadPart + pVolumeDiskExtents->Extents[dwIndex].ExtentLength.QuadPart > ullStartingOffset &&
					(ULONGLONG)pVolumeDiskExtents->Extents[dwIndex].StartingOffset.QuadPart + pVolumeDiskExtents->Extents[dwIndex].ExtentLength.QuadPart <= ullStartingOffset + ullExtentLength)
					break;
			}

			if (dwIndex == pVolumeDiskExtents->NumberOfDiskExtents)
				strVolumeArray.RemoveAt(i);
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CLogicalDiskMgr::DeleteDiskVolumeLnk(DWORD dwDiskIndex)
{
	BOOL bSuccess = FALSE;
	do 
	{
		DWORD dwIndex = CLogicalDiskMgr::GetSystemDiskIndex();
		if (dwIndex == dwDiskIndex || dwDiskIndex == -1)
			break;

		//ö�ٵ�ǰ�ƶ��洢�豸�����о��豸������������Ȼ��ж���豸
		PHYSICAL_DISK_SIZE PhysicalDisk;
		if (CLogicalDiskMgr::GetPhysicalDiskSize(dwDiskIndex, PhysicalDisk) == FALSE)
		{
			SetErrorTitle(_T("��ȡ���̴�Сʧ��"));
			break;
		}

		LARGE_INTEGER DiskSize;
		DiskSize.QuadPart = PhysicalDisk.DiskSectors.QuadPart*PhysicalDisk.dwBytesPerSector;

		CStdArray strVolumeArray;
		if (CLogicalDiskMgr::EnumVolumeForDisk(dwDiskIndex, 0, DiskSize.QuadPart, strVolumeArray) != FALSE)
		{
			int nIndex = 0;
			DWORD dwVolumeCount = (DWORD)strVolumeArray.GetCount();
			CString strVolumeName;
			for(DWORD i=0;i<dwVolumeCount;i++)
			{
				strVolumeName.Format(_T("%s"), strVolumeArray.GetAt(i));
				DefineDosDevice(DDD_REMOVE_DEFINITION, strVolumeName.TrimRight(_T("\\")),NULL);
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

DWORD CLogicalDiskMgr::GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath)
{
	DWORD dwHardDiskIndex = -1;
	do 
	{
		//����򿪾��豸ʧ�ܣ�ֱ�ӷ���-1
		CDiskVolumeInfo DiskVolume;
		if (DiskVolume.OpenVolume(lpszDiskVolumePath) == FALSE)
			break;

		BYTE lpData[1024];
		PVOLUME_DISK_EXTENTS lpVolumeDiskExtents = (PVOLUME_DISK_EXTENTS)lpData;
		DWORD dwSize = _countof(lpData);
		if (DiskVolume.GetDiskExtents(lpVolumeDiskExtents,dwSize) == FALSE)
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
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("��ȡWindowsĿ¼ʧ��"));
			break;
		}

		if (PathStripToRoot(szWindows) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("��ȡWindowsĿ¼���ڸ�Ŀ¼ʧ��"));
			break;
		}

		dwIndex = GetHardDiskIndexFromVolume(szWindows);
		if (dwIndex == -1)
		{
			SetErrorTitle(_T("��ȡWindows����Ӳ�����ʧ��"));
			break;
		}

	} while (FALSE);

	return dwIndex;
}

BOOL CLogicalDiskMgr::GetStorageDeviceProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	BOOL bResult;														// IOCTL����ֵ
	DWORD dwOutBytes;											// IOCTL������ݳ���
	STORAGE_PROPERTY_QUERY StorageQuery;		// ��ѯ�������

	// ָ����ѯ��ʽ
	StorageQuery.PropertyId = StorageDeviceProperty;
	StorageQuery.QueryType = PropertyStandardQuery;

	// ��IOCTL_STORAGE_QUERY_PROPERTYȡ�豸������Ϣ
	bResult = ::DeviceIoControl(hDevice, // �豸���
													IOCTL_STORAGE_QUERY_PROPERTY,    // ȡ�豸������Ϣ
													&StorageQuery, sizeof(STORAGE_PROPERTY_QUERY),    // �������ݻ�����
													pDevDesc, pDevDesc->Size,        // ������ݻ�����
													&dwOutBytes,                     // ������ݳ���
													(LPOVERLAPPED)NULL);             // ��ͬ��I/O    

	return bResult;
}

BOOL CLogicalDiskMgr::GetStorageDeviceBusType(DWORD dwDiskIndex, STORAGE_BUS_TYPE& StorageBusType)
{
	BOOL bSuccess = FALSE;
	HANDLE hStorageDevice = INVALID_HANDLE_VALUE;
	PSTORAGE_DEVICE_DESCRIPTOR pDeviceDesc = NULL;
	do 
	{
		StorageBusType = BusTypeUnknown;
		TCHAR szPhysicDriverPathFirst[MAX_PATH]={0};
		_stprintf_s(szPhysicDriverPathFirst,_T("\\\\.\\PHYSICALDRIVE%d"), dwDiskIndex);

		hStorageDevice = CreateFile(szPhysicDriverPathFirst,
														GENERIC_READ|GENERIC_WRITE,
														FILE_SHARE_READ | FILE_SHARE_WRITE,
														NULL, 
														OPEN_EXISTING, 
														0,
														NULL); 
		if (hStorageDevice == INVALID_HANDLE_VALUE) 
			break;

		pDeviceDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 1024*6];	
		pDeviceDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 1024*6;

		BOOL bGetDriveInfo = CLogicalDiskMgr::GetStorageDeviceProperty(hStorageDevice, pDeviceDesc);
		if (bGetDriveInfo == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ�豸��Ϣʧ��:"));
			break;
		}

		StorageBusType = pDeviceDesc->BusType;

		bSuccess = TRUE;
	} while (FALSE);

	if (pDeviceDesc != NULL)
	{
		delete[] pDeviceDesc;
		pDeviceDesc = NULL;
	}

	if (hStorageDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hStorageDevice);
		hStorageDevice = INVALID_HANDLE_VALUE;
	}
	return bSuccess;
}

BOOL CLogicalDiskMgr::GetDiskSerialNumber(DWORD dwDiskIndex, CString& strSerialNumber)
{
	BOOL bSuccess = FALSE;
	HANDLE hStorageDevice = INVALID_HANDLE_VALUE;
	PSTORAGE_DEVICE_DESCRIPTOR pDeviceDesc = NULL;

	do 
	{
		//StorageBusType = BusTypeUnknown;
		TCHAR szPhysicDriverPathFirst[MAX_PATH]={0};
		_stprintf_s(szPhysicDriverPathFirst,_T("\\\\.\\PHYSICALDRIVE%d"), dwDiskIndex);

		hStorageDevice = CreateFile(szPhysicDriverPathFirst,
														GENERIC_READ|GENERIC_WRITE,
														FILE_SHARE_READ | FILE_SHARE_WRITE,
														NULL, 
														OPEN_EXISTING, 
														0,
														NULL); 
		if (hStorageDevice == INVALID_HANDLE_VALUE) 
			break;

		STORAGE_PROPERTY_QUERY StoragePropertyQuery;
		StoragePropertyQuery.PropertyId = StorageDeviceProperty;
		StoragePropertyQuery.QueryType = PropertyStandardQuery;

		DWORD cbBytesReturned;
		BYTE bData[8096];
		//���ýӿڻ�ȡ
		if (::DeviceIoControl(hStorageDevice, IOCTL_STORAGE_QUERY_PROPERTY, &StoragePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
										bData, _countof(bData), &cbBytesReturned, NULL)==FALSE)
		{
			SetErrorTitle(_T("��ȡ����������Ϣʧ��"));
			break;
		}

		STORAGE_DEVICE_DESCRIPTOR* pStorageDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR *)bData;

		strSerialNumber.Empty();
		if (pStorageDeviceDescriptor->SerialNumberOffset != -1 && pStorageDeviceDescriptor->SerialNumberOffset != 0)
		{
			LPSTR lpszSerialNumberHexText = (char*)pStorageDeviceDescriptor+pStorageDeviceDescriptor->SerialNumberOffset;
			DWORD dwSerialNumberHexText = strlen(lpszSerialNumberHexText);
			if (dwSerialNumberHexText <= 20)//U�̷���20�ֽ�����ת��
				strSerialNumber = lpszSerialNumberHexText;
			else
			{
				char szSerialNumber[MAX_PATH];
				DWORD dwSerialNumberLen = sizeof(szSerialNumber);
				if (CryptStringToBinaryA(lpszSerialNumberHexText , strlen(lpszSerialNumberHexText) , CRYPT_STRING_HEX , (BYTE*)szSerialNumber , &dwSerialNumberLen , NULL , NULL))
				{
					for(DWORD i=0; i<dwSerialNumberLen; i+=2)
					{
						char ch = szSerialNumber[i];
						szSerialNumber[i] = szSerialNumber[i + 1];
						szSerialNumber[i + 1] = ch;
					}
					szSerialNumber[dwSerialNumberLen] = 0;
					strSerialNumber = szSerialNumber;
				}
			}
			strSerialNumber.TrimLeft(' ');
			strSerialNumber.TrimRight(' ');
		}

		bSuccess = FALSE;
	}while(FALSE);

	if (hStorageDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hStorageDevice);
		hStorageDevice = INVALID_HANDLE_VALUE;
	}

	return bSuccess;
}

BOOL CLogicalDiskMgr::FlushDiskDev()
{
	if (m_hStorageDevice == INVALID_HANDLE_VALUE) 
		return FALSE;

	DWORD dwSize = 0;
	return ::DeviceIoControl(m_hStorageDevice,IOCTL_DISK_UPDATE_PROPERTIES,NULL,0,NULL,0,&dwSize,NULL);
}