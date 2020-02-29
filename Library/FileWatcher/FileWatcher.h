#pragma once

#ifdef LS_STATIC_LIB_CALL
#define FILE_WATCHER_API 
#elif defined  FILEWATCHER_EXPORTS
#define FILE_WATCHER_API __declspec(dllexport)
#else
#define FILE_WATCHER_API __declspec(dllimport)
#endif


#define UILIB_COMDAT __declspec(selectany)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <shellapi.h>
#include <atlstr.h>

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif
#ifndef VERIFY
#define VERIFY(f)	ATLVERIFY(f)
#endif

#include "DllCore/Sync/SyncLock.h"
#include "DllCore/Log/LogHelper.h"

#define UWM_DELAYED_DIRECTORY_NOTIFICATION (WM_APP+1024)
#define READ_DIR_CHANGE_BUFFER_SIZE 4096

#include "TypePtrArray.h"
#include "FileNotifyInformation.h"
#include "DelayedNotifier.h"
#include "DirectoryWatchInfo.h"
#include "IDirectoryChangeHandler.h"
#include "DirectoryChangeHandler.h"
#include "DirectoryChangeWatcher.h"
#include "DirectoryChangeNotify.h"
#include "DelayedDirectoryChangeHandler.h"
#include "DirectoryChangeHandlerEx.h"