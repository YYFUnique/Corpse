#include "stdafx.h"
#include "../Utils/AdapterInfo.h"
#include <setupapi.h>
#include <devguid.h>
#include <cfgmgr32.h>
#include <locale>
#include <atltime.h>
#include <Iphlpapi.h>
#include <Rpc.h>
#include <WinIoCtl.h>
#include <ntddndis.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Rpcrt4.lib")

#pragma comment(lib,"ole32.lib")//CoCreateInstance函数需要

CAdapter::CAdapter()
{
	/*m_bTimer = TRUE;*/
}

CAdapter::~CAdapter()
{

}

void CAdapter::OnPaint()
{
	PIP_ADAPTER_ADDRESSES pIpAdapterAddress = NULL;
	GetAdapterInfo(pIpAdapterAddress);

	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pPaintManager->FindControl(_T("Adapter"));
	
	if (pTreeView->GetCount())
		pTreeView->RemoveAll();

	CMibIpforwardIpv4List MibIpForwardIpv4;
	GetIpForwardTableIpv4(MibIpForwardIpv4);

	if (pIpAdapterAddress == NULL)
		return;

	CNetPropertiesList NetPropertiesInfo;
	if (GetAllShowAdapter(NetPropertiesInfo) == FALSE)
		return;

	PIP_ADAPTER_ADDRESSES pAdapterInfo = pIpAdapterAddress;

	NETCON_MEDIATYPE MediaType = NCM_NONE;
	NETCON_STATUS MediaStatus = NCS_DISCONNECTED;
	do
	{
		BOOL bContinue = FALSE;
		GUID AdapterGuid;
		CHAR szGuidString[100];
		memcpy_s(szGuidString,_countof(szGuidString),&pAdapterInfo->AdapterName[1],MAX_GUID_STRING_LEN);
		szGuidString[strlen(szGuidString)-1] = _T('\0');
		RPC_STATUS rpcStatus = UuidFromStringA((BYTE*)szGuidString, (UUID*)&AdapterGuid);

		POSITION posProperties = NetPropertiesInfo.GetHeadPosition();
		while(posProperties)
		{
			const NETCON_PROPERTIES& NetProperties = NetPropertiesInfo.GetNext(posProperties);

			if (IsEqualGUID(AdapterGuid,NetProperties.guidId) != FALSE)
			{
				MediaType = NetProperties.MediaType;
				MediaStatus = NetProperties.Status;
				bContinue = TRUE;
				break;
			}
		}

		if (bContinue == FALSE)
			continue;

		//设置网卡名称
		CTreeNodeUI* pTreeNode = new CTreeNodeUI;
		pTreeView->Add(pTreeNode);

		CString strItemText;
		CString strAdapterName(pAdapterInfo->AdapterName);
		strItemText.Format(_T("%s : %s - %s"),pAdapterInfo->FriendlyName,pAdapterInfo->Description,strAdapterName);

		pTreeNode->SetItemText(strItemText);
		pTreeNode->GetItemButton()->SetFont(0);
		pTreeNode->SetFixedHeight(33);

		//设置媒体类型
		CTreeNodeUI* pSubNode1 = new CTreeNodeUI;
		pTreeNode->Add(pSubNode1);

		pTreeNode->SetChildPadding(10);

		strItemText.Format(_T("媒体类型 : %s"),GetMediaType(MediaType));
		pSubNode1->SetItemText(strItemText);
		pSubNode1->SetFixedHeight(20);
		pSubNode1->SetItemFont(0);

		MIB_IFROW MibIfRow = {0};
		MibIfRow.dwIndex = pAdapterInfo->IfIndex;
		if (GetIfEntry(&MibIfRow) != NO_ERROR)
			continue;

		////显示网卡状态
		strItemText.Format(_T("状态 : %s"),GetMediaStatus(MediaStatus));
		InsertNodeLabel(pSubNode1,strItemText);
		if (MediaStatus ==  NCS_DISCONNECTED)
			continue;

		////显示网卡连接时间

		DWORD dwAdapterChangeTime = 0;
		POSITION pos = MibIpForwardIpv4.GetHeadPosition();
		while(pos)
		{
			const MIB_IPFORWARDROW& MibIpForwardRow = MibIpForwardIpv4.GetNext(pos);
			if (MibIpForwardRow.dwForwardIfIndex == pAdapterInfo->IfIndex)
			{
				dwAdapterChangeTime = MibIpForwardRow.dwForwardAge;
				break;
			}
		}

		if (dwAdapterChangeTime)
		{
			TCHAR szTime[MAX_PATH];
			StrFromTimeInterval(szTime,_countof(szTime),dwAdapterChangeTime*1000,6);
			InsertNodeLabel(pSubNode1,szTime);
		}

		CTreeNodeUI* pSubNode2 = new CTreeNodeUI;
		strItemText.Format(_T("适配器索引 : %d"),pAdapterInfo->IfIndex);
		pSubNode2->SetItemText(strItemText);
		pTreeNode->Add(pSubNode2);
		pSubNode2->SetItemFont(0);

		strItemText.Format(_T("DHCP : %s"),pAdapterInfo->Dhcpv4Enabled ? _T("启用") : _T("未启用"));
		InsertNodeLabel(pSubNode2,strItemText);

		strItemText.Format(_T("混杂模式 : %s"),IsAdapterPromiscuous(pAdapterInfo->AdapterName) ? _T("是") : _T("否"));
		InsertNodeLabel(pSubNode2,strItemText);

		CTreeNodeUI* pAdapterLineInfo = new CTreeNodeUI;
		CDuiString strAdapterAddr;

		for (UINT n=0;n<pAdapterInfo->PhysicalAddressLength;++n)
			strAdapterAddr.AppendFormat(_T("%02X-"),pAdapterInfo->PhysicalAddress[n]);

		strAdapterAddr.TrimRight(_T("-"));
		strItemText.Format(_T("MAC : %s"),strAdapterAddr);
		pAdapterLineInfo->SetItemText(strItemText);
		pTreeNode->Add(pAdapterLineInfo);
		pAdapterLineInfo->SetItemFont(0);

		strItemText.Format(_T("MTU : %d"),pAdapterInfo->Mtu);
		InsertNodeLabel(pAdapterLineInfo,strItemText);

		int nSpeed = (int)(pAdapterInfo->TransmitLinkSpeed/(1000*1000));
		if (nSpeed <0)
			nSpeed = 0;
		strItemText.Format(_T("速率 : %d Mbps"),nSpeed);
		InsertNodeLabel(pAdapterLineInfo,strItemText);

		//显示IP地址、网关地址、DNS地址		
		CIpGatewayDnsList IpGatewayDns;
		GetIpGateWayDns(pAdapterInfo,IpGatewayDns);

		POSITION posAddress = IpGatewayDns.GetHeadPosition();
		int n=0;
		CTreeNodeUI* pIpAddress = NULL;
		while(posAddress)
		{
			const IP_GATEWAY_DNS& IpInfo = IpGatewayDns.GetNext(posAddress);
			if (n%3==0)
			{
				pIpAddress = new CTreeNodeUI;
				pTreeNode->Add(pIpAddress);

				pIpAddress->SetItemFont(0);
				pIpAddress->SetItemText(IpInfo.strIPAddress);
			}
			else
				InsertNodeLabel(pIpAddress,IpInfo.strIPAddress);

			++n;
		}

		CTreeNodeUI* pOutPackageNode = new CTreeNodeUI;


		strItemText.Format(_T("已发送 : %s 字节"),StrFormatMoney(MibIfRow.dwOutOctets));
		pTreeNode->Add(pOutPackageNode);
		pOutPackageNode->SetItemText(strItemText);
		pOutPackageNode->SetItemFont(0);

		strItemText.Format(_T("单播包 : %s 个"),StrFormatMoney(MibIfRow.dwOutUcastPkts));
		InsertNodeLabel(pOutPackageNode,strItemText);

		strItemText.Format(_T("非单播包 : %s 个"),StrFormatMoney(MibIfRow.dwOutNUcastPkts));
		InsertNodeLabel(pOutPackageNode,strItemText);

		CTreeNodeUI* pInPackageNode = new CTreeNodeUI;
		pInPackageNode->SetItemFont(0);

		strItemText.Format(_T("已接收 : %s 字节"),StrFormatMoney(MibIfRow.dwInOctets));
		pTreeNode->Add(pInPackageNode);
		pInPackageNode->SetItemText(strItemText);

		strItemText.Format(_T("单播包 : %s 个"),StrFormatMoney(MibIfRow.dwInUcastPkts));
		InsertNodeLabel(pInPackageNode,strItemText);

		strItemText.Format(_T("非单播包 : %s 个"),StrFormatMoney(MibIfRow.dwInNUcastPkts));
		InsertNodeLabel(pInPackageNode,strItemText);
		//显示下一个网卡
		
	}while(pAdapterInfo = pAdapterInfo->Next);
	
	if (pIpAdapterAddress)
	{
		delete pIpAdapterAddress;
		pIpAdapterAddress = NULL;
	}	
}

BOOL CAdapter::GetAdapterInfo(PIP_ADAPTER_ADDRESSES& pAdapterAddress)
{
	BOOL bSuccess = FALSE;

	ULONG uSize = 0;
	do 
	{
		ULONG uRet = GetAdaptersAddresses(AF_INET,GAA_FLAG_INCLUDE_ALL_INTERFACES|GAA_FLAG_INCLUDE_ALL_COMPARTMENTS|GAA_FLAG_INCLUDE_GATEWAYS,NULL,pAdapterAddress,&uSize);
		if (uRet == ERROR_BUFFER_OVERFLOW)
		{
			pAdapterAddress = (PIP_ADAPTER_ADDRESSES)new BYTE[uSize];
			if (pAdapterAddress == NULL)
				break;
			uRet = GetAdaptersAddresses(AF_INET,GAA_FLAG_INCLUDE_ALL_INTERFACES|GAA_FLAG_INCLUDE_ALL_COMPARTMENTS|GAA_FLAG_INCLUDE_GATEWAYS,NULL,pAdapterAddress,&uSize);
		}
		if (uRet != ERROR_SUCCESS)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

CString CAdapter::StrFormatMoney(DWORD dwDesc)
{
	CString strMoney;
	CStdValArray strItemArr(5);
	CString strText;
	strText.Format(_T("%u"),dwDesc);
	int nLen = strText.GetLength()%3;

	if (nLen != 0)
		strMoney.AppendFormat(_T("%s,"),strText.Left(nLen));

	for (int n=nLen;n<=strText.GetLength();n+=3)
		strMoney.AppendFormat(_T("%s,"),strText.Mid(n,3));

	strMoney.TrimRight(_T(","));

	return strMoney;
}

void CAdapter::InsertNodeLabel(CTreeNodeUI* pTreeNode, LPCTSTR lpszItemText)
{
	CLabelUI* pLable = new CLabelUI;
	pLable->SetText(lpszItemText);
	pLable->SetFont(0);
	CHorizontalLayoutUI* pHor = pTreeNode->GetTreeNodeHoriznotal();
	pHor->SetFixedWidth(100);
	pHor->Add(pLable);
}

void CAdapter::GetIpGateWayDns(PIP_ADAPTER_ADDRESSES pAdapterInfo,CIpGatewayDnsList& IpGatewayDns)
{
	IP_GATEWAY_DNS IpInfo;
	PIP_ADAPTER_UNICAST_ADDRESS pAdapterUnicastAddress = pAdapterInfo->FirstUnicastAddress;
	while(pAdapterUnicastAddress)
	{
		SOCKADDR_IN* pInAddr =  (SOCKADDR_IN*)pAdapterUnicastAddress->Address.lpSockaddr;
		CString strAddress(inet_ntoa(pInAddr->sin_addr));
		IpInfo.strIPAddress.Format(_T("IP地址 : %s/%d"),strAddress,pAdapterUnicastAddress->OnLinkPrefixLength);

		IpGatewayDns.AddTail(IpInfo);

		pAdapterUnicastAddress = pAdapterUnicastAddress->Next;
	}

	PIP_ADAPTER_GATEWAY_ADDRESS_LH pAdapterGatewayAddress = pAdapterInfo->FirstGatewayAddress;
	while(pAdapterGatewayAddress)
	{
		SOCKADDR_IN* pInAddr =  (SOCKADDR_IN*)pAdapterGatewayAddress->Address.lpSockaddr;
		CString strAddress(inet_ntoa(pInAddr->sin_addr));
		IpInfo.strIPAddress.Format(_T("网关 : %s"),strAddress);

		IpGatewayDns.AddTail(IpInfo);
		pAdapterGatewayAddress = pAdapterGatewayAddress->Next;
	}

	PIP_ADAPTER_DNS_SERVER_ADDRESS pAdapterDnsAddress = pAdapterInfo->FirstDnsServerAddress;
	while(pAdapterDnsAddress)
	{
		SOCKADDR_IN* pInAddr =  (SOCKADDR_IN*)pAdapterDnsAddress->Address.lpSockaddr;
		CString strAddress(inet_ntoa(pInAddr->sin_addr));
		IpInfo.strIPAddress.Format(_T("DNS : %s"),strAddress);

		IpGatewayDns.AddTail(IpInfo);
		pAdapterDnsAddress = pAdapterDnsAddress->Next;
	}
}

CString CAdapter::GetMediaType(DWORD dwMediaType)
{
	switch (dwMediaType)
	{
		case NCM_LAN:
				return _T("Lan");
			break;
		case NCM_PHONE:
				return _T("Phone");
			break;
		case	NCM_TUNNEL	:
				return _T("Tunnel");
			break;
		case NCM_PPPOE:
				return _T("PPPOE");
			break;
		case NCM_BRIDGE:
				return _T("Bridge");
			break;
		default:
			return _T("未知");
	}
}

CString CAdapter::GetMediaStatus(DWORD dwMediaStatus)
{
	switch(dwMediaStatus)
	{
	case NCS_DISCONNECTED:
			return _T("已禁用");
		break;
	case NCS_CONNECTING	:
			return _T("连接中...");
		break;
	case NCS_CONNECTED:
			return _T("已连接");
		break;
	case NCS_DISCONNECTING:
			return _T("断开连接中");
		break;
	case NCS_HARDWARE_NOT_PRESENT:
			return _T("硬件不存在");
		break;
	case NCS_HARDWARE_DISABLED:
			return _T("已禁用");
		break;
	//NCS_HARDWARE_MALFUNCTION	= ( NCS_HARDWARE_DISABLED + 1 ) ,
	case NCS_MEDIA_DISCONNECTED:
			return _T("未连接");
		break;
	//NCS_AUTHENTICATING	= ( NCS_MEDIA_DISCONNECTED + 1 ) ,
	//NCS_AUTHENTICATION_SUCCEEDED	= ( NCS_AUTHENTICATING + 1 ) ,
	//NCS_AUTHENTICATION_FAILED	= ( NCS_AUTHENTICATION_SUCCEEDED + 1 ) ,
	//NCS_INVALID_ADDRESS	= ( NCS_AUTHENTICATION_FAILED + 1 ) ,
	//NCS_CREDENTIALS_REQUIRED	= ( NCS_INVALID_ADDRESS + 1 ) 
	default:
		return _T("未知");
	}
}

BOOL CAdapter::IsAdapterPromiscuous(LPCSTR lpszAdapterName)
{
	CString strAdapterName(lpszAdapterName);
	TCHAR szAdapterName[MAX_PATH];

	_stprintf_s(szAdapterName,_countof(szAdapterName),_T("\\\\.\\%s"),strAdapterName);

	HANDLE hAdapter = INVALID_HANDLE_VALUE;

	BOOL bSuccess = FALSE;

	do 
	{
		hAdapter = CreateFile(szAdapterName,0,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if (hAdapter == INVALID_HANDLE_VALUE)
			break;

		DWORD dwInBuf = OID_GEN_CURRENT_PACKET_FILTER, dwOutBuf = 0, dwByteReturn = 0; 

		if(DeviceIoControl(hAdapter, IOCTL_NDIS_QUERY_GLOBAL_STATS, 
						&dwInBuf, sizeof(dwInBuf), &dwOutBuf, sizeof(dwOutBuf), &dwByteReturn, NULL))
		{
			if ((NDIS_PACKET_TYPE_PROMISCUOUS & dwOutBuf) != NDIS_PACKET_TYPE_PROMISCUOUS)
				break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hAdapter != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hAdapter);
	}

	return bSuccess;
}