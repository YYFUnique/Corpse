#include "StdAfx.h"
#include "IPv4Result.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Utils/TextTools.h"
#include "DllCore/Json/JsonObject.h"

CIPv4Result::CIPv4Result()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
	m_pHostInfo = NULL;
}

CIPv4Result::~CIPv4Result()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
	if (m_pHostInfo != NULL)
	{
		delete m_pHostInfo;
		m_pHostInfo = NULL;
	}
}

DUI_BEGIN_MESSAGE_MAP(CIPv4Result, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CIPv4Result::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CIPv4Result::SetVirtualWnd(LPCTSTR lpszVirtualWnd)
{
	m_strVirtualWnd = lpszVirtualWnd;
}

LPCTSTR CIPv4Result::GetVirtualWnd() const
{
	return m_strVirtualWnd;
}

void CIPv4Result::NotifyTask(PCNTCHDR pNTCHDR)
{
	if (m_pHostInfo == NULL)
	{
		m_pHostInfo = new HOST_INFO;
		if (m_pHostInfo == NULL)
			return;
	}

	CJsonObject JsonObject;
	JsonObject.FromString(pNTCHDR->strData);

	if (JsonObject.IsMember(JSON_HOSTNAME) != FALSE)
		m_pHostInfo->strHostName = JsonObject.GetValue(JSON_HOSTNAME);
	if (JsonObject.IsMember(JSON_COUNT) != FALSE)
		JsonObject.GetValue(JSON_COUNT, (UINT*)&m_pHostInfo->dwCount);
	if (JsonObject.IsMember(JSON_INTERVAL) != FALSE)
		JsonObject.GetValue(JSON_INTERVAL, (UINT*)&m_pHostInfo->dwInterval);
	if (JsonObject.IsMember(JSON_PACKETSIZE) != FALSE)
		JsonObject.GetValue(JSON_PACKETSIZE, (UINT*)&m_pHostInfo->dwPacketSize);
	if (JsonObject.IsMember(JSON_TIMEOUT) != FALSE)
		JsonObject.GetValue(JSON_TIMEOUT, (UINT*)&m_pHostInfo->dwTimeout);
	if (JsonObject.IsMember(JSON_TIMETOLIVE) != FALSE)
		JsonObject.GetValue(JSON_TIMETOLIVE, (UINT*)&m_pHostInfo->dwTTL);
}

void CIPv4Result::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	StartThread();
}

void CIPv4Result::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnStop"))
		OnStop();
}

void CIPv4Result::OnStop()
{
	m_bExitThread = TRUE;
	StopThread(FALSE);
}

BOOL CIPv4Result::IsIPv4String(LPCTSTR lpszIpString)
{
	DWORD dwStringLength = (DWORD)_tcslen(lpszIpString);
	if (dwStringLength < 7 || dwStringLength > 15)
		return FALSE;

	CStdArray strInfoArray;
	AnalyseFormattedTextInfoItem(lpszIpString, strInfoArray , '.');

	if (strInfoArray.GetCount() != 4)
		return FALSE;

	for(int i=0;i<4;i++)
	{
		const CString& strItem = strInfoArray.GetAt(i);

		LPTSTR lpszEndPtr = NULL;
		DWORD dwItem = _tcstoul(strItem , &lpszEndPtr , 10);

		//判断是否有非数字字符
		if (lpszEndPtr)
			if (lpszEndPtr[0] != 0)
				return FALSE;

		if (dwItem > 255)
			return FALSE;
	}

	return TRUE;
}

UINT CIPv4Result::ThreadProc()
{
	CPingHelper PingObject;
	PING_REPLY PingReply; 

	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("IPv4Wizard"));
	int nIndex = pTabLayout->GetCurSel();
	CVerticalLayoutUI* pLayout = (CVerticalLayoutUI*)pTabLayout->GetItemAt(nIndex);
	CListUI* pList = (CListUI*)pLayout->FindSubControlByClassName(_T("FileListUI"));

	CString strTipInfo;

	CString strHostIPAddr;
	CString strHostNameInfo;
	BOOL bIPv4String = IsIPv4String(m_pHostInfo->strHostName);
	if (bIPv4String == FALSE)
	{
		CDNSLookup DnsLookup;
		if (DnsLookup.DNSQuery(m_pHostInfo->strHostName, 5*1000) == FALSE)
		{
			strTipInfo.Format(_T("Ping 请求找不到主机 %s 。请检查该名称，然后重试。"), m_pHostInfo->strHostName);
			CListLabelElementUI* pListElement = new CListLabelElementUI;
			pList->Add(pListElement);
			pListElement->SetText(strTipInfo);
			pListElement->SetFixedHeight(27);
			//LOG_PRINT(_T("Ping 请求找不到主机 %s 。请检查该名称，然后重试。"), m_pHostInfo->strHostName);
			return FALSE;
		}
	
		CStdArray strNameArray;
		CStdArray strIPArray;
		DnsLookup.GetCNameInfo(strNameArray);
		DnsLookup.GetHostIPAddr(strIPArray);
		if (strNameArray.IsEmpty() == FALSE)
			strHostNameInfo = strNameArray.GetAt(0);
		else
			strHostNameInfo = m_pHostInfo->strHostName;

		strHostIPAddr = strIPArray.GetAt(0);
		if (strHostIPAddr.IsEmpty() != FALSE)
			strHostIPAddr = m_pHostInfo->strHostName;
	}
	else
		strHostIPAddr = m_pHostInfo->strHostName;

	if (m_pHostInfo->dwPacketSize != DEF_PACKET_SIZE)
		PingObject.SetPacketSize(m_pHostInfo->dwPacketSize);

	if (strHostNameInfo.IsEmpty())
		strTipInfo.Format(_T("正在 Ping %s 具有 %d 字节的数据:"), strHostIPAddr, m_pHostInfo->dwPacketSize);
	else
		strTipInfo.Format(_T("正在 Ping %s [%s] 具有 %d 字节的数据:"), strHostNameInfo, strHostIPAddr, m_pHostInfo->dwPacketSize);

	//strTipInfo.Format(_T("Ping 请求找不到主机 %s 。请检查该名称，然后重试。"), m_pHostInfo->strHostName);
	CListLabelElementUI* pListElement = new CListLabelElementUI;
	pList->Add(pListElement);
	pListElement->SetText(strTipInfo);
	pListElement->SetFixedHeight(27);

	int nCount = 0;
	while(TRUE)
	{
		if (nCount++ == m_pHostInfo->dwCount && m_pHostInfo->dwCount != 0)
			break;

		DWORD dwInterval = m_pHostInfo->dwInterval;
		DWORD dwRet = WaitForSingleObject(m_NotificationEvent, dwInterval);
		if (dwRet == WAIT_OBJECT_0 || m_bExitThread == TRUE)
			break;

		if (dwRet == WAIT_TIMEOUT)
		{
			DWORD dwRet = PingObject.Ping(strHostIPAddr, &PingReply);
			if (dwRet == ICMP_REQUEST_SUCCESS)
				strTipInfo.Format(_T("来自 %s 的回复: 字节=%ld 时间=%ldms TTL=%ld\n"), strHostIPAddr, PingReply.m_dwBytes, PingReply.m_dwRoundTripTime, PingReply.m_dwTTL);
			else 
				strTipInfo.Format(_T("Ping 超时\n"));

			CListLabelElementUI* pListElement = new CListLabelElementUI;
			pList->Add(pListElement);
			pListElement->SetText(strTipInfo);
			pListElement->SetFixedHeight(27);
		}
	}

	//CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("IPv4Wizard"));
	//int nIndex = pTabLayout->GetCurSel();
	//CVerticalLayoutUI* pLayout = (CVerticalLayoutUI*)pTabLayout->GetItemAt(nIndex);
	//CListUI* pList = (CListUI*)pLayout->FindSubControlByClassName(_T("FileListUI"));

	// 创建管道用于接收编译参数
	//HANDLE hRead = INVALID_HANDLE_VALUE ;
	//HANDLE hWrite = INVALID_HANDLE_VALUE;
	//SECURITY_ATTRIBUTES sa; 
	//sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	//sa.lpSecurityDescriptor= NULL;//使用系统默认的安全描述符
	//sa.bInheritHandle= TRUE;//创建的进程继承句柄

	//do 
	//{
	//	 创建匿名管道
	//	if (CreatePipe(&hRead, &hWrite, &sa, 0) == FALSE)
	//		break;

	//	 创建PING进程
	//	STARTUPINFO si; 
	//	PROCESS_INFORMATION pi;
	//	ZeroMemory(&pi, sizeof(pi));
	//	ZeroMemory(&si,sizeof(STARTUPINFO)); 
	//	si.cb=sizeof(STARTUPINFO); 
	//	GetStartupInfo(&si); 
	//	si.hStdError= hWrite; 
	//	si.hStdOutput= hWrite;//新创建进程的标准输出连在写管道一端
	//	si.wShowWindow= SW_HIDE;//隐藏窗口
	//	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//	创建子进程
	//	DWORD dwRet = CreateProcess(_T("C:\\windows\\system32\\ping.exe"), (LPTSTR)_T(" www.baidu.com -t"), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
	//	if (dwRet == 0)
	//	{
	//		OutputDebugString(_T("CreateProcess失败"));
	//		break;
	//	}
	//	
	//	DWORD dwRead = 0;
	//	CHAR szBuffer[MAX_PATH]={0};
	//	while(TRUE)
	//	{
	//		if (ReadFile(hRead, szBuffer, _countof(szBuffer), &dwRead,NULL) == FALSE)
	//			break;  

	//		CListLabelElementUI* pListElement = new CListLabelElementUI;
	//		pList->Add(pListElement);
	//		pListElement->SetText(CString(szBuffer));
	//		pListElement->SetFixedHeight(27);
	//	}

	//	DWORD dwQuitCode = 0;
	//	if (GetExitCodeThread(pi.hThread, &dwQuitCode))
	//	{
	//		if (dwQuitCode != 0)
	//		{
	//			PostMessage(AfxGetMainWnd()->m_hWnd, UM_PACK_FINISHED, (WPARAM)1, 0);
	//		}
	//	}

	//	CloseHandle(pi.hThread);
	//	CloseHandle(pi.hProcess);

	//} while (FALSE);

	//if (hRead != INVALID_HANDLE_VALUE)
	//	CloseHandle(hRead);
	//if (hWrite != INVALID_HANDLE_VALUE)
	//	CloseHandle(hWrite);

	return TRUE;
}