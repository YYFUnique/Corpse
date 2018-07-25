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

class CConnect : public CNotifyPump
{
public:
	CConnect();
	~CConnect();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnRefresh(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
protected:
	void GetTcpConnectionTable(CConnectList& TcpListInfo);
	void GetUdpConnectionTable(CConnectList& UdpListInfo);
private:
	BOOL	m_bLoad;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder	   m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};