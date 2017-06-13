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
	
	BOOL OpenDisk(LPCTSTR lpszDiskVolumePath);
	BOOL GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents,DWORD& dwLen);

	/************************************************************************/
	/* �������ƣ�Detach																				   */
	/* ����˵��������������豸�Ĺ�����ϵ													   */
	/* �� �� ֵ �����ش��豸���豸���											               */
	/************************************************************************/
	HANDLE Detach();

	/************************************************************************/
	/* �������ƣ�LockVolume																		   */
	/* ����˵�����������豸																			   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	BOOL LockVolume();

	/************************************************************************/
	/* �������ƣ�UnlockVolume																	   */
	/* ����˵����������豸����״̬																   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	BOOL UnlockVolume();

	/************************************************************************/
	/* �������ƣ�DismountVolume																   */
	/* ����˵����ж�ؾ��豸																			   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	BOOL DismountVolume();
	
	/************************************************************************/
	/* �������ƣ�GetSystemDiskIndex															   */
	/* ����˵������ȡϵͳ�����ڴ������														   */
	/* �� �� ֵ �����ش������															               */
	/************************************************************************/
	DWORD GetSystemDiskIndex();

	/************************************************************************/
	/* �������ƣ�GetHardDiskIndexFromVolume											   */
	/* ����˵�������ݾ���ȡ��ǰ�����ڴ���												   */
	/* ���������lpszDiskVolumePath	���豸���								           */
	/* �� �� ֵ �����ش������															               */
	/************************************************************************/
	DWORD GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath);

public:

	/************************************************************************/
	/* �������ƣ�GetDriveProperty																   */
	/* ����˵�����������������ͬ�����Ի�ȡ���̲�ͬ��Ϣ							   */
	/* ���������HANDLE Ϊ�򿪵Ĵ��̾��											           */
	/*					 pDevDesc::BusType �����˴��̽ӿڷ�ʽ							   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	static BOOL GetDriveProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);

	/************************************************************************/
	/* �������ƣ�GetPhysicalDiskSize															   */
	/* ����˵������ȡָ������豸���̴�С��Ϣ											   */
	/* ���������dwPhysicalIndex Ҫ�򿪵Ĵ������							           */
	/*					 PhysicalDiskSize ����������Ϣ											   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	static BOOL GetPhysicalDiskSize(DWORD dwPhysicalIndex, PHYSICAL_DISK_SIZE& PhysicalDiskSize);

	/************************************************************************/
	/* �������ƣ�DeleteDiskVolumeLnk														   */
	/* ����˵�������ָ�����������о���豸��������									   */
	/* ���������dwDiskIndex ָ�������豸�����								           */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	static BOOL DeleteDiskVolumeLnk(DWORD dwDiskIndex);

	//��ȡָ���洢�豸��������Ϣ
	static BOOL GetStorageDeviceProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
	//��ȡָ����Ŵ洢�豸����������
	static BOOL GetStorageDeviceBusType(DWORD dwDiskIndex, STORAGE_BUS_TYPE& StorageBusType);
	
	static BOOL GetDiskSerialNumber(DWORD dwDiskIndex, CString& strSerialNumber);
protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);

private:
	HANDLE m_hVolume;
};