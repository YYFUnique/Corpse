
// SendArpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SendArp.h"
#include "SendArpDlg.h"

#include "LsCommon/ErrorInfo.h"
#include "LsProtoIo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSendArpDlg 对话框

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


// CSendArpDlg 消息处理程序

BOOL CSendArpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString strSysFileName(_T("\\\\.\\LsProto"));
	HANDLE hFile = CreateFile(strSysFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("没有找到设备%s"),strSysFileName);
		MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK);
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
		SetErrorInfo(SYSTEM_ERROR,0,_T("发送数据失败"));
		MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK);
		return FALSE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSendArpDlg::OnPaint()
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
HCURSOR CSendArpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CSendArpDlg::OnSendArp(WPARAM wParam,LPARAM lParam)
 {

	 return TRUE;
 }