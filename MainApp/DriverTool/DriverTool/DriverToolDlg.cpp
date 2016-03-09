
// DriverToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DriverTool.h"
#include "DriverToolDlg.h"
#include "afxdialogex.h"


#include <afxdlgs.h>
#pragma comment(lib,"Advapi32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CDriverToolDlg �Ի���



CDriverToolDlg::CDriverToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriverToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriverToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDriverToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_INSTALL_GUIDE, &CDriverToolDlg::OnBnClickedBtnInstallGuide)
	ON_BN_CLICKED(IDC_BTN_UNINSTALL_GUIDE, &CDriverToolDlg::OnBnClickedBtnUninstallGuide)
END_MESSAGE_MAP()


// CDriverToolDlg ��Ϣ�������

BOOL CDriverToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDriverToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDriverToolDlg::OnPaint()
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
HCURSOR CDriverToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDriverToolDlg::OnBnClickedBtnInstallGuide()
{
	BOOL bSuccess = FALSE;
	do
	{
		// TODO: �ڴ���ӿؼ�֪ͨ����������
		CString strWinDDKInstallPath,strVCInstallPath;
		if (GetEnvironmentInstallPath(strWinDDKInstallPath,strVCInstallPath) == FALSE)
			break;

		//
		// ����ģ��Ŀ¼
		//
		CString strVCWizardPath,strVCProjectPath;
		strVCWizardPath.Format(_T("%s\\VCWizards\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCWizardPath))
			SHCreateDirectory(m_hWnd,strVCWizardPath);

		strVCProjectPath.Format(_T("%s\\vcprojects\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCProjectPath))
			SHCreateDirectory(m_hWnd,strVCProjectPath);

		//�����ļ�����ӦĿ¼
		LPCTSTR lpszProjectFileName[] = {_T("Driver.ico"),_T("Driver.vsz"),_T("Driver")};
		int i=0;
		TCHAR szModuleFileName[MAX_PATH];
		GetModuleFileName(NULL,szModuleFileName,_countof(szModuleFileName));
		PathRemoveFileSpec(szModuleFileName);
		for (i= 0; i < _countof(lpszProjectFileName); i++)
		{
			CString strExistFileName;
			CString strFileNewName;
			strExistFileName.Format(_T("%s\\%s"),szModuleFileName, lpszProjectFileName[i]);
			strFileNewName.Format(_T("%s\\%s"),strVCProjectPath.TrimRight(_T("\\")),lpszProjectFileName[i]);
			CopyFileToDestation(strExistFileName,strFileNewName);
		}
		if (i != _countof(lpszProjectFileName))
			break;

		LPCTSTR lpszWizardFileName[] = {_T("Scripts"),_T("Templates"),_T("Images"),_T("HTML"),_T("2052")};
		for (i = 0; i < _countof(lpszWizardFileName); i++)
		{
			CString strExistFileName;
			CString strFileNewName;
			strExistFileName.Format(_T("%s\\%s"),szModuleFileName,lpszWizardFileName[i]);
			strFileNewName.Format(_T("%s\\Driver\\%s"),strVCWizardPath.TrimRight(_T("\\")),lpszWizardFileName[i]);
			CopyFileToDestation(strExistFileName,strFileNewName);
		}

		if (i != _countof(lpszWizardFileName))
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		MessageBox(_T("��װ������ʧ��"),_T("��ʾ"),MB_OK);
	else
		MessageBox(_T("��װ�����򵼳ɹ���ллʹ��"),_T("��ʾ"),MB_OK);

	return;
}


void CDriverToolDlg::OnBnClickedBtnUninstallGuide()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL bSuccess = FALSE;
	do
	{
		// TODO: �ڴ���ӿؼ�֪ͨ����������
		CString strWinDDKInstallPath,strVCInstallPath;
		if (GetEnvironmentInstallPath(strWinDDKInstallPath,strVCInstallPath) == FALSE)
			break;

		//
		// ����ģ��Ŀ¼
		//
		CString strVCWizardPath,strVCProjectPath;
		strVCWizardPath.Format(_T("%s\\VCWizards\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCWizardPath))
			SHCreateDirectory(m_hWnd,strVCWizardPath);

		strVCProjectPath.Format(_T("%s\\vcprojects\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCProjectPath))
			SHCreateDirectory(m_hWnd,strVCProjectPath);

		//�����ļ�����ӦĿ¼
		LPCTSTR lpszProjectFileName[] = {_T("Driver.ico"),_T("Driver.vsz"),_T("Driver")};
		int i=0;
		TCHAR szModuleFileName[MAX_PATH];
		GetModuleFileName(NULL,szModuleFileName,_countof(szModuleFileName));
		PathRemoveFileSpec(szModuleFileName);
		for (i= 0; i < _countof(lpszProjectFileName); i++)
		{
			CString strExistFileName;
			CString strFileNewName;
			strExistFileName.Format(_T("%s\\%s"),szModuleFileName, lpszProjectFileName[i]);
			strFileNewName.Format(_T("%s\\%s"),strVCProjectPath.TrimRight(_T("\\")),lpszProjectFileName[i]);
			CopyFileToDestation(strExistFileName,strFileNewName);
		}
		if (i != _countof(lpszProjectFileName))
			break;

		LPCTSTR lpszWizardFileName[] = {_T("Scripts"),_T("Templates"),_T("Images"),_T("HTML"),_T("2052")};
		for (i = 0; i < _countof(lpszWizardFileName); i++)
		{
			CString strExistFileName;
			CString strFileNewName;
			strExistFileName.Format(_T("%s\\%s"),szModuleFileName,lpszWizardFileName[i]);
			strFileNewName.Format(_T("%s\\Driver\\%s"),strVCWizardPath.TrimRight(_T("\\")),lpszWizardFileName[i]);
			CopyFileToDestation(strExistFileName,strFileNewName);
		}

		if (i != _countof(lpszWizardFileName))
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		MessageBox(_T("ж��������ʧ��"),_T("��ʾ"),MB_OK);
	else
		MessageBox(_T("ж�������򵼳ɹ���ллʹ��"),_T("��ʾ"),MB_OK);

	return;
}

BOOL CDriverToolDlg::GetEnvironmentInstallPath(CString& strWinDDKInstallPath,CString& strVCInstallPath)
{
	if (GetWinDDKInstallPath(strWinDDKInstallPath) == FALSE)
		ShowSelectFolder(strWinDDKInstallPath);

	if (strWinDDKInstallPath.IsEmpty() != FALSE)
	{
		MessageBox(_T("û���ҵ�WinDDK�İ�װ·��"),_T("��ʾ"),MB_OK);
		return FALSE;
	}

	
	if (GetVCInstallPath(strVCInstallPath) == FALSE)
		ShowSelectFolder(strVCInstallPath);

	if (strVCInstallPath.IsEmpty() != FALSE)
	{
		MessageBox(_T("û���ҵ�VC�İ�װ·��"),_T("��ʾ"),MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CDriverToolDlg::GetWinDDKInstallPath(CString& strWinDDKInstallPath)
{
	BOOL bSuccess = FALSE;
	do
	{
		LPCTSTR lpszSubKey = _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
		TCHAR szDDKPath[MAX_PATH];
		DWORD dwSize = sizeof(szDDKPath);
		DWORD dwRet = RegGetValue(HKEY_LOCAL_MACHINE,lpszSubKey,_T("WinDDK"),RRF_RT_REG_SZ,NULL,szDDKPath,&dwSize);
		if (dwRet != ERROR_SUCCESS)
			break;

		strWinDDKInstallPath = szDDKPath;
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CDriverToolDlg::ShowSelectFolder(CString& strInstallPath)
{
	CFolderPickerDialog FolderDialog;
	if (FolderDialog.DoModal() == IDOK)
	{
		strInstallPath = FolderDialog.GetPathName();
	}
}

BOOL CDriverToolDlg::GetVCInstallPath(CString& strVCInstallPath)
{
	BOOL bSuccess = FALSE;
	do
	{
		LPCTSTR lpszSubKey = _T("SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VC");
		TCHAR szVCPath[MAX_PATH];
		DWORD dwSize = sizeof(szVCPath);
		DWORD dwRet = RegGetValue(HKEY_LOCAL_MACHINE,lpszSubKey,_T("ProductDir"),RRF_RT_REG_SZ,NULL,szVCPath,&dwSize);
		if (dwRet != ERROR_SUCCESS)
			break;

		strVCInstallPath = szVCPath;
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CDriverToolDlg::CopyFileToDestation(LPCTSTR lpszExistFileName,LPCTSTR lpszFileNewName)
{
	if (PathIsDirectory(lpszExistFileName) == FALSE)
		CopyFile(lpszExistFileName,lpszFileNewName,FALSE);
	else
		LookUpDirectory(lpszExistFileName,lpszFileNewName);

	return TRUE;
}

BOOL CDriverToolDlg::LookUpDirectory(LPCTSTR lpszExistFileName,LPCTSTR lpszFileNewName)
{
	WIN32_FIND_DATA FindData;
	CString strCurrentDirectory;
	SHCreateDirectory(m_hWnd,lpszFileNewName);
	strCurrentDirectory.Format(_T("%s\\*.*"),lpszExistFileName);
	HANDLE hFindFile = FindFirstFile(strCurrentDirectory,&FindData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{
		if (_tcsicmp(FindData.cFileName,_T(".")) == 0 || _tcsicmp(FindData.cFileName,_T("..")) == 0)
			continue;

		TCHAR szExistFileName[MAX_PATH],szFileNewName[MAX_PATH];
		PathCombine(szExistFileName,lpszExistFileName,FindData.cFileName);
		PathCombine(szFileNewName,lpszFileNewName,FindData.cFileName);

		if (PathIsDirectory(szExistFileName))
			LookUpDirectory(szExistFileName,szFileNewName);
		else
			CopyFile(szExistFileName,szFileNewName,FALSE);

	} while (FindNextFile(hFindFile,&FindData));

	FindClose(hFindFile);
	return TRUE;
}