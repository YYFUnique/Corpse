// AddIniRuleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CopyFile.h"
#include "AddIniRuleDlg.h"
#include <SetupAPI.h>
#include <WinDef.h>

// CAddIniRuleDlg �Ի���

IMPLEMENT_DYNAMIC(CAddIniRuleDlg, CDialog)

CAddIniRuleDlg::CAddIniRuleDlg(CString& strTargetFilePath , CWnd* pParent /*=NULL*/)
	: CDialog(CAddIniRuleDlg::IDD, pParent)
	, m_strTargetFilePath(strTargetFilePath)
{
	ChangeModifyStatus();
}

CAddIniRuleDlg::~CAddIniRuleDlg()
{
}

void CAddIniRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_ADD_FILE, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CAddIniRuleDlg, CDialog)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_MODIFIED_NOT_SAVE , &CAddIniRuleDlg::OnModifyNotSave)

	ON_BN_CLICKED(ID_SAVE_TO_INI, &CAddIniRuleDlg::OnBnClickedSaveToIni)
	ON_BN_CLICKED(IDC_BTN_LOOKUP, &CAddIniRuleDlg::OnBnClickedBtnLookup)

ON_BN_CLICKED(ID_SAVE_TO_INI_UPDATE, &CAddIniRuleDlg::OnBnClickedSaveToIniUpdate)
END_MESSAGE_MAP()


// CAddIniRuleDlg ��Ϣ�������

BOOL CAddIniRuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

//���浽�����ļ�
void CAddIniRuleDlg::OnBnClickedSaveToIni()
{
	TCHAR szCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH , szCurrentDir);

	CString strTargetFilePath;
	GetDlgItemText(IDC_EDIT_TARGET_FILEPATH , strTargetFilePath);
	if (strTargetFilePath.IsEmpty())
	{
		MessageBox(_T("Ŀ��Ŀ¼����Ϊ��"),_T("����"),MB_OK|MB_ICONERROR);
		return;
	}

	TCHAR szOutFilePath[MAX_PATH] = {0};
	PathRelativePathTo(szOutFilePath , szCurrentDir , FILE_ATTRIBUTE_DIRECTORY , strTargetFilePath , FILE_ATTRIBUTE_DIRECTORY);

	TCHAR szIniFilePath[MAX_PATH];
	GetModuleFileName(NULL , szIniFilePath , sizeof(szIniFilePath));
	PathRenameExtension(szIniFilePath , _T(".ini"));

	if (PathFileExists(szIniFilePath) ==FALSE )
	{
		HANDLE hFile = CreateFile(szIniFilePath , GENERIC_ALL , FILE_SHARE_READ , NULL , OPEN_ALWAYS , 0 , 0);
		CloseHandle(hFile);
	}

	if (szOutFilePath[0] == NULL)
		_tcscpy_s(szOutFilePath , MAX_PATH , strTargetFilePath);

	WritePrivateProfileString(_T("BaseInfo"),_T("DesPath"),szOutFilePath,szIniFilePath);		//����Ŀ��Ŀ¼λ��
	TCHAR szIniSection[1024];
	GetPrivateProfileString(NULL,NULL,NULL,szIniSection,1024,szIniFilePath);

	/*TCHAR* pszIniSection = szIniSection;

	while(pszIniSection[0] != NULL)
	{
		WritePrivateProfileString(pszIniSection,_T("DesPath"),szOutFilePath,szIniFilePath);
		pszIniSection = pszIniSection + _tcslen(pszIniSection) +1;
	}
	*/
	m_strTargetFilePath = szOutFilePath;
	EndDialog(IDOK);
}

//��������ť����ѡ���ļ��жԻ���
void CAddIniRuleDlg::OnBnClickedBtnLookup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPITEMIDLIST pDlist;
	BROWSEINFO BrowseInfo;

	ZeroMemory(&BrowseInfo, sizeof(BrowseInfo));
	BrowseInfo.hwndOwner = m_hWnd;
	BrowseInfo.lpszTitle = _T("��ѡ��·��: ");
	BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;    


	pDlist = SHBrowseForFolder(&BrowseInfo);
	if(pDlist)
	{
		TCHAR szFolderPath[MAX_PATH];
		SHGetPathFromIDList(pDlist , szFolderPath);
		SetDlgItemText(IDC_EDIT_TARGET_FILEPATH , szFolderPath);
		ChangeModifyStatus(MODIFY_NOT_SAVE);
	}
}

void CAddIniRuleDlg::OnOK()
{

}

//������沢���°�ť
void CAddIniRuleDlg::OnBnClickedSaveToIniUpdate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedSaveToIni();
	
}

void CAddIniRuleDlg::OnClose()
{
	if(m_ModifySave == MODIFY_NOT_SAVE)
	{
		int iRet = AfxMessageBox(_T("�Ƿ��޸ı��浽�����ļ���"),MB_ICONQUESTION|MB_YESNOCANCEL);
		if(iRet == IDYES)
			OnBnClickedSaveToIni();
		else if (iRet == IDCANCEL)
			return;
	}
	CWnd::OnClose();
}

void CAddIniRuleDlg::ChangeModifyStatus(MODIFY_SAVE MDStatus/*=NOT_MODIFY*/)
{
	m_ModifySave = MDStatus;
}

//
LRESULT CAddIniRuleDlg::OnModifyNotSave(WPARAM wParam , LPARAM lParam)
{
	ChangeModifyStatus(MODIFY_NOT_SAVE);
	return TRUE;
}