
// SendArpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SendArp.h"
#include "SendArpDlg.h"

#include "LsCommon/ErrorInfo.h"
#include "LsProtoIo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSendArpDlg �Ի���

#define	WM_SEND_ARP   (WM_USER + 0x100)

#pragma  pack (push,1) 
typedef struct _tgaARP 
{
	BYTE	 TargetMac[6];
	BYTE	 OwnMac[6];
	WORD  wType;
	WORD	 wHardType;
	WORD  wProcType;
	BYTE     HardSize;
	BYTE	 ProcSize;
	WORD	 wCodeOperate;
	BYTE     OwnMac2[6];
	DWORD  dwOwnIp;
	BYTE   TargetMac2[6];
	DWORD TargetIp;
	BYTE bR[18];
}ARP,*PARP;

#pragma pack(pop)

CSendArpDlg::CSendArpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendArpDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSendArpDlg::~CSendArpDlg()
{
	ReleaseProcessErrorInfo();
}

void CSendArpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSendArpDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SEND_ARP,&CSendArpDlg::OnSendArp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSendArpDlg ��Ϣ�������

BOOL CSendArpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CString strSysFileName(_T("\\\\.\\LsProto"));
	HANDLE hFile = CreateFile(strSysFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("û���ҵ��豸%s"),strSysFileName);
		MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK);
		return FALSE;
	}
	//SendMessage(WM_SEND_ARP,0,0);
	ARP arp;
	DWORD dwSizeArpPacket = sizeof(arp);
	memset(&arp,0,dwSizeArpPacket);
	PARP pSendArp = &arp;
	BYTE TargetMac[] ={0x00,0x24,0x21,0xB8,0x54,0xA9};
	BYTE OwnMac[] = {0x00,0x24,0x21,0xB8,0x54,0xA5};
	memcpy(arp.TargetMac,TargetMac,_countof(TargetMac));
	memcpy(arp.OwnMac,OwnMac,_countof(OwnMac));
	arp.wType = htons(0x0806);
	arp.wHardType = htons(0x01);
	arp.wProcType = htons(0x800);
	arp.HardSize = 6;
	arp.ProcSize = 4;
	arp.wCodeOperate = htons(0x2);
	memcpy(arp.OwnMac2,arp.OwnMac,_countof(arp.OwnMac));
	arp.dwOwnIp = inet_addr("10.1.1.254");
	memcpy(arp.TargetMac2,arp.TargetMac,_countof(arp.TargetMac));
	arp.TargetIp = inet_addr("10.1.1.164");
	
	DWORD dwSendByte;
	BOOL bRet = DeviceIoControl(hFile,IOCTL_NDISPROT_SEND_RAW_PACKET,(LPBYTE)&arp,dwSizeArpPacket,NULL,NULL,&dwSendByte,NULL);
	CloseHandle(hFile);
	if (bRet == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("��������ʧ��"));
		MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK);
		return FALSE;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSendArpDlg::OnPaint()
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
HCURSOR CSendArpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CSendArpDlg::OnSendArp(WPARAM wParam,LPARAM lParam)
 {

	 return TRUE;
 }