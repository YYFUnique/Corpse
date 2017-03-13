#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "../Utils/StdPtr.h"

typedef struct _LS_OS_USER_INFO
{
	CString strUserName;
	CString strFullName;
	CString strComment;
	DWORD dwFlags;
	union
	{
		SID sidUserSid;
		BYTE UserSidBuffer[SECURITY_MAX_SID_SIZE];
	};
}LS_OS_USER_INFO,*PLS_OS_USER_INFO;

typedef CYjList<LS_OS_USER_INFO , const LS_OS_USER_INFO&> CLsOsUserInfoList;

//==================================================================
//函 数 名: ConvertUserSidStringToName
//功    能: 转换用户SID字符串到用户名
//输入参数: [in]  lpszUserSid, 用户SID
//			[out]  strUserName, 返回用户名
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL ConvertUserSidStringToName(LPCTSTR lpszUserSid , CString& strUserName);

//==================================================================
//函 数 名: GetUsersBelongingToTheGroup
//功    能: 获取用户所在用户组
//输入参数: [in]  lpszUserName, 用户名称
//			[out]  strGroupArray, 用户所在组
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL GetUsersBelongingToTheGroup(LPCTSTR lpszUserName , CStdArray& strGroupArray);

//==================================================================
//函 数 名: IsUserInAdminGroup
//功    能: 判断用户是否属于Administrators组
//输入参数: [in]  lpszUserName, 用户名称
//返 回 值: 属于管理员组返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL IsUserInAdminGroup(LPCTSTR lpszUserName);

//==================================================================
//函 数 名: EnumOsUserInfo
//功    能: 枚举当前操作系统用户信息
//输入参数: [out]  OsUserInfoArray, 用户信息数组
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL EnumOsUserInfo(CLsOsUserInfoList& OsUserInfoArray);

//==================================================================
//函 数 名: LsQueryUserToken
//功    能: 获取指定会话的用户令牌信息(封装WTSQueryUserToken函数,以便在2000下能够启动程序)
//输入参数: [out]  OsUserInfoArray, 用户信息数组
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL LsQueryUserToken(DWORD dwSessionId , PHANDLE phHandle);