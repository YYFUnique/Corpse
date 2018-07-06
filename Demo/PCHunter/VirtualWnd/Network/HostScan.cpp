#include "StdAfx.h"
#include "HostScan.h"
#include "Utils/AdapterInfo.h"
#include "Wnd/StaticArpDialog.h"
#include "Wnd/IPTools.h"
#include "Wnd/AddUserName.h"
#include "Wnd/ArpList.h"
#include "Wnd/WakeOnLan.h"
#include "Wnd/BlockNet.h"
#include "Utils/TextTools.h"

#include "DllCore/Utils/ErrorInfo.h"

#include <Icmpapi.h>
#include <nmmintrin.h>
#pragma comment(lib,"Iphlpapi.lib")

#define	 TIMER_ID_PROCESS				1000								//定时器ID
#define	 BLOCK_NET_TIMER_ID			1009								//阻断网络定时器
#define	 WM_GET_TASK_RESULT		(WM_USER + 0x1000)		//获取Task结果消息
#define    WM_SCANHOST_FINSH		    (WM_USER + 0x1001)		//扫描完成
#define     PHAYADDR_LENGTH				6										//物理地址长度
#define	 HOST_USER_NAME				_T("UserName")				//用户名

CHostScan::CHostScan()
	: m_pThreadTaskMgr(NULL)
	,m_uIpCount(0)
{
	m_pPaintManager = NULL;
	GetModuleFileName(NULL,m_szMacAddressFilePath,_countof(m_szMacAddressFilePath));
	PathRemoveFileSpec(m_szMacAddressFilePath);
	_tcscpy_s(m_szUserNameFilePath,_countof(m_szUserNameFilePath),m_szMacAddressFilePath);
	PathAppend(m_szMacAddressFilePath,_T("MacAddress.ini"));
	PathAppend(m_szUserNameFilePath,_T("HostName.ini"));

	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSAData wsaData;
	WSAStartup(wVersionRequested,&wsaData);
}

CHostScan::~CHostScan()
{
	m_pPaintManager = NULL;
	if (m_pThreadTaskMgr != NULL)
	{
		m_pThreadTaskMgr->TerminateAll();
		delete m_pThreadTaskMgr;
		m_pThreadTaskMgr = NULL;
	}

	WSACleanup();
}

DUI_BEGIN_MESSAGE_MAP(CHostScan, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
DUI_END_MESSAGE_MAP()

void CHostScan::SetPaintManager(CPaintManagerUI* pPaintManager)
{	
	m_pPaintManager = pPaintManager;
}

void CHostScan::OnClick(TNotifyUI& msg)
{
	//CControlUI* pSender = m_pPaintManager->FindControl(msg.)
	CDuiString strSender = msg.pSender->GetName();
	if (strSender == _T("BtnBeginScan"))
		StartScan();
	else if (strSender == _T("BtnStopScan"))
		StopScan();
	else if (strSender == _T("DeleteAllArp"))
		DeleteAllArp();
	else if (strSender == _T("DeleteDynamicArp"))
		DeleteDynamicArp();
	else if (strSender == _T("AddStaticArp"))
		AddStaticArp();
	else if (strSender == _T("BtnIpRange"))
		OnIpRange(msg);
	else if (strSender == _T("IPTools"))
		OnIpTools(msg);
	else if (strSender == _T("ShowArp"))
		OnShowArp(msg);
	else if (strSender == _T("WakeOnLAN"))
		OnWOL(msg);
}

void CHostScan::OnHostScanMenu(CControlUI* pControl)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Scan"));
	if (pList->GetCurSel() == -1)
		return;

	CDuiString strName = pControl->GetName();
	CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(pList->GetCurSel());

	//获取应用程序句柄
	HWND hWnd = m_pPaintManager->GetPaintWindow();

	if (strName == _T("CopyIPAddress"))
		CopyDataToClipboard(CF_UNICODETEXT, hWnd, pItem->GetText(0));
	else if (strName == _T("CopyMacAddress"))
		CopyDataToClipboard(CF_UNICODETEXT, hWnd, pItem->GetText(1));
	else if (strName == _T("CopyUserName"))
		CopyDataToClipboard(CF_UNICODETEXT, hWnd, pItem->GetText(5));
	else if (strName == _T("AddUserName"))
		OnAddUserName(pItem);
	else if (strName == _T("StaticMac"))
		OnBindMac(pItem);
	else if (strName == _T("RemoteDesk"))
		OnRemoteDesktop(pItem);
	else if (strName == _T("Ping"))
		OnPingTarget(pItem);
	else if (strName == _T("BlockNet"))
		OnBlockNet(pItem);
	else if (strName == _T("HttpRequest"))
		OnHttpRequest(pItem);
}

void CHostScan::OnAddUserName(CListTextElementUI* pItem)
{
	CAddUserName* pAddUserName = new CAddUserName(m_pPaintManager->GetPaintWindow());
	if (pAddUserName == NULL)
		return;

	CListUI* pScanResultList = (CListUI*)m_pPaintManager->FindControl(_T("Scan"));
	if (pScanResultList == NULL)
		return ;

	CListTextElementUI* pListTextElement = (CListTextElementUI*)pScanResultList->GetItemAt(pScanResultList->GetCurSel());
	if (pListTextElement == NULL)
		return;

	pAddUserName->SetUserName(pListTextElement->GetText(5));
	if (pAddUserName->ShowModal() == IDOK)
	{
		CDuiString strUserName = pAddUserName->GetUserName();
		pListTextElement->SetText(5, strUserName);

		CDuiString strSectionName = pListTextElement->GetText(1);
		if (strSectionName.IsEmpty())
			strSectionName = pListTextElement->GetText(0);

		WritePrivateProfileString(strSectionName, HOST_USER_NAME, strUserName, m_szUserNameFilePath);
	}
}

void CHostScan::OnRemoteDesktop(CListTextElementUI* pItem)
{
	//格式化远程桌面参数
	CDuiString strCmdLine;
	strCmdLine.Format(_T("/v %s"), pItem->GetText(0));

	//获取应用程序句柄
	HWND hWnd = m_pPaintManager->GetPaintWindow();

	//启动远程桌面
	DWORD dwRet = (DWORD)ShellExecute(hWnd, _T("open"), _T("mstsc"), strCmdLine, NULL, SW_SHOW);

	if (dwRet <= 32)
	{
		MessageBox(hWnd, _T("远程桌面启动失败"), _T("提示"), MB_OK);
		return;
	}
}

void CHostScan::OnPingTarget(CListTextElementUI* pItem)
{
	CDuiString strCmdLine;
	strCmdLine.Format(_T("%s -t"), pItem->GetText(0));

	//获取应用程序句柄
	HWND hWnd = m_pPaintManager->GetPaintWindow();

	//启动远程桌面
	DWORD dwRet = (DWORD)ShellExecute(hWnd, _T("open"), _T("ping"), strCmdLine, NULL, SW_SHOW);

	if (dwRet <= 32)
	{
		MessageBox(hWnd, _T("启动Ping命令失败！"), _T("提示"), MB_OK);
		return;
	}
}

void CHostScan::OnBlockNet(CListTextElementUI* pItem)
{
	CBlockNet* pBlock = new CBlockNet(m_pPaintManager->GetPaintWindow());
	pBlock->ShowModal();
}

void CHostScan::OnBindMac(CListTextElementUI* pItem)
{
	MIB_IPNETROW MibIpNetRow;
	ZeroMemory(&MibIpNetRow, sizeof(MIB_IPNETROW));

	//遍历第一个可用的网卡接口
	//由于通过网卡列表获取的数据可能存在不可用的情况，故采用获取路由表的方式，
	//排除回环路由其余路由接口都是可用接口
	CMibIpforwardIpv4List IpForwardList;
	if (GetIpForwardTableIpv4(IpForwardList) == FALSE)
		return;

	POSITION pos = IpForwardList.GetHeadPosition();
	while(pos)
	{
		const MIB_IPFORWARDROW& MibIpFrorWard = IpForwardList.GetNext(pos);
		if (MibIpFrorWard.dwForwardIfIndex == 1)
			continue;

		MibIpNetRow.dwIndex = MibIpFrorWard.dwForwardIfIndex;
	}

	//采用网络地址
	MibIpNetRow.dwAddr = inet_addr(CStringA(pItem->GetText(0)));

	CDuiString strPhyAddr = pItem->GetText(1);

	BYTE bMac[20];
	_stscanf_s(strPhyAddr,_T("%x-%x-%x-%x-%x-%x"),
		&bMac[0],&bMac[1],&bMac[2],&bMac[3],&bMac[4],&bMac[5]);

	memcpy_s(MibIpNetRow.bPhysAddr,6,bMac,6);

	MibIpNetRow.dwPhysAddrLen = 6;
	MibIpNetRow.dwType = MIB_IPNET_TYPE_STATIC;

	DWORD dwRet = CreateIpNetEntry(&MibIpNetRow);
	if (dwRet != NO_ERROR)
	{
		SetErrorInfo(SYSTEM_ERROR, dwRet, _T("绑定静态MAC失败"));
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), _T("提示"), MB_OK);
	}
}

void CHostScan::OnHttpRequest(CListTextElementUI* pItem)
{
	CDuiString strTipInfo;
	strTipInfo.Format(_T("http://%s"), pItem->GetText(0));
	ShellExecute(m_pPaintManager->GetPaintWindow(),_T("open"),strTipInfo,NULL,NULL,NULL);
}

void CHostScan::OnRangeMenu(CControlUI* pControl)
{
	CDuiString strMenuText = pControl->GetName();
	if (strMenuText.IsEmpty())
		return;

	CEditUI2* pStartIP = (CEditUI2*)m_pPaintManager->FindControl(_T("EditStartIP"));
	CEditUI2* pEndIP = (CEditUI2*)m_pPaintManager->FindControl(_T("EditEndIP"));

	TCHAR szNetIP[20], szBroadcastIP[20];
	_stscanf(strMenuText, _T("%[^|]|%s"), szNetIP, szBroadcastIP);

	pStartIP->SetText(szNetIP);
	pEndIP->SetText(szBroadcastIP);
}

void CHostScan::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Scan"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
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
	else
	{
		m_pThreadTaskMgr->RunThread();
	}

	WORD wVer = MAKEWORD(2,2);
	WSADATA WSAData;
	if (WSAStartup(wVer,&WSAData) != 0 )
		return;

	CDuiString strTipInfo;
	do 
	{
		if (m_pThreadTaskMgr == NULL)
		{
			strTipInfo = _T("窗口初始化失败,请尝试重新运行程序");
			break;
		}

		CIPAddressUI* pStart = (CIPAddressUI*)m_pPaintManager->FindControl(_T("EditStartIP"));
		CIPAddressUI* pEnd = (CIPAddressUI*)m_pPaintManager->FindControl(_T("EditEndIP"));
		if (pStart == NULL || pEnd == NULL)
		{
			strTipInfo = _T("IP地址控件初始化失败，请检查资源是否损坏");
			break;
		}

		if (pStart->IsBlank() || pEnd->IsBlank() )
		{
			strTipInfo = _T("扫描范围不能为空，请输入扫描范围");
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
			strTipInfo = _T("请至少选择一项进行扫描");
			break;
		}

		if (pStart->GetText() == pEnd->GetText())
		{
			strTipInfo = _T("起始地址和结束地址一样，请重新输入");
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

		SwitchScanInfo(false);

	} while (FALSE);	

	WSACleanup();

	if (strTipInfo.IsEmpty() == FALSE)
		MessageBox(m_pPaintManager->GetPaintWindow(),strTipInfo,_T("提示"),MB_OK);
}

void CHostScan::StopScan()
{
	m_pThreadTaskMgr->TerminateAll();
	SwitchScanInfo(true);
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

	MessageBox(m_pPaintManager->GetPaintWindow(),_T("清除ARP缓存成功"),_T("网络分析工具提示"),MB_OK|MB_ICONINFORMATION);
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
	MessageBox(m_pPaintManager->GetPaintWindow(),_T("清除动态ARP缓存成功"),_T("网络分析工具提示"),MB_OK|MB_ICONINFORMATION);
}

void CHostScan::OnIpRange(TNotifyUI& msg)
{
	WORD wVer = MAKEWORD(2,2);
	WSADATA WSAData;
	if (WSAStartup(wVer,&WSAData) != 0 )
		return;

	CMenuWnd* pMenu = new CMenuWnd();
	const RECT& rcPos = msg.pSender->GetPos();
	CDuiPoint pt(rcPos.left, rcPos.bottom);
	ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
	STRINGorID strXmlFile(_T("Range.xml"));
	pMenu->Init(NULL,strXmlFile, pt, m_pPaintManager);

	MIB_IPADDRTABLE* pIPAddrTable = NULL;
	do 
	{
		if (GetIPAddrTable(pIPAddrTable) == FALSE)
			break;

		//获取菜单窗口的根结点
		CMenuUI* pMenuRoot = pMenu->GetMenuUI();
		if (pMenuRoot == NULL)
			break;

		for (DWORD dwItem=0; dwItem<pIPAddrTable->dwNumEntries; ++dwItem)
		{
			MIB_IPADDRROW MibIpAddrRow =	pIPAddrTable->table[dwItem];
			if (MibIpAddrRow.dwAddr == 0x100007F)	//排除 127.0.0.1 网络回环地址
				continue;

			CMenuElementUI* pMenuElement = new CMenuElementUI;
			if (pMenuElement == NULL)
				continue;

			//计算网络地址
			DWORD dwNetIP = MibIpAddrRow.dwAddr & MibIpAddrRow.dwMask;
			DWORD dwMask = -1 ^ MibIpAddrRow.dwMask;
			//计算广播地址
			DWORD dwBroadcastIP = dwMask | dwNetIP;
			CDuiString strNetIP, strBroadcastIP, strIPRange, strMaskRange;

			//采用MSDN封装的SSE4 指令集，计算二进制中1的个数
			DWORD dwCount = _mm_popcnt_u32(MibIpAddrRow.dwMask);

			LsIpv4AddressToString(dwNetIP, strNetIP);
			LsIpv4AddressToString(dwBroadcastIP, strBroadcastIP);
			strIPRange.Format(_T("%s/%d"), (LPCTSTR)strNetIP, dwCount);
			strMaskRange.Format(_T("%s|%s"), (LPCTSTR)strNetIP, (LPCTSTR)strBroadcastIP);

			pMenuRoot->Add(pMenuElement);
			pMenuElement->SetText(strIPRange);
			pMenuElement->SetName(strMaskRange);
		}
	} while (FALSE);

	if (pIPAddrTable != NULL)
	{
		delete[] pIPAddrTable;
		pIPAddrTable = NULL;
	}

	WSACleanup();
	//动态添加删除菜单后，需手动重置布局
	pMenu->ResizeMenu();
}

void CHostScan::OnIpTools(TNotifyUI& msg)
{
	CIPTools* pIPTools = new CIPTools(m_pPaintManager->GetPaintWindow());
	if (pIPTools == NULL)
		return;

	pIPTools->ShowModal();
}

void CHostScan::OnShowArp(TNotifyUI& msg)
{
	CArpList* pArpList = new CArpList(m_pPaintManager->GetPaintWindow());
	if (pArpList == NULL)
		return;

	pArpList->ShowModal();
}

void CHostScan::OnWOL(TNotifyUI& msg)
{
	CWakeOnLan* pWOL = new CWakeOnLan(m_pPaintManager->GetPaintWindow());

	pWOL->ShowModal();
}

void CHostScan::AddStaticArp()
{
	CStaticArpDialog* pArpDialog = new CStaticArpDialog(m_pPaintManager->GetPaintWindow());
	pArpDialog->ShowModal();
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
		//先判断是否有任务，如果没有任务才等待
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
			if (dwRet != NO_ERROR)
				continue;
		}
		if ((ResultInfo.HostScanType & HOST_SCAN_TYPE_PING) == HOST_SCAN_TYPE_PING )
		{
			//产生ping
			ResultInfo.bPingOk = CHostScan::PingCmd(TaskJob.dwIpAddress);
		}

		//获取IP对应的主机名称
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
			//添加IP地址
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
			//添加MAC地址
			pListElement->SetText(1,strMac);

			//添加网卡厂商
			TCHAR szName[MAX_PATH];
			if (pHostScan->GetAdapterName(strMac,szName,_countof(szName)) != FALSE)
				pListElement->SetText(2,szName);
			//添加主机用户名
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
		//添加Ping通信测试
		if ((TaskResultInfo.HostScanType & HOST_SCAN_TYPE_PING) == HOST_SCAN_TYPE_PING)
			pListElement->SetText(3,TaskResultInfo.bPingOk == TRUE ? _T("Pass"):_T(""));
		//添加主机名
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
		hIcmpFile = IcmpCreateFile();
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
	pHostScan->SwitchScanInfo(true);
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

void CHostScan::SwitchScanInfo(bool bVisible)
{
	CButtonUI* pBtnStart = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnBeginScan"));
	if (pBtnStart)
		pBtnStart->SetVisible(bVisible);

	CButtonUI* pBtnStop = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnStopScan"));
	if (pBtnStop)
		pBtnStop->SetVisible(bVisible == FALSE);
}