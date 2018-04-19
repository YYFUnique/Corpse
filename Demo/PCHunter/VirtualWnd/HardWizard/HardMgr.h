#pragma once

class CHardMgr : public CNotifyPump, public IVirtualWndMgr
{
public:
	CHardMgr();
	~CHardMgr();

public:
	void OnDrawClipboard();
	void OnChangeCbChain(WPARAM wParam, LPARAM lParam);
	BOOL DeviceChanged(WPARAM wParam, LPARAM lParam);
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);

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
	DUI_DECLARE_MESSAGE_MAP()
	void OnSelectChanged( TNotifyUI &msg );
private:
	CDeviceNotify			m_DeviceNotify;
	CClipBoardMonitor	m_ClipBordMonitor;
	CPaintManagerUI* m_pPaintManager;
};