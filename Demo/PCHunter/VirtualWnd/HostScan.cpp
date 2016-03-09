#include "stdafx.h"
#include "../Utils/AdapterInfo.h"
#include "../Wnd/StaticArpDialog.h"
#include <Icmpapi.h>
#include <nmmintrin.h>
#pragma comment(lib,"Iphlpapi.lib")

#define	 TIMER_ID_PROCESS				1000								//��ʱ��ID
#define	 BLOCK_NET_TIMER_ID			1009								//������綨ʱ��
#define	 WM_GET_TASK_RESULT		(WM_USER + 0x1000)		//��ȡTask�����Ϣ
#define    WM_SCANHOST_FINSH		    (WM_USER + 0x1001)		//ɨ�����
#define     PHAYADDR_LENGTH				6										//������ַ����
#define	 HOST_USER_NAME				_T("UserName")				//�û���

CHostScan::CHostScan()
	: m_pThreadTaskMgr(NULL)
	,m_uIpCount(0)
{
	GetModuleFileName(NULL,m_szMacAddressFilePath,_countof(m_szMacAddressFilePath));
	PathRemoveFileSpec(m_szMacAddressFilePath);
	_tcscpy_s(m_szUserNameFilePath,_countof(m_szUserNameFilePath),m_szMacAddressFilePath);
	PathAppend(m_szMacAddressFilePath,_T("MacAddress.ini"));
	PathAppend(m_szUserNameFilePath,_T("HostName.ini"));
}

CHostScan::~CHostScan()
{
	if (m_pThreadTaskMgr != NULL)
	{
		m_pThreadTaskMgr->StopAllTask();
		delete m_pThreadTaskMgr;
		m_pThreadTaskMgr = NULL;
	}
}

void CHostScan::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_MENU)
		OnMenu(msg);
}

void CHostScan::OnPaint()
{

}

void CHostScan::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_pPaintManager->FindControl(_T("BtnBeginScan")))
		StartScan();
	else if (msg.pSender == m_pPaintManager->FindControl(_T("DeleteAllArp")))
		DeleteAllArp();
	else if (msg.pSender == m_pPaintManager->FindControl(_T("DeleteDynamicArp")))
		DeleteDynamicArp();
	else if (msg.pSender == m_pPaintManager->FindControl(_T("AddStaticArp")))
		AddStaticArp();
	else if (msg.pSender == m_pPaintManager->FindControl(_T("BtnIpRange")))
		OnIpRange(msg);
}

void CHostScan::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Scan"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(_T("HostMenu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

void CHostScan::StartScan()
{
	CListUI* pScanResultList = (CListUI*)m_pPaintManager->FindControl(_T("Scan"));
	if (pScanResultList)
		pScanResultList->RemoveAll();

	if (m_pThreadTaskMgr == NULL)
	{
		m_pThreadTaskMgr = new CThreadTaskMgr(50,(ThreadPoolTaskFun)TaskThread);
 		if (m_pThreadTaskMgr != NULL)
 			m_pThreadTaskMgr->RegisterNotify((LPTASKJOBRESULT_NOTIFY)OnTaskResult,(LPTASKJOBFINSH_NOTIFY)OnTaskFinsh,this);
	}

	CDuiString strTipInfo;
	do 
	{
		if (m_pThreadTaskMgr == NULL)
		{
			strTipInfo = _T("���ڳ�ʼ��ʧ��,�볢���������г���");
			break;
		}

		CControlUI* pStart = m_pPaintManager->FindControl(_T("EditStartIP"));
		CControlUI* pEnd = m_pPaintManager->FindControl(_T("EditEndIP"));
		if (pStart->GetText().IsEmpty() || pEnd->GetText().IsEmpty())
		{
			strTipInfo = _T("������ɨ�跶Χ");
			break;
		}

		COptionUI* pControlARP = (COptionUI*)m_pPaintManager->FindControl(_T("BtnOpARP"));
		COptionUI* pControlPING = (COptionUI*)m_pPaintManager->FindControl(_T("BtnOpPING"));
		COptionUI* pControlHostName = (COptionUI*)m_pPaintManager->FindControl(_T("BtnOpHostName"));

		TaskQueue TaskInfo;
		TaskInfo.HostScanType = HOST_SCAN_TYPE_NONE;
		if (pControlARP->IsSelected())
			TaskInfo.HostScanType |= HOST_SCAN_TYPE_ARP;
		if (pControlPING->IsSelected() && TaskInfo.HostScanType == HOST_SCAN_TYPE_NONE)
			TaskInfo.HostScanType |= HOST_SCAN_TYPE_PING;
		if (pControlHostName->IsSelected() && TaskInfo.HostScanType == HOST_SCAN_TYPE_NONE)
			TaskInfo.HostScanType |= HOST_SCAN_TYPE_HOSTNAME;

		if (TaskInfo.HostScanType == HOST_SCAN_TYPE_NONE)
		{
			strTipInfo = _T("������ѡ��һ�����ɨ��");
			break;
		}

		SOCKADDR_IN StartSockAddr, EndSockAddr;
		LsIpv4StringToAddress(pStart->GetText(), &StartSockAddr);
		LsIpv4StringToAddress(pEnd->GetText(), &EndSockAddr);

		DWORD dwStartIP = ntohl(StartSockAddr.sin_addr.s_addr);
		DWORD dwEndIp = ntohl(EndSockAddr.sin_addr.s_addr);

		m_uIpCount = dwEndIp - dwStartIP+1;
		TaskInfo.nItemIndex = -1;
		for (UINT uIp =0;uIp<m_uIpCount;++uIp)
		{
			TaskInfo.dwIpAddress = htonl(uIp+dwStartIP);
			m_pThreadTaskMgr->AddJob(TaskInfo);
		}
	} while (FALSE);	

	if (strTipInfo.IsEmpty() == FALSE)
		MessageBox(m_pPaintManager->GetPaintWindow(),strTipInfo,_T("��ʾ"),MB_OK);
}

void CHostScan::DeleteAllArp()
{
	CMibIpNetRowList MibIpNetRowList;
	if (GetIpNetTable(MibIpNetRowList) == FALSE)
		return;

	POSITION pos = MibIpNetRowList.GetHeadPosition();
	while(pos)
	{
		MIB_IPNETROW& MibIpNetRow = MibIpNetRowList.GetNext(pos);
		DeleteIpNetEntry(&MibIpNetRow);
	}

	MessageBox(m_pPaintManager->GetPaintWindow(),_T("���ARP����ɹ�"),_T("�������������ʾ"),MB_OK|MB_ICONINFORMATION);
}

void CHostScan::DeleteDynamicArp()
{
	CMibIpNetRowList MibIpNetRowList;
	if (GetIpNetTable(MibIpNetRowList) == FALSE)
		return;

	POSITION pos = MibIpNetRowList.GetHeadPosition();
	while(pos)
	{
		MIB_IPNETROW& MibIpNetRow = MibIpNetRowList.GetNext(pos);
		if (MibIpNetRow.dwType == ARP_TYPE_Dynamic)
			DeleteIpNetEntry(&MibIpNetRow);
	}
	MessageBox(m_pPaintManager->GetPaintWindow(),_T("�����̬ARP����ɹ�"),_T("�������������ʾ"),MB_OK|MB_ICONINFORMATION);
}

void CHostScan::OnIpRange(TNotifyUI& msg)
{
	CMenuWnd* pMenu = new CMenuWnd();
	const RECT& rcPos = msg.pSender->GetPos();
	CPoint pt(rcPos.left, rcPos.bottom);
	ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
	STRINGorID strXmlFile(_T("Range.xml"));
	pMenu->Init(NULL,strXmlFile, pt, m_pPaintManager);

	MIB_IPADDRTABLE* pIPAddrTable = NULL;
	if (GetIPAddrTable(pIPAddrTable) == FALSE)
		return;

	//��ȡ�˵����ڵĸ����
	CMenuUI* pMenuRoot = pMenu->GetMenuUI();
	if (pMenuRoot == NULL)
		return;

	for (DWORD dwItem=0; dwItem<pIPAddrTable->dwNumEntries; ++dwItem)
	{
		MIB_IPADDRROW MibIpAddrRow =	pIPAddrTable->table[dwItem];
		if (MibIpAddrRow.dwAddr == 0x100007F)	//�ų� 127.0.0.1 ����ػ���ַ
			continue;

		CMenuElementUI* pMenuElement = new CMenuElementUI;
		if (pMenuElement == NULL)
			continue;

		//���������ַ
		DWORD dwNetIP = MibIpAddrRow.dwAddr & MibIpAddrRow.dwMask;
		DWORD dwMask = -1 ^ MibIpAddrRow.dwMask;
		//����㲥��ַ
		DWORD dwBroadcastIP = dwMask | dwNetIP;
		CDuiString strNetIP, strBroadcastIP, strIPRange, strMaskRange;
		
		//����MSDN��װ��SSE4 ָ��������������1�ĸ���
		DWORD dwCount = _mm_popcnt_u32(MibIpAddrRow.dwMask);

		LsIpv4AddressToString(dwNetIP, strNetIP);
		LsIpv4AddressToString(dwBroadcastIP, strBroadcastIP);
		strIPRange.Format(_T("%s/%d"), (LPCTSTR)strNetIP, dwCount);
		strMaskRange.Format(_T("%s|%s"), (LPCTSTR)strNetIP, (LPCTSTR)strBroadcastIP);

		pMenuRoot->Add(pMenuElement);
		pMenuElement->SetText(strIPRange);
		pMenuElement->SetName(strMaskRange);
	}

	if (pIPAddrTable != NULL)
	{
		delete[] pIPAddrTable;
		pIPAddrTable = NULL;
	}

	//��̬����ɾ���˵������ֶ����ò���
	pMenu->ResizeMenu();
}

void CHostScan::AddStaticArp()
{
	CStaticArpDialog* pArpDialog = new CStaticArpDialog(m_pPaintManager->GetPaintWindow());
	pArpDialog->ShowWindow();
}

CThreadTaskMgr* CHostScan::GetThreadMgr()
{
	return m_pThreadTaskMgr;
}

CPaintManagerUI* CHostScan::GetPaintManager()
{
	return m_pPaintManager;
}

UINT CHostScan::TaskThread(LPVOID lParam)
{
	CThreadTaskMgr* pThreadMgr = (CThreadTaskMgr*)lParam;
	while(TRUE)
	{
		if (pThreadMgr->IsStoped())
			break;
		//���ж��Ƿ����������û������ŵȴ�
		if (pThreadMgr->GetJobCount() <=0 )
			WaitForSingleObject(pThreadMgr->GetJobEvent(),INFINITE);

		TaskQueue TaskJob;
		if (pThreadMgr->GetJobInfo(TaskJob) == FALSE)
			continue;

		ResultQueue ResultInfo;
		ResultInfo.dwIpAddress = TaskJob.dwIpAddress;
		ResultInfo.HostScanType = TaskJob.HostScanType;
		ResultInfo.szHostName[0] = _T('\0');
		ResultInfo.nItemIndex = TaskJob.nItemIndex;
		ResultInfo.bPingOk = FALSE;
		ULONG uMacLen = PHAYADDR_LENGTH;
		if ((TaskJob.HostScanType & HOST_SCAN_TYPE_ARP) == HOST_SCAN_TYPE_ARP)
		{
			DWORD dwRet = SendARP(TaskJob.dwIpAddress,0,&ResultInfo.MacAddr,&uMacLen);
			//InterlockedIncrement(&m_sdwTaskDown);
			if (dwRet != NO_ERROR)
				continue;
		}
		if ((ResultInfo.HostScanType & HOST_SCAN_TYPE_PING) == HOST_SCAN_TYPE_PING )
		{
			//����ping
			ResultInfo.bPingOk = CHostScan::PingCmd(TaskJob.dwIpAddress);
		}

		//��ȡIP��Ӧ����������
		HOSTENT* pRemoteHost;
		if ((ResultInfo.HostScanType & HOST_SCAN_TYPE_HOSTNAME) == HOST_SCAN_TYPE_HOSTNAME)
		{
			pRemoteHost = gethostbyaddr((char*)&ResultInfo.dwIpAddress, 4, AF_INET);
			if (pRemoteHost != NULL)
				_tcscpy_s(ResultInfo.szHostName,_countof(ResultInfo.szHostName),CString(pRemoteHost->h_name));
		}

		pThreadMgr->AddResult(ResultInfo);
	}

	return 0;
}

void CHostScan::OnTaskResult(LPVOID lParam)
{
	CHostScan* pHostScan = (CHostScan*)lParam;

	ResultQueue TaskResultInfo;
	CThreadTaskMgr* pThreadMgr = pHostScan->GetThreadMgr();
	CPaintManagerUI* pPaintManager = pHostScan->GetPaintManager();

	if (pThreadMgr->GetResultInfo(TaskResultInfo) != FALSE)
	{
		CListUI* pScanResultList = (CListUI*)pPaintManager->FindControl(_T("Scan"));
		CListTextElementUI* pListElement = NULL;
		if (TaskResultInfo.nItemIndex == -1)
		{
			IN_ADDR InAddr;
			InAddr.S_un.S_addr = TaskResultInfo.dwIpAddress;

			pListElement = new CListTextElementUI;
			pScanResultList->Add(pListElement);
			pListElement->SetFixedHeight(23);
			//����IP��ַ
			pListElement->SetText(0,CString(inet_ntoa(InAddr)));
		}
		else
			pListElement = (CListTextElementUI*)pScanResultList->GetItemAt(TaskResultInfo.nItemIndex);

		if ((TaskResultInfo.HostScanType & HOST_SCAN_TYPE_ARP) == HOST_SCAN_TYPE_ARP)
		{
			CString strMac;
			BYTE* bPhysAddr = (BYTE*)&TaskResultInfo.MacAddr;
			for (int i=0;i<PHAYADDR_LENGTH;++i)
				strMac.AppendFormat(_T("%02X-"),bPhysAddr[i]);
			strMac.TrimRight(_T("-"));
			//����MAC��ַ
			pListElement->SetText(1,strMac);

			//������������
			TCHAR szName[MAX_PATH];
			if (pHostScan->GetAdapterName(strMac,szName,_countof(szName)) != FALSE)
				pListElement->SetText(2,szName);
			//���������û���
			if (pHostScan->GetHostUserName(strMac,szName,_countof(szName)) != FALSE)
				pListElement->SetText(5,szName);

			TaskQueue TaskInfo;
			TaskInfo.dwIpAddress = TaskResultInfo.dwIpAddress;
			TaskInfo.nItemIndex = pListElement->GetIndex();

			COptionUI* pHostName = (COptionUI*)pPaintManager->FindControl(_T("BtnOpHostName"));
			if (pHostName->IsSelected())
			{
				TaskInfo.HostScanType = HOST_SCAN_TYPE_HOSTNAME;
				pThreadMgr->AddJob(TaskInfo);
			}

			COptionUI* pPing = (COptionUI*)pPaintManager->FindControl(_T("BtnOpPING"));
			if (pPing->IsSelected())
			{
				TaskInfo.HostScanType = HOST_SCAN_TYPE_PING;
				pThreadMgr->AddJob(TaskInfo);
			}
		}
		//����Pingͨ�Ų���
		if ((TaskResultInfo.HostScanType & HOST_SCAN_TYPE_PING) == HOST_SCAN_TYPE_PING)
			pListElement->SetText(3,TaskResultInfo.bPingOk == TRUE ? _T("Pass"):_T(""));
		//����������
		if ((TaskResultInfo.HostScanType & HOST_SCAN_TYPE_HOSTNAME) == HOST_SCAN_TYPE_HOSTNAME)
			pListElement->SetText(4,TaskResultInfo.szHostName);
	}
}

BOOL CHostScan::PingCmd(DWORD dwIpAddress)
{
	BOOL bSuccess = FALSE;
	HANDLE hIcmpFile = INVALID_HANDLE_VALUE; 
	
	DWORD dwRetVal = 0;
	char SendData[] = "Data Buffer";
	LPBYTE pReplyBuffer = NULL;
	do 
	{
		DWORD dwReplySize = sizeof(ICMP_ECHO_REPLY)+sizeof(SendData);
		pReplyBuffer = new BYTE[dwReplySize];
		hIcmpFile=IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE)
			break;

		dwRetVal = IcmpSendEcho(hIcmpFile, dwIpAddress, SendData, sizeof(SendData),NULL, pReplyBuffer, dwReplySize, 1000);
		if (dwRetVal ==0 )
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (pReplyBuffer != NULL)
	{
		delete pReplyBuffer;
		pReplyBuffer = NULL;
	}

	if (hIcmpFile != INVALID_HANDLE_VALUE)
		IcmpCloseHandle(hIcmpFile);
	return bSuccess;
}

void CHostScan::OnTaskFinsh(LPVOID lParam)
{
	CHostScan* pHostScan = (CHostScan*)lParam;
}

BOOL CHostScan::GetAdapterName(LPCTSTR lpszMacAddress,LPTSTR szAdapterName,DWORD dwNameLen)
{
	CString strMacAddress(lpszMacAddress);
	strMacAddress.Replace(_T("-"),_T(""));
	strMacAddress.Replace(_T(":"),_T(""));

	return GetPrivateProfileString(strMacAddress.Left(6),_T("Name"),NULL,szAdapterName,dwNameLen,m_szMacAddressFilePath);
}

BOOL CHostScan::GetHostUserName(LPCTSTR lpszMacAddress,LPTSTR szAdapterName,DWORD dwNameLen)
{
	return GetPrivateProfileString(lpszMacAddress,HOST_USER_NAME,NULL,szAdapterName,dwNameLen,m_szUserNameFilePath);
}