#include "stdafx.h"
#include <atlstr.h>
#include <wlantypes.h>
#include <shlobj.h>

CDuiString GetFormatIP(DWORD dwIP)
{
	CDuiString strTipInfo;
	IN_ADDR Addr;
	Addr.s_addr = dwIP;
	CString strIpAddress = inet_ntoa(Addr);
	strTipInfo.Format(_T("%s"),strIpAddress);

	return strTipInfo;
}

CDuiString GetFormatIPAndPort(DWORD dwIP,DWORD dwPort)
{
	CDuiString strTipInfo;
	IN_ADDR Addr;
	Addr.s_addr = dwIP;
	CString strIpAddress = inet_ntoa(Addr);
	strTipInfo.Format(_T("%s:%d"),strIpAddress,dwPort);

	return strTipInfo;
}

CDuiString GetConnectionState(DWORD dwConnectionState)
{
	CDuiString strConnectionState;
	switch (dwConnectionState)
	{
	case 1:
		strConnectionState = _T("CLOSED");
		break;
	case 2:
		strConnectionState = _T("LISTENING");
		break;
	case 3:
		strConnectionState = _T("SYN_SENT");
		break;
	case 4:
		strConnectionState = _T("SYN_RCVD");
		break;
	case 5:
		strConnectionState = _T("ESTABLISHED");
		break;
	case 6:
		strConnectionState = _T("FIN_WAIT1");
		break;
	case 7:
		strConnectionState = _T("FIN_WAIT2");
		break;
	case 8:
		strConnectionState = _T("CLOSE_WAIT");
		break;
	case 9:
		strConnectionState = _T("CLOSING");
		break;
	case 10:
		strConnectionState = _T("LAST_ACK");
		break;
	case 11:
		strConnectionState = _T("TIME_WAIT");
		break;
	case 12:
		strConnectionState = _T("DELETE_TCB");
		break;
	}
	return strConnectionState;
}

CDuiString GetFormatNum(DWORD dwNum)
{
	CDuiString strTipInfo;
	strTipInfo.Format(_T("%d"),dwNum);
	return strTipInfo;
}

CDuiString GetAddressFamily(DWORD dwFamily)
{
	CDuiString strTipInfo;
	switch(dwFamily)
	{
		case AF_INET:
				strTipInfo = _T("AF_INET");
			break;
		case AF_INET6:
				strTipInfo = _T("AF_INET6");
			break;
		case AF_NETBIOS:
				strTipInfo = _T("AF_NETBIOS");
			break;
		case AF_NETDES:
				strTipInfo = _T("AF_NETDES");
			break;
		default:
			strTipInfo.Format(_T("unknown(%d)"),dwFamily);
	}

	return strTipInfo;
}

CDuiString GetAddressProtocol(DWORD dwProtocol)
{
	CDuiString strTipInfo;
	switch(dwProtocol)
	{
		case IPPROTO_HOPOPTS:
				strTipInfo = _T("HOPOPTS");
			break;
		case IPPROTO_TCP:
				strTipInfo = _T("TCP");
			break;
		case IPPROTO_UDP:
				strTipInfo = _T("UDP");
			break;
		default:
			strTipInfo.Format(_T("%d"),dwProtocol);
	}
	return strTipInfo;
}

CDuiString GetWlanAuthAlgorithm(DWORD dwAuth)
{
	CDuiString strTipInfo;
	switch(dwAuth)
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
			strTipInfo = _T("802.11 Open");
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
			strTipInfo = _T("802.11 Shared Key");
		break;
	case DOT11_AUTH_ALGO_WPA:
			strTipInfo = _T("WPA");
		break;
	case DOT11_AUTH_ALGO_WPA_PSK:
			strTipInfo = _T("WPA PSK");
		break;
	case DOT11_AUTH_ALGO_RSNA:
			strTipInfo = _T("RSNA");
		break;
	case DOT11_AUTH_ALGO_RSNA_PSK:
			strTipInfo = _T("RSNA with PSK");
		break;
	}
	return strTipInfo;
}

CDuiString GetWlanCipherAlgorithm(DWORD dwCipher)
{
	CDuiString strTipInfo;
	switch(dwCipher)
	{
	case DOT11_CIPHER_ALGO_NONE:
			strTipInfo = _T("NONE");
		break;
	case DOT11_CIPHER_ALGO_WEP40:
			strTipInfo = _T("WEP40");
		break;
	case DOT11_CIPHER_ALGO_TKIP:
			strTipInfo = _T("TKIP");
		break;
	case DOT11_CIPHER_ALGO_CCMP:
			strTipInfo = _T("AES-CCMP");
		break;
	case DOT11_CIPHER_ALGO_WEP104:
			strTipInfo = _T("WEP104");
		break;
	case DOT11_CIPHER_ALGO_WEP:
			strTipInfo = _T("WEP");
		break;
	}
	return strTipInfo;
}

BOOL CopyDataToClipboard(DWORD dwDataType,HWND hWndNewOwner,LPCTSTR lpszText)
{
	BOOL bCloseClipboard = FALSE;
	HGLOBAL hMem = NULL;
	BOOL bSuccess = FALSE;

	do
	{
		if (::OpenClipboard(hWndNewOwner)==0)
		{
			//SetErrorInfo(SYSTEM_ERROR,0,_T("�򿪼�����ʧ��"));
			break;
		}

		bCloseClipboard = TRUE;

		if (EmptyClipboard() == 0)
		{
			//SetErrorInfo(SYSTEM_ERROR,0,_T("��ռ���������ʧ��"));
			break;
		}

		if (dwDataType == CF_UNICODETEXT)
		{
			hMem=GlobalAlloc(GMEM_MOVEABLE,(lstrlen(lpszText)+1)*sizeof(TCHAR));
			if (hMem == NULL)
			{
				//SetErrorInfo(SYSTEM_ERROR,0,_T("���ü������ı�����ʱ�����ڴ�ʧ��"));
				break;
			}

			lstrcpy((TCHAR*)GlobalLock(hMem),lpszText);
			GlobalUnlock(hMem);
		}
		else if (dwDataType == CF_HDROP)
		{
			DROPFILES DropFile;
			DropFile.pFiles = sizeof(DROPFILES);
			DropFile.pt.x = 0;DropFile.pt.y = 0;
			DropFile.fNC = FALSE;
#ifndef UNICODE
			DropFile.fWide = FALSE;
#else
			DropFile.fWide = TRUE;
#endif
			DWORD dwSize = sizeof(DROPFILES) + _tcslen(lpszText)*sizeof(TCHAR) + 2;
			hMem=GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,dwSize);
			LPBYTE lpData = (LPBYTE)GlobalLock(hMem);
			memcpy(lpData,&DropFile,sizeof(DROPFILES));	
			memcpy(lpData+sizeof(DROPFILES),lpszText,_tcslen(lpszText)*sizeof(TCHAR));
			GlobalUnlock(hMem);	//��ͨ�ļ��������
		}

		bSuccess = SetClipboardData(dwDataType,hMem) != NULL;
		if (bSuccess)
			hMem = NULL;//�ɹ�����Ҫ�ͷ��ڴ�
		//else
		//	SetErrorInfo(SYSTEM_ERROR,0,_T("�����ı����ݵ�������ʧ��"));
	}while(0);

	if (bCloseClipboard)
		CloseClipboard();

	if (hMem)
		GlobalFree(hMem);

	return bSuccess;
}