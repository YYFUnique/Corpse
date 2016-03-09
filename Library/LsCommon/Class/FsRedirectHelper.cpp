#include "stdafx.h"
#include "FsRedirectHelper.h"
#include "../OsInfo.h"

//========================================================================================================
// 说明: 1. 该类是为了方便64位操作系统下重复进行关闭重定向设置的操作，支持嵌套使用 
//       2. 类对象消亡的时候析构函数会开启重定向设置，所以重定向设置的有效期仅限于该类对象的生命周期
//       3. bSet:TRUE 关闭文件系统重定向
// 例子: exp1: CFsRedirectHelper Helper(TRUE); // 关闭文件系统重定向,其操作伴随类对象的生命周期
//       exp2: CFsRedirectHelper Helper(FALSE); // 未关闭文件系统重定向
//             Helper.DisableFsRedirect();
//             Helper.RevertFsRedirect();
//========================================================================================================
char CFsRedirectHelper::bIs64 = -1;
BOOL CFsRedirectHelper::bInited = FALSE;
CFsRedirectHelper::Wow64DisableWow64FsRedirection CFsRedirectHelper::f_Wow64DisableWow64FsRedirection = NULL;
CFsRedirectHelper::Wow64RevertWow64FsRedirection CFsRedirectHelper::f_Wow64RevertWow64FsRedirection = NULL;  

CFsRedirectHelper::CFsRedirectHelper(BOOL bSet) : m_bSet(bSet), m_pOldValue(NULL)
{
	if(bIs64==-1)
	{
		bIs64 = IsWow64Process() ? 1 : 0; 
	}

	if(bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64DisableWow64FsRedirection(&m_pOldValue);
	}
}

CFsRedirectHelper::~CFsRedirectHelper()
{
	RevertFsRedirect();
}

BOOL CFsRedirectHelper::DisableFsRedirect()
{
	if(!m_bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64DisableWow64FsRedirection(&m_pOldValue);
		m_bSet = TRUE;
	}

	return (m_pOldValue != NULL) ? TRUE:FALSE;
}

void CFsRedirectHelper::RevertFsRedirect()
{
	if(m_bSet && bIs64 && Init() && f_Wow64DisableWow64FsRedirection)
	{
		f_Wow64RevertWow64FsRedirection(&m_pOldValue);
		m_bSet = FALSE;
	}
}

BOOL CFsRedirectHelper::Init()
{
	if(!bInited)
	{
		bInited = TRUE;

		HINSTANCE hlibrary = GetModuleHandle(_T("Kernel32.dll"));
		if(hlibrary)
		{
			f_Wow64DisableWow64FsRedirection = (Wow64DisableWow64FsRedirection) GetProcAddress(hlibrary,"Wow64DisableWow64FsRedirection");
			f_Wow64RevertWow64FsRedirection = (Wow64RevertWow64FsRedirection) GetProcAddress(hlibrary,"Wow64RevertWow64FsRedirection");
		}
	}
	return bInited;
}
