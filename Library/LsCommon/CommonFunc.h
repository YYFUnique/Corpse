#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#include "ErrorInfo.h"
#include <psapi.h>
#include <WtsApi32.h>
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"WtsApi32.lib")

typedef CMap<CString, LPCTSTR, time_t, time_t> CFileInfo;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG ProcessId;						//���̱�ʶ��
	UCHAR ObjectTypeNumber;		//�򿪶��������
	UCHAR Flags;								//������Ա�ʶ
	USHORT Handle;							//���ֵ������ڽ����е�Ψһ��ʶ��
	PVOID Object;								//������Ǿ����Ӧ��EPROCESS��ַ
	ACCESS_MASK GrantedAccess; //����ķ���Ȩ��
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct tgaSYSTEM_HANDLE
{
	DWORD dwCount;
	SYSTEM_HANDLE_INFORMATION shr[1];
}SYSTEM_HANDLE,*PSYSTEM_HANDLE;

typedef CList<SYSTEM_HANDLE_INFORMATION,SYSTEM_HANDLE_INFORMATION&> CSystemHandleInfoList;

/************************************************************************/
// �������ƣ�DeleteFileByExtension
// �������ܣ�ͨ���ļ���׺����ɾ��ָ��Ŀ¼������ļ���ͨ���Ա��ļ�����ʱ��
//					 �������ļ��޸�ʱ��ʣ�����µ��ļ�
// ����������lpszDirPath			Ҫɾ���ļ���Ŀ¼
//					 lpszExtension		Ҫ���д�����ļ���׺���Զ������׺�ķָ��
//					 dwKeepFile		ɾ���ļ�Ҫ�������ļ�����
// ��  �� ֵ���ú���Ŀǰ����TRUE
/************************************************************************/
LS_COMMON_API BOOL DeleteFileByExtension(LPCTSTR lpszDirPath,LPCTSTR lpszExtension = NULL,DWORD dwKeepFile=3);

/************************************************************************/
// �������ƣ�DeleteFileByCreateTime
// �������ܣ�ͨ���ļ���׺����ɾ��ָ��Ŀ¼������ļ���ͨ���Ա��ļ�����ʱ��
//					 �������ļ��޸�ʱ��ʣ�����µ��ļ�
// ����������pFileInfo				CFileInfo���͵�ָ��
//					 dwKeepFile		��Ҫ�����ļ��ĸ���
//					 lpszFilePath		������Ҫ���бȽϵ��ļ�·��
//					 CreateTime			�����ļ��Ĵ���ʱ��
// �� �� ֵ���������ִ�гɹ�����TRUE�����򷵻�ʧ�ܣ�����ͨ��GetThreadErrorInfo()��ȡ������Ϣ
/************************************************************************/
LS_COMMON_API BOOL DeleteFileByCreateTime(CFileInfo* pFileInfo,DWORD dwKeepFile,LPCTSTR lpszFilePath,FILETIME CreateTime);


/************************************************************************/
// �������ƣ�OpenFolderAndSelectFile
// �������ܣ����ļ�������ʾѡ����ļ�
// ����������lpszFilePath		Ҫ��ʾ�ļ���·��������ʹ������
// �� �� ֵ���������ִ�гɹ�����TRUE�����򷵻�ʧ��
/************************************************************************/
LS_COMMON_API BOOL OpenFolderAndSelectFile(LPCTSTR lpszFilePath);

/************************************************************************/
// �������ƣ�OpenFolderAndSelectFile
// �������ܣ����ļ�������ʾѡ����ļ�
// ����������lpszFilePath		Ҫ��ʾ�ļ���·��������ʹ������
// �� �� ֵ���������ִ�гɹ�����TRUE�����򷵻�ʧ��
/************************************************************************/
LS_COMMON_API BOOL LookupFileProperty(LPCTSTR lpszFilePath);

/************************************************************************/
// �������ƣ�GetMainModuleFileName
// �������ܣ���ȡ���������ƣ��Լ��ļ������ٽ����ĳ�ʼ��
// ������������
// �� �� ֵ����������������
/************************************************************************/
LS_COMMON_API CString GetMainModuleFileName();

/************************************************************************/
// �������ƣ�AdjustPurview
// �������ܣ���ȡ���������ƣ��Լ��ļ������ٽ����ĳ�ʼ��
// ����������AuthorityName����Ҫ��ӵ�Ȩ�ޣ���SE_DEBUG_NAME
//					 bEnable �Ƿ����õ�ǰ��Ȩ
// ��  �� ֵ���������Ȩ�޳ɹ����򷵻�TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL AdjustProcessAuthority(LPCTSTR AuthorityName,BOOL bEnable = TRUE);

/************************************************************************/
// �������ƣ�AdjustListWidth
// �������ܣ������б��Ŀ��������������ȫ����ʾ
// ����������ListCtrl �����б������
// ��  �� ֵ���ú�������TRUE
/************************************************************************/
LS_COMMON_API BOOL AdjustListWidth(CListCtrl& ListCtrl);

/************************************************************************/
// �������ƣ�InsertGroup
// �������ܣ���ӷ���
// ����������ListCtrl			�����б������
//					lpszGroupName	������
//					nGroupId				����ID
// ��  �� ֵ���ú�������TRUE
/************************************************************************/
LS_COMMON_API BOOL InsertGroup(CListCtrl& ListCtrl , LPCTSTR lpszGroupName , int nGroupId);

/************************************************************************/
// �������ƣ�DosPathToNtPath
// �������ܣ�������·��ת��Ϊ�߼�·��
// ����������lpszDosPath �ļ�������·��
//					 strNtPath     �ļ����߼�·��
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API CString CovenrtFileTimeToTimeString(FILETIME& FileTime);

LS_COMMON_API BOOL GetHardDriveLetter(CStringArray& strDriveLetterArray);

/************************************************************************/
// �������ƣ�GetOsInLogicalDrive
// �������ܣ���ȡ��ǰ����ϵͳ�����̷�
// ������������
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API CString GetOsInLogicalDrive();

/************************************************************************/
// �������ƣ�GetPhysicalHardIndex
// �������ܣ���ȡ��ǰ�̷�����Ӳ�����
// ����������lpszOsDriver �̷�����
//					 ָ���̷���Ӳ�̵����
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetPhysicalHardIndex(LPCTSTR lpszOsDriver,DWORD& dwHardIndex);

/************************************************************************/
// �������ƣ�GetSystemLastShutDownTime
// �������ܣ���ȡ����ϵͳ�ϴιػ�ʱ��
// ����������SystemShutDownTime �ػ�ʱ���FILETIME�ṹ��
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetSystemLastShutDownTime(FILETIME& SystemShutDownTime);

/************************************************************************/
// �������ƣ�GetOSInstallTime
// �������ܣ���ȡ��ǰ����ϵͳ�İ�װʱ��
// ����������dwInstallTime ϵͳ��װʱ�䣬���ش�1970��1��1�յ�����
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetOSInstallTime(DWORD& dwInstallTime);

//LS_COMMON_API BOOL CopyDataToClipboard(HWND hWnd,LPCTSTR lpszData);

//LS_COMMON_API BOOL CopyDataToClipboard(CStringArray& strDataArray);

LS_COMMON_API BOOL GetSystemHandleInfo(DWORD dwPid,CSystemHandleInfoList& SystemHandleInfoList);

LS_COMMON_API UINT InsertIconToImageList(LPCTSTR lpszSrcFilePath,CImageList& pImageList,BOOL bEnableDirectory = FALSE);

LS_COMMON_API BOOL AddSubMenuToMenu(CMenu* pMenu,LPCTSTR lpszMenuItem, UINT nIcon);