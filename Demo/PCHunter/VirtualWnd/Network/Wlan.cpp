#include "stdafx.h"
#include "Utils/TextTools.h"
#include <Wlanapi.h>
#pragma comment(lib,"Wlanapi.lib")

CWlan::CWlan()
{
	m_pPaintManager = NULL;
}

CWlan::~CWlan()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CWlan, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CWlan::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CWlan::OnLoadItem(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Wlan"));
	if (pList->GetCount())
		pList->RemoveAll();

	HANDLE hClient = NULL;
	PWLAN_INTERFACE_INFO_LIST pWlanInterfaceInfo = NULL;
	do 
	{
		DWORD dwVersion = 0;
		DWORD dwRet = WlanOpenHandle(2,NULL,&dwVersion,&hClient);
		if (dwRet != ERROR_SUCCESS)
			break;

		dwRet = WlanEnumInterfaces(hClient,NULL,&pWlanInterfaceInfo);
		if (dwRet != ERROR_SUCCESS)
			break;

		for (UINT n=0;n<pWlanInterfaceInfo->dwNumberOfItems;++n)
		{
			PWLAN_AVAILABLE_NETWORK_LIST pWlanAvailableList = NULL;
			dwRet = WlanGetAvailableNetworkList(hClient,&pWlanInterfaceInfo->InterfaceInfo[n].InterfaceGuid,
							WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES,NULL,
							&pWlanAvailableList);
			if (dwRet != ERROR_SUCCESS)
				continue;

			for (UINT nIndex=0;nIndex<pWlanAvailableList->dwNumberOfItems;++nIndex)
			{
				CListTextElementUI* pTextElement = new CListTextElementUI;
				pList->Add(pTextElement);

				WLAN_AVAILABLE_NETWORK WlanAvailable = pWlanAvailableList->Network[nIndex];
				int m=0;
				pTextElement->SetText(m++,CString(WlanAvailable.dot11Ssid.ucSSID));
				pTextElement->SetText(m++,WlanAvailable.bSecurityEnabled ? _T("是"):_T("否"));
				//pTextElement->SetText(m++,WlanAvailable.dot11DefaultAuthAlgorithm)
				pTextElement->SetText(m++,GetWlanAuthAlgorithm(WlanAvailable.dot11DefaultAuthAlgorithm));
				pTextElement->SetText(m++,GetWlanCipherAlgorithm(WlanAvailable.dot11DefaultCipherAlgorithm));
				pTextElement->SetText(m++,GetFormatNum(WlanAvailable.wlanSignalQuality));
			}
			//显示搜索出来的无线网络
			WlanFreeMemory(pWlanAvailableList);
		}

	} while (FALSE);

	if (pWlanInterfaceInfo)
	{
		WlanFreeMemory(pWlanInterfaceInfo);
		pWlanInterfaceInfo = NULL;
	}
	if (hClient)
		WlanCloseHandle(hClient,NULL);
}