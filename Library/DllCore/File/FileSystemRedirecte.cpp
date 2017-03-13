#include "StdAfx.h"
#include "FileSystemRedirecte.h"
#include "../Utils/OsInfo.h"

OSType CFileSystemRedirection::m_OSType = OSTYPENULL;
BOOL CFileSystemRedirection::m_bInited = FALSE;

pfnWow64DisableWow64FsRedirection CFileSystemRedirection::pWow64DisableWow64FsRedirection = NULL;
pfnWow64RevertWow64FsRedirection CFileSystemRedirection::pWow64RevertWow64FsRedirection = NULL;  

CFileSystemRedirection::CFileSystemRedirection(BOOL bDisableRedirection)
{
	m_bDisableRedirection = bDisableRedirection;
	m_pOldValue = NULL;
	if (m_OSType == OSTYPENULL)
	{
		m_OSType = OsIsWow64Process() ? OSTYPEx64 : OSTYPEx86; 
	}

	if (bDisableRedirection && m_OSType == OSTYPEx64 && Init() && pWow64DisableWow64FsRedirection)
	{
		pWow64DisableWow64FsRedirection(&m_pOldValue);
	}
}

CFileSystemRedirection::~CFileSystemRedirection()
{
	RevertFileSystemRedirection();
}

BOOL CFileSystemRedirection::Init()
{
	if (m_bInited == FALSE)
	{
		m_bInited = TRUE;

		HMODULE  hModule = GetModuleHandle(_T("Kernel32.dll"));
		if (hModule)
		{
			pWow64DisableWow64FsRedirection = (pfnWow64DisableWow64FsRedirection) GetProcAddress(hModule,"Wow64DisableWow64FsRedirection");
			pWow64RevertWow64FsRedirection = (pfnWow64RevertWow64FsRedirection) GetProcAddress(hModule,"Wow64RevertWow64FsRedirection");
		}
	}
	return m_bInited;
}

BOOL CFileSystemRedirection::DisableFileSystemRedirection()
{
	//如果是32位系统直接返回真
	if (m_OSType == OSTYPEx86)
		return TRUE;

	if (Init() && pWow64DisableWow64FsRedirection)
	{
		return pWow64DisableWow64FsRedirection(&m_pOldValue);
	}

	return FALSE;
}

BOOL CFileSystemRedirection::RevertFileSystemRedirection()
{
	if (m_OSType == OSTYPEx86)
		return TRUE;

	if (m_bDisableRedirection && m_OSType == OSTYPEx64 && Init() && pWow64DisableWow64FsRedirection)
		return pWow64DisableWow64FsRedirection(&m_pOldValue);

	return FALSE;
}