#include "stdafx.h"
#include "ArpList.h"
#include "../Utils/AdapterInfo.h"

CArpList::CArpList(HWND hWnd)
{
	Create(hWnd,_T("²é¿´ARP"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CArpList::~CArpList()
{

}

void CArpList::InitWindow()
{
	CenterWindow();

	ShowArpList();
}

void CArpList::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CArpList::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

CDuiString CArpList::GetSkinFile()
{
	return _T("ArpList.xml");
}

CDuiString CArpList::GetSkinFolder()
{
	return _T("PCHunter");
}

LPCTSTR CArpList::GetWindowClassName(void) const
{
	return _T("ArpList");
}

LRESULT CArpList::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CArpList::OnClick(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	if (strSender == _T("BtnClose"))
		Close(IDOK);
}

void CArpList::ShowArpList()
{
	CMibIpNetRowList MibIpNetRowList;
	if (GetIpNetTable(MibIpNetRowList) == FALSE)
		return;

	CListUI* pArpList = (CListUI*)m_PaintManager.FindControl(_T("ArpInfo"));
	if (pArpList == NULL)
		return;

	POSITION pos = MibIpNetRowList.GetHeadPosition();
	while(pos)
	{
		const MIB_IPNETROW& MibIpInfo = MibIpNetRowList.GetNext(pos);
		if (MibIpInfo.dwType != 3 && MibIpInfo.dwType != 4)
			continue;

		CListTextElementUI* pListElement = new CListTextElementUI;
		pArpList->Add(pListElement);
		pListElement->SetFixedHeight(23);
		CDuiString strHostIPAddr;
		if (LsIpv4AddressToString(MibIpInfo.dwAddr,strHostIPAddr) == FALSE)
			continue;

		pListElement->SetText(0,strHostIPAddr);
		CDuiString strHostMacAddr;
		for (int n=0;n<6;++n)
			strHostMacAddr.AppendFormat(_T("%02X-"),MibIpInfo.bPhysAddr[n]);
		strHostMacAddr.TrimRight(_T("-"));

		pListElement->SetText(1,strHostMacAddr);
		CDuiString strArpType(_T("¶¯Ì¬"));
		if (MibIpInfo.dwType == MIB_IPNET_TYPE_STATIC)
			strArpType = _T("¾²Ì¬");
		pListElement->SetText(2,strArpType);
	}
}