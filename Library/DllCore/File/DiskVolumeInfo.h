#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

class YCLIB_CORE_API CDiskVolumeInfo
{
public:
	CDiskVolumeInfo();
	~CDiskVolumeInfo();

public:
	//BOOL EnumAllVolume(CStringArray& strVolumeArray);
	//BOOL EnumVolumeForDisk(DWORD dwPhysicalDiskNumber , ULONGLONG ullStartingOffset , ULONGLONG ullExtentLength , CStringArray& strVolumeArray);
};