#include "stdafx.h"
#include "Route.h"
#include "Utils/AdapterInfo.h"
#include "Utils/TextTools.h"
#include <atlstr.h>

CRoute::CRoute()
{
	m_pPaintManager = NULL;
}

CRoute::~CRoute()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CRoute, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnPaint)
DUI_END_MESSAGE_MAP()

void CRoute::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CRoute::OnPaint(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Route"));
	if (pList->GetCount())
		pList->RemoveAll();

	CMibIpAddressList MibIpAddress;
	GetIpAddress(MibIpAddress);

	CString strTipInfo;

	CMibIpforwardIpv4List MibIpRouteList;
	GetIpForwardTableIpv4(MibIpRouteList);
	POSITION pos = MibIpRouteList.GetHeadPosition();
	while(pos)
	{
		const MIB_IPFORWARDROW& MibIpRow = MibIpRouteList.GetNext(pos);
		CListTextElementUI* pTextElement = new CListTextElementUI;
		pList->Add(pTextElement);

		int m = 0;
		pTextElement->SetText(m++,GetFormatIP(MibIpRow.dwForwardDest));
		pTextElement->SetText(m++,GetFormatIP(MibIpRow.dwForwardMask));
		pTextElement->SetText(m++,GetFormatIP(MibIpRow.dwForwardNextHop));

		DWORD dwInterface = GetInterfaceIp(MibIpAddress,MibIpRow);
		pTextElement->SetText(m++,GetFormatIP(dwInterface));

		MIB_IFROW InterFaceRow;
		InterFaceRow.dwIndex = MibIpRow.dwForwardIfIndex;
		GetIfEntry(&InterFaceRow);
		strTipInfo = InterFaceRow.bDescr;
		pTextElement->SetText(m++,strTipInfo);
		pTextElement->SetText(m++,GetFormatNum(MibIpRow.dwForwardMetric1));
	}
}

void CRoute::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Route"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(_T("RouteMenu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

void CRoute::OnRouteInfo(CControlUI* pControl)
{
	CListUI* pAppList = (CListUI*)m_pPaintManager->FindControl(_T("Route"));
	if (pAppList->GetCurSel() == -1)
		return;

	CDuiString strItemName = pControl->GetName();
	if (strItemName == _T("NewRoute")) {

	} else if (strItemName == _T("DeleteRoute")) {

	} else if (strItemName == _T("SaveList")) {

	}
}

DWORD CRoute::GetInterfaceIp(const CMibIpAddressList& MibIpAddress,const MIB_IPFORWARDROW& MibIpRow)
{
	POSITION pos = MibIpAddress.GetHeadPosition();
	while(pos)
	{
		const MIB_IPADDRROW& MibIpAddressRow = MibIpAddress.GetNext(pos);
		if (MibIpAddressRow.dwIndex != MibIpRow.dwForwardIfIndex)
			continue;

		if (MibIpRow.dwForwardMask == 0)
		{
			if ((MibIpAddressRow.dwAddr & MibIpAddressRow.dwMask) == 
				(MibIpRow.dwForwardNextHop & MibIpAddressRow.dwMask))
					return MibIpAddressRow.dwAddr;
				continue;
		}

		if ((MibIpAddressRow.dwAddr & MibIpRow.dwForwardMask) == 
				(MibIpRow.dwForwardNextHop & MibIpRow.dwForwardMask))
			return MibIpAddressRow.dwAddr;
	}
	return 0;
}

BOOL CRoute::GetIpAddress(CMibIpAddressList& MibIpAddress)
{
	BOOL bSuccess = FALSE;
	PMIB_IPADDRTABLE pMibIpAddressTable = NULL;
	do 
	{
		DWORD dwSize = 0;
		DWORD dwRet = GetIpAddrTable(pMibIpAddressTable,&dwSize,FALSE);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibIpAddressTable = (PMIB_IPADDRTABLE)new BYTE[dwSize];
			if (pMibIpAddressTable == NULL)
				break;
			dwRet = GetIpAddrTable(pMibIpAddressTable,&dwSize,FALSE);
		}

		if (dwRet != NO_ERROR)
			break;

		for (UINT n=0;n<pMibIpAddressTable->dwNumEntries;++n)
			MibIpAddress.AddTail(pMibIpAddressTable->table[n]);

		bSuccess = TRUE;
	} while (FALSE);
	
	if (pMibIpAddressTable != NULL)
	{
		delete[] pMibIpAddressTable;
		pMibIpAddressTable = NULL;
	}

	return bSuccess;
}

void CRoute::NotifyTask(PCNTCHDR pNTCHDR)
{

}