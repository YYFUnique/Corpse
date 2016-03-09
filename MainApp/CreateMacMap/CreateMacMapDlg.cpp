
// CreateMacMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CreateMacMap.h"
#include "CreateMacMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_LOAD_MACADDRESS				(WM_USER + 0x1000)
// CCreateMacMapDlg 对话框


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


// CCreateMacMapDlg 消息处理程序

BOOL CCreateMacMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	PostMessage(WM_LOAD_MACADDRESS,0,0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCreateMacMapDlg::OnPaint()
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
HCURSOR CCreateMacMapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCreateMacMapDlg::OnBnClickedBtnAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strMacAddress,strMacOwner;
	GetDlgItem(IDC_EDIT_MAC_ADDRESS)->GetWindowText(strMacAddress);
	GetDlgItem(IDC_EDIT_MAC_NAME)->GetWindowText(strMacOwner);
	if (strMacAddress.IsEmpty() || strMacOwner.IsEmpty())
	{
		MessageBox(_T("请输入MAC地址或者是MAC名称"),_T("提示"),MB_OK);
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
	// TODO: 在此添加控件通知处理程序代码
	OnClose();
	OnCancel();
}

void CCreateMacMapDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
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