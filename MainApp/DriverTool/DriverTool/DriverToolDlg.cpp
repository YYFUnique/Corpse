
// DriverToolDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CDriverToolDlg 对话框



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


// CDriverToolDlg 消息处理程序

BOOL CDriverToolDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDriverToolDlg::OnPaint()
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
HCURSOR CDriverToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDriverToolDlg::OnBnClickedBtnInstallGuide()
{
	BOOL bSuccess = FALSE;
	do
	{
		// TODO: 在此添加控件通知处理程序代码
		CString strWinDDKInstallPath,strVCInstallPath;
		if (GetEnvironmentInstallPath(strWinDDKInstallPath,strVCInstallPath) == FALSE)
			break;

		//
		// 创建模版目录
		//
		CString strVCWizardPath,strVCProjectPath;
		strVCWizardPath.Format(_T("%s\\VCWizards\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCWizardPath))
			SHCreateDirectory(m_hWnd,strVCWizardPath);

		strVCProjectPath.Format(_T("%s\\vcprojects\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCProjectPath))
			SHCreateDirectory(m_hWnd,strVCProjectPath);

		//拷贝文件到对应目录
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
		MessageBox(_T("安装驱动向导失败"),_T("提示"),MB_OK);
	else
		MessageBox(_T("安装驱动向导成功，谢谢使用"),_T("提示"),MB_OK);

	return;
}


void CDriverToolDlg::OnBnClickedBtnUninstallGuide()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bSuccess = FALSE;
	do
	{
		// TODO: 在此添加控件通知处理程序代码
		CString strWinDDKInstallPath,strVCInstallPath;
		if (GetEnvironmentInstallPath(strWinDDKInstallPath,strVCInstallPath) == FALSE)
			break;

		//
		// 创建模版目录
		//
		CString strVCWizardPath,strVCProjectPath;
		strVCWizardPath.Format(_T("%s\\VCWizards\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCWizardPath))
			SHCreateDirectory(m_hWnd,strVCWizardPath);

		strVCProjectPath.Format(_T("%s\\vcprojects\\"),strVCInstallPath.TrimRight(_T("\\")));
		if (PathFileExists(strVCProjectPath))
			SHCreateDirectory(m_hWnd,strVCProjectPath);

		//拷贝文件到对应目录
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
		MessageBox(_T("卸载驱动向导失败"),_T("提示"),MB_OK);
	else
		MessageBox(_T("卸载驱动向导成功，谢谢使用"),_T("提示"),MB_OK);

	return;
}

BOOL CDriverToolDlg::GetEnvironmentInstallPath(CString& strWinDDKInstallPath,CString& strVCInstallPath)
{
	if (GetWinDDKInstallPath(strWinDDKInstallPath) == FALSE)
		ShowSelectFolder(strWinDDKInstallPath);

	if (strWinDDKInstallPath.IsEmpty() != FALSE)
	{
		MessageBox(_T("没有找到WinDDK的安装路径"),_T("提示"),MB_OK);
		return FALSE;
	}

	
	if (GetVCInstallPath(strVCInstallPath) == FALSE)
		ShowSelectFolder(strVCInstallPath);

	if (strVCInstallPath.IsEmpty() != FALSE)
	{
		MessageBox(_T("没有找到VC的安装路径"),_T("提示"),MB_OK);
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