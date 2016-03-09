
// CreateMacMapDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CreateMacMap.h"
#include "CreateMacMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_LOAD_MACADDRESS				(WM_USER + 0x1000)
// CCreateMacMapDlg �Ի���


CCreateMacMapDlg::CCreateMacMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateMacMapDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	GetModuleFileName(NULL,m_MacAddressFilePath,_countof(m_MacAddressFilePath));
	PathRemoveFileSpec(m_MacAddressFilePath);
	PathAppend(m_MacAddressFilePath,_T("MacAddress.ini"));
	if (PathFileExists(m_MacAddressFilePath) == FALSE)
	{
		HANDLE hFile =	CreateFile(m_MacAddressFilePath,GENERIC_ALL,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}
}

void CCreateMacMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreateMacMapDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BTN_ADD, &CCreateMacMapDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDCANCEL, &CCreateMacMapDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_BTN_SAVE, &CCreateMacMapDlg::OnBnClickedBtnSave)
	ON_MESSAGE(WM_LOAD_MACADDRESS,&CCreateMacMapDlg::OnLoadMacAddress)
END_MESSAGE_MAP()


// CCreateMacMapDlg ��Ϣ�������

BOOL CCreateMacMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	PostMessage(WM_LOAD_MACADDRESS,0,0);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCreateMacMapDlg::OnPaint()
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
HCURSOR CCreateMacMapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCreateMacMapDlg::OnBnClickedBtnAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strMacAddress,strMacOwner;
	GetDlgItem(IDC_EDIT_MAC_ADDRESS)->GetWindowText(strMacAddress);
	GetDlgItem(IDC_EDIT_MAC_NAME)->GetWindowText(strMacOwner);
	if (strMacAddress.IsEmpty() || strMacOwner.IsEmpty())
	{
		MessageBox(_T("������MAC��ַ������MAC����"),_T("��ʾ"),MB_OK);
		return;
	}

	strMacAddress.Replace(_T("-"),_T(""));
	strMacAddress.Replace(_T(":"),_T(""));

	DWORD dwMac;
	CString strMac = strMacAddress.Left(6);
	_stscanf_s(strMac,_T("%x"),&dwMac);
	m_MacAddressMap.SetAt(dwMac,strMacOwner);
}

void CCreateMacMapDlg::OnClose()
{
	OnBnClickedBtnSave();
	CDialog::OnClose();
}
void CCreateMacMapDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnClose();
	OnCancel();
}

void CCreateMacMapDlg::OnBnClickedBtnSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMacAddressMap::CPair* pProcessInfo = m_MacAddressMap.PGetFirstAssoc();
	CString strMacKey;
	while(pProcessInfo != NULL)
	{
		strMacKey.Format(_T("%06X"),pProcessInfo->key);
		WritePrivateProfileString(strMacKey,_T("Name"),pProcessInfo->value,m_MacAddressFilePath);
		pProcessInfo = m_MacAddressMap.PGetNextAssoc(pProcessInfo);
	}
}


LRESULT CCreateMacMapDlg::OnLoadMacAddress(WPARAM wParam,LPARAM lParam)
{
	int n=2*1024*1024;
	LPTSTR lpszDataBuffer = new TCHAR[n];
	if (lpszDataBuffer == NULL)
		PostQuitMessage(0);

	GetPrivateProfileString(NULL,NULL,NULL,lpszDataBuffer,n,m_MacAddressFilePath);
	LPTSTR lpszStringPointer = lpszDataBuffer;
	TCHAR szMacName[MAX_PATH];
	DWORD dwMac;
	while(lpszStringPointer[0] != NULL)
	{
		GetPrivateProfileString(lpszStringPointer,_T("Name"),NULL,szMacName,_countof(szMacName),m_MacAddressFilePath);
		_stscanf_s(lpszStringPointer,_T("%x"),&dwMac);
		m_MacAddressMap.SetAt(dwMac,szMacName);
		lpszStringPointer += _tcslen(lpszStringPointer) + 1;
	}

	delete[] lpszDataBuffer;
	return TRUE;
}