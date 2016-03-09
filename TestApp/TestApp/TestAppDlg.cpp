// TestAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestApp.h"
#include "TestAppDlg.h"

#include <stdio.h>
#include <FltUser.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "fltLib.lib")
#pragma comment(lib, "fltMgr.lib")
#pragma comment(lib, "ntoskrnl.lib")
#pragma comment(lib, "hal.lib")

HANDLE g_hPort;
HANDLE g_hEvent;
#define		EVENT_NAME				L"MzfFileMonEvent"
#define		NPMINI_PORT_NAME       L"\\NPMiniPort"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT __cdecl MyControllingFunction( LPVOID pParam )
{
	CTestAppDlg *pDlg = (CTestAppDlg*)pParam;
	static int i = 0;

	while (::WaitForSingleObject(g_hEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DWORD bytesReturned = 0;
		DWORD hResult;
		TCHAR buf[260]={0};

		hResult = FilterSendMessage(
			g_hPort,
			L"Test",
			8,
			buf,
			260,
			&bytesReturned );

		if (hResult != S_OK) {
			AfxMessageBox(_T("接收数据失败!"));
			return 0;
		}		
		
		for (CHAR c = 'A'; c<= 'Z'; c++)
		{
			CString str;
			char temp[0x200] = {0};

			str.Format("%c:", c);
			QueryDosDevice(str, temp, 0x200);

			if (!strncmp(temp, buf, 23))
			{	
				CString strPath, strNum, strTime;
				COleDateTime time ;

				strNum.Format("%d", i+1);

				time = COleDateTime::GetCurrentTime();
				strTime = time.Format("%Y/%m/%d %H:%M:%S");

				strPath.Format("%c:%s", c, buf+23);

				pDlg->m_list.InsertItem(i, strNum);
				pDlg->m_list.SetItemText(i, 1, strTime);
				pDlg->m_list.SetItemText(i, 2, strPath);

				i++;
				break;
			}
		}	
	}
	return 0;
}
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CTestAppDlg 对话框




CTestAppDlg::CTestAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CTestAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_LINK, &CTestAppDlg::OnStnClickedLink)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CTestAppDlg 消息处理程序

BOOL CTestAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// TODO: 在此添加额外的初始化代码
	m_list.InsertColumn(0, _T("计数"));
	m_list.InsertColumn(1, _T("时间"));
	m_list.InsertColumn(2, _T("路径"));

	m_list.SetColumnWidth(0, 50);
	m_list.SetColumnWidth(1, 150);
	m_list.SetColumnWidth(2, 550);

	GetDlgItem(IDC_LINK)->GetWindowRect(&m_pRectLink);
	ScreenToClient(&m_pRectLink);

	g_hEvent = ::OpenEvent(SYNCHRONIZE, FALSE, _T("MzfFileMonEvent"));
	DWORD hResult = FilterConnectCommunicationPort( 
		NPMINI_PORT_NAME, 
		0, 
		NULL, 
		0, 
		NULL, 
		&g_hPort );

	if (hResult != S_OK) {
		MessageBox( _T("连接驱动失败~"));
		return FALSE;
	}
//	MessageBox(L"连接驱动成功~");

	AfxBeginThread(MyControllingFunction, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestAppDlg::OnPaint()
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
HCURSOR CTestAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestAppDlg::OnStnClickedLink()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute( NULL, NULL, _T("http://www.csdn.net"), NULL, NULL, SW_NORMAL );
}

void CTestAppDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(point.x >m_pRectLink.left && point.x < m_pRectLink.right && point.y > m_pRectLink.top && point.y < m_pRectLink.bottom )
	{
		HCURSOR hCursor;
		hCursor = ::LoadCursor(NULL, IDC_HAND );
		SetCursor( hCursor );
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CTestAppDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(point.x >m_pRectLink.left && point.x < m_pRectLink.right && point.y > m_pRectLink.top && point.y < m_pRectLink.bottom )
	{
		if( nFlags == MK_LBUTTON )
		ShellExecute( NULL, NULL, _T("http://hi.baidu.com/hu3167343"), NULL, NULL, SW_NORMAL );
	}

	CDialog::OnLButtonDown(nFlags, point);
}
