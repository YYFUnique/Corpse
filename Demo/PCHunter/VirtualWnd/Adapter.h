#pragma once
#include <atlstr.h>

typedef CDuiList<IP_ADAPTER_ADDRESSES,const IP_ADAPTER_ADDRESSES&> CAdapterAddressList;

typedef struct tagIP_GATEWAY_DNS
{
	CString strIPAddress;
}IP_GATEWAY_DNS;
typedef CDuiList<IP_GATEWAY_DNS,const IP_GATEWAY_DNS&>CIpGatewayDnsList;

class CAdapter : public CBase
{
public:
	CAdapter();
	~CAdapter();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void OnPaint();
protected:
	/*void ShowWindow();*/
	//static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
	BOOL GetAdapterInfo(PIP_ADAPTER_ADDRESSES& pAdapterAddress);

	void InsertNodeLabel(CTreeNodeUI* pTreeNode, LPCTSTR lpszItemText);
	CString StrFormatMoney(DWORD dwDesc);
	void GetIpGateWayDns(PIP_ADAPTER_ADDRESSES pAdapterInfo,CIpGatewayDnsList& IpGatewayDns);

	CString GetMediaType(DWORD dwMediaType);

	CString GetMediaStatus(DWORD dwMediaStatus);
	BOOL IsAdapterPromiscuous(LPCSTR lpszAdapterName);
	BOOL GetAdapterInfoByGuid(PIP_ADAPTER_ADDRESSES pAdapterInfo, const GUID DeviceGuid,PIP_ADAPTER_ADDRESSES& pIpAdapterInfo);
};