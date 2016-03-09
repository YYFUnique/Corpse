
// CrackUrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrackUrl.h"
#include "CrackUrlDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCrackUrlDlg 对话框




CCrackUrlDlg::CCrackUrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrackUrlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCrackUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCrackUrlDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_PRASE, &CCrackUrlDlg::OnBnClickedBtnPrase)
END_MESSAGE_MAP()


// CCrackUrlDlg 消息处理程序

BOOL CCrackUrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCrackUrlDlg::OnPaint()
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
HCURSOR CCrackUrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCrackUrlDlg::OnBnClickedBtnPrase()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strUrl;
	GetDlgItem(IDC_EDIT_CRACK_URL)->GetWindowText(strUrl);
//	PACKET_ITEM_HTTP_INFO PacketItemHttpInfo;
	CString strCrackUrlFile;
	strCrackUrlFile.Format(_T("\\\\.\\%s"),_T("TestDriver"));
	HANDLE hFile = CreateFile(strCrackUrlFile,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int i = GetLastError();
		//SetErrorInfo(SYSTEM_ERROR,)
		MessageBox(_T("打开驱动失败"),_T("提示"),MB_OK);
		return;
	}

	BYTE bytePacketItemHttpInfo[1024*10];
	PPACKET_ITEM_HTTP_INFO pPacketItemHttpInfo = (PPACKET_ITEM_HTTP_INFO)bytePacketItemHttpInfo;
	DWORD dwSize = sizeof(PACKET_ITEM_HTTP_INFO);
	if (DeviceIoControl(hFile,KPH_WHITE_URL_ACC,(LPVOID)(LPCTSTR)strUrl,strUrl.GetLength()*sizeof(TCHAR),
				pPacketItemHttpInfo,sizeof(PACKET_ITEM_HTTP_INFO),&dwSize,NULL) == FALSE)
	{
		MessageBox(_T("解析URL失败"),_T("提示"),MB_OK);

	}
	CloseHandle(hFile);
/*
	CString strTipInfo;
	strTipInfo.Format(_T("主机名:%s,长度:%d.\r\n页   面:%s,长度:%d."),
		CString(PacketItemHttpInfo.lpszHostName,PacketItemHttpInfo.dwHostNameLen),PacketItemHttpInfo.dwHostNameLen,
		CString(PacketItemHttpInfo.lpszUrlPath,PacketItemHttpInfo.dwUrlPathLen),PacketItemHttpInfo.dwUrlPathLen);


	MessageBox(strTipInfo,_T("提示"),MB_OK|MB_ICONINFORMATION);
	*/
}


BOOL CCrackUrlDlg::LsCrackUrl(LPCTSTR lpszCrackUrl,DWORD dwUrlLength,PPACKET_ITEM_HTTP_INFO lpPacketItemHttpInfo)
{
	/*
	int nch = strlen("http://");
	if (_strnicmp(lpszCrackUrl,"http://",nch) !=0 )
		return FALSE;

	lpszCrackUrl += nch;
	dwUrlLength -= nch;
	
	LPCSTR lpRet = strchr(lpszCrackUrl,'/');
	LPCSTR lpszHostName,lpszPage;
	DWORD dwHostNameLen,dwPageLen;
	if (lpRet != NULL)
	{
		lpszHostName = lpszCrackUrl;
		dwHostNameLen = lpRet - lpszCrackUrl;
		
		lpszPage = lpRet + 1;
		//去掉域名和端口部分，去掉斜杠'/',剩下的就是页面长度
		dwPageLen = dwUrlLength - dwHostNameLen - 1;
	}
	else
	{
		lpszHostName = lpszCrackUrl;
		dwHostNameLen = dwUrlLength;

		lpszPage = NULL;
		dwPageLen = 0;
	}

	//在域名部分查找是否存在端口信息，如果存在则移除端口信息
	lpRet = strrchr(lpszHostName,':');
	if (lpRet != NULL)
	{
		lpszHostName = lpszCrackUrl;
		dwHostNameLen = lpRet - lpszCrackUrl - 1;
	}

	lpPacketItemHttpInfo->lpszHostName = lpszHostName;
	lpPacketItemHttpInfo->dwHostNameLen = dwHostNameLen;
	
	lpPacketItemHttpInfo->lpszUrlPath = lpszPage;
	lpPacketItemHttpInfo->dwUrlPathLen = dwPageLen;
*/
	return TRUE;
}