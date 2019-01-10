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

#include "NetCore/Application/PingHelper.h"
#include "NetCore/Application/DNSLookup.h"

#include "Utils/UIFileList.h"
#include "Utils/ControlBuilder.h"

#define	WM_MESSAGE_BASE					(WM_USER + 0x2000)
#define   WM_NOTIFY_TAB_CHANGE		(WM_MESSAGE_BASE + 0x1)			// 页面自定义跳转到其他页面

#include "UserDefine.h"
#include "VirtualWnd.h"
#include "MacroDefine.h"


#define	DUI_MSGTYPE_LOADITEM	_T("loaditem")
// 主界面管理的时间器
#define	DUI_MSGTYPE_TIMEREX		_T("timerex")