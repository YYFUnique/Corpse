
// ServerAppDlg.cpp : 实现文件
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


// CServerAppDlg 对话框
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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


// CServerAppDlg 消息处理程序

BOOL CServerAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//::SetMainWnd(this);
	OutputWhenDebugMode(_T("启动终端"));
	//::SetInfoList(&m_Info);
	//SetAppState(ST_STOPED);
	EVENT_FUNC EventFunc;
	memset(&EventFunc,0,sizeof(EVENT_FUNC));
	EventFunc.pOnReceive = ReceiveData;
	EventFunc.pOnSend = SendData;
	EventFunc.pOnClose = CloseSocket;
	EventFunc.pOnConnect = ConnectOk;
	m_Client.RegEventFunc(&EventFunc);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
				SetErrorInfo(SYSTEM_ERROR,0,_T("创建连接失败"));
			//else
			//	//GetDlgItem(ID_CREATE_NEW_CONNECT_DLG)->SetWindowText(_T("断开连接"));
		}
	}
	else
	{
		//if (m_Client.Stop() == TRUE)
		//	//GetDlgItem(ID_CREATE_NEW_CONNECT_DLG)->SetWindowText(_T("新建连接"));
		//else
		//	SetErrorInfo(SYSTEM_ERROR,0,_T("断开连接失败"));
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
			OutputWhenDebugMode(_T("收到来自客户端的未处理消息[%d:%s]"),Msg.GetMsgId(),Msg.GetMsgValue());
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
	strInfo.Format(_T("连接远程服务器:%s:%d成功"),lpszIPAddr,wPort);
	OutputDebugString(strInfo);

	AfxGetClientSocket().Send(dwConnID,(PBYTE)(LPCTSTR)strInfo,strInfo.GetLength()*sizeof(TCHAR));*/
	return ISocketListener::HR_OK;
}

CClientSocket& CServerAppDlg::GetClientSocket()
{
	return m_Client;
}