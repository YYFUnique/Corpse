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
	BOOL EnumAllVolume(CStdArray& strVolumeArray);
	BOOL EnumVolumeForDisk(DWORD dwPhysicalDiskNumber , ULONGLONG ullStartingOffset , ULONGLONG ullExtentLength , CStdArray& strVolumeArray);
};