#pragma once
#include <Iphlpapi.h>
typedef CDuiList<IP_ADAPTER_ADDRESSES, const IP_ADAPTER_ADDRESSES&> CAdapterAddressList;

typedef struct tagIP_GATEWAY_DNS
{
	CString strIPAddress;
}IP_GATEWAY_DNS;
typedef CDuiList<IP_GATEWAY_DNS,const IP_GATEWAY_DNS&>CIpGatewayDnsList;

class CAdapter : public CNotifyPump
{
public:
	CAdapter();
	~CAdapter();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnRefresh(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
protected:
	CString StrFormatMoney(DWORD dwDesc);
	CString GetMediaType(DWORD dwMediaType);
	CString GetMediaStatus(DWORD dwMediaStatus);
	BOOL IsAdapterPromiscuous(LPCSTR lpszAdapterName);
	void InsertNodeLabel(CTreeNodeUI* pTreeNode, LPCTSTR lpszItemText);
	void GetIpGateWayDns(PIP_ADAPTER_ADDRESSES pAdapterInfo, CIpGatewayDnsList& IpGatewayDns);
	BOOL GetAdapterInfo(PIP_ADAPTER_ADDRESSES& pAdapterAddress);
	BOOL GetAdapterInfoByGuid(PIP_ADAPTER_ADDRESSES pAdapterInfo, const GUID DeviceGuid,PIP_ADAPTER_ADDRESSES& pIpAdapterInfo);
protected:
	BOOL	m_bLoad;
	CPaintManagerUI* m_pPaintManager;
};