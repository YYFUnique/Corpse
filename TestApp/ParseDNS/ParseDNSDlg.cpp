
// ParseDNSDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ParseDNS.h"
#include "ParseDNSDlg.h"

#pragma comment(lib,"Dnsapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CParseDNSDlg �Ի���

#define	TRAYTOOLTIPTEXT					_T("������ͨ��̽�⹤��V1.0")
#define	WM_TRAYMESSAGE				WM_USER + 0x1000
#define	WM_SHOWTRAYMESSAGE		WM_USER + 0x1001
#define	THREAD_EVENT						_T("2CECFE49-0614-439b-AB20-5690DE8B8D32")

CParseDNSDlg::CParseDNSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParseDNSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pEditCtrl= NULL;
	m_bDetecteOver = TRUE;
	m_hThreadRemoteDname = NULL;
	m_hThreadRemoteIp = NULL;

	m_hEvent = CreateEvent(NULL,FALSE,FALSE,THREAD_EVENT);
}

CParseDNSDlg::~CParseDNSDlg()
{
	m_bDetecteOver = TRUE;
	SetEvent(m_hEvent);
	SetEvent(m_hEvent);
	if (m_hThreadRemoteDname)
	{
		if (WaitForSingleObject(m_hThreadRemoteDname,2*1000) == WAIT_TIMEOUT)
			TerminateThread(m_hThreadRemoteDname,0);
		CloseHandle(m_hThreadRemoteDname);
	}
	if (m_hThreadRemoteIp)
	{
		if (WaitForSingleObject(m_hThreadRemoteIp,2*1000) == WAIT_TIMEOUT)
			TerminateThread(m_hThreadRemoteIp,0);
		CloseHandle(m_hThreadRemoteIp);
	}

	if (m_hEvent)
		CloseHandle(m_hEvent);

	Shell_NotifyIcon(NIM_DELETE, &m_trayData);
}

void CParseDNSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CParseDNSDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CParseDNSDlg::OnBtnStartClicked)
	ON_BN_CLICKED(IDC_STOP,&CParseDNSDlg::OnBtnStopClicked)
	ON_MESSAGE(WM_SHOWTRAYMESSAGE,&CParseDNSDlg::OnShowTrayMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CParseDNSDlg ��Ϣ�������

BOOL CParseDNSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	EnableWindowControl(FALSE);

	CreateTrayIcon();

	GetDlgItem(IDC_EDIT_DNAME)->SetWindowText(_T("www.sina.com.cn"));
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(_T("180.97.33.108"));
	GetDlgItem(IDC_EDIT_PERIOD)->SetWindowText(_T("10"));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CParseDNSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CParseDNSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CParseDNSDlg::CreateTrayIcon()
{
	m_trayData.cbSize = sizeof(NOTIFYICONDATA);
	m_trayData.hWnd	 = m_hWnd;
	m_trayData.uID	 = IDR_MAINFRAME;
	m_trayData.hIcon = m_hIcon;
	m_trayData.uFlags =  NIF_MESSAGE|NIF_ICON | NIF_TIP |NIF_INFO;
	m_trayData.uCallbackMessage = WM_TRAYMESSAGE;
	wcscpy_s(m_trayData.szInfo ,_countof(m_trayData.szInfo), _T("\0"));
	wcscpy_s(m_trayData.szInfoTitle , _countof(m_trayData.szInfoTitle) , _T("\0"));
	wcscpy_s(m_trayData.szTip,_countof(m_trayData.szTip),TRAYTOOLTIPTEXT);
	m_trayData.uTimeout = 10;
	m_trayData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_ADD,&m_trayData);
}

void CParseDNSDlg::ShowTrayIconTextTip(LPCTSTR lpszTitle,LPCTSTR lpszTooltip)
{
	wcscpy_s(m_trayData.szInfo ,_countof(m_trayData.szInfo), lpszTooltip);
	wcscpy_s(m_trayData.szInfoTitle , _countof(m_trayData.szInfoTitle) , lpszTitle );
	Shell_NotifyIcon(NIM_MODIFY,&m_trayData);
}

LRESULT CParseDNSDlg::OnShowTrayMessage(WPARAM wParam,LPARAM lParam)
{
	ShowTrayIconTextTip((LPCTSTR)wParam,(LPCTSTR)lParam);

	return TRUE;
}

void CParseDNSDlg::OnBtnStartClicked()
{
	if (m_bDetecteOver != TRUE)
		return;

	GetDlgItem(IDC_EDIT_DNAME)->GetWindowText(m_strRemoteDname);
	if (m_strRemoteDname.IsEmpty())
	{
		MessageBox(_T("��������Ҫ����������"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		m_pEditCtrl->SetFocus();
		return;
	}

	GetDlgItem(IDC_IPADDRESS)->GetWindowText(m_strRemoteIp);
	if (m_strRemoteIp.IsEmpty())
	{
		MessageBox(_T("��������Ҫ�������ӵ�IP��ַ"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		m_pEditCtrl->SetFocus();
		return;
	}

	m_pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_PERIOD);
	CString strPeriod;
	m_pEditCtrl->GetWindowText(strPeriod);
	int nPeriod = _ttoi(strPeriod);
	if (nPeriod<=0)
	{
		MessageBox(_T("�����������ʱ�䲻��ȷ������������"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		m_pEditCtrl->SetFocus();
		return;
	}

	m_nPeriod = nPeriod;
	m_bDetecteOver = FALSE;

	m_hThreadRemoteDname = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ParseDNS,this,0,NULL);
	m_hThreadRemoteIp = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ConnectRemoteIP,this,0,NULL);

	EnableWindowControl(TRUE);
}

void CParseDNSDlg::OnBtnStopClicked()
{
	m_bDetecteOver = TRUE;
	SetEvent(m_hEvent);
	SetEvent(m_hEvent);
	EnableWindowControl(FALSE);
}

void CParseDNSDlg::EnableWindowControl(BOOL bEnabled)
{
	GetDlgItem(IDC_BTN_START)->EnableWindow(bEnabled == FALSE);
	GetDlgItem(IDC_EDIT_DNAME)->EnableWindow(bEnabled == FALSE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(bEnabled == FALSE);
	GetDlgItem(IDC_EDIT_PERIOD)->EnableWindow(bEnabled == FALSE);

	GetDlgItem(IDC_STOP)->EnableWindow(bEnabled == TRUE);
}


BOOL CParseDNSDlg::IsDetectOver()
{
	return m_bDetecteOver;
}

int CParseDNSDlg::GetPeriod()
{
	return m_nPeriod;
}

CString CParseDNSDlg::GetRemoteDname()
{
	return m_strRemoteDname;
}

CString CParseDNSDlg::GetRemoteIp()
{
	return m_strRemoteIp;
}

HANDLE CParseDNSDlg::GetEventHandle()
{
	return m_hEvent;
}

UINT CParseDNSDlg::ParseDNS(LPARAM lParam)
{
	CParseDNSDlg* pMainDlg = (CParseDNSDlg*)lParam;
	DNS_STATUS DnsStatus;               //Return value of  DnsQuery_A() function.
	PDNS_RECORD pDnsRecord;          //Pointer to DNS_RECORD structure.

	CString strRemoteDname = pMainDlg->GetRemoteDname();
	HANDLE hEvent = pMainDlg->GetEventHandle();
	int nPeriod = pMainDlg->GetPeriod();
	while (TRUE)
	{
		if (pMainDlg->IsDetectOver())
			break;

		DnsStatus = DnsQuery_W(strRemoteDname,DNS_TYPE_A,DNS_QUERY_BYPASS_CACHE,NULL,&pDnsRecord,NULL);
		if (DnsStatus == 0)
		{
			PDNS_RECORD pDnsChche = pDnsRecord;

			while(pDnsChche)
			{
				if (pDnsChche->wType == DNS_TYPE_A)
				{
					SYSTEMTIME SystemTime;
					GetLocalTime(&SystemTime);
					CString strTime;
					strTime.Format(_T("%02d:%02d:%02d:%03d"),SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,SystemTime.wMilliseconds);

					IN_ADDR InAddr;
					InAddr.s_addr = pDnsChche->Data.A.IpAddress;
					CString strIPAddress(inet_ntoa(InAddr));
					CString strDnsData;
					CString strTitle(_T("���������߳���ʾ"));
					strDnsData.Format(_T("��ǰʱ��[%s],����%s�ɹ�,IP��ַ%s"),strTime,strRemoteDname,strIPAddress);
					pMainDlg->SendMessage(WM_SHOWTRAYMESSAGE,(WPARAM)(LPCTSTR)strTitle,(LPARAM)(LPCTSTR)strDnsData);
					OutputDebugString(strDnsData);
					break;
				}
				pDnsChche = pDnsChche->pNext;
			}

			DnsRecordListFree(pDnsRecord,DnsFreeRecordList);
		}
		//ʹ��waitforsingleobject
		//Sleep(pMainDlg->GetPeriod()*1000);
		if (WaitForSingleObject(hEvent,nPeriod*1000) == WAIT_OBJECT_0)
			break;
	}

	return 0;
}

UINT CParseDNSDlg::ConnectRemoteIP(LPARAM lParam)
{	
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
		return 0;

	CParseDNSDlg* pMainDlg = (CParseDNSDlg*)lParam;
	CString strRemoteIPAddress = pMainDlg->GetRemoteIp();
	CStringA strRemoteIp(strRemoteIPAddress);

	HANDLE hEvent = pMainDlg->GetEventHandle();
	int nPeriod = pMainDlg->GetPeriod();

	while(TRUE)
	{
		if (pMainDlg->IsDetectOver())
			break;

		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in clientService; 
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(strRemoteIp);
		clientService.sin_port = htons(80);

		if ( connect( s, (SOCKADDR*) &clientService, sizeof(clientService) ) == 0) 
		{
			SYSTEMTIME SystemTime;
			GetLocalTime(&SystemTime);
			CString strTime;
			strTime.Format(_T("%02d:%02d:%02d:%03d"),SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,SystemTime.wMilliseconds);

			CString strTitle(_T("����Զ�̵�ַ�߳���ʾ"));
			CString strData;
			strData.Format(_T("��ǰʱ��[%s],Զ�̵�ַ[%s]���ӳɹ�"),strTime,strRemoteIPAddress);
	
			pMainDlg->SendMessage(WM_SHOWTRAYMESSAGE,(WPARAM)(LPCTSTR)strTitle,(LPARAM)(LPCTSTR)strData);
			OutputDebugString(strData);
		}		
		closesocket(s);

		if (WaitForSingleObject(hEvent,nPeriod*1000) == WAIT_OBJECT_0)
			break;
	}

	WSACleanup();
	return 0;
}