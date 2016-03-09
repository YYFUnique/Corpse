#include "stdafx.h"
#include <atltime.h>
#include <Iphlpapi.h>
#include <Winsock2.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Ws2_32.lib")


#define	TABLE_SIZE			2048

CConnect::CConnect()
{
	/*m_bTimer = TRUE;*/
}

CConnect::~CConnect()
{

}

void CConnect::OnPaint()
{
	CConnectList ConnectListInfo;
	GetTcpConnectionTable(ConnectListInfo);
	GetUdpConnectionTable(ConnectListInfo);

	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Connect"));
	if (pList->GetCount())
		pList->RemoveAll();

	POSITION pos = ConnectListInfo.GetHeadPosition();
	CDuiString strTipInfo;
	while (pos)
	{
		const CONNECTION_INFO& ConnectionInfo = ConnectListInfo.GetNext(pos);
		CListTextElementUI* pTextElement = new CListTextElementUI;
		pList->Add(pTextElement);
		int m=0;
		pTextElement->SetText(m++,ConnectionInfo.dwProtoclType != IPPROTO_TCP ? _T("UDP") : _T("TCP"));
		pTextElement->SetText(m++,GetFormatIPAndPort(ConnectionInfo.dwLocalIp,ntohs((WORD)ConnectionInfo.dwLocalPort)));
		if (ConnectionInfo.dwProtoclType == IPPROTO_TCP)
		{
			pTextElement->SetText(m++,GetFormatIPAndPort(ConnectionInfo.dwRemoteIp,ntohs((WORD)ConnectionInfo.dwRemotePort)));
			pTextElement->SetText(m++,GetConnectionState(ConnectionInfo.dwConnectionState));
		}
		else
		{
			pTextElement->SetText(m++,_T(""));
			pTextElement->SetText(m++,_T(""));
		}

		if (ConnectionInfo.dwProtoclType != IPPROTO_TCP)
		{
			CTime tm(ConnectionInfo.nCreateTime/10000000);
			strTipInfo.Format(_T("%02d:%02d:%02d"),tm.GetHour(),tm.GetMinute(),tm.GetSecond());
			pTextElement->SetText(m++,strTipInfo);
		}
		else
			pTextElement->SetText(m++,_T(""));
		strTipInfo.Format(_T("%d"),ConnectionInfo.nPID);
		pTextElement->SetText(m++,strTipInfo);
		pTextElement->SetText(m++,ConnectionInfo.strProcessName);
		pTextElement->SetText(m++,_T(""));
	}
}

void CConnect::GetTcpConnectionTable(CConnectList& TcpListInfo)
{
	BYTE pBuf[TABLE_SIZE];
	DWORD dwSize = TABLE_SIZE;
	PMIB_TCPTABLE_OWNER_MODULE pTcpTable = (PMIB_TCPTABLE_OWNER_MODULE)pBuf;
	DWORD dwSizeData=sizeof(PMIB_TCPTABLE_OWNER_MODULE);
	DWORD dwRet = GetExtendedTcpTable(pTcpTable,&dwSizeData,FALSE,AF_INET,TCP_TABLE_OWNER_MODULE_ALL,0);
	if (dwRet == ERROR_INSUFFICIENT_BUFFER)
	{
		pTcpTable = (PMIB_TCPTABLE_OWNER_MODULE) new BYTE[dwSizeData];
		if (pTcpTable != NULL)
			dwRet = GetExtendedTcpTable(pTcpTable,&dwSizeData,TRUE,AF_INET,TCP_TABLE_OWNER_MODULE_ALL,0);
	}
	if (dwRet == NO_ERROR)
	{
		CDuiString strProcessName,strLocalProcessPath;
		for (DWORD dwIndex = 0 ; dwIndex < pTcpTable->dwNumEntries ; dwIndex++)
		{
			strProcessName = _T("");
			DWORD dwConnectionState = pTcpTable->table[dwIndex].dwState;
			DWORD PID = pTcpTable->table[dwIndex].dwOwningPid;
			//获取模块路径
			//GetProcessNameAndPathByPID(PID, strProcessName, strLocalProcessPath);
			BYTE pData[TABLE_SIZE];
			TCPIP_OWNER_MODULE_BASIC_INFO* TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO)pData;
			DWORD dwSize = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
			DWORD dwRet = GetOwnerModuleFromTcpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			BOOL bReset = FALSE;
			if (dwRet == ERROR_INSUFFICIENT_BUFFER)
			{
				bReset = TRUE;
				TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO) new BYTE[dwSize];
				if (TcpInfo != NULL)
					dwRet = GetOwnerModuleFromTcpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			}

			if (dwRet == NO_ERROR)
				strProcessName = TcpInfo->pModuleName;		//进程或服务名
			//GetProcessName(strLocalProcessPath,strProcessName);
			if (TcpInfo && bReset)
			{
				delete[] TcpInfo;
				TcpInfo = NULL;
			}
			DWORD dwRemoteIP = pTcpTable->table[dwIndex].dwRemoteAddr;           //远程IP
			DWORD dwRemotePort = pTcpTable->table[dwIndex].dwRemotePort;         //远程端口
			if (dwConnectionState == MIB_TCP_STATE_LISTEN)
			{
				dwRemotePort = 0;
				dwRemoteIP   = 0;
			}
			
			MIB_TCPROW_OWNER_MODULE TcpOwnerModule = pTcpTable->table[dwIndex];

			CONNECTION_INFO ConnectInfo;
			ConnectInfo.dwProtoclType = IPPROTO_TCP;
			ConnectInfo.dwLocalIp = TcpOwnerModule.dwLocalAddr;
			ConnectInfo.dwLocalPort = TcpOwnerModule.dwLocalPort;
			ConnectInfo.dwRemoteIp = TcpOwnerModule.dwRemoteAddr;
			ConnectInfo.dwRemotePort = TcpOwnerModule.dwRemotePort;
			ConnectInfo.dwConnectionState = TcpOwnerModule.dwState;
			ConnectInfo.nCreateTime = TcpOwnerModule.liCreateTimestamp.QuadPart;
			ConnectInfo.nPID = TcpOwnerModule.dwOwningPid;
			ConnectInfo.strProcessName = strProcessName;

			TcpListInfo.AddTail(ConnectInfo);
		}
		if (pTcpTable)
		{
			delete[] pTcpTable;
		}
	}
}

void CConnect::GetUdpConnectionTable(CConnectList& UdpListInfo)
{
	BYTE pBuf[TABLE_SIZE];
	DWORD dwSize = TABLE_SIZE;
	PMIB_UDPTABLE_OWNER_MODULE pTcpTable = (PMIB_UDPTABLE_OWNER_MODULE)pBuf;
	DWORD dwSizeData=sizeof(PMIB_UDPTABLE_OWNER_MODULE);
	DWORD dwRet = GetExtendedUdpTable(pTcpTable,&dwSizeData,FALSE,AF_INET,UDP_TABLE_OWNER_MODULE,0);
	if (dwRet == ERROR_INSUFFICIENT_BUFFER)
	{
		pTcpTable = (PMIB_UDPTABLE_OWNER_MODULE) new BYTE[dwSizeData];
		if (pTcpTable != NULL)
			dwRet = GetExtendedUdpTable(pTcpTable,&dwSizeData,TRUE,AF_INET,UDP_TABLE_OWNER_MODULE,0);
	}
	if (dwRet == NO_ERROR)
	{
		CDuiString strProcessName,strLocalProcessPath;
		for (DWORD dwIndex = 0 ; dwIndex < pTcpTable->dwNumEntries ; dwIndex++)
		{
			strProcessName = _T("");
			DWORD PID = pTcpTable->table[dwIndex].dwOwningPid;
			//获取模块路径
			//GetProcessNameAndPathByPID(PID, strProcessName, strLocalProcessPath);
			BYTE pData[TABLE_SIZE];
			TCPIP_OWNER_MODULE_BASIC_INFO* TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO)pData;
			DWORD dwSize = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
			DWORD dwRet = GetOwnerModuleFromUdpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			BOOL bReset = FALSE;
			if (dwRet == ERROR_INSUFFICIENT_BUFFER)
			{
				bReset = TRUE;
				TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO) new BYTE[dwSize];
				if (TcpInfo != NULL)
					dwRet = GetOwnerModuleFromUdpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			}

			if (dwRet == NO_ERROR)
				strProcessName = TcpInfo->pModuleName;		//进程或服务名
			//GetProcessName(strLocalProcessPath,strProcessName);
			if (TcpInfo && bReset)
			{
				delete[] TcpInfo;
				TcpInfo = NULL;
			}

			MIB_UDPROW_OWNER_MODULE TcpOwnerModule = pTcpTable->table[dwIndex];

			CONNECTION_INFO ConnectInfo;
			ConnectInfo.dwProtoclType = IPPROTO_UDP;
			ConnectInfo.dwLocalIp = TcpOwnerModule.dwLocalAddr;
			ConnectInfo.dwLocalPort = TcpOwnerModule.dwLocalPort;
			ConnectInfo.dwRemoteIp = 0;
			ConnectInfo.dwRemotePort = 0;
			ConnectInfo.dwConnectionState = 0;
			ConnectInfo.nCreateTime = TcpOwnerModule.liCreateTimestamp.QuadPart;
			ConnectInfo.nPID = TcpOwnerModule.dwOwningPid;
			ConnectInfo.strProcessName = strProcessName;

			UdpListInfo.AddTail(ConnectInfo);
		}
		if (pTcpTable)
		{
			delete[] pTcpTable;
		}
	}
}
