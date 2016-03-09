
// CrackUrlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CrackUrl.h"
#include "CrackUrlDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCrackUrlDlg �Ի���




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


// CCrackUrlDlg ��Ϣ�������

BOOL CCrackUrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCrackUrlDlg::OnPaint()
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
HCURSOR CCrackUrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCrackUrlDlg::OnBnClickedBtnPrase()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		MessageBox(_T("������ʧ��"),_T("��ʾ"),MB_OK);
		return;
	}

	BYTE bytePacketItemHttpInfo[1024*10];
	PPACKET_ITEM_HTTP_INFO pPacketItemHttpInfo = (PPACKET_ITEM_HTTP_INFO)bytePacketItemHttpInfo;
	DWORD dwSize = sizeof(PACKET_ITEM_HTTP_INFO);
	if (DeviceIoControl(hFile,KPH_WHITE_URL_ACC,(LPVOID)(LPCTSTR)strUrl,strUrl.GetLength()*sizeof(TCHAR),
				pPacketItemHttpInfo,sizeof(PACKET_ITEM_HTTP_INFO),&dwSize,NULL) == FALSE)
	{
		MessageBox(_T("����URLʧ��"),_T("��ʾ"),MB_OK);

	}
	CloseHandle(hFile);
/*
	CString strTipInfo;
	strTipInfo.Format(_T("������:%s,����:%d.\r\nҳ   ��:%s,����:%d."),
		CString(PacketItemHttpInfo.lpszHostName,PacketItemHttpInfo.dwHostNameLen),PacketItemHttpInfo.dwHostNameLen,
		CString(PacketItemHttpInfo.lpszUrlPath,PacketItemHttpInfo.dwUrlPathLen),PacketItemHttpInfo.dwUrlPathLen);


	MessageBox(strTipInfo,_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
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
		//ȥ�������Ͷ˿ڲ��֣�ȥ��б��'/',ʣ�µľ���ҳ�泤��
		dwPageLen = dwUrlLength - dwHostNameLen - 1;
	}
	else
	{
		lpszHostName = lpszCrackUrl;
		dwHostNameLen = dwUrlLength;

		lpszPage = NULL;
		dwPageLen = 0;
	}

	//���������ֲ����Ƿ���ڶ˿���Ϣ������������Ƴ��˿���Ϣ
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