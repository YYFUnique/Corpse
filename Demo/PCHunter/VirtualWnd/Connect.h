#pragma once

typedef struct  CONNECTION_INFO
{
	DWORD dwProtoclType;
	DWORD dwLocalIp;
	DWORD dwRemoteIp;
	DWORD  dwLocalPort;
	DWORD  dwRemotePort;
	DWORD dwConnectionState;
	INT64   nCreateTime;
	UINT	 nPID;
	CDuiString strProcessName;
	CDuiString strProcessPath;
}CONNECTION_INFO,*PCONNECTION_INFO;

typedef CDuiList<CONNECTION_INFO,const CONNECTION_INFO&> CConnectList;

class CConnect : public CBase
{
public:
	CConnect();
	~CConnect();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
protected:
	virtual void OnPaint();
	void GetTcpConnectionTable(CConnectList& ConnectListInfo);
	void GetUdpConnectionTable(CConnectList& ConnectListInfo);
	//static BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam);
};