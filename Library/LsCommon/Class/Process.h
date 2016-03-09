#pragma once

#include "../Authority/ImpersonateSessionUser.h"
#include "../Authority/ImpersonateSystemUser.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//ģ�⵱ǰ�û�Ȩ��
#define  RunFunctionAsCurrentUser() \
	CImpersonateSessionUser ImpersionateSessionUser;	\
	ImpersionateSessionUser.Impersonate()

//ģ��System�û�Ȩ��
#define  RunFunctionAsSystemUser() \
	CImpersonateSystemUser ImpersonateSystemUser; \
	ImpersonateSystemUser.Impersonate()

#include <Shlwapi.h>
#include <Psapi.h>
//#include <NtDll.h>

typedef struct  _tgaLS_SYSTEM_PROCESS
{
	DWORD dwProcessId;
	CString strProcessPath;
	CString strProcessName;
	CString strUserName;
	DWORD dwMemUse;
	DWORD dwThread;
	DWORD dwHandle;
	DWORD dwConsole;
	CString strFileVersion;
	CString strCompanyName;
	CString strDescrible;
	__int64 ullProcessUseTime;		//�������õ�ϵͳʱ�䣬����CPUʹ����ʱ��
	DWORD dwCPU;
}LS_SYSTEM_PROCESS,*PLS_SYSTEM_PROCESS;

typedef CList<LS_SYSTEM_PROCESS,LS_SYSTEM_PROCESS&> CLsSystemProcessList;

/************************************************************************/
// �������ƣ�EnumSystemProcess
// �������ܣ�ö�ٲ���ϵͳ�еĽ���
// ����������LsSystemProcessList �����б�ṹ��
// ��  �� ֵ�����ִ�гɹ����򷵻ض�Ӧ�ļ���ͼ����ImgList�е�����
/************************************************************************/
LS_COMMON_API BOOL EnumSystemProcess(CLsSystemProcessList& LsSystemProcessList);

/************************************************************************/
// �������ƣ�GetAppIcon
// �������ܣ���ȡ�ļ���Ӧ��ͼ��
// ����������lpszProcessPath �ļ�·��
//					 ImgList ͼ���ļ��б�
// ��  �� ֵ�����ִ�гɹ����򷵻ض�Ӧ�ļ���ͼ����ImgList�е�����
//					�����򷵻�0�������г�ʼ����1��ͼ��
/************************************************************************/
LS_COMMON_API UINT GetModuleFileIcon(LPCTSTR lpszProcessPath,CImageList* ImgList);

/************************************************************************/
// �������ƣ�GetProcessHandle
// �������ܣ���ָ������ID��Handle
// ����������dwPID		����ID
//					 hProcess ���̾��
// ��  �� ֵ�����ִ�гɹ�������TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);

/************************************************************************/
// �������ƣ�GetProcessFullPath
// �������ܣ���ȡָ������ID��ȫ·��
// ����������dwPID			����ID
//					 strFullPath  ����·��
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath);

/************************************************************************/
// �������ƣ�ConvertToDriveLetter
// �������ܣ����豸·��ת��Ϊ�����������
// ����������lpszDeviceName �豸����
//					 strSysbolLnkName ������������
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL ConvertToDriveLetter(LPCTSTR lpszDeviceName,CString& strSysbolLnkName);

/************************************************************************/
// �������ƣ�DosPathToNtPath
// �������ܣ�������·��ת��Ϊ�߼�·��
// ����������lpszDosPath �ļ�������·��
//					 strNtPath     �ļ����߼�·��
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath);

/************************************************************************/
// �������ƣ�GetProcessUserName
// �������ܣ���ȡ�����û���
// ����������dwPID ����ID 
//					 strProcessName     �����û���
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessUserName(DWORD dwPID,CString& strProcessName);


/************************************************************************/
// �������ƣ�GetProcessCommandLine
// �������ܣ���ȡ���̵�������
// ����������dwPid ָ������
//					 strCmdLine     ָ�����̵�������
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine);

/************************************************************************/
// �������ƣ�GetProcessCommandLine
// �������ܣ���ȡ���̵�������
// ����������dwPid ָ������
//					 strCmdLine     ָ�����̵�������
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetProcessCurrentDirctory(DWORD dwPid,CString& strProcessCurrentDirctory);

/************************************************************************/
// �������ƣ�GetParentProcessID
// �������ܣ���ȡָ�����̵ĸ�����ID
// ����������dwCurrentProcessID ָ������ID
//					 dwParentProcessID     ������ID
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetParentProcessID(DWORD dwCurrentProcessID,DWORD& dwParentProcessID);

/************************************************************************/
// �������ƣ�GetPriorityClass
// �������ܣ���ȡ�������ȼ�
// ����������dwPid ����ID
//					 strPricrityDescribe ���ȼ�����
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
LS_COMMON_API BOOL GetPriorityClass(DWORD dwPid,CString& strPricrityDescribe);

/************************************************************************/
// �������ƣ�GetHardDriveLetter
// �������ܣ���ȡ�̷�����
// ����������strDriveLetterArray �����̷�����
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/

LS_COMMON_API BOOL TerminateProcessByProcessId(DWORD dwProcessId);

LS_COMMON_API BOOL TerminateProcessTreeByProcessId(DWORD dwPid);

LS_COMMON_API BOOL FreeRemoteLibrary(DWORD dwProcessID,LPCTSTR lpszModulePath);

LS_COMMON_API DWORD GetThreadEntryPtr(DWORD dwThreadID);

LS_COMMON_API BOOL GetFilePathByFileHandle(HANDLE hFile,CString& strFilePath);

LS_COMMON_API CString GetModuleAddressRange(MODULEINFO& ModuleInfo);

LS_COMMON_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

LS_COMMON_API BOOL IsSystemUserProcess();