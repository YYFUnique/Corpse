#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//实现\Device\HarddiskVolume1\Test\1.txt转换为C:\Test\1.txt的转换
//注意:该类不能跨线程调用!!

typedef struct _VOLUME_NAME_INFO
{
	TCHAR szDriveLetter;//盘符
	CString strVolumeName;
}VOLUME_NAME_INFO,*PVOLUME_NAME_INFO;

typedef CYjList<VOLUME_NAME_INFO,const VOLUME_NAME_INFO&> CVolumeInfo;

class DLL_API CVolumeNameToDosName
{
public:
	CVolumeNameToDosName(void);

	void UpdateVolumeNameInfo();

	BOOL ConvertVolumeNameToDosName(LPCTSTR lpszVolumeNamePath , CString& strDosName);

	//转换未知卷路径为默认U盘路径
	BOOL ConvertUnknownVolumeNameToDosName(LPCTSTR lpszVolumeNamePath , CString& strDosName);

protected:

	CVolumeInfo m_VolumeNameList;
};
