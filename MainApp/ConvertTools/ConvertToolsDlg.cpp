
// ConvertToolsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ConvertTools.h"
#include "ConvertToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

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


// CConvertToolsDlg �Ի���




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


// CConvertToolsDlg ��Ϣ�������

BOOL CConvertToolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CConvertToolsDlg::OnPaint()
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nIndex = m_TabCtrl.GetCurSel();
	ShowSelectSel(nIndex);
	*pResult = 0;
}