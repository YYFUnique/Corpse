#include "StdAfx.h"
#include "ToggleDesktopHelper.h"

#define PROGMAN_CLASSNAME			_T("Progman")
#define PROGMAN_WINDOWNAME		_T("Program Manager")

const UINT WM_FreeLibrary						= RegisterWindowMessage(FREELIBRARYANDEXIT);
const UINT WM_TOGGLEDESKTOPMSG	= RegisterWindowMessage(TOGGLEDESKTOPMSG);

ToggleDesktopHelper* ToggleDesktopHelper::GetInstance()
{
	// 非多线程安全
	static ToggleDesktopHelper sHelper;
	return &sHelper;
}

ToggleDesktopHelper::ToggleDesktopHelper()
{

}

ToggleDesktopHelper::~ToggleDesktopHelper()
{

}

void ToggleDesktopHelper::AddRef()
{
	HWND hProgman = FindWindow(PROGMAN_CLASSNAME, PROGMAN_WINDOWNAME);
	if (IsWindow(hProgman))
		SendMessage(hProgman, WM_TOGGLEDESKTOPMSG, DESKTOP_PLUGIN_ADDREF, NULL);
}

void ToggleDesktopHelper::Release()
{
	HWND hProgman = FindWindow(PROGMAN_CLASSNAME, PROGMAN_WINDOWNAME);
	if (IsWindow(hProgman))
		SendMessage(hProgman, WM_FreeLibrary, FALSE, NULL);
}

void ToggleDesktopHelper::FreeLibrary()
{
	HWND hProgman = FindWindow(PROGMAN_CLASSNAME, PROGMAN_WINDOWNAME);
	if (IsWindow(hProgman))
		SendMessage(hProgman, WM_FreeLibrary, TRUE, NULL);
}

BOOL ToggleDesktopHelper::IsShellDesktopRunning()
{
	BOOL bRet = FALSE;
	HWND hProgman = FindWindow(PROGMAN_CLASSNAME, PROGMAN_WINDOWNAME);
	if (IsWindow(hProgman))
		bRet = SendMessage(hProgman, WM_TOGGLEDESKTOPMSG, DESKTOP_PLUGIN_WORKING, NULL);

	return bRet;
}

DWORD ToggleDesktopHelper::GetReferenceCount()
{
	BOOL bRet = FALSE;
	HWND hProgman = FindWindow(PROGMAN_CLASSNAME, PROGMAN_WINDOWNAME);
	if (IsWindow(hProgman))
		bRet = SendMessage(hProgman, WM_TOGGLEDESKTOPMSG, DESKTOP_PLUGIN_COUNT, NULL);

	return bRet;
}