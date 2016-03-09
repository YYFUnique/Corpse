// SystemSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FindWindow.h"
#include "SystemSet.h"
#include "FindWindowDlg.h"
#include "IniFile.h"
// CSystemSet �Ի���

IMPLEMENT_DYNAMIC(CSystemSet, CDialog)

CSystemSet::CSystemSet(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemSet::IDD, pParent)
{
	
}

CSystemSet::~CSystemSet()
{
}

void CSystemSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSystemSet, CDialog)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CSystemSet::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CSystemSet::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CSystemSet ��Ϣ�������

BOOL CSystemSet::OnInitDialog()
{
	TCHAR szShowWindowBuff[MAX_TITLE_LEN];
	CIniFile IniFile;
	IniFile.GetData(_T("SystemSet") , _T("ShowWindowOnStartUp") , sizeof(szShowWindowBuff) , szShowWindowBuff );

	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_SHOWWINDOW);
	
	pCheckBox->SetCheck(_tcscmp(szShowWindowBuff , _T("Hide")) == FALSE );

	return TRUE;
}


void CSystemSet::OnBnClickedBtnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CSystemSet::OnBnClickedBtnSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szModuleFileName[MAX_PATH];
	GetModuleFileName(NULL , szModuleFileName , sizeof(szModuleFileName));

	PathRenameExtension(szModuleFileName,_T(".ini"));
	CFindWindowDlg* pFindWindowDlg = (CFindWindowDlg*)AfxGetApp()->m_pMainWnd;

	TCHAR szShowWindow[MAX_TITLE_LEN] = _T("SystemSet");
	//pFindWindowDlg->LoadStringById(IDS_SYSTEM_SECTION , szShowWindow , sizeof(szShowWindow));

	TCHAR szKeyShowWindow[MAX_PATH] = _T("ShowWindowOnStartUp");
	//pFindWindowDlg->LoadStringById(IDS_SYSTEM_KEY_SHOWWINDOW , szKeyShowWindow ,sizeof(szKeyShowWindow));

//	TCHAR szShowWindowDefault[MAX_TITLE_LEN];
	//pFindWindowDlg->LoadStringById(IDS_SYSTEM_SHOWWINDOW_DEFAULT , szShowWindowDefault , sizeof(szShowWindowDefault));

	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_SHOWWINDOW);

	CString strShowWindowValue;//
	strShowWindowValue.Format(_T("%s") , pCheckBox->GetCheck()?_T("Hide"):_T("Show"));
	WritePrivateProfileString(szShowWindow , szKeyShowWindow , strShowWindowValue , szModuleFileName);

	OnOK();
}
