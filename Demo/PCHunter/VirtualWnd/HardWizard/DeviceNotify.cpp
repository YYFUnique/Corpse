#include "StdAfx.h"
#include "DeviceNotify.h"

CDeviceNotify::CDeviceNotify()
{
	m_hDevNotify = NULL;
	m_pPaintManager = NULL;
}

CDeviceNotify::~CDeviceNotify()
{
	m_pPaintManager = NULL;
	if (m_hDevNotify != NULL)
	{
		UnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify = NULL;
	}
}

DUI_BEGIN_MESSAGE_MAP(CDeviceNotify, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CDeviceNotify::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CDeviceNotify::OnLoadItem(TNotifyUI& msg)
{
	if (m_hDevNotify != NULL)
		return;

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	// 注册为所有变更事件都接收
	m_hDevNotify = RegisterDeviceNotification(m_pPaintManager->GetPaintWindow(),&NotificationFilter,DEVICE_NOTIFY_WINDOW_HANDLE|DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
}

BOOL CDeviceNotify::OnDeviceChanged(WPARAM wParam, LPARAM lParam)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Hard"));
	if (pList == NULL)
		return TRUE;

	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

	TCHAR szEventInfo[MAX_PATH*4];
	CListTextElementUI* pTextElement = new CListTextElementUI;
	pList->Add(pTextElement);

	//设置序号
	_stprintf_s(szEventInfo, _countof(szEventInfo), _T("%d"), pList->GetCount());
	pTextElement->SetText(0, szEventInfo);

	//设置变化通知时间
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	_stprintf_s(szEventInfo, _countof(szEventInfo), _T("%02d:%02d:%02d"), SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	pTextElement->SetText(1, szEventInfo);

	//设置变化类型
	CHardMgr::GetDeviceChangeEventTypeText(wParam, szEventInfo);
	pTextElement->SetText(2, szEventInfo);

	//只有硬件接口类型才解析GUID
	if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
	{
		//设置硬件设备的GUID
		WCHAR	szGuid[MAX_PATH], szInfo[50];
		PDEV_BROADCAST_DEVICEINTERFACE pDevInterface = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
		CHardMgr::ConvertGuidToHardwareType(pDevInterface->dbcc_classguid, szInfo);

		StringFromGUID2(pDevInterface->dbcc_classguid, szGuid, _countof(szGuid));

		wcscat(szGuid, szInfo);

		pTextElement->SetText(3, szGuid);
	}

	//设置此次变化的额外信息
	CHardMgr::GetDeviceEventText(wParam, lParam, szEventInfo);
	pTextElement->SetText(4, szEventInfo);

	return TRUE;
}