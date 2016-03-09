
// DuilibWizardDlg.cpp : ʵ���ļ�
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

// CDuilibWizardDlg �Ի���


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


// CDuilibWizardDlg ��Ϣ�������

BOOL CDuilibWizardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetWindowTheme(m_ListCtrl.GetSafeHwnd(),L"Explorer",NULL);
	m_ListCtrl.ModifyStyle(0,LVS_REPORT,NULL);
	m_ListCtrl.SetExtendedStyle(LVS_EX_SINGLEROW|LVS_EX_DOUBLEBUFFER|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
	m_ListCtrl.InsertColumn(0,_T("���"));
	m_ListCtrl.InsertColumn(1,_T("VS�汾"));
	m_ListCtrl.InsertColumn(2,_T("��װ·��"));
	//��ȡVS �����汾
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
		MessageBox(_T("��ȷ�����Ѿ���װVisualStudio��������"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		PostQuitMessage(0);
	}

	int nTotal = m_ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i=0;i<nTotal;++i)
		m_ListCtrl.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDuilibWizardDlg::OnPaint()
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
HCURSOR CDuilibWizardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDuilibWizardDlg::OnBnClickedInstall()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UINT nCount = m_ListCtrl.GetItemCount();
	for(UINT n=0;n<nCount;++n)
	{
		if (m_ListCtrl.GetCheck(n) == FALSE)
			continue;

		//��װDuilib��Ŀ�򵼵�VS��װĿ¼
		POSITION pos	=(POSITION)m_ListCtrl.GetItemData(n);
		const VSVersionInfo& VersionInfo = m_VersionInfoList.GetAt(pos);
		TCHAR szVSVCProjectPath[MAX_PATH];
		PathCombine(szVSVCProjectPath,VersionInfo.strVSInstallPath,_T("../../VC/vcprojects/Duilib"));
		InstallDuilibWizard(szVSVCProjectPath);
	}
}

void CDuilibWizardDlg::OnBnClickedUninstall()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UINT nCount = m_ListCtrl.GetItemCount();
	for(UINT n=0;n<nCount;++n)
	{
		if (m_ListCtrl.GetCheck(n) == FALSE)
			continue;

		//��װDuilib��Ŀ�򵼵�VS��װĿ¼
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
			MessageBox(_T("ж��Duilib�����ʧ��"),_T("��ʾ"),MB_OK);
		else
			MessageBox(_T("ж��Duilib�����ɹ�"),_T("��ʾ"),MB_OK);
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
		strVSVersion = _T("δ֪VS�汾");
	
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
 		//������ͼ�굽VC��ĿĿ¼
 		LPCTSTR lpszWizardIcon = _T("DuilibWizard.ico");
 		PathCombine(szSourceFilePath,szModulePath,lpszWizardIcon);
		PathCombine(szTargetFilePath,lpszVCProjectPath,lpszWizardIcon);
		if (CopyFile(szSourceFilePath,szTargetFilePath,FALSE) == FALSE)
		{
			pErrorInfo->SetErrorInfo(SYSTEM_ERROR,0,_T("����DuilibWizard.ico��VC��ĿĿ¼ʧ��"));
			m_ListBox.AddString(pErrorInfo->GetThreadErrorInfoString());
			break;
		}
		
		//�����򵼴��ݲ�����VC��ĿĿ¼
		LPCTSTR lpszWizardVsz = _T("DuilibWizard.vsz");
		PathCombine(szSourceFilePath,szModulePath,lpszWizardVsz);
		PathCombine(szTargetFilePath,lpszVCProjectPath,lpszWizardVsz);
		if (CopyFile(szSourceFilePath,szTargetFilePath,FALSE) == FALSE)
		{
			pErrorInfo->SetErrorInfo(SYSTEM_ERROR,0,_T("����DuilibWizard.vsz��VC��ĿĿ¼ʧ��"));
			m_ListBox.AddString(pErrorInfo->GetThreadErrorInfoString());
			break;
		}
		//�����������ļ���VC��ĿĿ¼
		LPCTSTR lpszWizardVsdir = _T("DuilibWizard.vsdir");
		PathCombine(szSourceFilePath,szModulePath,lpszWizardVsdir);
		PathCombine(szTargetFilePath,lpszVCProjectPath,lpszWizardVsdir);
		if (CopyFile(szSourceFilePath,szTargetFilePath,FALSE) == FALSE)
		{
			pErrorInfo->SetErrorInfo(SYSTEM_ERROR,0,_T("����DuilibWizard.vsdir��VC��ĿĿ¼ʧ��"));
			m_ListBox.AddString(pErrorInfo->GetThreadErrorInfoString());
			break;
		}
		//�����򵼰�װģ�浽VC��ĿĿ¼
		PathCombine(szSourceFilePath,szModulePath,_T("DuilibWizard"));
		PathCombine(szTargetFilePath,lpszVCProjectPath,_T("../VCWizards/AppWiz/Generic/DuilibWizard"));

		CopyDirectory(szSourceFilePath,szTargetFilePath);

		bSuccess = TRUE;
	} while (FALSE);

	pErrorInfo->Release();
	if (bSuccess)
		MessageBox(_T("��װDuilib�򵼳ɹ���"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);

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