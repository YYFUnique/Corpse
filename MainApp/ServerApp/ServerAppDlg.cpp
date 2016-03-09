
// ServerAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServerApp.h"
#include "ServerAppDlg.h"
#include "CommonDlg/LsFunc.h"

#include "LsCommon/DebugInfo.h"
#include "Include/Define.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerAppDlg �Ի���
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	
END_MESSAGE_MAP()



CServerAppDlg::CServerAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CServerAppDlg::~CServerAppDlg()
{
}

void CServerAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CServerAppDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENU_ABOUT,&CServerAppDlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT,&CServerAppDlg::OnMenuExit)
	ON_COMMAND(ID_CREATE_NEW_CONNECT_DLG,&CServerAppDlg::OnCreateNewConnectDlg)
END_MESSAGE_MAP()


// CServerAppDlg ��Ϣ�������

BOOL CServerAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//::SetMainWnd(this);
	OutputWhenDebugMode(_T("�����ն�"));
	//::SetInfoList(&m_Info);
	//SetAppState(ST_STOPED);
	EVENT_FUNC EventFunc;
	memset(&EventFunc,0,sizeof(EVENT_FUNC));
	EventFunc.pOnReceive = ReceiveData;
	EventFunc.pOnSend = SendData;
	EventFunc.pOnClose = CloseSocket;
	EventFunc.pOnConnect = ConnectOk;
	m_Client.RegEventFunc(&EventFunc);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerAppDlg::OnPaint()
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
HCURSOR CServerAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerAppDlg::OnMenuAbout()
{
	CAboutDlg Dlg;
	Dlg.DoModal();
}

void CServerAppDlg::OnMenuExit()
{
	SendMessage(WM_CLOSE,0,0);
}

void CServerAppDlg::OnCreateNewConnectDlg()
{
	EnServiceState EnState = m_Client.GetConnStat();
	if (EnState != SS_STARTED || EnState != SS_STARTING)
	{
		CString strIP,strPort;
		BOOL bRet = ShowNewConnectDlg(strIP,strPort);
		if (bRet == TRUE)
		{
			WORD wPort = (WORD)_tstoi(strPort);
			if(m_Client.Start(strIP, wPort, TRUE) == FALSE)
				SetErrorInfo(SYSTEM_ERROR,0,_T("��������ʧ��"));
			//else
			//	//GetDlgItem(ID_CREATE_NEW_CONNECT_DLG)->SetWindowText(_T("�Ͽ�����"));
		}
	}
	else
	{
		//if (m_Client.Stop() == TRUE)
		//	//GetDlgItem(ID_CREATE_NEW_CONNECT_DLG)->SetWindowText(_T("�½�����"));
		//else
		//	SetErrorInfo(SYSTEM_ERROR,0,_T("�Ͽ�����ʧ��"));
	}
}

ISocketListener::EnHandleResult CServerAppDlg::ReceiveData(CONNID dwConnID, const BYTE* pData, int iLength)
{
	CMsgHead Msg((PMSG_HEAD)pData);
	switch(Msg.GetMsgId())
	{
		case CORPSE_SHOW_CARD:
			{
				CComputerInfo UserComputer;
				Msg.SetData(UserComputer.GetComputerInfo());
				CServerAppDlg* pDlg = (CServerAppDlg*)AfxGetApp()->m_pMainWnd;
				pDlg->GetClientSocket().Send(dwConnID,Msg.GetMsgHead(),Msg.GetDataLen()+sizeof(MSG_HEAD));
			}
			break;
		default:
			OutputWhenDebugMode(_T("�յ����Կͻ��˵�δ������Ϣ[%d:%s]"),Msg.GetMsgId(),Msg.GetMsgValue());
	}
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerAppDlg::CloseSocket(CONNID dwConnID)
{

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerAppDlg::SendData(CONNID dwConnID, const BYTE* pData, int iLength)
{
	//CString strInfo;
	//strInfo.Format(_T("%d"),dwConnID);
	//OutputDebugString(strInfo);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerAppDlg::ConnectOk(CONNID dwConnID,LPCTSTR lpszIPAddr,WORD wPort)
{
	/*CString strInfo;
	strInfo.Format(_T("����Զ�̷�����:%s:%d�ɹ�"),lpszIPAddr,wPort);
	OutputDebugString(strInfo);

	AfxGetClientSocket().Send(dwConnID,(PBYTE)(LPCTSTR)strInfo,strInfo.GetLength()*sizeof(TCHAR));*/
	return ISocketListener::HR_OK;
}

CClientSocket& CServerAppDlg::GetClientSocket()
{
	return m_Client;
}