#include "StdAfx.h"
#include "HardMgr.h"

CHardMgr::CHardMgr()
{
	m_pPaintManager = NULL;
}

CHardMgr::~CHardMgr()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CHardMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CHardMgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_DEVICE);

	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_CLIPBORD);
}

void CHardMgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_DEVICE, &m_DeviceNotify);
	m_DeviceNotify.SetPaintMagager(pPaintManager);

	/*pNotifyPump->AddVirtualWnd(VIRTUAL_WND_CLIPBORD, &m_ClipBordMonitor);
	m_ClipBordMonitor.SetPaintMagager(pPaintManager);*/
}

void CHardMgr::OnSelectChanged(TNotifyUI &msg)
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("HardWizard"));
	if (pTabLayout == NULL)
		return;

	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}

BOOL CHardMgr::DeviceChanged(WPARAM wParam, LPARAM lParam)
{
	if (wParam == DBT_DEVICEREMOVECOMPLETE || wParam == DBT_DEVICEARRIVAL )
	{
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
		if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			return m_DeviceNotify.OnDeviceChanged(wParam, lParam);
		//else if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
	}

	return TRUE;	
}

void CHardMgr::OnChangeCbChain(WPARAM wParam, LPARAM lParam)
{
	//m_ClipBordMonitor.OnChangeCbChain((HWND)wParam, (HWND)lParam);
}

void CHardMgr::OnDrawClipboard()
{
	//m_ClipBordMonitor.OnDrawClipboard();
}

void CHardMgr::GetDeviceChangeEventTypeText(UINT nEventType,LPTSTR lpszEventTypeText)
{
	LPCTSTR lpszEventName=NULL;

	switch(nEventType)
	{
	case DBT_CONFIGCHANGECANCELED:
		lpszEventName=_T("DBT_CONFIGCHANGECANCELED");
		break;
	case DBT_CONFIGCHANGED:
		lpszEventName=_T("DBT_CONFIGCHANGED");
		break;
	case DBT_CUSTOMEVENT:
		lpszEventName=_T("DBT_CUSTOMEVENT");
		break;
	case DBT_DEVICEARRIVAL:
		lpszEventName=_T("DBT_DEVICEARRIVAL");
		break;
	case DBT_DEVICEQUERYREMOVE:
		lpszEventName=_T("DBT_DEVICEQUERYREMOVE");
		break;
	case DBT_DEVICEQUERYREMOVEFAILED:
		lpszEventName=_T("DBT_DEVICEQUERYREMOVEFAILED");
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		lpszEventName=_T("DBT_DEVICEREMOVECOMPLETE");
		break;
	case DBT_DEVICEREMOVEPENDING:
		lpszEventName=_T("DBT_DEVICEREMOVEPENDING");
		break;
	case DBT_DEVICETYPESPECIFIC:
		lpszEventName=_T("DBT_DEVICETYPESPECIFIC");
		break;
	case DBT_DEVNODES_CHANGED:
		lpszEventName=_T("DBT_DEVNODES_CHANGED");
		break;
	case DBT_QUERYCHANGECONFIG:
		lpszEventName=_T("DBT_QUERYCHANGECONFIG");
		break;
	case DBT_USERDEFINED:
		lpszEventName=_T("DBT_USERDEFINED");
		break;
	case DBT_LOW_DISK_SPACE:
		lpszEventName=_T("DBT_LOW_DISK_SPACE");
		break;
	case DBT_NO_DISK_SPACE:
		lpszEventName=_T("DBT_NO_DISK_SPACE");
		break;
	default:
		lpszEventName=_T("UNKNOWN");
	}
	_stprintf_s(lpszEventTypeText,100,_T("%s"), lpszEventName, nEventType);
}


void CHardMgr::ConvertGuidToHardwareType(REFGUID rguid, LPWSTR lpwszHardwareType)
{
	static const GUID GuidDevInterfaceList[] = 
	{ 
		{ 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } }, //GUID_DEVINTERFACE_USB_DEVICE
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, //GUID_DEVINTERFACE_DISK
		{ 0x53f56308, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, //GUID_DEVINTERFACE_CDROM
		{ 0x53f5630a, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, //GUID_DEVINTERFACE_PARTITION
		{ 0x53f5630d, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, //GUID_DEVINTERFACE_VOLUME
		{ 0x4d1e55b2, 0xf16f, 0x11Cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } }, //GUID_DEVINTERFACE_HID
		{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }, //GUID_NDIS_LAN_CLASS
		{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } }, //GUID_DEVINTERFACE_COMPORT
		{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } }, //GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
		{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } }, //GUID_DEVINTERFACE_PARALLEL
		{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }, //GUID_DEVINTERFACE_PARCLASS
		{ 0x2c7089aa, 0x2e0e, 0x11d1, { 0xb1, 0x14, 0x00, 0xc0, 0x4f, 0xc2, 0xaa, 0xe4 } }, //GUID_DEVINTERFACE_MODEM
		{ 0x378de44c, 0x56ef, 0x11d1, { 0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd } }, //GUID_DEVINTERFACE_MOUSE
	}; 

	static const LPCWSTR szHardwareType[] = 
	{
		L"USB_DEVICE",
		L"DISK",
		L"CDROM",
		L"PARTITION",
		L"VOLUME",
		L"HID",
		L"NDIS_LAN_CLASS",
		L"COMPORT",
		L"SERENUM_BUS_ENUMERATOR",
		L"PARALLEL",
		L"PARCLASS",
		L"Modem",
		L"Mouse",
	};

	for(int i=0;i<sizeof(GuidDevInterfaceList)/sizeof(GUID);i++)
	{
		if (IsEqualGUID(rguid,GuidDevInterfaceList[i])==FALSE)
			continue;

		swprintf_s(lpwszHardwareType ,50, L"(%s)", szHardwareType[i]);
		return;
	}
	lpwszHardwareType[0]=0;
}

void CHardMgr::GetDeviceEventText(DWORD dwType, DWORD dwData, LPTSTR szEventText)
{
	szEventText[0]=0;
	switch(dwType)
	{
	case DBT_CONFIGCHANGECANCELED:
		OnConfigChangeCanceled(dwData,szEventText);
		break;
	case DBT_DEVICEARRIVAL:
		OnDeviceArrival(dwData,szEventText);
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		OnDeviceRemoveComplete(dwData,szEventText);
		break;
	case DBT_DEVICEREMOVEPENDING:
		OnDeviceRemovePending(dwData,szEventText);
		break;
	case DBT_DEVICEQUERYREMOVE:
		OnDeviceQueryRemove(dwData,szEventText);
		break;
	}
}

void CHardMgr::OnConfigChangeCanceled(DWORD dwData,LPTSTR szEventText)
{
	_stprintf_s(szEventText,100,_T("lParam=%08X"),dwData);
}

void CHardMgr::OnDeviceArrival(DWORD dwData,LPTSTR szEventText)
{
	GetDevBroadcastInfo((DEV_BROADCAST_HDR*)dwData, szEventText);
}

void CHardMgr::OnDeviceRemoveComplete(DWORD dwData,LPTSTR szEventText)
{
	GetDevBroadcastInfo((DEV_BROADCAST_HDR*)dwData, szEventText);
}

void CHardMgr::OnDeviceRemovePending(DWORD dwData,LPTSTR szEventText)
{
	GetDevBroadcastInfo((DEV_BROADCAST_HDR*)dwData, szEventText);
}

void CHardMgr::OnDeviceQueryRemove(DWORD dwData,LPTSTR szEventText)
{
	GetDevBroadcastInfo((DEV_BROADCAST_HDR*)dwData, szEventText);
}

void CHardMgr::GetDevBroadcastInfo(PDEV_BROADCAST_HDR pDevBroadcastHdr,LPTSTR lpszEventText)
{
	if (pDevBroadcastHdr==NULL)
		return;

	switch(pDevBroadcastHdr->dbch_devicetype)
	{
	case DBT_DEVTYP_DEVICEINTERFACE:
		{
			PDEV_BROADCAST_DEVICEINTERFACE pDevInterface = (PDEV_BROADCAST_DEVICEINTERFACE)pDevBroadcastHdr;
			_stprintf_s(lpszEventText, MAX_PATH, _T("Device Name=%s"), pDevInterface->dbcc_name);
		}
		break;
	case DBT_DEVTYP_VOLUME:
		{
			PDEV_BROADCAST_VOLUME pDevVolume = (PDEV_BROADCAST_VOLUME)pDevBroadcastHdr;
			TCHAR szDriveName[]=_T("A:\\ ");
			for(int i=1;i<(1<<26);i<<=1,szDriveName[0]++)
			{
				if ((pDevVolume->dbcv_unitmask & i) == 0)
					continue;

				_tcscat_s(lpszEventText, 10, szDriveName);
			}
		}
		break;
	}
}