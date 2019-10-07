#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include "DuiLib/UIlib.h"
using namespace DuiLib;

#include "DllCore/Utils/DuiList.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Sync/SyncLock.h"
#include "DllCore/Utils/Registry.h"

#define	DUI_MSGTYPE_REFRESH		_T("refresh")
#define	DUI_MSGTYPE_LOADITEM	_T("loaditem")
// 主界面管理的时间器
#define	DUI_MSGTYPE_TIMEREX		_T("timerex")

#include <DllCore/Utils/StdPtr.h>
#include <DllCore/File/FileSystemWatcher.h>

#include <CommDlg.h>

#include "Helper/XUnzip.h"
#include "Helper/XZip.h"

#include "DuiLib/Utils/Utils.h"
#include "Helper/TypePtrArray.h"
#include "Helper/FileWatcher/DirectoryChanges.h"
#include "Helper/FileWatcher/DelayedDirectoryChangeHandler.h"
#include "Helper/FileWatcher/DirectoryChangeHandlerEx.h"


#include "Wnd/ListViewWnd.h"
#include "Utils/UIFileList.h"
#include "Utils/ControlBuilder.h"
#include "Utils/ParseDsp.h"

#include "MacroDefine.h"
#include "UserDefine.h"
#include "WndLib.h"