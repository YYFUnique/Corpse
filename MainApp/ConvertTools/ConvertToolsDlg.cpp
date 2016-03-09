
// ConvertToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ConvertTools.h"
#include "ConvertToolsDlg.h"

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


// CConvertToolsDlg 对话框




CConvertToolsDlg::CConvertToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertToolsDlg::IDD, pParent)
{
	for (int i=0;i<TAB_COUNT;++i)
		m_pToolDlg[i] = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CConvertToolsDlg::~CConvertToolsDlg()
{
	for (int i=0;i<TAB_COUNT;++i)
	{
		if (m_pToolDlg[i] != NULL)
		{
			delete m_pToolDlg[i];
			m_pToolDlg[i] = NULL;
			continue;
		}

		break;
	}
}

void CConvertToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONVERT_TOOLS, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CConvertToolsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONVERT_TOOLS, &CConvertToolsDlg::OnTcnSelchangeTabCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CConvertToolsDlg 消息处理程序

BOOL CConvertToolsDlg::OnInitDialog()
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
	m_pToolDlg[0] = new CTimeDlg(&m_TabCtrl);
	CreateTabSel(m_pToolDlg[0],IDD_TIME_DIALOG);
	m_pToolDlg[1] = new CIpAddressDlg(&m_TabCtrl);
	CreateTabSel(m_pToolDlg[1],IDD_IPADDRESS_DIALOG);

	for (UINT nIndex = 0; nIndex<TAB_COUNT && m_pToolDlg[nIndex]; ++nIndex)
	{
		CString strTabTitle;
		m_pToolDlg[nIndex]->GetWindowText(strTabTitle);
		m_TabCtrl.InsertItem(nIndex,strTabTitle);
	}

	ShowSelectSel(0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CConvertToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CConvertToolsDlg::OnPaint()
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
HCURSOR CConvertToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConvertToolsDlg::ShowSelectSel(UINT uSelectedSel)
{
	UINT nTotal = m_TabCtrl.GetItemCount();
	for (UINT nIndex=0; nIndex<nTotal; ++nIndex)
		if (nIndex == uSelectedSel)
			m_pToolDlg[nIndex]->ShowWindow(SW_SHOW);
		else if (m_pToolDlg[nIndex]&&IsWindow(m_pToolDlg[nIndex]->m_hWnd))
			m_pToolDlg[nIndex]->ShowWindow(SW_HIDE);
}


BOOL CConvertToolsDlg::CreateTabSel(CDialog* dlg,UINT nIDTemplate)
{
	ASSERT(nIDTemplate);
	CRect rcTab;
	m_TabCtrl.GetClientRect(rcTab);
	rcTab.top+=35;
	rcTab.left+=14;
	rcTab.right+=7;
	rcTab.bottom+=8;
	dlg->Create(nIDTemplate);
	dlg->MoveWindow(rcTab);
	dlg->ShowWindow(SW_HIDE);
	return TRUE;
}

void CConvertToolsDlg::OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_TabCtrl.GetCurSel();
	ShowSelectSel(nIndex);
	*pResult = 0;
}