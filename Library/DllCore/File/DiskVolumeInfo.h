#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "../Utils/StdPtr.h"

class DLL_API CDiskVolumeInfo
{
public:
	CDiskVolumeInfo();
	~CDiskVolumeInfo();

public:

	BOOL OpenVolume(LPCTSTR lpszDiskVolumePath);

	/************************************************************************/
	/* ��  ��  ����GetDiskExtents	                                                                   */
	/* �������ܣ���ȡ���Ӧ�Ĵ�����Ϣ															   */
	/* ���������lpVolumeDiskExtents ���Ӧ�Ĵ�����Ϣ								   */
	/*					 dwLen ָ�����ݳ���															   */
	/* ��  �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											   */
	/************************************************************************/
	BOOL GetDiskExtents(PVOLUME_DISK_EXTENTS lpVolumeDiskExtents,DWORD& dwLen);

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

public:
	/************************************************************************/
	/* ��  ��  ����EnumAllVolume                                                                   */
	/* �������ܣ�ö�ٵ�ǰϵͳ�����еľ��豸												   */
	/* ���������strVolumeArray ���ص�ǰϵͳ�д��ڵľ��豸						   */
	/* ��  �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											   */
	/************************************************************************/
	static BOOL EnumAllVolume(CStdArray& strVolumeArray);
protected:
	BOOL DeviceIoControl(DWORD dwIoControlCode);

protected:
	HANDLE m_hVolume;
};