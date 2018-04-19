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

	// ע��Ϊ���б���¼�������
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

	//�������
	_stprintf_s(szEventInfo, _countof(szEventInfo), _T("%d"), pList->GetCount());
	pTextElement->SetText(0, szEventInfo);

	//���ñ仯֪ͨʱ��
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	_stprintf_s(szEventInfo, _countof(szEventInfo), _T("%02d:%02d:%02d"), SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	pTextElement->SetText(1, szEventInfo);

	//���ñ仯����
	CHardMgr::GetDeviceChangeEventTypeText(wParam, szEventInfo);
	pTextElement->SetText(2, szEventInfo);

	//ֻ��Ӳ���ӿ����ͲŽ���GUID
	if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
	{
		//����Ӳ���豸��GUID
		WCHAR	szGuid[MAX_PATH], szInfo[50];
		PDEV_BROADCAST_DEVICEINTERFACE pDevInterface = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
		CHardMgr::ConvertGuidToHardwareType(pDevInterface->dbcc_classguid, szInfo);

		StringFromGUID2(pDevInterface->dbcc_classguid, szGuid, _countof(szGuid));

		wcscat(szGuid, szInfo);

		pTextElement->SetText(3, szGuid);
	}

	//���ô˴α仯�Ķ�����Ϣ
	CHardMgr::GetDeviceEventText(wParam, lParam, szEventInfo);
	pTextElement->SetText(4, szEventInfo);

	return TRUE;
}