#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

//ʵ��\Device\HarddiskVolume1\Test\1.txtת��ΪC:\Test\1.txt��ת��
//ע��:���಻�ܿ��̵߳���!!

typedef struct _VOLUME_NAME_INFO
{
	TCHAR szDriveLetter;//�̷�
	CString strVolumeName;
}VOLUME_NAME_INFO,*PVOLUME_NAME_INFO;

typedef CYjList<VOLUME_NAME_INFO,const VOLUME_NAME_INFO&> CVolumeInfo;

class YCLIB_CORE_API CVolumeNameToDosName
{
public:
	CVolumeNameToDosName(void);

	void UpdateVolumeNameInfo();

	BOOL ConvertVolumeNameToDosName(LPCTSTR lpszVolumeNamePath , CString& strDosName);

	//ת��δ֪��·��ΪĬ��U��·��
	BOOL ConvertUnknownVolumeNameToDosName(LPCTSTR lpszVolumeNamePath , CString& strDosName);

protected:

	CVolumeInfo m_VolumeNameList;
};
