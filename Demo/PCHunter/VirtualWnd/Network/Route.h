#pragma once
#include <Iphlpapi.h>

typedef CDuiList<MIB_IPADDRROW,const MIB_IPADDRROW&> CMibIpAddressList;

class CRoute : public CNotifyPump
{
public:
	CRoute();
	~CRoute();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	void OnRouteInfo(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnPaint(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);
protected:
	void ShowWindow();
	BOOL GetIpAddress(CMibIpAddressList& MibIpAddress);
	
	DWORD GetInterfaceIp(const CMibIpAddressList& MibIpAddress,const MIB_IPFORWARDROW& MibIpRow);
private:
	CPaintManagerUI* m_pPaintManager;
};