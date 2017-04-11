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
	/************************************************************************/
	/* ��  ��  ����EnumAllVolume                                                                   */
	/* �������ܣ�ö�ٵ�ǰϵͳ�����еľ��豸												   */
	/* ���������strVolumeArray ���ص�ǰϵͳ�д��ڵľ��豸						   */
	/* ��  �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											   */
	/************************************************************************/
	static BOOL EnumAllVolume(CStdArray& strVolumeArray);

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
};