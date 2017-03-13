#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/************************************************************************/
/* 函  数  名：SHDeleteDirectory                                                               */
/* 函数功能：删除目录，包括子目录															   */
/* 输入参数：lpszPathName 需要删除的目录											   */
/* 返  回 值：删除成功返回TRUE，失败返回FALSE									   */
/************************************************************************/
DLL_API BOOL SHDeleteDirectory(LPCTSTR lpszPathName);

/************************************************************************/
/* 函  数  名：CheckFileIsX64                                                                     */
/* 函数功能：检测指定应用程序是否是64位应用程序									   */
/* 输入参数：lpszFilePath 需要检测文件的绝对路径									   */
/* 返  回 值：如果是64位应用返回TRUE，否则返回FALSE							   */
/************************************************************************/
DLL_API BOOL CheckFileIsX64(LPCTSTR lpszFilePath);

/************************************************************************/
/* 函  数  名：GetFileVersion                                                                     */
/* 函数功能：获取指定文件的文件版本号													   */
/* 输入参数：lpszFilePath 程序文件绝对路径											   */
/* 返  回 值：如果文件存在，则返回文件版本号，如果不存在，返回v1.0      */
/************************************************************************/
DLL_API DWORD GetFileVersion(LPCTSTR lpszFilePath);

/************************************************************************/
/* 函  数  名：GetVersionText                                                                    */
/* 函数功能：将整形版本号内容转换为字符串类型										   */
/* 输入参数：dwVersion 需要转换的整形版本号										   */
/* 返  回 值：返回转换后的字符串版本号													   */
/************************************************************************/
DLL_API CString GetVersionText(DWORD dwVersion);

/************************************************************************/
/* 函  数  名：GetModuleFileVersion                                                         */
/* 函数功能：获取当前进程文件的文件版本												   */
/* 输入参数：无																						   */
/* 返  回 值：返回当前进程文件的格式化后的字符串									   */
/************************************************************************/
DLL_API CString GetModuleFileVersion();

/************************************************************************/
/* 函  数  名：GetCommonAppDataFilePath                                              */
/* 函数功能：构造程序在指定目录下面的保存路径										   */
/* 输入参数：lpszFileDirectory 文件的保存类型   									   */
/*					 lpszFileTitle	文件目录名称，如果存在多种类型，可以设置   */
/*					 lpszFilePath    返回的文件路径											   */
/* 返  回 值：如果获取成功返回TRUE，否则返回FALSE								   */
/************************************************************************/
DLL_API BOOL GetCommonAppDataFilePath(LPCTSTR lpszFileDirectory,LPCTSTR lpszFileTitle,LPTSTR lpszFilePath);

/************************************************************************/
/* 函  数  名：GetProgramDataFilePath                                                      */
/* 函数功能：获取程序在ProgramData目录下的路径									   */
/* 输入参数：lpszDirName 目录类型														   */
/*					 lpszExtendName  文件扩展名(不需要带".")							   */
/*					 lpszVersion		文件版本号													   */
/* 返  回 值：返回获构造的文件连接															   */
/************************************************************************/
//返回字符串，例如： C:\ProgramData\Corpse\MiniDump\PCHunter(2017-03-10)-V1.0.0.0.dmp
DLL_API CString GetProgramDataFilePath(LPCTSTR lpszDirName,LPCTSTR lpszExtendName,LPCTSTR lpszVersion /* = NULL*/);