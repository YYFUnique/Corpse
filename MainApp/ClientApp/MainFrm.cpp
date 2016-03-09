
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "ClientApp.h"

#include "MainFrm.h"
#include "LsCommon/ErrorInfo.h"

#include "Toolbar/ToolbarMain.h"
#include "Toolbar/ToolbarMgr.h"
#include "CommonDlg/LsFunc.h"
#include "NetTransport/NetWork.h"
#include "Include/Define.h"
#include "LsCommon/DebugInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_START_SERVER_DLG,&CMainFrame::OnStartServerDlg)
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,&CMainFrame::OnToolTipText)
	ON_COMMAND_RANGE(ID_TOOL_COMPUTER,ID_TOOL_TASK,&CMainFrame::OnToolbarClick)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	SetMainAppTitle(_T("超级控制系统"));
}

CMainFrame::~CMainFrame()
{
	ReleaseProcessErrorInfo();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("创建TOOLBAR工具栏失败\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableToolTips();

	m_wndToolBar.ShowWindow(SW_SHOW);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	m_wndToolBar.LoadTrueColorToolBar(48,IDB_TOOLBAR_NORMAL_BMP,IDB_TOOLBAR_NORMAL_BMP,IDB_TOOLBAR_DISABLE_BMP);

	for (int i=0;i<m_wndToolBar.GetCount();i++)
	{
		UINT id=m_wndToolBar.GetItemID(i);
		CString s;
		if (!s.LoadString(id))
			continue;

		m_wndToolBar.SetButtonText(i,s);
	}
	CRect rect;
	m_wndToolBar.GetItemRect(0,&rect);
	m_wndToolBar.SetSizes(rect.Size(),CSize(48,48));	

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	EVENT_FUNC EventFunc;
	memset(&EventFunc,0,sizeof(EVENT_FUNC));
	EventFunc.pOnAccept = AcceptClient;
	EventFunc.pOnReceive = ReceiveData;
	EventFunc.pOnClose = CloseSocket;
	m_Server.RegEventFunc(&EventFunc);
	m_Server.Start(_T("0.0.0.0"),7890);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序


BOOL CMainFrame::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	//UNICODE消息
	TOOLTIPTEXT* pTT = (TOOLTIPTEXT*)pNMHDR;

	CString strTipText;
	
	return TRUE;
}

void CMainFrame::OnToolbarClick(UINT nID)
{
	AfxGetToolbarMgr()->SetCurrentObjectByID(nID);
	CToolbarMain* pToolbar = AfxGetToolbarMgr()->GetCurrentToolbar();
	ASSERT(pToolbar);

	pToolbar->Launch();
}

CString& CMainFrame::GetMainAppTitle()
{
	return m_strMainAppTitle;
}

void CMainFrame::SetMainAppTitle(LPCTSTR lpszMainAppTitle)
{
	ASSERT(lpszMainAppTitle && _tcslen(lpszMainAppTitle) != 0);
	m_strMainAppTitle = lpszMainAppTitle;
}
void CMainFrame::OnStartServerDlg()
{
	CString strIP,strPort;
	BOOL bRet = ShowNewConnectDlg(strIP,strPort);
	if (bRet == TRUE)
	{
		WORD wPort = (WORD)_tstoi(strPort);
		if(m_Server.Start(strIP, wPort) == FALSE)
		{
			
		}
	}
}

BOOL CMainFrame::SendData(CONNID dwConnID, PMSG_HEAD pData, int iLength)
{
	return m_Server.Send(dwConnID,pData,iLength);
}

ISocketListener::EnHandleResult CMainFrame::SendData(CONNID dwConnID, const BYTE* pData, int iLength)
{
	return ISocketListener::HR_OK;
}
ISocketListener::EnHandleResult CMainFrame::ReceiveData(CONNID dwConnID, const BYTE* pData, int iLength)
{
	CMsgHead Msg((PMSG_HEAD)pData);
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	switch(Msg.GetMsgId())
	{
		case CORPSE_SHOW_CARD:
				pMain->InsertItem(dwConnID,Msg);
			break;
		default:
			OutputWhenDebugMode(_T("收到来自客户端未处理的消息%d:%s"),Msg.GetMsgId(),Msg.GetMsgValue());
	}
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CMainFrame::CloseSocket(CONNID dwConnID)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CClientAppView* pView = (CClientAppView*)pMain->GetActiveView();

	pView->DeleteItem(dwConnID);

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CMainFrame::AcceptClient(CONNID dwConnID, SOCKET soClient)
{
	CString strWellcom;
	
	MSG_HEAD Msg;
	Msg.dwMsgId = CORPSE_SHOW_CARD;
	Msg.bRet = TRUE;
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMain->GetServer().Send(dwConnID,&Msg,sizeof(Msg));
	
	return ISocketListener::HR_OK;
}

CServerSocket& CMainFrame::GetServer()
{
	return m_Server;
}

void CMainFrame::OnClose()
{
	m_Server.Stop();
	CFrameWnd::OnClose();
}

void CMainFrame::InsertItem(DWORD dwConnID,CMsgHead& Msg)
{
	CClientAppView* pView = (CClientAppView*)GetActiveView();
	TCHAR szClientAddress[20];
	WORD wPort;
	CString strClientAddress;
	int nSize = sizeof(szClientAddress);
	if (m_Server.GetClientAddress(dwConnID,szClientAddress,nSize,wPort) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("获取客户端IP地址失败"));
		return;
	}

	strClientAddress.Format(_T("%s:%d"),szClientAddress,wPort);
	pView->InsertIntoList(dwConnID,strClientAddress,Msg.GetMsgValue());
}