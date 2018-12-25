#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/************************************************************************/
/* 函  数  名：OsIsWow64Process                                                              */
/* 函数功能：判断当前进程是否运行在WOW64下										   */
/* 输入参数：无																						   */
/* 返  回 值：如果当前进程运行在WOW64中返回TRUE，否则返回FALSE    */
/************************************************************************/

DLL_API BOOL OsIsWow64Process();

/************************************************************************/
/* 函  数  名：Is64BitOs										                                       */
/* 函数功能：判断是否是64位操作系统														   */
/* 输入参数：无																						   */
/* 返  回 值：如果64位系统返回TRUE，否则返回FALSE							       */
/************************************************************************/
DLL_API BOOL Is64BitOS();

/************************************************************************/
/* 函  数  名：Is64BitPorcess								                                       */
/* 函数功能：判断指定进程是32位还是64位应用										   */
/* 输入参数：进程ID																					   */
/* 返  回 值：如果64位应用返回TRUE，否则返回FALSE							       */
/************************************************************************/
DLL_API BOOL Is64BitPorcess(DWORD dwProcessID);

/************************************************************************/
/* 函  数  名：OsIsWinXpSp2OrLater                                                         */
/* 函数功能：判断当前系统是否是XPsp2及以后系统									   */
/* 输入参数：无																						   */
/* 返  回 值：如果是XPsp2及以后系统返回TRUE，否则返回FALSE			   */
/************************************************************************/
DLL_API BOOL OsIsWinXpSp2OrLater();

/************************************************************************/
/* 函  数  名：OsIsWorkstationWinXpSp2OrLater                                     */
/* 函数功能：判断当前系统是否是OsIsWorkstationWinXpSp2OrLater
					 及以后的非服务器操作系统													   */
/* 输入参数：无																						   */
/* 返  回 值：如果是XPsp2及以后非服务器系统返回TRUE，否则返回FALSE */
/************************************************************************/
DLL_API BOOL OsIsWorkstationWinXpSp2OrLater();

/************************************************************************/
/* 函  数  名：OsIsWinXpOrLater	                                                               */
/* 函数功能：判断当前系统是否是XP及以后系统										   */
/* 输入参数：无																						   */
/* 返  回 值：如果是XP及以后系统返回TRUE，否则返回FALSE					   */
/************************************************************************/
DLL_API BOOL OsIsWinXpOrLater();

/************************************************************************/
/* 函  数  名：OsIsWinXpSp2OrWin2003Sp1Later                                     */
/* 函数功能：判断当前系统是否是XPsp2或2003sp1以后系统					   */
/* 输入参数：无																						   */
/* 返  回 值：如果是以上系统返回TRUE，否则返回FALSE      					   */
/************************************************************************/
DLL_API BOOL OsIsWinXpSp2OrWin2003Sp1Later();

/************************************************************************/
/* 函  数  名：OsIsVista                                                                               */
/* 函数功能：判断当前系统是否是Vista系统					                               */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista系统返回TRUE，否则返回FALSE      					   */
/************************************************************************/
DLL_API BOOL OsIsVista();

/************************************************************************/
/* 函  数  名：OsIsVistaOrLater	                                                               */
/* 函数功能：判断当前系统是否是Vista及以后系统									   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista及以后系统返回TRUE，否则返回FALSE				   */
/************************************************************************/
DLL_API BOOL OsIsVistaOrLater();

/************************************************************************/
/* 函  数  名：OsIsWin7	                                                                           */
/* 函数功能：判断当前系统是否是Win7及以后系统									   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Win7及以后系统返回TRUE，否则返回FALSE				   */
/************************************************************************/
DLL_API BOOL OsIsWin7();

/************************************************************************/
/* 函  数  名：OsIsWorkstationVistaOrLater	                                           */
/* 函数功能：判断当前系统是否是Vista及以后非服务器系统						   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista及以后非服务器系统返回TRUE，否则返回FALSE	   */
/************************************************************************/
DLL_API BOOL OsIsWorkstationVistaOrLater();

/************************************************************************/
/* 函  数  名：OsIsWin8OrLater                   	                                           */
/* 函数功能：判断当前系统是否是Win8及以后系统           						   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Win8及以后系统返回TRUE，否则返回FALSE	               */
/************************************************************************/
DLL_API BOOL OsIsWin8OrLater();

/************************************************************************/
/* 函  数  名：GetOsTypeName                   	                                           */
/* 函数功能：获取系统名称	                                            						   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista及以后非服务器系统返回TRUE，否则返回FALSE	   */
/************************************************************************/
DLL_API CString GetOsTypeName();

/************************************************************************/
/* 函  数  名：GetOsSpText                          	                                           */
/* 函数功能：获取操作系统sp版本号                                    						   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista及以后非服务器系统返回TRUE，否则返回FALSE	   */
/************************************************************************/
DLL_API CString GetOsSpText();

/************************************************************************/
/* 函  数  名：OsIsServer                   	                                                       */
/* 函数功能：判断操作系统是否是服务器系统                       						   */
/* 输入参数：无																						   */
/* 返  回 值：如果是Vista及以后非服务器系统返回TRUE，否则返回FALSE	   */
/************************************************************************/
DLL_API BOOL OsIsServer();

/************************************************************************/
/* 函  数  名：GetOsVersion               	                                                       */
/* 函数功能：获取操作系统主版本号和次版本号                   						   */
/* 输入参数：OUT dwMajorVersion 主版本号											  
                     OUT dwMinorVersion 次版本号                                            */
/* 返  回 值：如果是Vista及以后非服务器系统返回TRUE，否则返回FALSE	   */
/************************************************************************/
DLL_API BOOL GetOsVersion(DWORD& dwMajorVersion, DWORD& dwMinorVersion);

/************************************************************************/
/* 函  数  名：GetOsInLogicalDrive    	                                                       */
/* 函数功能：获取操作系统所在逻辑驱动器符号                   						   */
/* 输入参数：无                                                                                           */
/* 返  回 值：返回操作系统所在驱动器符号名称，如"C:\"                          	   */
/************************************************************************/
DLL_API CString GetOsInLogicalDrive();

DLL_API CString GetSystemDirectory(LPCTSTR lpszFileName);

DLL_API CString GetWindowsDirectory();
