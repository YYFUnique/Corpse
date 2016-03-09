
// TextConvertDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TextConvert.h"
#include "TextConvertDlg.h"
#include "LsCommon/TextTools.h"

#include <Wincrypt.h>
#pragma comment(lib,"Crypt32.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTextConvertDlg 对话框




CTextConvertDlg::CTextConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextConvertDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTextConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTextConvertDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_TEXT_CONTENT, &CTextConvertDlg::OnEnChangeEditTextContent)
	ON_COMMAND_RANGE(IDC_RADIO_LOWER_LITTER,IDC_RADIO_UPPER_LITTER,&CTextConvertDlg::OnRadioClick)
END_MESSAGE_MAP()


// CTextConvertDlg 消息处理程序

BOOL CTextConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CFont Font,*pFont = GetFont();
	LOGFONT LogFont;
	pFont->GetLogFont(&LogFont);
	_tcscpy_s(LogFont.lfFaceName,_countof(LogFont.lfFaceName),_T("宋体"));
	Font.CreateFontIndirect(&LogFont);
	SetFont(&Font,TRUE);

	((CButton*)GetDlgItem(IDC_RADIO_LOWER_LITTER))->SetCheck(TRUE);

	GetDlgItem(IDC_EDIT_TEXT_CONTENT)->SetFocus();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTextConvertDlg::OnPaint()
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
HCURSOR CTextConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTextConvertDlg::OnEnChangeEditTextContent()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	ShowAnsiText();
	ShowUnicodeText();
	ShowUtf8Text();
}


void CTextConvertDlg::OnRadioClick(UINT nID)
{	
	CButton* pRadioButton = (CButton*)GetDlgItem(nID);
	CEdit* pEditTextCode = (CEdit*)GetDlgItem(IDC_EDIT_TEXT_CONTENT);
	//pEditTextCode->
}

BOOL CTextConvertDlg::ShowAnsiText()
{
	CString strAnsiText;
	CString strAnsiCode;
	CString strTextCode;
	GetDlgItem(IDC_EDIT_TEXT_CONTENT)->GetWindowText(strTextCode);

#ifdef _UNICODE
	//CStringW strUnicode();
	CStringA strAnsiTextTmp = UnicodeToAnsi((LPWSTR)(LPCWSTR)(LPCTSTR)strTextCode);
	PBYTE bCode = (PBYTE)(LPCSTR)strAnsiTextTmp;
	CHAR szData[1024*4];
	DWORD dwSize = sizeof(szData);
	CryptBinaryToStringA(bCode,strAnsiTextTmp.GetLength(),CRYPT_STRING_HEXASCII,szData,&dwSize);
#else
	//strAnsiText = strTextCode;
#endif

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ANSI_CODE);
	::SetWindowTextA(pEdit->m_hWnd,szData);
	return TRUE;
}

BOOL CTextConvertDlg::ShowUnicodeText()
{
	CStringW strUnicodeText;
	CStringW strUnicodeCode;
	CStringArray strUnicodeTextArr;
	CString strTextCode;
	GetDlgItem(IDC_EDIT_TEXT_CONTENT)->GetWindowText(strTextCode);

#ifdef _UNICODE	
	PBYTE bCode = (PBYTE)(LPCTSTR)strTextCode;
	DWORD dwCount = strTextCode.GetLength()*sizeof(TCHAR);
	TCHAR szData[1024*4];
	DWORD dwSize = sizeof(szData);
	CryptBinaryToString(bCode,strTextCode.GetLength()*sizeof(TCHAR),CRYPT_STRING_HEXASCII,szData,&dwSize);
#else
	strUnicodeText = strTextCode;
#endif

	GetDlgItem(IDC_EDIT_UNICODE_CODE)->SetWindowText(szData);
	return TRUE;
}

BOOL CTextConvertDlg::ShowUtf8Text()
{
	CStringA strUtf8Text;
	CStringA strUtf8Code;
	CString strTextCode;
	GetDlgItem(IDC_EDIT_TEXT_CONTENT)->GetWindowText(strTextCode);

#ifdef _UNICODE	
	strUtf8Text = UnicodeToUTF8((LPWSTR)(LPCWSTR)(LPCTSTR)strTextCode);
	PBYTE bCode = (PBYTE)(LPCSTR)strUtf8Text;
	TCHAR szData[1024*4];
	DWORD dwSize = sizeof(szData);
	CryptBinaryToString(bCode,strUtf8Text.GetLength(),CRYPT_STRING_HEXASCII,szData,&dwSize);
	
#else
	strUtf8Text = strTextCode;
#endif

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_UTF8_CODE);
	pEdit->SetWindowText(szData);

	return TRUE;
}