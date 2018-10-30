#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>
#include <Dbt.h>
#include <Cryptuiapi.h>
#include <atltime.h>
#include "DuiLib/UIlib.h"
using namespace DuiLib;

#include "DllCore/Utils/DuiList.h"

#include "MacroDefine.h"
#include "WndLib.h"

#define	DUI_MSGTYPE_REFRESH		_T("refresh")
#define	DUI_MSGTYPE_LOADITEM	_T("loaditem")
// 主界面管理的时间器
#define	DUI_MSGTYPE_TIMEREX		_T("timerex")

#include <winsock2.h>
#include <iphlpapi.h>

#include "Wnd/AboutDialog.h"
#include "Wnd/SkinPickerDialog.h"

#include "Utils/UIFileList.h"