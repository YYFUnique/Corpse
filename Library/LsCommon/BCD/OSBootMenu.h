#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#include <WbemIdl.h>

class LS_COMMON_API COSBootMenu
{
public:
	COSBootMenu();
	~COSBootMenu();
public:
	//修改启动菜单
	BOOL ChangeOsBCDMenu();

	//修改Vista系统及以后系统的启动菜单
	BOOL ModifyVistaBootMenu();

	//通过注册表修改系统新版本的启动菜单
	BOOL ModifyBootMenuByReg();

	//备份NT系统启动菜单boot.ini
	BOOL BackupNtBootMenu();

	//修改NT系统启动菜单的boot.ini
	BOOL ModifyNtBootMenu();
protected:
	//获取操作系统启动菜单的GUID字符串
	BOOL GetOsListGUIDString(CStringArray& strOsListGuid);

	//获取启动菜单默认操作系统GUID字符串
	BOOL GetDefaultOsGuid(CString& strDefaultOsGuid);

	BOOL DisableOtherBootMenu(LPCTSTR lpszResveredGuid);

	BOOL LoadVistaBCDMenu();

	BOOL ConvertBootMenuStringToDriverLetter(LPCTSTR lpszOSKeyName,CString& strDriverLetter);
protected:
	BOOL m_bIsVista;
};