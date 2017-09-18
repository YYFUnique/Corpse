#pragma once
#include <Iphlpapi.h>

typedef CDuiList<MIB_IPADDRROW,const MIB_IPADDRROW&> CMibIpAddressList;
class CRoute : public CBase
{
public:
	CRoute();
	~CRoute();

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint();
	void OnMenu(TNotifyUI& msg);
	void OnRouteInfo(CControlUI* pControl);
protected:
	void ShowWindow();
	BOOL GetIpAddress(CMibIpAddressList& MibIpAddress);
	
	DWORD GetInterfaceIp(const CMibIpAddressList& MibIpAddress,const MIB_IPFORWARDROW& MibIpRow);
	//static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
};