#pragma once

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "Utils/SnmpMgr.h"

#include "VirtualWnd/Task/Application.h"
#include "VirtualWnd/Task/Process.h"
#include "VirtualWnd/Task/Services.h"
#include "VirtualWnd/Task/UserInfo.h"
#include "VirtualWnd/Task/CoreDriver.h"
#include "VirtualWnd/Task/ViewCert.h"

#include "VirtualWnd/Network/Adapter.h"
#include "VirtualWnd/Network/Route.h"
#include "VirtualWnd/Network/HostScan.h"
#include "VirtualWnd/Network/Snmp.h"
#include "VirtualWnd/Network/Wlan.h"
#include "VirtualWnd/Network/Connect.h"

#include "VirtualWnd/HardWizard/DeviceNotify.h"
#include "VirtualWnd/HardWizard/ClipBoardMonitor.h"

#include "VirtualWnd/System/TrayIcon.h"

#include "VirtualWnd/Security/AutoStart.h"

#include "VirtualWnd/IVirtualWndMgr.h"

#include "VirtualWnd/Task/TaskMgr.h"
#include "VirtualWnd/Network/NetworkMgr.h"
#include "VirtualWnd/HardWizard/HardMgr.h"
#include "VirtualWnd/System/SystemMgr.h"
#include "VirtualWnd/Security/SecurityMgr.h"