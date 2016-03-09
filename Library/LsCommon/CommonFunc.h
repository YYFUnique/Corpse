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
	ULONG ProcessId;						//进程标识符
	UCHAR ObjectTypeNumber;		//打开对象的类型
	UCHAR Flags;								//句柄属性标识
	USHORT Handle;							//句柄值，句柄在进程中的唯一标识符
	PVOID Object;								//这个就是句柄对应的EPROCESS地址
	ACCESS_MASK GrantedAccess; //对象的访问权限
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct tgaSYSTEM_HANDLE
{
	DWORD dwCount;
	SYSTEM_HANDLE_INFORMATION shr[1];
}SYSTEM_HANDLE,*PSYSTEM_HANDLE;

typedef CList<SYSTEM_HANDLE_INFORMATION,SYSTEM_HANDLE_INFORMATION&> CSystemHandleInfoList;

/************************************************************************/
// 函数名称：DeleteFileByExtension
// 函数功能：通过文件后缀名称删除指定目录下面的文件，通过对比文件创建时间
//					 或者是文件修改时间剩下最新的文件
// 函数参数：lpszDirPath			要删除文件的目录
//					 lpszExtension		要进行处理的文件后缀，自动处理后缀的分割点
//					 dwKeepFile		删除文件要保留的文件个数
// 返  回 值：该函数目前返回TRUE
/************************************************************************/
LS_COMMON_API BOOL DeleteFileByExtension(LPCTSTR lpszDirPath,LPCTSTR lpszExtension = NULL,DWORD dwKeepFile=3);

/************************************************************************/
// 函数名称：DeleteFileByCreateTime
// 函数功能：通过文件后缀名称删除指定目录下面的文件，通过对比文件创建时间
//					 或者是文件修改时间剩下最新的文件
// 函数参数：pFileInfo				CFileInfo类型的指针
//					 dwKeepFile		需要保留文件的个数
//					 lpszFilePath		传入需要进行比较的文件路径
//					 CreateTime			传人文件的创建时间
// 返 回 值：如果函数执行成功返回TRUE，否则返回失败，可以通过GetThreadErrorInfo()获取出错信息
/************************************************************************/
LS_COMMON_API BOOL DeleteFileByCreateTime(CFileInfo* pFileInfo,DWORD dwKeepFile,LPCTSTR lpszFilePath,FILETIME CreateTime);


/************************************************************************/
// 函数名称：OpenFolderAndSelectFile
// 函数功能：在文件夹中显示选择的文件
// 函数参数：lpszFilePath		要显示文件的路径，必须使用数组
// 返 回 值：如果函数执行成功返回TRUE，否则返回失败
/************************************************************************/
LS_COMMON_API BOOL OpenFolderAndSelectFile(LPCTSTR lpszFilePath);

/************************************************************************/
// 函数名称：OpenFolderAndSelectFile
// 函数功能：在文件夹中显示选择的文件
// 函数参数：lpszFilePath		要显示文件的路径，必须使用数组
// 返 回 值：如果函数执行成功返回TRUE，否则返回失败
/************************************************************************/
LS_COMMON_API BOOL LookupFileProperty(LPCTSTR lpszFilePath);

/************************************************************************/
// 函数名称：GetMainModuleFileName
// 函数功能：获取主程序名称，以及文件操作临界区的初始化
// 函数参数：无
// 返 回 值：返回主程序名称
/************************************************************************/
LS_COMMON_API CString GetMainModuleFileName();

/************************************************************************/
// 函数名称：AdjustPurview
// 函数功能：获取主程序名称，以及文件操作临界区的初始化
// 函数参数：AuthorityName，需要添加的权限，如SE_DEBUG_NAME
//					 bEnable 是否启用当前特权
// 返  回 值：如果提升权限成功，则返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL AdjustProcessAuthority(LPCTSTR AuthorityName,BOOL bEnable = TRUE);

/************************************************************************/
// 函数名称：AdjustListWidth
// 函数功能：调整列表框的宽带，是所有内容全部显示
// 函数参数：ListCtrl 对象列表框引用
// 返  回 值：该函数返回TRUE
/************************************************************************/
LS_COMMON_API BOOL AdjustListWidth(CListCtrl& ListCtrl);

/************************************************************************/
// 函数名称：InsertGroup
// 函数功能：添加分组
// 函数参数：ListCtrl			对象列表框引用
//					lpszGroupName	分组名
//					nGroupId				分组ID
// 返  回 值：该函数返回TRUE
/************************************************************************/
LS_COMMON_API BOOL InsertGroup(CListCtrl& ListCtrl , LPCTSTR lpszGroupName , int nGroupId);

/************************************************************************/
// 函数名称：DosPathToNtPath
// 函数功能：将物理路径转换为逻辑路径
// 函数参数：lpszDosPath 文件的物理路径
//					 strNtPath     文件的逻辑路径
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API CString CovenrtFileTimeToTimeString(FILETIME& FileTime);

LS_COMMON_API BOOL GetHardDriveLetter(CStringArray& strDriveLetterArray);

/************************************************************************/
// 函数名称：GetOsInLogicalDrive
// 函数功能：获取当前操作系统所在盘符
// 函数参数：无
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API CString GetOsInLogicalDrive();

/************************************************************************/
// 函数名称：GetPhysicalHardIndex
// 函数功能：获取当前盘符所在硬盘序号
// 函数参数：lpszOsDriver 盘符符号
//					 指定盘符在硬盘的序号
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetPhysicalHardIndex(LPCTSTR lpszOsDriver,DWORD& dwHardIndex);

/************************************************************************/
// 函数名称：GetSystemLastShutDownTime
// 函数功能：获取操作系统上次关机时间
// 函数参数：SystemShutDownTime 关机时间的FILETIME结构体
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetSystemLastShutDownTime(FILETIME& SystemShutDownTime);

/************************************************************************/
// 函数名称：GetOSInstallTime
// 函数功能：获取当前操作系统的安装时间
// 函数参数：dwInstallTime 系统安装时间，返回从1970年1月1日的秒数
// 返  回 值：如果执行成功，则返回TRUE
/************************************************************************/
LS_COMMON_API BOOL GetOSInstallTime(DWORD& dwInstallTime);

//LS_COMMON_API BOOL CopyDataToClipboard(HWND hWnd,LPCTSTR lpszData);

//LS_COMMON_API BOOL CopyDataToClipboard(CStringArray& strDataArray);

LS_COMMON_API BOOL GetSystemHandleInfo(DWORD dwPid,CSystemHandleInfoList& SystemHandleInfoList);

LS_COMMON_API UINT InsertIconToImageList(LPCTSTR lpszSrcFilePath,CImageList& pImageList,BOOL bEnableDirectory = FALSE);

LS_COMMON_API BOOL AddSubMenuToMenu(CMenu* pMenu,LPCTSTR lpszMenuItem, UINT nIcon);