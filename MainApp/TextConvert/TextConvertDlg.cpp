
// TextConvertDlg.cpp : ʵ���ļ�
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


// CTextConvertDlg �Ի���




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


// CTextConvertDlg ��Ϣ�������

BOOL CTextConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CFont Font,*pFont = GetFont();
	LOGFONT LogFont;
	pFont->GetLogFont(&LogFont);
	_tcscpy_s(LogFont.lfFaceName,_countof(LogFont.lfFaceName),_T("����"));
	Font.CreateFontIndirect(&LogFont);
	SetFont(&Font,TRUE);

	((CButton*)GetDlgItem(IDC_RADIO_LOWER_LITTER))->SetCheck(TRUE);

	GetDlgItem(IDC_EDIT_TEXT_CONTENT)->SetFocus();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTextConvertDlg::OnPaint()
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
HCURSOR CTextConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTextConvertDlg::OnEnChangeEditTextContent()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

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