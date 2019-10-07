#include "StdAfx.h"
#include "KSPInfo.h"
#include <shlobj.h>
#include "DllCore/Utils/FileTools.h"

#define DELAY_TIME 20*1000
#define APP_LIST_ITEM_INFO _T("KSPList.xml")
#define APPLICATION_MENU _T("KSPMenu.xml")

CKSPInfo::CKSPInfo()
{
	m_bExit = FALSE;
	m_bLoad = FALSE;
	m_pPaintManager = NULL;

	m_pListView = NULL;
	m_hThread = NULL;
	m_hFileEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CKSPInfo::~CKSPInfo()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;

	m_bExit = TRUE;

	if (m_hFileEvent != NULL)
	{
		SetEvent(m_hFileEvent);
		CloseHandle(m_hFileEvent);
		m_hFileEvent = NULL;
	}

	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_pListView != NULL)
	{
		delete m_pListView;
		m_pListView = NULL;
	}
}

DUI_BEGIN_MESSAGE_MAP(CKSPInfo, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_RETURN, OnReturn)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_KILLFOCUS, OnKillFocus)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TEXTCHANGED, OnTextChanged)
DUI_END_MESSAGE_MAP()

void CKSPInfo::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

BOOL CKSPInfo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	bHandle = FALSE;

	if (uMsg == WM_KEYDOWN) {
		if (m_pListView->OnKeyDown(uMsg, wParam, lParam, bHandle) == FALSE)
			return FALSE;
	} else if (uMsg == WM_WINDOWPOSCHANGING) {
		if (m_pListView == NULL)
			return TRUE;
		return m_pListView->OnPosChanged(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_MOUSEWHEEL) {
		return m_pListView->OnMouseWheel(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN) {
		// 点击应用程序标题栏应该关闭提示框
		if (wParam == HTCAPTION)
			m_pListView->ShowWindow(false);
	}

	return bHandle;
}

void CKSPInfo::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnTryVersion"))
		ClonePackage();
	else if (msg.pSender->GetName() == _T("BtnExport"))
		ExportPackage();
	else if (msg.pSender->GetName() == _T("LookupFile"))
		LookupPackageFile();
	else if (msg.pSender->GetName() == _T("BtnUnzipFile"))
		UnpackageFile();
	else if (msg.pSender->GetName() == _T("BtnAddPlugin"))
		OnAddPlugin();
}

void CKSPInfo::OnRefresh(TNotifyUI& msg)
{

}

void CKSPInfo::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	if (m_pListView == NULL)
	{
		CEditUI2* pSupport = (CEditUI2*)m_pPaintManager->FindControl(_T("SupportKernel"));
		m_pListView = new CListViewWnd(m_pPaintManager->GetPaintWindow(), pSupport);
	}

	if (m_hThread == NULL)
	{
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, NULL);
	}
}

void CKSPInfo::OnReturn(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("SupportKernel"))
		m_pListView->ShowWindow(false);
}

void CKSPInfo::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("KSPList"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(APPLICATION_MENU);
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

void CKSPInfo::OnKSPMenu(CControlUI* pControl)
{
	CListUI* pKSPList = (CListUI*)m_pPaintManager->FindControl(_T("KSPList"));
	if (pKSPList->GetCurSel() == -1)
		return;

	CDuiString strItemName = pControl->GetName();

	CFileListItemUI* pItem = (CFileListItemUI*)pKSPList->GetItemAt(pKSPList->GetCurSel());
	if (strItemName == _T("Del"))
	{
		pKSPList->RemoveAt(pKSPList->GetCurSel());
		CDuiString strPluginName = pItem->GetSubControlText(_T("PluginName"));
		for (int n=0; n<m_strKernelPluginList.GetCount(); ++n)
		{
			LPCTSTR lpszPluginName = m_strKernelPluginList.GetAt(n);
			if (strPluginName.CompareNoCase(lpszPluginName) == 0)
			{
				m_strKernelPluginList.RemoveAt(n);
				break;
			}
		}

		//更新序列号
		for (int n=0; n<pKSPList->GetCount(); ++n)
		{
			CFileListItemUI* pItem = (CFileListItemUI*)pKSPList->GetItemAt(n);
			CString strIndex;
			strIndex.Format(_T("%d"), n+1);
			pItem->SetSubControlText(_T("Index"), strIndex);
		}
	}
}

void CKSPInfo::OnKillFocus(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("SupportKernel"))
	{
		if (m_pListView != NULL)
			m_pListView->ShowWindow(false);
	}
}

void CKSPInfo::OnTextChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("SupportKernel"))
	{
		CDuiString strKernelVersion = msg.pSender->GetText();

		strKernelVersion.Replace(_T("."), _T("_"));
		strKernelVersion.Replace(_T("-"), _T("_"));
		strKernelVersion.Replace(_T(" "), _T("_"));

		if (strKernelVersion.IsEmpty() != FALSE)
		{
			m_pListView->ShowWindow(false);
			return;
		}

		m_pListView->RemoveAll();
 
		NAutoLock Lock(&m_pLock);
		for (int n=0; n<m_strKernelPackageList.GetSize(); ++n)
		{
			const CString& strKernelPack = m_strKernelPackageList.GetAt(n);
			if (strKernelPack.IsEmpty() || StrStrI(strKernelPack, strKernelVersion) == NULL)
				continue;
			m_pListView->Add(strKernelPack);
		}

		m_pListView->ResizeWnd();
	}
}

UINT CKSPInfo::ThreadProc(LPVOID lParam)
{
	CKSPInfo* pAIToolkit = (CKSPInfo*)lParam;
	if (pAIToolkit == NULL)
		return FALSE;

	DWORD dwTimeDelay = 0;

	while(TRUE)
	{
		DWORD dwRet = WaitForSingleObject(pAIToolkit->m_hFileEvent, dwTimeDelay);
		if (pAIToolkit->m_bExit)
			break;

		CStdArray strKernelPackage;
		pAIToolkit->EnumPluginKernelVersion(strKernelPackage);

		if (strKernelPackage.IsEmpty() == FALSE)
		{
			NAutoLock Lock(&pAIToolkit->m_pLock);
			pAIToolkit->m_strKernelPackageList.RemoveAll();
			pAIToolkit->m_strKernelPackageList.Copy(strKernelPackage);
		}

		if (dwTimeDelay == 0)
		{
			dwTimeDelay = DELAY_TIME;
		}
	}

	return TRUE;
}

void CKSPInfo::EnumPluginKernelVersion(CStdArray& strKernelPackageList)
{
	WIN32_FIND_DATA Win32Data;
	//LPCTSTR lpszSearchPath = _T("C:\\Users\\admin\\Downloads\\Agent-SuSE_11-10.0.0-2888.x86_64\\*.dsp");
	TCHAR szSearchPath[MAX_PATH];
	PathCombine(szSearchPath, m_strReleasePath, _T("*.dsp"));
	HANDLE hFindFile = FindFirstFile(szSearchPath, &Win32Data);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return;

	do 
	{
		if (_tcsicmp(Win32Data.cFileName, _T(".")) == 0 || 
			_tcsicmp(Win32Data.cFileName, _T("..")) == 0 )
			continue;

		if (Win32Data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		strKernelPackageList.Add(Win32Data.cFileName);

	} while (FindNextFile(hFindFile, &Win32Data));

	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
}

BOOL CKSPInfo::GetDspFileByPreName(LPCTSTR lpszDspPreName, CString& strDspFileName)
{
	WIN32_FIND_DATA Win32Data;
	TCHAR szSearchPath[MAX_PATH];
	PathCombine(szSearchPath, m_strReleasePath, lpszDspPreName);
	
	_tcscat_s(szSearchPath, _T("*.dsp"));
	HANDLE hFindFile = FindFirstFile(szSearchPath, &Win32Data);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do 
	{
		if (_tcsicmp(Win32Data.cFileName, _T(".")) == 0 || 
			_tcsicmp(Win32Data.cFileName, _T("..")) == 0 )
			continue;

		if (Win32Data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		strDspFileName = Win32Data.cFileName;
		break;

	} while (FindNextFile(hFindFile, &Win32Data));

	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}

BOOL CKSPInfo::GetDepluaPlugin(LPCTSTR lpszNoDepPackage, CString& strDepluaPlugin)
{
	CString strPackageName = PathFindFileName(lpszNoDepPackage);
	
	TCHAR szDepluaPlugin[MAX_PATH];
	_stscanf_s(strPackageName, _T("%[^0-9]"), 
										szDepluaPlugin, _countof(szDepluaPlugin));

	strDepluaPlugin = szDepluaPlugin;
	strDepluaPlugin.TrimRight(_T("-"));
	strDepluaPlugin.TrimRight(_T("_"));
	strDepluaPlugin.TrimRight(_T(" "));

	return TRUE;
}

BOOL CKSPInfo::GetNoDepPackage(LPCTSTR lpszSourcePackageFile, CString& strNoDepPackage)
{
	TCHAR szPackageFileName[MAX_PATH];
	PathCombine(szPackageFileName, m_strReleasePath, lpszSourcePackageFile);

	CString strPackageDeplua;
	if (CParseDsp::IsPackageHasDep(szPackageFileName, strPackageDeplua) == FALSE)
	{
		strNoDepPackage = szPackageFileName;
		return TRUE;
	}

	CParseDsp				DepLua;
	DepLua.Init(strPackageDeplua);
	CString strDepluaFileName =  DepLua.GetFileName();
	CString strDepluaFileType  = DepLua.GetLuaFileType();

	CString strDspFilePre;
	strDspFilePre.Format(_T("%s-%s"), strDepluaFileType, strDepluaFileName);

	return GetDspFileByPreName(strDspFilePre, strNoDepPackage);
}

void CKSPInfo::ClonePackage()
{
	CEditUI2* pTargetFile = (CEditUI2*)m_pPaintManager->FindControl(_T("KernelVersion"));

	if (m_strKernelPluginList.GetCount() == 0)
	{
		MessageBox(m_pPaintManager->GetPaintWindow(), _T("请输入Linux内核基版版本号"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	if (pTargetFile->GetText().IsEmpty() != FALSE)
	{
		MessageBox(m_pPaintManager->GetPaintWindow(), _T("请输入需要适配的内核版本号"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	TCHAR szWorkDir[MAX_PATH];
	GetCurrentDirectory(_countof(szWorkDir), szWorkDir);
	SetCurrentDirectory(m_strReleasePath);

	for (int n=0; n<m_strKernelPluginList.GetCount(); ++n)
	{
		LPCTSTR lpszPluginName = m_strKernelPluginList.GetAt(n);
		if (ClonePluginToNewName(lpszPluginName) == FALSE)
		{
			MessageBox(m_pPaintManager->GetPaintWindow(), _T("没有找到依赖包，请确认依赖包存在！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
			break;
		}
	}

	SetCurrentDirectory(szWorkDir);
}

void CKSPInfo::ExportPackage()
{
	CLabelUI* pPackageVersion = (CLabelUI*)m_pPaintManager->FindControl(_T("PackageVersion"));

	CString strAgentPackage;
	strAgentPackage.Format(_T("Agent-%s.zip"), pPackageVersion->GetText());

	TCHAR szTargetAgentFilePath[MAX_PATH];
	PathCombine(szTargetAgentFilePath, m_strReleasePath, strAgentPackage);

	CParseDsp::PackageFile(m_strReleasePath, szTargetAgentFilePath);
}

BOOL CKSPInfo::ClonePluginToNewName(LPCTSTR lpszBasedPlugin)
{
	CString strTargetPackageName;
	CString strNoDepPackage;
	if (GetNoDepPackage(lpszBasedPlugin, strNoDepPackage) != FALSE)
	{
		CString strDepluaPlugin;
		GetDepluaPlugin(strNoDepPackage, strDepluaPlugin);

		CLabelUI* pPackageVersion = (CLabelUI*)m_pPaintManager->FindControl(_T("PackageVersion"));

		CEditUI2* pTargetFile = (CEditUI2*)m_pPaintManager->FindControl(_T("KernelVersion"));
		CString strPackage = pTargetFile->GetText().GetData();
		strPackage.Replace(_T("."), _T("_"));
		strPackage.Replace(_T("-"), _T("_"));
		strPackage.Replace(_T(" "), _T("_"));

		strTargetPackageName.Format(_T("%s_%s-%s.dsp"), strDepluaPlugin, strPackage, pPackageVersion->GetText().GetData());

		TCHAR szTargetFilePath[MAX_PATH];
		PathCombine(szTargetFilePath, m_strReleasePath, strTargetPackageName);

		TCHAR szTempPath[MAX_PATH];
		TCHAR szTempDir[MAX_PATH];

		GetTempPath(_countof(szTempPath), szTempPath);

		CString strKSPTempFolderName;
		strKSPTempFolderName.Format(_T("KSP%04X.tmp"), this);

		// create temp directory to unpackage zip file
		PathCombine(szTempDir, szTempPath, strKSPTempFolderName);
		SHCreateDirectory(m_pPaintManager->GetPaintWindow(), szTempDir);

		CParseDsp::UnPackageFile(szTempDir, strNoDepPackage);

		//rename directory to real version name
		CString strKernelPackage = pTargetFile->GetText().GetData();
		RenameDirectoryToNewName(szTempDir, strKernelPackage);

		CParseDsp::PackageFile(szTempDir, szTargetFilePath);

		//delete temp dir
		SHDeleteDirectory(szTempDir);

		return TRUE;
	} 
	return FALSE;
}

void CKSPInfo::LookupPackageFile()
{
	TCHAR szReleaseFilePath[MAX_PATH] = {0};
	OPENFILENAME OFNFileName;
	ZeroMemory(&OFNFileName ,sizeof(OFNFileName)); 

	OFNFileName.lStructSize		= sizeof(OFNFileName); 
	OFNFileName.hwndOwner		= m_pPaintManager->GetPaintWindow(); 
	OFNFileName.lpstrFile			= szReleaseFilePath ; 
	OFNFileName.nMaxFile			= _countof(szReleaseFilePath);
	OFNFileName.lpstrFilter			= _T("已发布客户端软件版本\0*.zip\0\0"); 
	OFNFileName.nFilterIndex		= 1; 
	OFNFileName.lpstrFileTitle	= NULL; 
	OFNFileName.nMaxFileTitle	= 0;
	OFNFileName.lpstrTitle			= _T("请选择已发布的Linux系统版本");
	OFNFileName.lpstrInitialDir	= NULL; 
	OFNFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetOpenFileName(&OFNFileName))
	{
		CEditUI2* pReleaseFilePath = (CEditUI2*)m_pPaintManager->FindControl(_T("ReleaseFilePath"));
		if (pReleaseFilePath == NULL)
			return;

		pReleaseFilePath->SetText(szReleaseFilePath);
	}

	CLabelUI* pPackageVersion = (CLabelUI*)m_pPaintManager->FindControl(_T("PackageVersion"));

	CString strReleasePackage(szReleaseFilePath);
	m_strReleasePath = strReleasePackage;
	m_strReleasePath.TrimRight(_T(".zip"));

	CString strReleasePackageFileName = PathFindFileName(strReleasePackage);

	strReleasePackageFileName.TrimLeft(_T("Agent-"));
	strReleasePackageFileName.TrimRight(_T(".zip"));

	pPackageVersion->SetText(strReleasePackageFileName);

	SetEvent(m_hFileEvent);
}

void CKSPInfo::UnpackageFile()
{
	CEditUI2* pReleaseFilePath = (CEditUI2*)m_pPaintManager->FindControl(_T("ReleaseFilePath"));
	if (pReleaseFilePath == NULL)
		return;

	if (pReleaseFilePath->GetText().IsEmpty() != FALSE)
	{
		MessageBox(m_pPaintManager->GetPaintWindow(), _T("未选择已发布软件包，请选择已发布软件包后重试！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	if (PathFileExists(pReleaseFilePath->GetText()) == FALSE)
	{
		MessageBox(m_pPaintManager->GetPaintWindow(), _T("已发布软件包不存在，请确认文件是否正确！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	CParseDsp::UnPackageFile(m_strReleasePath, pReleaseFilePath->GetText());
}

void CKSPInfo::OnAddPlugin()
{
	CEditUI2* pSupportKernel = (CEditUI2*)m_pPaintManager->FindControl(_T("SupportKernel"));
	CString strPluginName = pSupportKernel->GetText();
	if (strPluginName.IsEmpty())
	{
		MessageBox(m_pPaintManager->GetPaintWindow(), _T("搜索内核版本不能为空，请输入后重试！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return ;
	}

	for (int n=0; n<m_strKernelPluginList.GetCount(); ++n)
	{
		LPCTSTR lpszPluginName = m_strKernelPluginList.GetAt(n);
		if (strPluginName.CompareNoCase(lpszPluginName) == 0)
			return;
	}

	//刷新列表
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("KSPList"));
	CreateNewAppItem(pList, strPluginName);
	m_strKernelPluginList.Add(strPluginName);
}

void CKSPInfo::CreateNewAppItem(CListUI* pList, LPCTSTR lpszPluginName)
{
	CFileListItemUI* pFileItem = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(APP_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
	else
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

	pList->Add(pFileItem);
	pFileItem->SetFixedHeight(33);

	const TListInfoUI* pListInfo = pList->GetListInfo();

	CLabelUI* pLabelIndex = (CLabelUI*)pFileItem->FindSubControl(_T("Index"));
	CString strIndex;
	strIndex.Format(_T("%d"), pList->GetCount());
	pLabelIndex->SetText(strIndex);
	pLabelIndex->SetFont(pListInfo->nFont);
	pLabelIndex->SetForeColor(pListInfo->dwTextColor);
	pLabelIndex->AppendTextStyle(DT_END_ELLIPSIS);

	CLabelUI* pPluginName = (CLabelUI*)pFileItem->FindSubControl(_T("PluginName"));
	pPluginName->SetText(lpszPluginName);
	pPluginName->SetFont(pListInfo->nFont);
	pPluginName->SetForeColor(pListInfo->dwTextColor);
	pPluginName->AppendTextStyle(DT_END_ELLIPSIS);
}

BOOL CKSPInfo::RenameDirectoryToNewName(LPCTSTR lpszBaseDirectoryName, LPCTSTR lpszDirectoryNewName)
{	
	BOOL bSuccess = FALSE;

	WIN32_FIND_DATA Win32Data;
	TCHAR szSearchPath[MAX_PATH];
	PathCombine(szSearchPath, lpszBaseDirectoryName, _T("*.*"));
	HANDLE hFile = FindFirstFile(szSearchPath, &Win32Data);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do 
	{
		if (_tcsicmp(Win32Data.cFileName, _T(".")) ==0 || _tcsicmp(Win32Data.cFileName, _T("..")) == 0)
			continue;

		if (Win32Data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR szDirectoryName[MAX_PATH];
			TCHAR szDirectoryNewName[MAX_PATH];
			PathCombine(szDirectoryName, lpszBaseDirectoryName, Win32Data.cFileName);
			PathCombine(szDirectoryNewName, lpszBaseDirectoryName, lpszDirectoryNewName);
			SHRenameDirectory(szDirectoryName, szDirectoryNewName);

			break;
		}

	} while (FindNextFile(hFile, &Win32Data));

	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bSuccess;
}