
// TestSendRecvByteDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestSendRecvByte.h"
#include "TestSendRecvByteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestSendRecvByteDlg �Ի���




CTestSendRecvByteDlg::CTestSendRecvByteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestSendRecvByteDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestSendRecvByteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestSendRecvByteDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CTestSendRecvByteDlg ��Ϣ�������

BOOL CTestSendRecvByteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	MIB_IFTABLE* pIfTable = (MIB_IFTABLE *) new MIB_IFTABLE;

	if (GetIfEntry(&pIfTable) != FALSE)
		return TRUE;
		

	if (pIfTable != NULL)
	{
		delete pIfTable;
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestSendRecvByteDlg::OnPaint()
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
HCURSOR CTestSendRecvByteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

