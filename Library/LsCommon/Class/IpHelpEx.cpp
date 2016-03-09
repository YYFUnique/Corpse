#include "StdAfx.h"
#include "IpHelpEx.h"
#include "../ErrorInfo.h"
#include "../OsInfo.h"
#include "../CommonFunc.h"
#include "Process.h"
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

BOOL LsGetIpNetTable(CMibIpNetRowList& MibIpNetRowList)
{
	MibIpNetRowList.RemoveAll();

	BYTE byBuffer[16*1024];
	memset(byBuffer,0,sizeof(byBuffer));
	PMIB_IPNETTABLE pIpNetTable = (PMIB_IPNETTABLE)byBuffer;
	DWORD dwBufferSize = sizeof(byBuffer);

	BOOL bSuccess = FALSE;
	do 
	{
		DWORD dwErrorCode=GetIpNetTable(pIpNetTable,&dwBufferSize,TRUE);
		if (dwErrorCode==ERROR_INSUFFICIENT_BUFFER)
		{
			pIpNetTable = (PMIB_IPNETTABLE)new BYTE[dwBufferSize];
			if (pIpNetTable==NULL)
			{
				SetErrorInfo(CUSTOM_ERROR,0,_T("查询适配器ARP缓存信息失败:分配内存失败(%d字节)."),dwBufferSize);
				break;
			}

			dwErrorCode=GetIpNetTable(pIpNetTable,&dwBufferSize,TRUE);
		}

		if (dwErrorCode!=NO_ERROR)
		{
			SetErrorInfo(SYSTEM_ERROR,dwErrorCode,_T("查询适配器地址信息失败"));
			break;
		}

		for(DWORD i=0;i<pIpNetTable->dwNumEntries;i++)
			MibIpNetRowList.AddTail(pIpNetTable->table[i]);

		bSuccess = TRUE;
	} while (FALSE);

	if (pIpNetTable && (BYTE*)pIpNetTable!=byBuffer)
		delete[] pIpNetTable;

	return bSuccess;
}

BOOL FlushDynamicArp()
{
	BOOL bSuccess=FALSE;

	do 
	{
		CMibIpNetRowList MibIpNetRowList;
		if (!LsGetIpNetTable(MibIpNetRowList))
		{
			SetErrorTitle(_T("清除本地动态ARP缓存时获取ARP列表失败"));
			break;
		}

		bSuccess = TRUE;
		POSITION pos=MibIpNetRowList.GetHeadPosition();
		while(pos)
		{
			MIB_IPNETROW& MibIpNetRow=MibIpNetRowList.GetNext(pos);

			if (MibIpNetRow.dwType != MIB_IPNET_TYPE_DYNAMIC)
				continue;

			DWORD dwErrorCode = DeleteIpNetEntry(&MibIpNetRow);
			if (dwErrorCode==NO_ERROR)
				continue;

			/*TCHAR szArpAddres[MAX_PATH];
			PrintMacAddr(MibIpNetRow.bPhysAddr,MibIpNetRow.dwPhysAddrLen,szArpAddres);
			SetErrorInfo(SYSTEM_ERROR,dwErrorCode,_T("清除动态ARP缓存项[IP:%s ARP:%s]失败"),GetIpString(MibIpNetRow.dwAddr),szArpAddres);
			*/
			bSuccess = FALSE;
		}	

	} while (FALSE);

	return bSuccess;
}

CString GetIpString(DWORD dwIPAddr)
{
	CString strAddr;
	IN_ADDR InAddr;
	InAddr.s_addr = dwIPAddr;

	strAddr = inet_ntoa(InAddr);

	return strAddr;
}

BOOL GetSystemNetworkProtocol(LPINT lpiProtocols,CWsaProtocolInfoList& WsaProtocolInfoList)
{
	BOOL bSuccess = FALSE;

	LPWSAPROTOCOL_INFO pWsaProtocolInfo = NULL;
	do
	{
		int nErrCode = 0;
		DWORD dwSize = 0;
		
		int nRet = WSCEnumProtocols(lpiProtocols,pWsaProtocolInfo,&dwSize,&nErrCode);
		if (nErrCode == WSAENOBUFS || nRet == SOCKET_ERROR)
		{
			pWsaProtocolInfo = (LPWSAPROTOCOL_INFO)new BYTE[dwSize];
			if (pWsaProtocolInfo == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统协议时，分配内存失败"));
				break;
			}

			nRet = WSCEnumProtocols(lpiProtocols,pWsaProtocolInfo,&dwSize,&nErrCode);
		}

		if (nRet != SOCKET_ERROR)
		{
			for (int i=0;i<nRet;++i)
				WsaProtocolInfoList.AddTail(pWsaProtocolInfo[i]);
		}

		bSuccess = TRUE;
	}while(FALSE);

	if (pWsaProtocolInfo != NULL)
	{
		delete[] pWsaProtocolInfo;
		pWsaProtocolInfo = NULL;
	}

	return bSuccess;
}

BOOL GetIpForwardTableIpv4(CMibIpforwardIpv4List& MibIpforwardIpv4List)
{
	BOOL bSuccess = FALSE;

	PMIB_IPFORWARDTABLE pMibIpforwardTable = NULL;

	do 
	{
		DWORD dwSize = 0;
		DWORD dwRet = GetIpForwardTable(pMibIpforwardTable,&dwSize,FALSE);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibIpforwardTable =(PMIB_IPFORWARDTABLE) new BYTE[dwSize];
			if (pMibIpforwardTable == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统IPv4路由信息，分配内存失败"));
				break;
			}
			dwRet = GetIpForwardTable(pMibIpforwardTable,&dwSize,FALSE);
		}

		if (dwRet == NO_ERROR)
		{
			for (DWORD dwIndex=0;dwIndex<pMibIpforwardTable->dwNumEntries;++dwIndex)
				MibIpforwardIpv4List.AddTail(pMibIpforwardTable->table[dwIndex]);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (pMibIpforwardTable != NULL)
	{
		delete[] pMibIpforwardTable;
		pMibIpforwardTable = NULL;
	}
	
	return bSuccess;
}

BOOL GetIpForwardTableList(CMibIpforwardTableList& MibIpforwardTableList,ADDRESS_FAMILY Family /* = AF_UNSPEC*/)
{
	BOOL bSuccess = FALSE;

	if (OsIsVistaOrLater() == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("当前操作系统不支持GetIpForwardTable2调用，请使用GetIpForwardTableIpv4函数"));
		return FALSE;
	}

	PMIB_IPFORWARD_TABLE2 pMibIpforwardTable =NULL;
	do 
	{
		DWORD dwRet = GetIpForwardTable2(Family,&pMibIpforwardTable);

		if (dwRet == NO_ERROR)
		{
			for (DWORD dwIndex=0;dwIndex<pMibIpforwardTable->NumEntries;++dwIndex)
				MibIpforwardTableList.AddTail(pMibIpforwardTable->Table[dwIndex]);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == TRUE)
		FreeMibTable(pMibIpforwardTable);

	return bSuccess;
}


BOOL GetTcpNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList)
{
	BOOL bSuccess = FALSE;
	PMIB_TCPTABLE_OWNER_MODULE pMibTcpTableOwnerModule = NULL;
	do 
	{
		DWORD dwSize = 0;
		DWORD dwRet = GetExtendedTcpTable(pMibTcpTableOwnerModule, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_MODULE_ALL , 0);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibTcpTableOwnerModule = (PMIB_TCPTABLE_OWNER_MODULE) new BYTE[dwSize];
			if (pMibTcpTableOwnerModule == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取TCP连接状态时，分配内存失败"));
				break;
			}

			dwRet = GetExtendedTcpTable(pMibTcpTableOwnerModule, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_MODULE_ALL , 0);
		}
		
		if (dwRet != NO_ERROR)
			break;

		BYTE pData[TABLE_SIZE];
		
		for (DWORD dwIndex = 0;dwIndex<pMibTcpTableOwnerModule->dwNumEntries;++dwIndex)
		{
			LOCAL_NET_CONNECTION_STATE LocalNetConnectState;
			TCPIP_OWNER_MODULE_BASIC_INFO* TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO)pData;
			DWORD dwSize = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
			DWORD dwRet = GetOwnerModuleFromTcpEntry(&pMibTcpTableOwnerModule->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			if (dwRet == ERROR_INSUFFICIENT_BUFFER)
			{
				TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO) new BYTE[dwSize];
				if (TcpInfo == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("获取TCP连接详细信息，分配内存失败"));
					break;
				}
				dwRet = GetOwnerModuleFromTcpEntry(&pMibTcpTableOwnerModule->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			}
			if (dwRet != NO_ERROR)
				break;

			LocalNetConnectState.strProcessName = TcpInfo->pModuleName;		//进程或服务名

			if (TcpInfo != (PTCPIP_OWNER_MODULE_BASIC_INFO)pData)
			{
				delete[] TcpInfo;
				TcpInfo = NULL;
			}

			LocalNetConnectState.dwLocalIpAddr = pMibTcpTableOwnerModule->table[dwIndex].dwLocalAddr;
			LocalNetConnectState.dwLocalPort = (DWORD)ntohs((WORD)pMibTcpTableOwnerModule->table[dwIndex].dwLocalPort);
			LocalNetConnectState.dwRemoteIpAddr = pMibTcpTableOwnerModule->table[dwIndex].dwRemoteAddr;
			LocalNetConnectState.dwRemotePort = (DWORD)ntohs((WORD)pMibTcpTableOwnerModule->table[dwIndex].dwRemotePort);
			LocalNetConnectState.dwState = pMibTcpTableOwnerModule->table[dwIndex].dwState;
			LocalNetConnectState.dwProcessID = pMibTcpTableOwnerModule->table[dwIndex].dwOwningPid;
			LocalNetConnectState.liCreateTime.QuadPart = pMibTcpTableOwnerModule->table->liCreateTimestamp.QuadPart;
			GetProcessUserName(LocalNetConnectState.dwProcessID,LocalNetConnectState.strProcessAuth);
			GetProcessFullPath(LocalNetConnectState.dwProcessID,LocalNetConnectState.strLocalProcessPath);
			LocalNetConnectState.dwProtol = IPPROTO_TCP;

			LocalNetConnectionStateList.AddTail(LocalNetConnectState);
		}

		bSuccess = LocalNetConnectionStateList.GetSize() != FALSE;
	} while (FALSE);

	if (pMibTcpTableOwnerModule)
	{
		delete[] pMibTcpTableOwnerModule;
		pMibTcpTableOwnerModule = NULL;
	}

	return bSuccess;
}

BOOL GetUdpNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList)
{
	BOOL bSuccess = FALSE;
	PMIB_UDPTABLE_OWNER_MODULE pMibUdpTableOwnerModule = NULL;

	do 
	{
		DWORD dwSize = 0;
		DWORD dwRet = GetExtendedUdpTable(pMibUdpTableOwnerModule,&dwSize,TRUE,AF_INET,UDP_TABLE_OWNER_MODULE,0);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibUdpTableOwnerModule = (PMIB_UDPTABLE_OWNER_MODULE)new BYTE[dwSize];
			if (pMibUdpTableOwnerModule == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取UDP连接状态时，分配内存失败"));
				break;
			}
			dwRet = GetExtendedUdpTable(pMibUdpTableOwnerModule,&dwSize,TRUE,AF_INET,UDP_TABLE_OWNER_MODULE,0);
		}

		if (dwRet != NO_ERROR)
			break;

		BYTE pData[TABLE_SIZE];
		for(DWORD dwIndex=0;dwIndex<pMibUdpTableOwnerModule->dwNumEntries;++dwIndex)
		{
			MIB_UDPROW_OWNER_MODULE  UdpRow = pMibUdpTableOwnerModule->table[dwIndex];
			LOCAL_NET_CONNECTION_STATE localUdpConnectionState;

			TCPIP_OWNER_MODULE_BASIC_INFO* UdpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO)pData;
			DWORD dwSize = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
			DWORD dwRet = GetOwnerModuleFromUdpEntry(&pMibUdpTableOwnerModule->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,UdpInfo,&dwSize);
			if (dwRet == ERROR_INSUFFICIENT_BUFFER)
			{
				UdpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO) new BYTE[dwSize];
				if (UdpInfo == NULL)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("获取UDP连接详细信息失败"));
					break;
				}
				dwRet = GetOwnerModuleFromUdpEntry(&pMibUdpTableOwnerModule->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,UdpInfo,&dwSize);
			}

			if (dwRet != NO_ERROR)
				break;

			localUdpConnectionState.strProcessName = UdpInfo->pModuleName;		//进程或服务名

			if (UdpInfo != (PTCPIP_OWNER_MODULE_BASIC_INFO)pData)
			{
				delete[] UdpInfo;
				UdpInfo = NULL;
			}

			localUdpConnectionState.dwLocalIpAddr = UdpRow.dwLocalAddr;
			localUdpConnectionState.dwLocalPort   = (DWORD)ntohs((WORD)UdpRow.dwLocalPort);
			localUdpConnectionState.dwRemoteIpAddr = 0;
			localUdpConnectionState.dwRemotePort = 0;
			localUdpConnectionState.dwProcessID   = UdpRow.dwOwningPid;
			localUdpConnectionState.liCreateTime.QuadPart = UdpRow.liCreateTimestamp.QuadPart;

			GetProcessUserName(UdpRow.dwOwningPid, localUdpConnectionState.strProcessAuth);
			GetProcessFullPath(localUdpConnectionState.dwProcessID,localUdpConnectionState.strLocalProcessPath);
			localUdpConnectionState.dwProtol = IPPROTO_UDP;
			LocalNetConnectionStateList.AddTail(localUdpConnectionState);
		}

		bSuccess = LocalNetConnectionStateList.GetSize() != FALSE;
	} while (FALSE);

	if (pMibUdpTableOwnerModule != NULL)
	{
		delete[] pMibUdpTableOwnerModule;
		pMibUdpTableOwnerModule = NULL;
	}

	return bSuccess;
}

BOOL GetNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList)
{
	AdjustProcessAuthority(SE_DEBUG_NAME);
	GetTcpNetWorkConnectTable(LocalNetConnectionStateList);
	GetUdpNetWorkConnectTable(LocalNetConnectionStateList);
	
	return TRUE;
}