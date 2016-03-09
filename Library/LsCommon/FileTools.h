#pragma once
#include <Shlwapi.h>
#include "DebugInfo.h"
#include "CommonFunc.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#define			FILE_VERSION_FileVersion			_T("FileVersion")
#define			FILE_VERSION_FileDescription  _T("FileDescription")
#define			FILE_VERSION_CompanyName _T("CompanyName")
#define			FILE_VERSION_ProductName		_T("ProductName")

typedef struct _LANGANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
}LANGANDCODEPAGE,*PLANGANDCODEPAGE;

#include <shlwapi.h>

//==================================================================
//函 数 名: GetCommonAppDataFilePath
//功    能: 获取\Documents and Settings\All Users\Application Data\lonsin\Languard
//			下的文件路径，无用户权限要求(目录不存在则创建目录)
//输入参数: [in] lpszFileDirectory, 欲添加的文件夹
//			[in] lpszFileTitle, 欲添加的文件名
//			[in] lpszFilePath, 存放得到的最终文件路径的缓冲区
//返 回 值: 无返回值
//==================================================================
LS_COMMON_API void GetCommonAppDataFilePath(LPCTSTR lpszFileDirectory,LPCTSTR lpszFileTitle,LPTSTR lpszFilePath);

//==================================================================
//函 数 名: GetAppDataFileName
//功    能: 获取日志文件路径,(例如C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\龙信终端安全管理系统\"lpszDirName"\文件名."lpszExtendName"
//输入参数: 
//				lpszDirName			要获取的lpszDirName文件夹路径
//				lpszExtendName		获取lpszExtendName类型的文件
//				lpszVersion				用于添加系统主程序版本号
//返 回 值: 返回获取到的日志文件的路径
//==================================================================
LS_COMMON_API CString GetFilePath(LPCTSTR lpszFileName,LPCTSTR lpszExtendName,LPCTSTR lpszInfo = NULL);

//==================================================================
//函 数 名: GetFileVersion
//功    能: 获取文件版本信息
//输入参数: [in] lpszFilePath, 文件路径,该参数为空则为当前模块路径
//返 回 值: 返回DWORD型的版本号信息
//==================================================================
LS_COMMON_API DWORD GetFileVersion(LPCTSTR lpszFilePath);

LS_COMMON_API BOOL GetFileVersionEx(LPCTSTR lpszFilePath,LPCTSTR FileVersion,CString& strFileVersionInfo);

/*LS_COMMON_API CString GetFileVersionInfoText(FILE_VERSION FileVersion);*/
//==================================================================
//函 数 名: GetVersionText
//功    能: 把dword型的版本号,转换为字符串型
//输入参数: [in] dwVersion, 版本号
//返 回 值: 返回字符串型的版本号信息
//==================================================================
LS_COMMON_API CString GetVersionText(DWORD dwVersion);

//==================================================================
//函 数 名: LsGetMainAppVersion
//功     能: 获取当前主程序版本号
//输入参数: 无
//返 回 值: 返回一个无符号长整形值
//==================================================================
LS_COMMON_API DWORD LsGetMainAppVersion();

//==================================================================
//函 数 名: LsGetMainAppVersionText
//功     能: 获取当前主程序版本号
//输入参数: 无
//返 回 值: 返回由无符号长整型构造出的主程序版本号，如(V1.0.1.42)
//==================================================================
LS_COMMON_API CString LsGetMainAppVersionText();

//==================================================================
//函 数 名: GetFileSizeByName
//功     能: 获取指定文件的大小
//输入参数: lpszFilePath 文件路径
//返 回 值: 返回文件大小
//==================================================================
LS_COMMON_API ULONGLONG GetFileSizeByName(LPCTSTR lpszFilePath);

//==================================================================
//函 数 名: GetFileLastModifyTime
//功     能: 获取文件的最后修改时间
//输入参数: lpszFilePath 文件路径
//返 回 值: 返回文件修改时间
//==================================================================
LS_COMMON_API ULONGLONG GetFileLastModifyTime(LPCTSTR lpszFilePath);

LS_COMMON_API LPSHELLFOLDER GetSpecialFolder(HWND hwnd, UINT nFolderCsidl);

LS_COMMON_API LPSHELLFOLDER GetSpecialFolder(HWND hWnd,LPCTSTR lpszSepcialFolderPath);

LS_COMMON_API void	DoDrop(LPDATAOBJECT lpDataObject, LPDROPTARGET lpDropTarget);

LS_COMMON_API LPITEMIDLIST PidlFromPath(HWND hWnd, LPCTSTR lpszPath);

LS_COMMON_API HRESULT GetUIObjectOfPath(HWND hWnd, LPCTSTR pszPath, REFIID riid, LPVOID *ppvOut);

LS_COMMON_API HRESULT GetUIObjectOfAbsPidl(HWND hWnd, LPITEMIDLIST pidl, REFIID riid, LPVOID *ppvOut);

LS_COMMON_API BOOL GetFilePathByLink(LPCTSTR lpszLinkName,CString& strFilePath);

LS_COMMON_API BOOL SaveBitmapFile(HBITMAP hBitmap,LPCTSTR lpszFilePath);