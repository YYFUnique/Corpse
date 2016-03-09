#include "stdafx.h"
#include "HostScan.h"
#include "resource.h"
#include "Window/AddUserName.h"
#include <Winsock2.h>
#include <atlstr.h>
#include <DbgHelp.h>
#include <Iphlpapi.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"Iphlpapi.lib")

#define	 TIMER_ID_PROCESS				1000								//定时器ID
#define	 BLOCK_NET_TIMER_ID			1009								//阻断网络定时器
#define	 WM_GET_TASK_RESULT		(WM_USER + 0x1000)		//获取Task结果消息
#define    WM_SCANHOST_FINSH		    (WM_USER + 0x1001)		//扫描完成
#define     PHAYADDR_LENGTH				6										//物理地址长度
#define	 HOST_USER_NAME				_T("UserName")				//用户名

volatile LONG CHostScan::m_sdwTaskDown = 0;

int CALLBACK ListCompare(UINT_PTR lParam1, UINT_PTR lParam2, UINT_PTR lParamSort)
{
	CListTextElementUI* pControl = (CListTextElementUI*)lParam1;
	CDuiString s = pControl->GetText(0);
	return 1;
}

LONG WINAPI LsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	CString strDumpFilePath(_T("C:\\1.dmp"));

	HANDLE lhDumpFile = CreateFile(strDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpWithPrivateReadWriteMemory , &loExceptionInfo, NULL, NULL);
	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}

CHostScan::CHostScan()
	:m_uTimerId(0)
	,m_pTaskbarList(NULL)
	,m_pThreadBlockMgr(NULL)
{ 
	/*WM_TASKBARBUTTONCREATED = :: RegisterWindowMessage (TEXT ("TaskbarButtonCreated" ));*/
}

CHostScan::~CHostScan()
{
	WSACleanup();
	if (m_pThreadTaskMgr != NULL)
	{
		m_pThreadTaskMgr->StopAllTaskQueue();
		delete m_pThreadTaskMgr;
	}

	if (m_pThreadBlockMgr != NULL)
	{
		m_pThreadBlockMgr->StopAllTaskQueue();
		delete m_pThreadBlockMgr;
	}

	if (m_uTimerId !=0)
		KillTimer(m_hWnd,m_uTimerId);
	if (m_pTaskbarList != NULL)
		m_pTaskbarList->Release();
}

LPCTSTR CHostScan::GetWindowClassName() const
{
	return _T("HostScanDuiWnd");
}

UILIB_RESOURCETYPE CHostScan::GetResourceType() const
{
	return UILIB_FILE;
}

LPCTSTR CHostScan::GetResourceID() const
{
	return _T("");
}

CDuiString CHostScan::GetZIPFileName() const
{
	return _T("HostScan.zip");
}

CDuiString CHostScan::GetSkinFile()
{
	return _T("MainWindow.xml");
}

CDuiString CHostScan::GetSkinFolder()
{
	return _T("HostScan");
}

void CHostScan::Notify(TNotifyUI& msg)
{
 	if (msg.sType == DUI_MSGTYPE_CLICK)
 		OnClick(msg);
	else if(msg.sType == DUI_MSGTYPE_MENU)
		OnItemMenu(msg);
	else if(msg.sType == DUI_MSGTYPE_HEADERCLICK)
		OnHeaderClick(msg);
}

void CHostScan::InitWindow()
{
	SetUnhandledExceptionFilter(LsUnhandledExceptionFilter);
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2),&wsaData);
	SetIcon(IDI_MAINFRAME);
	m_pThreadTaskMgr = new CThreadTaskMgr(50,(ThreadPoolTaskFun)TaskThread);
	if (m_pThreadTaskMgr != NULL)
		m_pThreadTaskMgr->RegisterNotify(m_hWnd,WM_GET_TASK_RESULT);
	
	GetModuleFileName(NULL,m_szMacAddressFilePath,_countof(m_szMacAddressFilePath));
	PathRemoveFileSpec(m_szMacAddressFilePath);
	_tcscpy_s(m_szUserNameFilePath,_countof(m_szUserNameFilePath),m_szMacAddressFilePath);
	PathAppend(m_szMacAddressFilePath,_T("MacAddress.ini"));
	PathAppend(m_szUserNameFilePath,_T("HostName.ini"));
}


LRESULT CHostScan::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
// 	if (uMsg == WM_TASKBARBUTTONCREATED)
// 	{
// 		CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList, (void **)&m_pTaskbarList);
// 		if (m_pTaskbarList == NULL)
// 			return;
// 		//添加任务栏覆盖图标
// 		OnCreateTaskBarButton();
// 	}

 	switch (uMsg)
 	{
		case WM_MENUCLICK:
				OnMenuClick((CControlUI*)lParam);
			break;
 		case WM_GET_TASK_RESULT:
				OnGetTaskResult();
 			break;
		case WM_SCANHOST_FINSH:
				OnScanHostFinsh();
			break;
 	}
	return TRUE;
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

LRESULT CHostScan::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_TIMER:
				OnTimer(uMsg,wParam,lParam);
			break;
	}
	
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

CControlUI* CHostScan::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}


UINT CHostScan::AddTaskThread(LPVOID lParam)
{
	return 0;
}

void CHostScan::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close();
	else if (msg.pSender->GetName() == _T("BtnMax"))
	{
		m_PaintManager.FindControl(_T("BtnMax"))->SetVisible(false);
		m_PaintManager.FindControl(_T("BtnRestore"))->SetVisible(true);
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("BtnMin")))
	{
		//RestoreBtnStatus(msg.pSender);
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("BtnRestore")))
	{
		m_PaintManager.FindControl(_T("BtnMax"))->SetVisible(true);
		m_PaintManager.FindControl(_T("BtnRestore"))->SetVisible(false);
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("BtnBeginScan")))
		OnBeginScan(msg);
}

void CHostScan::OnItemMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ScanInfo"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("MenuWindow.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
	}
}

void CHostScan::OnHeaderClick(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetName() == _T("scanheader"))
	{
		CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("ScanInfo"));
		pList->SortItems(&ListCompare,1);
	}
}

void CHostScan::OnBeginScan(TNotifyUI& msg)
{
	CDuiString strTipInfo;
	do 
	{
		if (m_pThreadTaskMgr == NULL)
		{
			strTipInfo = _T("窗口初始化失败,请尝试重新运行程序");
			break;
		}

		CControlUI* pControlStart = m_PaintManager.FindControl(_T("EditStartIP"));
		CControlUI* pCOntrolEnd = m_PaintManager.FindControl(_T("EditEndIP"));
		if (pControlStart->GetText().IsEmpty() || pCOntrolEnd->GetText().IsEmpty())
		{
			strTipInfo = _T("请输入扫描范围");
			break;
		}

		COptionUI* pControlARP = (COptionUI*)m_PaintManager.FindControl(_T("BtnOpARP"));
		COptionUI* pControlPING = (COptionUI*)m_PaintManager.FindControl(_T("BtnOpPING"));
		COptionUI* pControlHostName = (COptionUI*)m_PaintManager.FindControl(_T("BtnOpHostName"));

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

		CListUI* pScanResultList = (CListUI*)m_PaintManager.FindControl(_T("ScanInfo"));
		pScanResultList->RemoveAll();

		DWORD dwStartIP = ntohl(inet_addr(CStringA(pControlStart->GetText())));
		DWORD dwEndIp = ntohl(inet_addr(CStringA(pCOntrolEnd->GetText())));
		m_uIpCount = dwEndIp - dwStartIP+1;
		TaskInfo.nItemIndex = -1;
		for (UINT uIp =0;uIp<m_uIpCount;++uIp)
		{
			TaskInfo.dwIpAddress = htonl(uIp+dwStartIP);
			m_pThreadTaskMgr->AddTaskToQueue(TaskInfo);
		}
		m_sdwTaskDown = 0;
		if (m_uTimerId !=0)
			KillTimer(m_hWnd,m_uTimerId);
		m_uTimerId = SetTimer(m_hWnd,TIMER_ID_PROCESS,1000,NULL);

		if (m_pTaskbarList)
		{
			m_pTaskbarList->SetProgressState(m_hWnd,TBPF_INDETERMINATE);
			m_pTaskbarList->SetProgressValue(m_hWnd,0,m_uIpCount);
		}
	} while (FALSE);	

	if (strTipInfo.IsEmpty() == FALSE)
		MessageBox(m_hWnd,strTipInfo,_T("提示"),MB_OK);
}

LRESULT CHostScan::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIMER_ID_PROCESS && m_pTaskbarList != NULL)
	{
		m_pTaskbarList->SetProgressValue(m_hWnd,m_sdwTaskDown,m_uIpCount);
		if (m_sdwTaskDown == m_uIpCount)
		{
			KillTimer(m_hWnd,TIMER_ID_PROCESS);
			m_pTaskbarList->SetProgressState(m_hWnd,TBPF_NOPROGRESS);
			m_pTaskbarList->Release();
			PostMessage(WM_SCANHOST_FINSH,0,0);
		}
	}
	else if (wParam == BLOCK_NET_TIMER_ID)
	{

	}
	return TRUE;
}

void CHostScan::OnMenuClick(CControlUI* pMenuControl)
{
	if (pMenuControl->GetName() == _T("AddUserName"))
		OnAddUserName();
	else if (pMenuControl->GetName() == _T("BlockNet"))
		OnBlockHost();
	else if (pMenuControl->GetName() == _T("CopyIPAddress"))
		OnCopyIpAddress();
	else if (pMenuControl->GetName() == _T("CopyMacAddress"))
		OnCopyMacAddress();
	else if (pMenuControl->GetName() == _T("CopyUserName"))
		OnCopyUserName();
}

LRESULT CHostScan::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( IsWindowVisible(m_hWnd) && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED))
	{
		CButtonUI* pMax = (CButtonUI*)m_PaintManager.FindControl(_T("BtnMax"));
		CButtonUI* pRestor = (CButtonUI*)m_PaintManager.FindControl(_T("BtnRestore"));
		pMax->SetVisible(wParam == SIZE_RESTORED);
		pRestor->SetVisible(wParam != SIZE_RESTORED);
	}

	return WindowImplBase::OnSize(uMsg,wParam,lParam,bHandled);
}

void CHostScan::OnAddUserName()
{
	CAddUserName AddUserName(m_hWnd);
	AddUserName.CenterWindow();
	if (AddUserName.ShowModal() == IDOK)
	{
		CDuiString strUserName = AddUserName.GetUserName();
		CListUI* pScanResultList = (CListUI*)m_PaintManager.FindControl(_T("ScanInfo"));
		CListTextElementUI* pListTextElement = (CListTextElementUI*)pScanResultList->GetItemAt(pScanResultList->GetCurSel());
		pListTextElement->SetText(5,strUserName);
		CDuiString strSectionName = pListTextElement->GetText(1);
		if (strSectionName.IsEmpty())
			strSectionName = pListTextElement->GetText(0);
		WritePrivateProfileString(strSectionName,HOST_USER_NAME,strUserName,m_szUserNameFilePath);
	}
}

void CHostScan::OnCopyIpAddress()
{

}

void CHostScan::OnCopyMacAddress()
{

}

void CHostScan::OnCopyUserName()
{

}

void CHostScan::OnBlockHost()
{
	static BOOL bInit = FALSE;
	CListUI* pScanResultList = (CListUI*)m_PaintManager.FindControl(_T("ScanInfo"));
	DWORD dwItem = pScanResultList->GetCurSel();
	if (bInit == FALSE)
	{
		//设置全局信息，包括ARP源MAC、源IP等信息
		m_pThreadBlockMgr = new CThreadBlockMgr(5,(ThreadPoolTaskFun)BlockHostThread);
		bInit = TRUE;
	}
	/*TaskInfo HostInfo;*/
	/*HostInfo.MacAddr = ;*/
	CListTextElementUI* pListText = (CListTextElementUI*)pScanResultList->GetItemAt(dwItem);

// 	被阻断者MAC
// 		CString strHostMac = pListText->GetText(1);
// 		strHostMac.Replace(_T("-"),_T(""));
// 		BYTE* lpByte = (BYTE*)HostInfo.MacAddrDst;
// 		for (int i=0;i<PHAYADDR_LENGTH;++i)
// 			lpByte[i] = strHostMac.GetAt(i);
// 	
// 		//发送者MAC
// 		strHostMac = _T("00-24-21-B8-54-A5");
// 		strHostMac.Replace(_T("-"),_T(""));
// 		lpByte = (BYTE*)HostInfo.MacAddrOrg;
// 		for (int i=0;i<PHAYADDR_LENGTH;++i)
// 			lpByte[i] = strHostMac.GetAt(i);
// 	
// 		//模拟MAC地址
// 		strHostMac = _T("00-23-5E-CB-54-A3");
// 		strHostMac.Replace(_T("-"),_T(""));
// 		lpByte = (BYTE*)HostInfo.MacAddrArp;
// 		for (int i=0;i<PHAYADDR_LENGTH;++i)
// 			lpByte[i] = strHostMac.GetAt(i);
// 	
// 		//模拟IP地址
// 		CStringA strIpAddressArp = _T("10.1.1.163");
// 		HostInfo.dwIpAddress = inet_addr(strIpAddressArp);
// 		m_pThreadBlockMgr->AddTaskToQueue(HostInfo);
}

void CHostScan::OnGetTaskResult()
{
	ResultQueue TaskResultInfo;
	if (m_pThreadTaskMgr->GetTaskResultInfo(TaskResultInfo) != FALSE)
	{
		CListUI* pScanResultList = (CListUI*)m_PaintManager.FindControl(_T("ScanInfo"));
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
			if (GetAdapterName(strMac,szName,_countof(szName)) != FALSE)
				pListElement->SetText(2,szName);
			//添加主机用户名
			if (GetHostUserName(strMac,szName,_countof(szName)) != FALSE)
				pListElement->SetText(5,szName);

			TaskQueue TaskInfo;
			TaskInfo.dwIpAddress = TaskResultInfo.dwIpAddress;
			TaskInfo.nItemIndex = pListElement->GetIndex();

			COptionUI* pControlHostName = (COptionUI*)m_PaintManager.FindControl(_T("BtnOpHostName"));
			if (pControlHostName->IsSelected())
			{
				TaskInfo.HostScanType = HOST_SCAN_TYPE_HOSTNAME;
				m_pThreadTaskMgr->AddTaskToQueue(TaskInfo);
			}
			
			COptionUI* pControlPing = (COptionUI*)m_PaintManager.FindControl(_T("BtnOpPING"));
			if (pControlPing->IsSelected())
			{
				TaskInfo.HostScanType = HOST_SCAN_TYPE_PING;
				m_pThreadTaskMgr->AddTaskToQueue(TaskInfo);
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

void CHostScan::OnScanHostFinsh()
{
	if (GetForegroundWindow() != m_hWnd)
	{
		FLASHWINFO FlashWinfo;
		FlashWinfo.cbSize = sizeof(FLASHWINFO);
		FlashWinfo.hwnd = m_hWnd;
		FlashWinfo.dwFlags = FLASHW_ALL;
		FlashWinfo.uCount = 5;
		FlashWinfo.dwTimeout = 1*1000;
		FlashWindowEx(&FlashWinfo);
	}
}

void CHostScan::OnCreateTaskBarButton()
{
	if (m_pTaskbarList != NULL)
	{
		m_hIcon = (HICON)LoadImage(NULL,_T("C:\\Correct.ico"),IMAGE_ICON,32,32,LR_LOADFROMFILE);
		m_pTaskbarList->SetOverlayIcon(m_hWnd,m_hIcon,_T("Demo"));
	}
}

UINT CHostScan::TaskThread(LPVOID lParam)
{
	CThreadTaskMgr* pThreadMgr = (CThreadTaskMgr*)lParam;
	while(TRUE)
	{
		if (pThreadMgr->IsTaskStoped())
			break;
		//先判断是否有任务，如果没有任务才等待
		if (pThreadMgr->GetTaskJobCount() <=0 )
			WaitForSingleObject(pThreadMgr->GetAddTaskJobEvent(),INFINITE);
		if (pThreadMgr->IsTaskStoped())
			break;

		TaskQueue TaskJob;
		if (pThreadMgr->GetTaskJobInfo(TaskJob) == FALSE)
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
			InterlockedIncrement(&m_sdwTaskDown);
			if (dwRet != NO_ERROR)
				continue;
		}
		if ((ResultInfo.HostScanType & HOST_SCAN_TYPE_PING) == HOST_SCAN_TYPE_PING )
		{
			//产生ping
		}

		HOSTENT* pRemoteHost;
		if ((ResultInfo.HostScanType & HOST_SCAN_TYPE_HOSTNAME) == HOST_SCAN_TYPE_HOSTNAME)
		{
			pRemoteHost = gethostbyaddr((char*)&ResultInfo.dwIpAddress, 4, AF_INET);
			if (pRemoteHost != NULL)
				_tcscpy_s(ResultInfo.szHostName,_countof(ResultInfo.szHostName),CString(pRemoteHost->h_name));
		}

		pThreadMgr->AddTaskResultToQueue(ResultInfo);
	}

	return 0;
}

UINT CHostScan::BlockHostThread(LPVOID lParam)
{
	CThreadTaskMgr* pThreadMgr = (CThreadTaskMgr*)lParam;

// 	SOCKET ArpSocket = socket(AF_INET, SOCK_PACKET, htons(0x0806));  
// 	if (ArpSocket == INVALID_SOCKET) 
// 		return FALSE;

	while(TRUE)
	{
		if (pThreadMgr->IsTaskStoped())
			break;
		//先判断是否有任务，如果没有任务才等待
		if (pThreadMgr->GetTaskJobCount() <=0 )
			WaitForSingleObject(pThreadMgr->GetAddTaskJobEvent(),INFINITE);
		if (pThreadMgr->IsTaskStoped())
			break;

// 		TaskInfo TaskHostJob;
// 		if (pThreadMgr->GetTaskJobInfo(TaskHostJob) == FALSE)
// 			continue;

		//sendto(S,(CHAR*)&TaskHostJob,sizeof(TaskHostJob),)
	}

	return TRUE;
}