#include "stdafx.h"
#include "IPTools.h"
#include "../Utils/AdapterInfo.h"
#include <math.h>
#include <nmmintrin.h>

CIPTools::CIPTools(HWND hWnd)
{
	Create(hWnd,_T("IP计算器"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CIPTools::~CIPTools()
{

}

void CIPTools::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CIPTools::InitWindow()
{
	CenterWindow();
}

void CIPTools::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
		OnTextChanged(msg);
}

CDuiString CIPTools::GetSkinFile()
{
	return _T("IPTools.xml");
}

CDuiString CIPTools::GetSkinFolder()
{
	return _T("PCHunter");
}

LPCTSTR CIPTools::GetWindowClassName(void) const
{
	return _T("IPCalc");
}

void CIPTools::OnClick(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	if (strSender == _T("BtnClose"))
		Close(IDOK);
}

void CIPTools::OnTextChanged(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetParent()->GetName();

	if (strSender == _T("IPConvernt"))
		OnIPConvernt(msg);
	else if (strSender == _T("MaskCalc"))
		OnMaskCalc(msg);
	else if (strSender == _T("NetAddrLayout"))
		OnNetAddr(msg);
}

LRESULT CIPTools::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CIPTools::OnIPConvernt(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	CDuiString strText = msg.pSender->GetText();

	DWORD dwIP = 0;
	do 
	{
		if (strText.IsEmpty() == TRUE)
			break;

		if (strSender == _T("IpAddr"))
		{
			SOCKADDR_IN SockAddr;
			if (LsIpv4StringToAddress(strText, &SockAddr) == TRUE)
				dwIP = SockAddr.sin_addr.s_addr;
		}
		else if (strSender == _T("HexHostOrder"))
		{
			TCHAR szData[50];
			int nHexHost = 0, nDec;
			_stscanf_s(strText, _T("%x"), &nHexHost);
			_itot_s(nHexHost, szData, _countof(szData), 10);
			_stscanf_s(szData, _T("%u"), &nDec);
			dwIP = nDec;
		}
		else if (strSender == _T("HexNetOrder"))
		{
			TCHAR szData[50];
			int nHexHost = 0, nDec;
			_stscanf(strText, _T("%x"), &nHexHost);
			_itot_s(nHexHost, szData, _countof(szData), 10);
			_stscanf_s(szData, _T("%u"), &nDec);
			dwIP = ntohl(nDec);
		}
		else if (strSender == _T("TenHostOrder"))
		{
			int nHost = 0;
			_stscanf(strText, _T("%u"), &nHost);
			dwIP = nHost;
		}
		else if (strSender == _T("TenNetOrder"))
		{
			int nNet = 0;
			_stscanf(strText, _T("%u"), &nNet);
			dwIP = ntohl(nNet);
		}
	} while (FALSE);

	if (dwIP == 0)
		ClearIPInfo(msg.pSender);
	else
		SetIPInfo(msg.pSender, dwIP);
}

void CIPTools::OnMaskCalc(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	CDuiString strText = msg.pSender->GetText();

	int nCount = 0;
	do 
	{
		//如果内容为空，则清除所有项目
		if (strText.IsEmpty() == TRUE )
			break;

		if (strSender == _T("MaskCount"))
			nCount = _ttoi(strText);
		else if (strSender == _T("SubMask"))
		{
			SOCKADDR_IN SockAddr;
			if (LsIpv4StringToAddress(strText, &SockAddr) == FALSE)
				break;
			nCount = _mm_popcnt_u32(SockAddr.sin_addr.s_addr);
		}
		else if (strSender == _T("HexNet"))
		{
			int nHexNet = 0;
			_stscanf(strText, _T("%x"), &nHexNet);
			nCount = _mm_popcnt_u32(nHexNet);
			if (nHexNet != -1 << (32 - nCount))
				nCount = 0;
		}
		else if ( strSender == _T("TotalAddr") || strSender == _T("ValidAddr"))
		{
			DWORD nValidAddr = _ttoi(strText);
			
			int n=0;
			while(TRUE)
			{
				DWORD dwItem = 1 << ++n;
				if (dwItem >= nValidAddr)
					break;
			}
			nCount = 32 - n;		//通过将地址总数-1获取到二进制1的各数
		}

	} while (FALSE);

	//如果掩码位数为0，则清空所有内容
	if (nCount == 0)
		ClearMaskInfo(msg.pSender);
	else
		SetMaskInfo(msg.pSender, nCount);
}

void CIPTools::OnNetAddr(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	if (strSender != _T("IpAddrInfo") && strSender != _T("SubMaskInfo"))
		return;

	DWORD dwNetIP = 0;
	DWORD dwMask = 0;

	BOOL bSuccess = FALSE;
	do 
	{
		CIPAddressUI* pIPAddr = (CIPAddressUI*)m_PaintManager.FindControl(_T("IpAddrInfo"));
		if (pIPAddr == NULL)
			break;
		if (pIPAddr->IsBlank())
			break;

		SOCKADDR_IN SockAddr;
		if (LsIpv4StringToAddress(pIPAddr->GetText(), &SockAddr) == TRUE)
			dwNetIP = SockAddr.sin_addr.s_addr;
		else
			break;
		
		CIPAddressUI* pSubMask = (CIPAddressUI*)m_PaintManager.FindControl(_T("SubMaskInfo"));
		if (pSubMask == NULL)
			break;
		if (pSubMask->IsBlank())
			break;

		if (LsIpv4StringToAddress(pSubMask->GetText(), &SockAddr) == TRUE)
			dwMask = SockAddr.sin_addr.s_addr;
		else
			break;

		//由于掩码的特殊性，从左往右依次为1，这里偷懒直接认为当前字节二进制显示 > 1000000
		if ((0xFF & dwMask) < 128 )
			break;

		DWORD dwNetAddr = dwNetIP & dwMask;
		DWORD dwBoradcast = dwNetIP | (-1 ^ dwMask);
		DWORD dwAddrCount = (DWORD)pow((double)2, 32 - _mm_popcnt_u32(dwMask));

		CDuiString strNetAddr, strBoradcast, strAddrCount;
		CIPAddressUI* pNetAddr = (CIPAddressUI*)m_PaintManager.FindControl(_T("NetAddr"));
		CIPAddressUI* pBoradcast = (CIPAddressUI*)m_PaintManager.FindControl(_T("Broadcast"));
		CEditUI2* pAddrCount = (CEditUI2*)m_PaintManager.FindControl(_T("Total"));

		if (LsIpv4AddressToString(dwNetAddr, strNetAddr))
			pNetAddr->SetText(strNetAddr);
		if (LsIpv4AddressToString(dwBoradcast, strBoradcast))
			pBoradcast->SetText(strBoradcast);

		strAddrCount.Format(_T("%u"), dwAddrCount);
		pAddrCount->SetText(strAddrCount);

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		ClearNetInfo();
}

void CIPTools::SetIPInfo(CControlUI* pControl, int dwIP)
{
	TCHAR szAddr[50];
	CIPAddressUI* pIPAddr = (CIPAddressUI*)m_PaintManager.FindControl(_T("IpAddr"));
	if (pIPAddr != pControl)
	{
		CDuiString strIPAddr;
		if (LsIpv4AddressToString(dwIP, strIPAddr))
			pIPAddr->SetText(strIPAddr);
	}

	CEditUI2* pHexHost = (CEditUI2*)m_PaintManager.FindControl(_T("HexHostOrder"));
	if (pHexHost != pControl)
	{
		_stprintf_s(szAddr, _countof(szAddr), _T("0x%08X"), dwIP);
		pHexHost->SetText(szAddr);
	}

	CEditUI2* pHexNet = (CEditUI2*)m_PaintManager.FindControl(_T("HexNetOrder"));
	if (pHexNet != pControl)
	{
		_stprintf_s(szAddr, _countof(szAddr), _T("0x%08X"), htonl(dwIP));
		pHexNet->SetText(szAddr);
	}

	CEditUI2* pTenHost = (CEditUI2*)m_PaintManager.FindControl(_T("TenHostOrder"));
	if (pTenHost != pControl)
	{
		_stprintf_s(szAddr, _countof(szAddr), _T("%u"), dwIP);
		pTenHost->SetText(szAddr);
	}

	CEditUI2* pTenNet = (CEditUI2*)m_PaintManager.FindControl(_T("TenNetOrder"));
	if (pTenNet != pControl)
	{
		_stprintf_s(szAddr, _countof(szAddr), _T("%u"), htonl(dwIP));
		pTenNet->SetText(szAddr);
	}
}

void CIPTools::ClearIPInfo(CControlUI* pControl)
{
	LPCTSTR lpszItemName[] = {_T("IpAddr"),_T("HexHostOrder"),_T("HexNetOrder"),_T("TenHostOrder"),_T("TenNetOrder")};
	for(DWORD dwItem = 0; dwItem < _countof(lpszItemName); ++dwItem)
	{
		CControlUI* pMaskInfo = (CControlUI*)m_PaintManager.FindControl(lpszItemName[dwItem]);
		if (pMaskInfo != pControl)
			pMaskInfo->SetText(_T(""));
	}
}

void CIPTools::ClearNetInfo()
{
	LPCTSTR lpszItemName[] = {_T("NetAddr"),_T("Broadcast"),_T("Total")};
	for(DWORD dwItem = 0; dwItem < _countof(lpszItemName); ++dwItem)
	{
		CControlUI* pMaskInfo = (CControlUI*)m_PaintManager.FindControl(lpszItemName[dwItem]);
		if (pMaskInfo)
			pMaskInfo->SetText(_T(""));
	}
}

void CIPTools::SetMaskInfo(CControlUI* pControl, int nCount)
{
	/*CDuiString strMaskCount = msg.pSender->GetText();
	int nCount = _ttoi(strMaskCount);*/
	//将1左移32-nCount得到掩码(形如:-1(0xFFFFFFFF) << 8 --> 0xFFFFFF00)
	int nHex = -1  << (32 - nCount);

	TCHAR szAddrText[50];
	//设置子网掩码位数
	CEditUI2* pMaskCount = (CEditUI2*)m_PaintManager.FindControl(_T("MaskCount"));
	if (pMaskCount != pControl)
	{
		_stprintf_s(szAddrText, _countof(szAddrText), _T("%u"), nCount);
		pMaskCount->SetText(szAddrText);
	}

	//设置子网掩码
	CDuiString strSubMask;
	CIPAddressUI* pSubMask = (CIPAddressUI*)m_PaintManager.FindControl(_T("SubMask"));
	//将子网掩码网络序转换得到主机序
	if (pSubMask != pControl)
	{
		int nSubMask = ntohl(nHex);		
		if (LsIpv4AddressToString(nSubMask, strSubMask) != FALSE)
			pSubMask->SetText(strSubMask);
	}

	//设置16进制网络序
	CEditUI2* pHexNet = (CEditUI2*)m_PaintManager.FindControl(_T("HexNet"));
	if (pHexNet != pControl)
	{
		_stprintf_s(szAddrText, _countof(szAddrText), _T("0x%X"), nHex);
		pHexNet->SetText(szAddrText);
	}

	//设置IP地址总数
	DWORD dwTotalAddr = (DWORD)pow((double)2, 32 - nCount);
	CEditUI2* pTotalAddr = (CEditUI2*)m_PaintManager.FindControl(_T("TotalAddr"));
	if (pTotalAddr != pControl)
	{
		_stprintf_s(szAddrText, _countof(szAddrText), _T("%u"), dwTotalAddr);
		pTotalAddr->SetText(szAddrText);
	}
	
	//设置有效IP地址总数
	CEditUI2* pValidAddr = (CEditUI2*)m_PaintManager.FindControl(_T("ValidAddr"));
	if (pValidAddr != pControl)
	{
		DWORD dwValidAddr = dwTotalAddr - 2;
		if (dwTotalAddr <= 2)
			dwValidAddr = 0;
		
		_stprintf_s(szAddrText, _countof(szAddrText), _T("%u"), dwValidAddr);
		pValidAddr->SetText(szAddrText);
	}
}

void CIPTools::ClearMaskInfo(CControlUI* pControl)
{
	LPCTSTR lpszItemName[] = {_T("MaskCount"),_T("SubMask"),_T("HexNet"),_T("TotalAddr"),_T("ValidAddr")};
	for(DWORD dwItem = 0; dwItem < _countof(lpszItemName); ++dwItem)
	{
		CControlUI* pMaskInfo = (CControlUI*)m_PaintManager.FindControl(lpszItemName[dwItem]);
		if (pMaskInfo != pControl)
			pMaskInfo->SetText(_T(""));
	}
}