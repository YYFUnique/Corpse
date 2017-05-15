#pragma once
#include "Dbt.h"

class CHardNotify : public CVirtualWnd
{
public:
	CHardNotify();
	~CHardNotify();
	
public:
	BOOL DeviceChanged(WPARAM wParam, LPARAM lParam);

protected:
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnSelectChanged(TNotifyUI& msg);
	//void SetPaintMagager(CPaintManagerUI* pPaintMgr);

public:
	static void GetDeviceEventText(DWORD dwType, DWORD dwData, LPTSTR szEventText);
	static void GetDeviceChangeEventTypeText(UINT nEventType,LPTSTR lpszEventTypeText);
	static void ConvertGuidToHardwareType(REFGUID rguid,LPWSTR lpwszHardwareType);

protected:
	static void OnConfigChangeCanceled(DWORD dwData,LPTSTR szEventText);
	static void OnDeviceArrival(DWORD dwData,LPTSTR szEventText);
	static void OnDeviceRemoveComplete(DWORD dwData,LPTSTR szEventText);
	static void OnDeviceRemovePending(DWORD dwData,LPTSTR szEventText);
	static void OnDeviceQueryRemove(DWORD dwData,LPTSTR szEventText);
	static void GetDevBroadcastInfo(PDEV_BROADCAST_HDR pDevBroadcastHdr,LPTSTR lpszEventText);
protected:
	CHardware m_Hardware;
};