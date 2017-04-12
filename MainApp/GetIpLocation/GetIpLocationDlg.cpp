
// GetIpLocationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GetIpLocation.h"
#include "GetIpLocationDlg.h"
#include "LsCommon/Class/Localtion.h"
#include "LsCommon/Json/JsonObject.h"
#include "LsCommon/FileTools.h"
#include "LsCommon/TextTools.h"
#include "LsCommon/ErrorInfo.h"

#include "libcurl/libcurl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CHTTPResponse : public ILibcurlCallback
{
public:
	int Progress(LPVOID lpData, size_t size, size_t nmemb);
};

int CHTTPResponse::Progress(LPVOID lpData, size_t size, size_t nmemb)
{
	CString strData((CHAR*)lpData,nmemb);
	CGetIpLocationDlg* pDlg = (CGetIpLocationDlg*)(AfxGetApp()->m_pMainWnd);

	pDlg->FillTheEditBox(strData);

	return size * nmemb;
}

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

// CGetIpLocationDlg 对话框

CGetIpLocationDlg::CGetIpLocationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetIpLocationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CGetIpLocationDlg::~CGetIpLocationDlg()
{
	ReleaseProcessErrorInfo();
}

void CGetIpLocationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGetIpLocationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_OK, &CGetIpLocationDlg::OnBnClickedBtnOk)
END_MESSAGE_MAP()

// CGetIpLocationDlg 消息处理程序

BOOL CGetIpLocationDlg::OnInitDialog()
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
	//GetDlgItem(IDC_EDIT_IP_ADDR)->SetWindowText(_T("http://ip.taobao.com/service/getIpInfo.php"));
	GetDlgItem(IDC_EDIT_IP_ADDR)->SetWindowText(_T("https://10.1.1.250"));
	GetDlgItem(IDC_EDIT_PRARM)->SetWindowText(_T("ip=61.139.2.69"));
	GetDlgItem(IDC_EDIT_IP_ADDR)->SetFocus();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGetIpLocationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGetIpLocationDlg::OnPaint()
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
HCURSOR CGetIpLocationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGetIpLocationDlg::OnBnClickedBtnOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strIPAddr;
	GetDlgItem(IDC_EDIT_IP_ADDR)->GetWindowText(strIPAddr);
	if (strIPAddr.IsEmpty() == TRUE)
	{
		GetDlgItem(IDC_EDIT_IP_ADDR)->SetFocus();
		MessageBox(_T("请输入网址!"),_T("提示"),MB_OK);
		return;
	}

	CLibcurl libcurl;
	CHTTPResponse HttpResponse;
	libcurl.SetCallback(&HttpResponse);
	CString strPrama;
	GetDlgItem(IDC_EDIT_PRARM)->GetWindowText(strPrama);
	if (libcurl.doHttpGet(strIPAddr,strPrama,3) != CURLE_OK)
	{
		CString strTipInfo;
		libcurl.GetErrorInfo(strTipInfo);
		MessageBox(strTipInfo,_T("提示"),MB_OK|MB_ICONINFORMATION);
	}
}
/*
int CGetIpLocationDlg::Progress(void* pData , size_t size , size_t nmemb)
{
	CString strData((CHAR*)pData,nmemb);
	CGetIpLocationDlg* pDlg = (CGetIpLocationDlg*)(AfxGetApp()->m_pMainWnd);
	
	pDlg->FillTheEditBox(strData);

	return size*nmemb;
}*/

BOOL CGetIpLocationDlg::FillTheEditBox(LPCTSTR lpszJson)
{
	CJsonObject Json;
	Json.FromString(lpszJson);
	CString strData;
	CJsonObject JsonData;
	if (Json.IsMember(_T("data")))
	{
		UINT uCode;
		Json.GetValue(_T("code"),&uCode);
		if (uCode != 0)
		{
			SetErrorInfo(CUSTOM_ERROR,0,Json.GetValue(_T("data")));
			MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK);
			return FALSE;
		}
		Json.GetValue(_T("data"),&JsonData);
	}
	else
		JsonData = Json;

	GetDlgItem(IDC_EDIT_COUNTRY)->SetWindowText(UTF8ToUniCode(JsonData.GetUTF8Value(_T("country"))));
	GetDlgItem(IDC_EDIT_AREA)->SetWindowText(UTF8ToUniCode(JsonData.GetUTF8Value(_T("area"))));
	GetDlgItem(IDC_EDIT_PROVINCE)->SetWindowText(UTF8ToUniCode(JsonData.GetUTF8Value(_T("region"))));
	GetDlgItem(IDC_EDIT_CITY)->SetWindowText(UTF8ToUniCode(JsonData.GetUTF8Value(_T("city"))));
	GetDlgItem(IDC_EDIT_ISP)->SetWindowText(UTF8ToUniCode(JsonData.GetUTF8Value(_T("isp"))));

	return TRUE;
}
