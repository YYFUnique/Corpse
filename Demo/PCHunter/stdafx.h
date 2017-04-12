#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include "DuiLib/UIlib.h"
using namespace DuiLib;

#ifndef __POSITION
typedef struct __POSITION
{
}__POSITION,*POSITION;
#endif

#include "Utils/DuiList.h"

#include "Utils/TextTools.h"

#define	DUI_MSGTYPE_REFRESH		_T("refresh")

#include <winsock2.h>
#include <iphlpapi.h>

//���̹�����
#include "VirtualWnd/Base.h"
#include "VirtualWnd/VirtualWnd.h"
#include "VirtualWnd/Application.h"
#include "VirtualWnd/Process.h"
#include "VirtualWnd/Services.h"
#include "VirtualWnd/CoreDriver.h"
#include "VirtualWnd/ViewCSP.h"
#include "VirtualWnd/ViewCert.h"
#include "VirtualWnd/UserInfo.h"

//���������
#include "VirtualWnd/Adapter.h"
#include "VirtualWnd/Connect.h"
#include "VirtualWnd/Route.h"
#include "VirtualWnd/Provider.h"
#include "VirtualWnd/HostScan.h"
#include "VirtualWnd/Wlan.h"
#include "VirtualWnd/snmp.h"

//ϵͳ��Ϣ�鿴����
#include "VirtualWnd/OsInfo.h"

//Ӳ���仯֪ͨ
#include "VirtualWnd/Hardware.h"

//ģ�������
#include "VirtualWnd/TaskMgr.h"
#include "VirtualWnd/NetMgr.h"
#include "VirtualWnd/SysInfoMgr.h"
#include "VirtualWnd/HardNotify.h"

