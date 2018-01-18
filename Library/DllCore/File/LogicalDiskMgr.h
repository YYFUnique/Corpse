#pragma once
#include "../Utils/StdPtr.h"
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
	/************************************************************************/
	/* �������ƣ�OpenDisk																			   */
	/* ����˵����ʹ�ô�����Ŵ򿪴���															   */
	/* �� �� ֵ ���򿪴��̳ɹ�����TRUE�����򷵻�FALSE					               */
	/************************************************************************/
	BOOL OpenDisk(UINT nPhycialDiskIndex);

	/************************************************************************/
	/* �������ƣ�OpenDisk																			   */
	/* ����˵����ʹ��ͨ��SetupAPI��ȡ�Ĵ����������򿪴���						   */
	/* �� �� ֵ ���򿪴��̳ɹ�����TRUE�����򷵻�FALSE					               */
	/************************************************************************/
	BOOL OpenDisk(LPCTSTR lpszDiskVolumePath);

	/************************************************************************/
	/* �������ƣ�Detach																				   */
	/* ����˵��������������豸�Ĺ�����ϵ													   */
	/* �� �� ֵ �����ش��豸���豸���											               */
	/************************************************************************/
	HANDLE Detach();
	
	/************************************************************************/
	/* �������ƣ�GetDriveProperty																   */
	/* ����˵�����������������ͬ�����Ի�ȡ���̲�ͬ��Ϣ							   */
	/* ���������HANDLE Ϊ�򿪵Ĵ��̾��											           */
	/*					 pDevDesc::BusType �����˴��̽ӿڷ�ʽ							   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	BOOL GetDriveProperty(PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);

	/************************************************************************/
	/* �������ƣ�FlushDiskDev																		   */
	/* ����˵����ˢ�´��̹���������Դ���������¼��ش���							   */
	/* �����������									 											           */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	BOOL FlushDiskDev();
public:

	/************************************************************************/
	/* �������ƣ�GetSystemDiskIndex															   */
	/* ����˵������ȡϵͳ�����ڴ������														   */
	/* �� �� ֵ �����ش������															               */
	/************************************************************************/
	static DWORD GetSystemDiskIndex();

	/************************************************************************/
	/* �������ƣ�GetHardDiskIndexFromVolume											   */
	/* ����˵�������ݾ���ȡ��ǰ�����ڴ���												   */
	/* ���������lpszDiskVolumePath	���豸���								           */
	/* �� �� ֵ �����ش������															               */
	/************************************************************************/
	static DWORD GetHardDiskIndexFromVolume(LPCTSTR lpszDiskVolumePath);

	/************************************************************************/
	/* �������ƣ�DeleteDiskVolumeLnk														   */
	/* ����˵�������ָ�����������о���豸��������									   */
	/* ���������dwDiskIndex ָ�������豸�����								           */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	static BOOL DeleteDiskVolumeLnk(DWORD dwDiskIndex);

	/************************************************************************/
	/* �������ƣ�GetPhysicalDiskSize															   */
	/* ����˵������ȡָ������豸���̴�С��Ϣ											   */
	/* ���������dwPhysicalIndex Ҫ�򿪵Ĵ������							           */
	/*					 PhysicalDiskSize ����������Ϣ											   */
	/* �� �� ֵ ������TRUE��ʾ�ɹ�������FALSE��ʾʧ��					               */
	/************************************************************************/
	static BOOL GetPhysicalDiskSize(DWORD dwPhysicalIndex, PHYSICAL_DISK_SIZE& PhysicalDiskSize);

	//��ȡָ���洢�豸��������Ϣ
	static BOOL GetStorageDeviceProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc);
	//��ȡָ����Ŵ洢�豸����������
	static BOOL GetStorageDeviceBusType(DWORD dwDiskIndex, STORAGE_BUS_TYPE& StorageBusType);
	
	static BOOL GetDiskSerialNumber(DWORD dwDiskIndex, CString& strSerialNumber);

	/************************************************************************/
	/* ��  ��  ����EnumVolumeForDisk                                                           */
	/* �������ܣ�ö��ָ�����������еľ��豸												   */
	/* ���������dwPhysicalDiskNumber  ָ���������								   */
	/*					 ullStartingOffset ���ڴ����еĿ�ʼƫ��								   */
	/*					 ullExtentLength	�����չ����											   */
	/*					 strVolumeArray ����ָ�������д��ڵľ��豸						   */
	/* ��  �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											   */
	/************************************************************************/
	static BOOL EnumVolumeForDisk(DWORD dwPhysicalDiskNumber , ULONGLONG ullStartingOffset , ULONGLONG ullExtentLength , CStdArray& strVolumeArray);

private:
	HANDLE m_hStorageDevice;
};