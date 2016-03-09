
// CreateMultiThreadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CreateMultiThread.h"
#include "CreateMultiThreadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CCreateMultiThreadDlg 对话框




CCreateMultiThreadDlg::CCreateMultiThreadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateMultiThreadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCreateMultiThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreateMultiThreadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCreateMultiThreadDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateMultiThreadDlg 消息处理程序

BOOL CCreateMultiThreadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: 在此添加额外的初始化代码
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
	{
		MessageBox(_T("初始化套接字失败"),_T("错误"),MB_OK);
		return FALSE;
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCreateMultiThreadDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCreateMultiThreadDlg::OnPaint()
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
HCURSOR CCreateMultiThreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCreateMultiThreadDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//
	CString strTestAddr,strTestCount;
	GetDlgItem(IDC_EDIT_TEST_ADDR)->GetWindowText(strTestAddr);
	GetDlgItem(IDC_EDIT_TEST_COUNT)->GetWindowText(strTestCount);

	if (strTestAddr.IsEmpty() || strTestCount.IsEmpty())
	{
		MessageBox(_T("请输入测试内容"),_T("提示"),MB_OK);
		return;
	}

	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_AUTO_CLOSE);
	BOOL bAutoClosed = pCheckBox->GetCheck() == BST_CHECKED;
	 
	for (int n=0;n<_ttoi(strTestCount);++n)
	{
		SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (s == INVALID_SOCKET)
		{
			OutputDebugString(_T("创建套接字失败！！！.\r\n"));
			continue;
		}

		sockaddr_in clientService; 
		clientService.sin_family = AF_INET;
		CStringA strRemoteAddr(strTestAddr);
		clientService.sin_addr.s_addr = inet_addr(strRemoteAddr);
		
		clientService.sin_port = htons( 80 );

		if ( connect( s, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) 
		{
			OutputDebugString(_T("连接失败！！！.\r\n"));
			continue;
		}
		
		CString strTipInfo;
		strTipInfo.Format(_T("序号%d操作成功！！.\r\n"),n);
		OutputDebugString(strTipInfo);

		if (bAutoClosed)
			closesocket(s);
	}

	MessageBox(_T("工作完成啦"),_T("提示"),MB_OK);
}

