
// DuilibWizardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DuilibWizard.h"
#include "DuilibWizardDlg.h"
#include "WinErrorMsg.h"

#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDuilibWizardDlg 对话框


CDuilibWizardDlg::CDuilibWizardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDuilibWizardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pErrorMsg = CWinErrorMsg::Init();
}

void CDuilibWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VS_VERSION_LIST, m_ListCtrl);
	DDX_Control(pDX, IDC_LOG_LIST, m_ListBox);
}

BEGIN_MESSAGE_MAP(CDuilibWizardDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_INSTALL, &CDuilibWizardDlg::OnBnClickedInstall)
	ON_BN_CLICKED(IDC_UNINSTALL, &CDuilibWizardDlg::OnBnClickedUninstall)
END_MESSAGE_MAP()


// CDuilibWizardDlg 消息处理程序

BOOL CDuilibWizardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetWindowTheme(m_ListCtrl.GetSafeHwnd(),L"Explorer",NULL);
	m_ListCtrl.ModifyStyle(0,LVS_REPORT,NULL);
	m_ListCtrl.SetExtendedStyle(LVS_EX_SINGLEROW|LVS_EX_DOUBLEBUFFER|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
	m_ListCtrl.InsertColumn(0,_T("序号"));
	m_ListCtrl.InsertColumn(1,_T("VS版本"));
	m_ListCtrl.InsertColumn(2,_T("安装路径"));
	//获取VS 各个版本
	//
	if (GetVisualStudioVersion(m_VersionInfoList))
	{
		POSITION pos = m_VersionInfoList.GetHeadPosition();
		int nItem = 1;
		while(pos)
		{
			POSITION CurrentPos = pos;
			const VSVersionInfo& VersionInfo = m_VersionInfoList.GetNext(pos);
			CString strIndex;
			strIndex.Format(_T("%d"),nItem++);
			DWORD dwItemIndex = m_ListCtrl.InsertItem(0x7FFFFFFF,strIndex);
			m_ListCtrl.SetItemText(dwItemIndex,1,VersionInfo.strVersion);
			m_ListCtrl.SetItemText(dwItemIndex,2,VersionInfo.strVSInstallPath);
			m_ListCtrl.SetItemData(dwItemIndex,(DWORD_PTR)CurrentPos);
		}
	}
	else
	{
		MessageBox(_T("请确认您已经安装VisualStudio开发工具"),_T("提示"),MB_OK|MB_ICONWARNING);
		PostQuitMessage(0);
	}

	int nTotal = m_ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i=0;i<nTotal;++i)
		m_ListCtrl.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDuilibWizardDlg::OnPaint()
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
HCURSOR CDuilibWizardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDuilibWizardDlg::OnBnClickedInstall()
{
	// TODO: 在此添加控件通知处理程序代码
	UINT nCount = m_ListCtrl.GetItemCount();
	for(UINT n=0;n<nCount;++n)
	{
		if (m_ListCtrl.GetCheck(n) == FALSE)
			continue;

		//安装Duilib项目向导到VS安装目录
		POSITION pos	=(POSITION)m_ListCtrl.GetItemData(n);
		const VSVersionInfo& VersionInfo = m_VersionInfoList.GetAt(pos);
		TCHAR szVSVCProjectPath[MAX_PATH];
		PathCombine(szVSVCProjectPath,VersionInfo.strVSInstallPath,_T("../../VC/vcprojects/Duilib"));
		InstallDuilibWizard(szVSVCProjectPath);
	}
}

void CDuilibWizardDlg::OnBnClickedUninstall()
{
	// TODO: 在此添加控件通知处理程序代码
	UINT nCount = m_ListCtrl.GetItemCount();
	for(UINT n=0;n<nCount;++n)
	{
		if (m_ListCtrl.GetCheck(n) == FALSE)
			continue;

		//安装Duilib项目向导到VS安装目录
		POSITION pos	=(POSITION)m_ListCtrl.GetItemData(n);
		const VSVersionInfo& VersionInfo = m_VersionInfoList.GetAt(pos);
		TCHAR szVSVCProjectPath[MAX_PATH];
		PathCombine(szVSVCProjectPath,VersionInfo.strVSInstallPath,_T("../../VC/vcprojects/Duilib"));
		//InstallDuilibWizard(szVSVCProjectPath);
		szVSVCProjectPath[_tcslen(szVSVCProjectPath) + 1] = _T('\0');
		SHFILEOPSTRUCT  ShFileOpstruct;
		ZeroMemory(&ShFileOpstruct,sizeof(ShFileOpstruct));
		ShFileOpstruct.hwnd = m_hWnd;
		ShFileOpstruct.fFlags = FOF_NOERRORUI|FOF_SILENT|FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION;
		ShFileOpstruct.wFunc = FO_DELETE;
		ShFileOpstruct.pFrom = szVSVCProjectPath;
		if (SHFileOperation(&ShFileOpstruct) >0)
			MessageBox(_T("卸载Duilib界面库失败"),_T("提示"),MB_OK);
		else
			MessageBox(_T("卸载Duilib界面库成功"),_T("提示"),MB_OK);
	}
}

BOOL CDuilibWizardDlg::GetVisualStudioVersion(CVSVersionInfoList& VersionInfoList)
{
	HKEY hKey = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		LPCTSTR lpszVisualStudioPath = _T("SOFTWARE\\Microsoft\\VisualStudio\\");
		DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpszVisualStudioPath,0,KEY_READ,&hKey);
		if (dwRet != ERROR_SUCCESS)
			break;

		DWORD dwItem = 0;
		while(TRUE)
		{
			TCHAR szRegItemName[MAX_PATH];
			DWORD dwItemSize = _countof(szRegItemName);
			DWORD dwRet = RegEnumKeyEx(hKey,dwItem++,szRegItemName,&dwItemSize,0,NULL,NULL,NULL);
			if (dwRet != ERROR_SUCCESS)
			{
				if (dwRet == ERROR_NO_MORE_ITEMS)
				{
					bSuccess = TRUE;
					break;
				}
			}

			HKEY hSubKey = NULL;

			do 
			{
				TCHAR szSubKeyPath[MAX_PATH];
				PathCombine(szSubKeyPath,lpszVisualStudioPath,szRegItemName);
				DWORD dwSubKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKeyPath,0,KEY_READ,&hSubKey);
				if (dwSubKey != ERROR_SUCCESS)
					break;

				TCHAR szItemData[MAX_PATH];
				DWORD dwDataSize = _countof(szItemData);
				dwSubKey = RegQueryValueEx(hSubKey,_T("InstallDir"),NULL,NULL,(LPBYTE)szItemData,&dwDataSize);
				if (dwSubKey != ERROR_SUCCESS)
					break;

				VSVersionInfo VersionInfo;
				VersionInfo.strVersion = GetVSVersion(szRegItemName);
				VersionInfo.strVSInstallPath = szItemData;
				VersionInfoList.AddTail(VersionInfo);
			} while (FALSE);

			if (hSubKey != NULL)
				RegCloseKey(hSubKey);
		}
		
		bSuccess = TRUE;
	} while (FALSE);
	
	if (hKey != NULL)
		RegCloseKey(hKey);

	return bSuccess;
}

CString CDuilibWizardDlg::GetVSVersion(LPCTSTR szRegItemName)
{
	CString strVSVersion;
	if (_tcsicmp(szRegItemName,_T("7.0")) == 0)
		strVSVersion = _T("VS 2003");
	else if (_tcsicmp(szRegItemName,_T("8.0")) == 0)
		strVSVersion = _T("VS 2005");
	else if (_tcsicmp(szRegItemName,_T("9.0")) == 0)
		strVSVersion = _T("VS 2008");
	else if (_tcsicmp(szRegItemName,_T("10.0")) == 0)
		strVSVersion = _T("VS 2010");
	else if (_tcsicmp(szRegItemName,_T("11.0")) == 0)
		strVSVersion = _T("VS 2012");
	else if (_tcsicmp(szRegItemName,_T("12.0")) == 0)
		strVSVersion = _T("VS 2013");
	else if (_tcsicmp(szRegItemName,_T("13.0")) == 0)
		strVSVersion = _T("VS 2014");
	else if (_tcsicmp(szRegItemName,_T("14.0")) == 0)
		strVSVersion = _T("VS 2015");
	else
		strVSVersion = _T("未知VS版本");
	
	return strVSVersion;
}

BOOL CDuilibWizardDlg::InstallDuilibWizard(LPCTSTR lpszVCProjectPath)
{
	BOOL bSuccess = FALSE;
	CWinErrorMsg* pErrorInfo = CWinErrorMsg::Init();
	do 
	{
		if (PathFileExists(lpszVCProjectPath) == FALSE)
			SHCreateDirectory(m_hWnd,lpszVCProjectPath);
		TCHAR szModulePath[MAX_PATH],szSourceFilePath[MAX_PATH],szTargetFilePath[MAX_PATH];
 		GetModuleFileName(NULL,szModulePath,_countof(szModulePath));
 		PathRemoveFileSpec(szModulePath);
 		//拷贝向导图标到VC项目目录
 		LPCTSTR lpszWizardIcon = _T("DuilibWizard.ico");
 		PathCombine(szSourceFilePath,szModulePath,lpszWizardIcon);
		PathCombine(szTargetFilePath,lpszVCProjectPath,lpszWizardIcon);
		if (CopyFile(szSourceFilePath,szTargetFilePath,FALSE) == FALSE)
		{
			pErrorInfo->SetErrorInfo(SYSTEM_ERROR,0,_T("拷贝DuilibWizard.ico到VC项目目录失败"));
			m_ListBox.AddString(pErrorInfo->GetThreadErrorInfoString());
			break;
		}
		
		//拷贝向导传递参数到VC项目目录
		LPCTSTR lpszWizardVsz = _T("DuilibWizard.vsz");
		PathCombine(szSourceFilePath,szModulePath,lpszWizardVsz);
		PathCombine(szTargetFilePath,lpszVCProjectPath,lpszWizardVsz);
		if (CopyFile(szSourceFilePath,szTargetFilePath,FALSE) == FALSE)
		{
			pErrorInfo->SetErrorInfo(SYSTEM_ERROR,0,_T("拷贝DuilibWizard.vsz到VC项目目录失败"));
			m_ListBox.AddString(pErrorInfo->GetThreadErrorInfoString());
			break;
		}
		//拷贝向导描述文件到VC项目目录
		LPCTSTR lpszWizardVsdir = _T("DuilibWizard.vsdir");
		PathCombine(szSourceFilePath,szModulePath,lpszWizardVsdir);
		PathCombine(szTargetFilePath,lpszVCProjectPath,lpszWizardVsdir);
		if (CopyFile(szSourceFilePath,szTargetFilePath,FALSE) == FALSE)
		{
			pErrorInfo->SetErrorInfo(SYSTEM_ERROR,0,_T("拷贝DuilibWizard.vsdir到VC项目目录失败"));
			m_ListBox.AddString(pErrorInfo->GetThreadErrorInfoString());
			break;
		}
		//拷贝向导安装模版到VC项目目录
		PathCombine(szSourceFilePath,szModulePath,_T("DuilibWizard"));
		PathCombine(szTargetFilePath,lpszVCProjectPath,_T("../VCWizards/AppWiz/Generic/DuilibWizard"));

		CopyDirectory(szSourceFilePath,szTargetFilePath);

		bSuccess = TRUE;
	} while (FALSE);

	pErrorInfo->Release();
	if (bSuccess)
		MessageBox(_T("安装Duilib向导成功！"),_T("提示"),MB_OK|MB_ICONINFORMATION);

	return bSuccess;
}

BOOL CDuilibWizardDlg::CopyDirectory(LPCTSTR lpszSourceFilePath,LPCTSTR lpszTargetFilePath)
{
	TCHAR szFindFilePath[MAX_PATH];
	PathCombine(szFindFilePath,lpszSourceFilePath,_T("*.*"));
	WIN32_FIND_DATA Win32FileData;
	HANDLE hFile = FindFirstFile(szFindFilePath,&Win32FileData);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do 
	{
		if (_tcsicmp(Win32FileData.cFileName,_T(".")) == 0 ||
			_tcsicmp(Win32FileData.cFileName,_T("..")) == 0)
				continue;

		TCHAR szSourceFilePath[MAX_PATH],szTargetFilePath[MAX_PATH];
		PathCombine(szSourceFilePath,lpszSourceFilePath,Win32FileData.cFileName);
		PathCombine(szTargetFilePath,lpszTargetFilePath,Win32FileData.cFileName);

		if (Win32FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			CreateDirectory(szTargetFilePath,NULL);
			CopyDirectory(szSourceFilePath,szTargetFilePath);
		}
		else
			CopyFile(szSourceFilePath,szTargetFilePath,FALSE);

	} while (FindNextFile(hFile,&Win32FileData));

	FindClose(hFile);

	return TRUE;
}