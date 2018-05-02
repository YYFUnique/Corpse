#include "StdAfx.h"
#include "Policies.h"
#include "Registry.h"

#define TASKMGR_POLICIES_PATH _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")

void SetTaskmgrStatus(BOOL bEnable /* = TRUE */)
{
	LsRegSetValue(HKEY_CURRENT_USER, TASKMGR_POLICIES_PATH, _T("DisableTaskmgr"), bEnable);
}

void SetTaskmgrStatusForMachine(BOOL bEnable /* = TRUE */)
{
	LsRegSetValue(HKEY_LOCAL_MACHINE, TASKMGR_POLICIES_PATH, _T("DisableTaskmgr"), bEnable);
}