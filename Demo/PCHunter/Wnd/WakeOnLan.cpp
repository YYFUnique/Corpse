#include "StdAfx.h"
#include "WakeOnLan.h"
#include "DllCore/Utils/ErrorInfo.h"
#include <WinSock2.h>
#include <ws2ipdef.h>

CWakeOnLan::CWakeOnLan(HWND hParent)
{
	GetModuleFileName(NULL,m_szHostNameFilePath,_countof(m_szHostNameFilePath));
	PathRemoveFileSpec(m_szHostNameFilePath);
	PathAppend(m_szHostNameFilePath,_T("HostName.ini"));

	Create(hParent,_T("WOL"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CWakeOnLan::~CWakeOnLan()
{

}

void CWakeOnLan::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CWakeOnLan::GetWindowClassName(void) const
{
	return _T("WakeOnLan");
}

CDuiString CWakeOnLan::GetSkinFile()
{
	return _T("WakeOnLan.xml");
}

CDuiString CWakeOnLan::GetSkinFolder()
{
	return _T("PCHunter");
}

void CWakeOnLan::InitWindow()
{
	CenterWindow();

	CComboUI* pHostList = (CComboUI*)m_PaintManager.FindControl(_T("HostList"));

	TCHAR szMacAddr[1024*4];
	TCHAR szHostName[50];
	GetPrivateProfileString(NULL,NULL,NULL,szMacAddr,_countof(szMacAddr),m_szHostNameFilePath);
	LPCTSTR lpKeyName = szMacAddr;

	while(lpKeyName[0] != _T('\0'))
	{
		//获取所有的结点名称
		GetPrivateProfileString(lpKeyName,_T("UserName"),NULL,szHostName,_countof(szHostName),m_szHostNameFilePath);
		
		//过滤掉备注名为空的项
		if (szHostName[0] != _T('\0'))
		{
			CListLabelElementUI* pElement = new CListLabelElementUI;
			pElement->SetText(szHostName);
			pElement->SetUserData(lpKeyName);
			
			pHostList->Add(pElement);
			//选中第一项
			if (pHostList->GetCount() == 1)
				pElement->Select(true);
		}
		
		lpKeyName += _tcslen(lpKeyName) + 1;
	}
}

void CWakeOnLan::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemSelect(msg);
}

void CWakeOnLan::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		Close(IDOK);
	/*else if (msg.pSender == m_PaintManager.FindControl(_T("BtnSend")))
		OnWOLSend(msg);*/
}

LRESULT CWakeOnLan::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close(IDOK);
		return TRUE;
	}

	return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

void CWakeOnLan::OnItemSelect(TNotifyUI& msg)
{
	CComboUI* pHostList = (CComboUI*)m_PaintManager.FindControl(_T("HostList"));
	if (msg.pSender == pHostList)
	{
		CControlUI* pSelItem = pHostList->GetItemAt(msg.wParam);
		CLabelUI* pMacAddr = (CLabelUI*)m_PaintManager.FindControl(_T("Mac"));
		pMacAddr->SetText(pSelItem->GetUserData());
	}	
}

//void CWakeOnLan::OnWOLSend(TNotifyUI& msg)
//{
//	WSADATA wsd;
//	WSAStartup(MAKEWORD(2, 2), &wsd);
//
//	SOCKET ClientSock;
//
//	do 
//	{
//		if ((ClientSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
//			break;
//
//		SOCKADDR_IN ClientAddr;
//		ZeroMemory(&ClientAddr, sizeof(SOCKADDR_IN));
//		ClientAddr.sin_family=AF_INET;
//		ClientAddr.sin_addr.s_addr=htonl(INADDR_ANY);
//		ClientAddr.sin_port=htons(7);
//
//		if (bind(ClientSock, (SOCKADDR*)&ClientAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
//		{
//			SetErrorInfo(WINSOCK_ERROR, WSAGetLastError(), _T("绑定地址失败"));
//			break;
//		}
//
//		BOOL bIncl = TRUE; 
//		//::setsockopt(Sock, IPPROTO_IP, IP_HDRINCL, (char *)&bIncl, sizeof(bIncl)); 
//		::setsockopt(ClientSock, SOL_SOCKET, SO_BROADCAST, (char*)&bIncl, sizeof(bIncl));
//		CComboUI* pHostList = (CComboUI*)m_PaintManager.FindControl(_T("HostList"));
//		CControlUI* pSelItem = pHostList->GetItemAt(pHostList->GetCurSel());
//		
//		BYTE bMac[20];
//		CDuiString strHostAddr = pSelItem->GetUserData();
//		//由于以下按照32位来处理，故接收缓冲区需要大一些，但是不影响结果
//		_stscanf_s(strHostAddr,_T("%x-%x-%x-%x-%x-%x"),
//					&bMac[0],&bMac[1],&bMac[2],&bMac[3],&bMac[4],&bMac[5]);
//
//		BYTE bData[17*6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//		for (int n=1; n<17; ++n)
//			memcpy(&bData[n*6], bMac, 6);
//
//		SOCKADDR_IN AddrIn;
//		AddrIn.sin_family = AF_INET;
//		//原始套接字没有端口概念，任意
//		AddrIn.sin_port = htons(7773);
//		AddrIn.sin_addr.s_addr = htonl(INADDR_BROADCAST);
//		//AddrIn.sin_addr.s_addr = inet_addr("10.1.1.163");
//
//		if (sendto(ClientSock,(char*)bData,_countof(bData),0,(SOCKADDR*)&AddrIn,sizeof(AddrIn)) == SOCKET_ERROR)
//		{
//			SetErrorInfo(WINSOCK_ERROR, WSAGetLastError(), _T("发送数据包失败"));
//			break;
//		}
//
//		
//	} while (FALSE);
//
//	if (ClientSock != 0)
//		closesocket(ClientSock);
//
//	WSACleanup();
//}