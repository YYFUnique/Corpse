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

#include "VirtualWnd/DSA/DSDebug.h"

#include "VirtualWnd/KSP/KSPInfo.h"
#include "VirtualWnd/KSP/KvmSec.h"

#include "VirtualWnd/ISPS/Phison.h"
#include "VirtualWnd/ISPS/ISPSDebug.h"
#include "VirtualWnd/ISPS/DeviceTag.h"
#include "VirtualWnd/ISPS/ISPSInterfaceTest.h"

#include "VirtualWnd/FileMonitor/FileMonitor.h"

#include "VirtualWnd/IVirtualWndMgr.h"

#include "VirtualWnd/DSA/DSManager.h"
#include "VirtualWnd/KSP/KSPManager.h"
#include "VirtualWnd/ISPS/ISPSManager.h"
#include "VirtualWnd/FileMonitor/FileManager.h"