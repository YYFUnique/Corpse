#include "StdAfx.h"
#include "Adapter.h"
#include <WinIoCtl.h>
#include <ntddndis.h>
#include "../../Utils/AdapterInfo.h"
#include "DllCore/Utils/TextTools.h"

CAdapter::CAdapter()
{
	m_bLoad = FALSE;
}

CAdapter::~CAdapter()
{

}

DUI_BEGIN_MESSAGE_MAP(CAdapter, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CAdapter::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CAdapter::OnRefresh(TNotifyUI& msg)
{
	m_bLoad = FALSE;
	OnLoadItem(msg);
}

void CAdapter::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pPaintManager->FindControl(_T("Adapter"));

	if (pTreeView->GetCount())
		pTreeView->RemoveAll();

	PIP_ADAPTER_ADDRESSES pIpAdapterAddress = NULL;
	GetAdapterInfo(pIpAdapterAddress);

	CMibIpforwardIpv4List MibIpForwardIpv4;
	GetIpForwardTableIpv4(MibIpForwardIpv4);

	CNetPropertiesList NetPropertiesInfo;
	if (GetAllShowAdapter(NetPropertiesInfo) == FALSE)
		return;

	PIP_ADAPTER_ADDRESSES pAdapterInfo = NULL;

	NETCON_MEDIATYPE MediaType = NCM_NONE;
	NETCON_STATUS MediaStatus = NCS_DISCONNECTED;

	POSITION pos = NetPropertiesInfo.GetHeadPosition();
	while (pos)
	{
		const NETCON_PROPERTIES& NetConn = NetPropertiesInfo.GetNext(pos);
		
		//������������
		CTreeNodeUI* pTreeNode = new CTreeNodeUI;
		pTreeView->Add(pTreeNode);

		BSTR strAdapterGUID;
		StringFromCLSID(NetConn.guidId, &strAdapterGUID);

		CString strItemText;
		CString strAdapterFriendlyName(NetConn.pszwName);
		CString strAdapterDeviceName(NetConn.pszwDeviceName);
		CString strAdapter = strAdapterGUID;
		strItemText.Format(_T("%s : %s - %s"), strAdapterFriendlyName, strAdapterDeviceName, strAdapterGUID);

		pTreeNode->SetItemText(strItemText);
		pTreeNode->GetItemButton()->SetFont(0);
		pTreeNode->SetFixedHeight(33);

		//����ý������
		CTreeNodeUI* pSubNode1 = new CTreeNodeUI;
		pTreeNode->Add(pSubNode1);

		pTreeNode->SetChildPadding(10);

		strItemText.Format(_T("ý������ : %s"), GetMediaType(NetConn.MediaType));
		pSubNode1->SetItemText(strItemText);
		pSubNode1->SetFixedHeight(20);
		pSubNode1->SetItemFont(0);

		////��ʾ����״̬
		strItemText.Format(_T("״̬ : %s"), GetMediaStatus(NetConn.Status));
		InsertNodeLabel(pSubNode1,strItemText);
		if (NetConn.Status ==  NCS_DISCONNECTED)
			continue;

		if (GetAdapterInfoByGuid(pIpAdapterAddress, NetConn.guidId, pAdapterInfo) == FALSE)
			continue;

		MIB_IFROW MibIfRow = {0};
		MibIfRow.dwIndex = pAdapterInfo->IfIndex;
		if (GetIfEntry(&MibIfRow) != NO_ERROR)
			continue;
		//��ʾ��������ʱ��
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
			StrFromTimeIntervalEx(szTime,_countof(szTime),dwAdapterChangeTime*1000,6);
			InsertNodeLabel(pSubNode1,szTime);
		}

		CTreeNodeUI* pSubNode2 = new CTreeNodeUI;
		strItemText.Format(_T("���������� : %d"),pAdapterInfo->IfIndex);
		pSubNode2->SetItemText(strItemText);
		pTreeNode->Add(pSubNode2);
		pSubNode2->SetItemFont(0);

		strItemText.Format(_T("DHCP : %s"),pAdapterInfo->Dhcpv4Enabled ? _T("����") : _T("δ����"));
		InsertNodeLabel(pSubNode2,strItemText);

		strItemText.Format(_T("����ģʽ : %s"),IsAdapterPromiscuous(pAdapterInfo->AdapterName) ? _T("��") : _T("��"));
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
		strItemText.Format(_T("���� : %d Mbps"),nSpeed);
		InsertNodeLabel(pAdapterLineInfo,strItemText);

		//��ʾIP��ַ�����ص�ַ��DNS��ַ		
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

		strItemText.Format(_T("�ѷ��� : %s �ֽ�"), StrFormatMoney(MibIfRow.dwOutOctets));
		pTreeNode->Add(pOutPackageNode);
		pOutPackageNode->SetItemText(strItemText);
		pOutPackageNode->SetItemFont(0);

		strItemText.Format(_T("������ : %s ��"), StrFormatMoney(MibIfRow.dwOutUcastPkts));
		InsertNodeLabel(pOutPackageNode,strItemText);

		strItemText.Format(_T("�ǵ����� : %s ��"), StrFormatMoney(MibIfRow.dwOutNUcastPkts));
		InsertNodeLabel(pOutPackageNode,strItemText);

		CTreeNodeUI* pInPackageNode = new CTreeNodeUI;
		pInPackageNode->SetItemFont(0);

		strItemText.Format(_T("�ѽ��� : %s �ֽ�"), StrFormatMoney(MibIfRow.dwInOctets));
		pTreeNode->Add(pInPackageNode);
		pInPackageNode->SetItemText(strItemText);

		strItemText.Format(_T("������ : %s ��"), StrFormatMoney(MibIfRow.dwInUcastPkts));
		InsertNodeLabel(pInPackageNode,strItemText);

		strItemText.Format(_T("�ǵ����� : %s ��"), StrFormatMoney(MibIfRow.dwInNUcastPkts));
		InsertNodeLabel(pInPackageNode,strItemText);
	}
}


BOOL CAdapter::GetAdapterInfoByGuid(PIP_ADAPTER_ADDRESSES pAdapterInfo, const GUID DeviceGuid,PIP_ADAPTER_ADDRESSES& pIpAdapterInfo)
{
	BOOL bSuccess = FALSE;
	PIP_ADAPTER_ADDRESSES pDeviceInfo = pAdapterInfo;
	while(pDeviceInfo)
	{
		CStringW strAdapterName = pDeviceInfo->AdapterName;
		GUID DeviceNameGuid;
		CLSIDFromString(_bstr_t(strAdapterName),&DeviceNameGuid);
		if (IsEqualGUID(DeviceNameGuid,DeviceGuid))
		{
			pIpAdapterInfo = pDeviceInfo;
			return TRUE;
		}
		pDeviceInfo = pDeviceInfo->Next;
	}

	return bSuccess;
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

void CAdapter::GetIpGateWayDns(PIP_ADAPTER_ADDRESSES pAdapterInfo,CIpGatewayDnsList& IpGatewayDns)
{
	IP_GATEWAY_DNS IpInfo;
	PIP_ADAPTER_UNICAST_ADDRESS pAdapterUnicastAddress = pAdapterInfo->FirstUnicastAddress;
	while(pAdapterUnicastAddress)
	{
		SOCKADDR_IN* pInAddr =  (SOCKADDR_IN*)pAdapterUnicastAddress->Address.lpSockaddr;
		CString strAddress(inet_ntoa(pInAddr->sin_addr));
		IpInfo.strIPAddress.Format(_T("IP��ַ : %s/%d"),strAddress,pAdapterUnicastAddress->OnLinkPrefixLength);

		IpGatewayDns.AddTail(IpInfo);

		pAdapterUnicastAddress = pAdapterUnicastAddress->Next;
	}

	PIP_ADAPTER_GATEWAY_ADDRESS_LH pAdapterGatewayAddress = pAdapterInfo->FirstGatewayAddress;
	while(pAdapterGatewayAddress)
	{
		SOCKADDR_IN* pInAddr =  (SOCKADDR_IN*)pAdapterGatewayAddress->Address.lpSockaddr;
		CString strAddress(inet_ntoa(pInAddr->sin_addr));
		IpInfo.strIPAddress.Format(_T("���� : %s"),strAddress);

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
		return _T("δ֪");
	}
}

CString CAdapter::GetMediaStatus(DWORD dwMediaStatus)
{
	switch(dwMediaStatus)
	{
	case NCS_DISCONNECTED:
		return _T("�ѽ���");
		break;
	case NCS_CONNECTING	:
		return _T("������...");
		break;
	case NCS_CONNECTED:
		return _T("������");
		break;
	case NCS_DISCONNECTING:
		return _T("�Ͽ�������");
		break;
	case NCS_HARDWARE_NOT_PRESENT:
		return _T("Ӳ��������");
		break;
	case NCS_HARDWARE_DISABLED:
		return _T("�ѽ���");
		break;
		//NCS_HARDWARE_MALFUNCTION	= ( NCS_HARDWARE_DISABLED + 1 ) ,
	case NCS_MEDIA_DISCONNECTED:
		return _T("δ����");
		break;
		//NCS_AUTHENTICATING	= ( NCS_MEDIA_DISCONNECTED + 1 ) ,
		//NCS_AUTHENTICATION_SUCCEEDED	= ( NCS_AUTHENTICATING + 1 ) ,
		//NCS_AUTHENTICATION_FAILED	= ( NCS_AUTHENTICATION_SUCCEEDED + 1 ) ,
		//NCS_INVALID_ADDRESS	= ( NCS_AUTHENTICATION_FAILED + 1 ) ,
		//NCS_CREDENTIALS_REQUIRED	= ( NCS_INVALID_ADDRESS + 1 ) 
	default:
		return _T("δ֪");
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

		if (DeviceIoControl(hAdapter, IOCTL_NDIS_QUERY_GLOBAL_STATS, 
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

void CAdapter::InsertNodeLabel(CTreeNodeUI* pTreeNode, LPCTSTR lpszItemText)
{
	CLabelUI* pLable = new CLabelUI;
	pLable->SetText(lpszItemText);
	pLable->SetFont(0);
	CHorizontalLayoutUI* pHor = pTreeNode->GetTreeNodeHoriznotal();
	pHor->SetFixedWidth(100);
	pHor->Add(pLable);
}