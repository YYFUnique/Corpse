#pragma once

#include <WinIoCtl.h>

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//��ȡ���̴�С��Ϣ
typedef struct _PHYSICAL_DISK_SIZE
{
	ULARGE_INTEGER DiskSectors;		//����������
	DWORD dwBytesPerSector;			//ÿ�����ֽ���
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
	/* �������ƣ�GetDriveProperty																   */
	/* ����˵�����������������ͬ�����Ի�ȡ���̲�ͬ��Ϣ							   */
	/* ���������HANDLE Ϊ�򿪵Ĵ��̾��											           */
	/*					 pDevDesc::BusType �����˴��̽ӿڷ�ʽ							   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	static BOOL GetDriveProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
	static BOOL GetPhysicalDiskSize(HANDLE hPhysical, PHYSICAL_DISK_SIZE& PhysicalDiskSize);

protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);

private:
	HANDLE m_hVolume;
};