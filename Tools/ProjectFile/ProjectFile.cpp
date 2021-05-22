#include "StdAfx.h"
#include "Resource.h"
#include "ProjectFile.h"
#include <atltime.h>
#include <CommDlg.h>

CProjectFile::CProjectFile()
{
	m_pHistory = new CHistory;
	m_pCreateControl = new CControlBuilder;
}

CProjectFile::~CProjectFile()
{
	if (m_pHistory != NULL)
	{
		delete m_pHistory;
	}
	if (m_pCreateControl != NULL)
	{
		delete m_pCreateControl;
	}

	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("FileList"));
	if (pList->GetCount() != 0)
		ReleaseFileItemTagObject(pList);
}

void CProjectFile::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CProjectFile::GetWindowClassName() const
{
	return _T("CopyProjectFile");
}

CDuiString CProjectFile::GetSkinFile()
{
	return _T("ProjectFile.xml");
}

CDuiString CProjectFile::GetZIPFileName() const
{
	return _T("ProjectFile.zip");
}

CDuiString CProjectFile::GetSkinFolder()
{
	return _T("ProjectFile");
}

UILIB_RESOURCETYPE CProjectFile::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_RESOURCE
#endif
}

CControlUI* CProjectFile::CreateControl(LPCTSTR pstrClass)
{
	return m_pCreateControl->CreateControl(pstrClass);
}

void CProjectFile::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CProjectFile::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	/*else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);*/
}

void CProjectFile::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose")) {
		Close(IDOK);
		PostQuitMessage(0);
	} else if (msg.pSender->GetName() == _T("BtnMin")) {
		SendMessage(WM_SYSCOMMAND, WS_MINIMIZE, 0);
	} else if (msg.pSender->GetName() == _T("OptionUI1")) {
		TCHAR szProjectFilePath[MAX_PATH];
		ZeroMemory(szProjectFilePath, sizeof(szProjectFilePath));

		OPENFILENAME ProjectFileCfg = {0};
		ProjectFileCfg.lStructSize		= sizeof(ProjectFileCfg); 
		ProjectFileCfg.hwndOwner		= m_hWnd; 
		ProjectFileCfg.lpstrFile			= szProjectFilePath; 
		ProjectFileCfg.nMaxFile			= _countof(szProjectFilePath);
		ProjectFileCfg.lpstrFilter			= _T("项目文件配置信息\0*.xml\0\0"); 
		ProjectFileCfg.nFilterIndex		= 1; 
		ProjectFileCfg.lpstrFileTitle	= NULL; 
		ProjectFileCfg.nMaxFileTitle	= 0; 
		ProjectFileCfg.lpstrTitle			= _T("请选择需要操作的项目文件");
		ProjectFileCfg.lpstrInitialDir	= NULL ; 
		ProjectFileCfg.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

		if (GetOpenFileName(&ProjectFileCfg) == FALSE)
			return ;

		LoadProjectFileList(szProjectFilePath);

		if (m_ProjectFileManager.GetProjectName().IsEmpty() == FALSE)
			AddToHistory(m_ProjectFileManager.GetProjectName(), szProjectFilePath);
	} else if (msg.pSender->GetName() == _T("OptionUI2")) {
		MessageBox(m_hWnd, _T("暂未开放！"), _T("提示"), MB_OK |MB_ICONINFORMATION);

	} else if (msg.pSender->GetName() == _T("OptionUI3")) {
		OnCopyFiles(msg);
	} else if (msg.pSender->GetName() == _T("BtnLogo")) {
		CAboutDialog* pDlg = new CAboutDialog(m_hWnd);
		pDlg->ShowModal();
	} else if (msg.pSender->GetName() == _T("BtnSkin")) {
		CStdArray strProjectName;
		m_pHistory->EnumHistoryProjectName(strProjectName);

		if (strProjectName.IsEmpty() == FALSE)
		{
			CMenuWnd* pMenu = new CMenuWnd();
			const RECT& rcPos = msg.pSender->GetPos();
			CDuiPoint pt(rcPos.left,rcPos.bottom);
			ClientToScreen(m_hWnd, &pt);
			STRINGorID strXmlFile(_T("History.xml"));
			pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);

			CMenuUI* pRoot = pMenu->GetMenuUI();
			for (int n=0; n<strProjectName.GetCount(); ++n)
			{
				CMenuElementUI* pElementItem = new CMenuElementUI;
				pRoot->Add(pElementItem);

				pElementItem->SetText(strProjectName.GetAt(n));
			}
			pMenu->ResizeMenu();
		}
	}
}

void CProjectFile::OnCopyFiles(TNotifyUI& msg)
{
	CloseHandle(CreateThread(NULL , NULL , (LPTHREAD_START_ROUTINE)CopyFilesThreadProc , this , 0 ,NULL));
}

UINT CProjectFile::OnCopyFiles()
{
	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("FileList"));
	if (pList == NULL)
		return 0;

	CString strSrcFile, strDestFile;
	for (int nIndex = 0; nIndex < pList->GetCount(); ++nIndex)
	{
		CFileListItemUI* pFileItem = (CFileListItemUI*)pList->GetItemAt(nIndex);
		if (pFileItem == NULL)
			continue;
		CProjectFileInfo* pProjectFileInfo = (CProjectFileInfo*)pFileItem->GetTag();
		
		GetFilePath(pProjectFileInfo, strSrcFile, strDestFile);

		::CopyFile(strSrcFile, strDestFile, FALSE);
		DWORD dwResult = CompareFileAttribute(strSrcFile, strDestFile);
		if (dwResult & PATH_NO_SUCH_FILE)
			UpdateItemTextColor(pFileItem, m_ProjectFileManager.GetNoSuchFileColor());
		else if (dwResult & ERROR_ATTRIBUTE)
			UpdateItemTextColor(pFileItem, m_ProjectFileManager.GetErrorAttributeColor());
		else 
			UpdateItemTextColor(pFileItem, pList->GetListInfo()->dwTextColor);
	}

	return TRUE;
}

UINT CProjectFile::CopyFilesThreadProc(LPVOID lParam)
{
	CProjectFile* pProjectFile = (CProjectFile*)lParam;
	return pProjectFile->OnCopyFiles();
}

int CProjectFile::CompareFileAttribute(LPCTSTR lpszSrcFile , LPCTSTR lpszDestFile)
{
	if (PathFileExists(lpszSrcFile) == FALSE || PathFileExists(lpszDestFile) == FALSE)
		return PATH_NO_SUCH_FILE;

	DWORD dwRet = 0;
	HANDLE hFileSrc = INVALID_HANDLE_VALUE , hFileDes = INVALID_HANDLE_VALUE;

	do 
	{
		FILETIME SrcLastWriteFileTime , DesLastWriteFileTime;
		hFileSrc = CreateFile(lpszSrcFile , GENERIC_READ , FILE_SHARE_READ , NULL ,OPEN_EXISTING , 0 , NULL);
		hFileDes = CreateFile(lpszDestFile , GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , 0 , NULL);
		if (hFileDes == INVALID_HANDLE_VALUE || hFileSrc == INVALID_HANDLE_VALUE)
		{
			dwRet = PATH_NO_SUCH_FILE;
			break;
		}

		GetFileTime(hFileSrc , NULL ,NULL , &SrcLastWriteFileTime);
		GetFileTime(hFileDes , NULL , NULL , &DesLastWriteFileTime);

		LARGE_INTEGER LarIntSrc;
		LARGE_INTEGER LarIntDes;
		LarIntSrc.LowPart  = SrcLastWriteFileTime.dwLowDateTime;
		LarIntSrc.HighPart = SrcLastWriteFileTime.dwHighDateTime;

		LarIntDes.LowPart = DesLastWriteFileTime.dwLowDateTime;
		LarIntDes.HighPart = DesLastWriteFileTime.dwHighDateTime;

		if (LarIntSrc.QuadPart != LarIntDes.QuadPart)
		{
			dwRet = ERROR_ATTRIBUTE;
			break;
		}

		DWORD dwSrcFileSize = GetFileSize(hFileSrc, NULL);
		DWORD dwDestFileSize = GetFileSize(hFileDes, NULL);

		if (dwSrcFileSize != dwDestFileSize)
		{
			dwRet = ERROR_ATTRIBUTE;
			break;
		}

	} while (FALSE);

	if (hFileSrc != INVALID_HANDLE_VALUE)
		CloseHandle(hFileSrc);
	if (hFileDes != INVALID_HANDLE_VALUE)
		CloseHandle(hFileDes);

	return dwRet;
}

void CProjectFile::CreateProjectFileItem(CListUI* pList, const TListInfoUI* pListInfo, IMarkup* pMarkup)
{
	CProjectFileInfo* pProjectFileInfo = (CProjectFileInfo*)pMarkup->GetInterface(MARK_FILE);
	if (pProjectFileInfo == NULL)
		return;

	CFileListItemUI* pFileItem = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(PROJECT_FILE_ITEM_INFO, 0, m_pCreateControl, &m_PaintManager);
	else
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(m_pCreateControl, &m_PaintManager);

	DWORD dwCompare = 0;
	pList->Add(pFileItem);
	pFileItem->SetFixedHeight(27);
	pFileItem->SetTag((UINT_PTR)pProjectFileInfo);

	// 设置列表项序号
	CDuiString strIndex;
	strIndex.Format(_T("%d"), pList->GetCount());

	CLabelUI* pIndex = (CLabelUI*)pFileItem->FindSubControl(_T("Index"));
	pIndex->SetText(strIndex);
	pIndex->SetFont(pListInfo->nFont);
	pIndex->SetForeColor(pListInfo->dwTextColor);
	pIndex->AppendTextStyle(DT_END_ELLIPSIS);

	CLabelUI* pName = (CLabelUI*)pFileItem->FindSubControl(_T("Name"));
	pName->SetText(pProjectFileInfo->GetFileName());
	pName->SetFont(pListInfo->nFont);
	pName->SetForeColor(pListInfo->dwTextColor);
	pName->AppendTextStyle(DT_END_ELLIPSIS);

	CLabelUI* pSrcPath = (CLabelUI*)pFileItem->FindSubControl(_T("SrcPath"));
	pSrcPath->SetText(pProjectFileInfo->GetFileSrcPath());
	pSrcPath->SetFont(pListInfo->nFont);
	pSrcPath->SetForeColor(pListInfo->dwTextColor);
	pSrcPath->AppendTextStyle(DT_END_ELLIPSIS);

	CLabelUI* pDestPath = (CLabelUI*)pFileItem->FindSubControl(_T("DestPath"));
	pDestPath->SetText(pProjectFileInfo->GetFileDestPath());
	pDestPath->SetFont(pListInfo->nFont);
	pDestPath->SetForeColor(pListInfo->dwTextColor);
	pDestPath->AppendTextStyle(DT_END_ELLIPSIS);

	CString strSrcFilePath, strDestFilePath;
	GetFilePath(pProjectFileInfo, strSrcFilePath, strDestFilePath);

	LPCTSTR lpszFileName[] = {strSrcFilePath, strDestFilePath};
	WIN32_FILE_ATTRIBUTE_DATA Win32Data[2] = {0};
	for (int n=0; n<_countof(lpszFileName); ++n)
	{
		if (PathFileExists(lpszFileName[n]) == FALSE)
			dwCompare |= PATH_NO_SUCH_FILE;

		GetFileAttributesEx(lpszFileName[n], GetFileExInfoStandard, &Win32Data[n]);
	}

	// 获取文件修改时间
	CTime TimeSrc(Win32Data[0].ftLastWriteTime);
	CTime TimeDest(Win32Data[1].ftLastWriteTime);
	if (TimeSrc.GetTime() != TimeDest.GetTime())
		dwCompare |= ERROR_ATTRIBUTE;

	CString strFileTime;
	strFileTime.Format(_T("%02d:%02d:%02d | %02d:%02d:%02d"),
									 TimeSrc.GetHour(), TimeSrc.GetMinute(), TimeSrc.GetSecond(),
									 TimeDest.GetHour(),TimeDest.GetMinute(), TimeSrc.GetSecond());

	CLabelUI* pFileTime = (CLabelUI*)pFileItem->FindSubControl(_T("FileTime"));
	pFileTime->SetText(strFileTime);
	pFileTime->SetFont(pListInfo->nFont);
	pFileTime->SetForeColor(pListInfo->dwTextColor);
	pFileTime->AppendTextStyle(DT_END_ELLIPSIS);
	
	// 获取文件大小
	if (Win32Data[0].nFileSizeLow != Win32Data[1].nFileSizeLow)
		dwCompare |= ERROR_ATTRIBUTE;

	TCHAR szSrcFileSize[MAX_PATH], szDestFileSize[MAX_PATH];
	StrFormatByteSize64(Win32Data[0].nFileSizeLow, szSrcFileSize, _countof(szSrcFileSize));
	StrFormatByteSize64(Win32Data[1].nFileSizeLow, szDestFileSize, _countof(szDestFileSize));

	CString strFileSize;
	strFileSize.Format(_T("%s | %s"), szSrcFileSize, szDestFileSize);

	CLabelUI* pFileSize = (CLabelUI*)pFileItem->FindSubControl(_T("FileSize"));
	pFileSize->SetText(strFileSize);
	pFileSize->SetFont(pListInfo->nFont);
	pFileSize->SetForeColor(pListInfo->dwTextColor);
	pFileSize->AppendTextStyle(DT_END_ELLIPSIS);

	if (dwCompare & PATH_NO_SUCH_FILE)
		UpdateItemTextColor(pFileItem, m_ProjectFileManager.GetNoSuchFileColor());
	else if (dwCompare & ERROR_ATTRIBUTE)
		UpdateItemTextColor(pFileItem, m_ProjectFileManager.GetErrorAttributeColor());
}

void CProjectFile::UpdateItemTextColor(CFileListItemUI* pFileListItem, DWORD dwTextColor) const
{
	CLabelUI* pIndex = (CLabelUI*)pFileListItem->FindSubControl(_T("Index"));
	if (pIndex)
		pIndex->SetTextColor(dwTextColor);
	CLabelUI* pName = (CLabelUI*)pFileListItem->FindSubControl(_T("Name"));
	if (pName)
		pName->SetTextColor(dwTextColor);
	CLabelUI* pSrcPath = (CLabelUI*)pFileListItem->FindSubControl(_T("SrcPath"));
	if (pSrcPath)
		pSrcPath->SetTextColor(dwTextColor);
	CLabelUI* pDestPath = (CLabelUI*)pFileListItem->FindSubControl(_T("DestPath"));
	if (pDestPath)
		pDestPath->SetTextColor(dwTextColor);
	CLabelUI* pFileTime = (CLabelUI*)pFileListItem->FindSubControl(_T("FileTime"));
	if (pFileTime)
		pFileTime->SetTextColor(dwTextColor);
	CLabelUI* pFileSize = (CLabelUI*)pFileListItem->FindSubControl(_T("FileSize"));
	if (pFileSize)
		pFileSize->SetTextColor(dwTextColor);
}

void CProjectFile::ReleaseFileItemTagObject(CListUI* pList)
{
	if (pList->GetCount() != 0)
	{
		for (int n=0; n<pList->GetCount(); ++n )
		{
			CFileListItemUI* pFileItem = (CFileListItemUI*)pList->GetItemAt(n);
			IMarkup* pMarkup = (IMarkup*)pFileItem->GetTag();
			if (pMarkup)
				delete pMarkup;
		}
	}
}

void CProjectFile::GetFilePath(CProjectFileInfo* pProjectFileInfo, CString& strSrcFile, CString& strDestFile) const
{
	TCHAR szProjectFilePath[MAX_PATH];
	_tcscpy_s(szProjectFilePath, _countof(szProjectFilePath), m_ProjectFileManager.GetProjectFilePath());

	PathRemoveFileSpec(szProjectFilePath);

	TCHAR szFilePath[MAX_PATH], szDestFilePath[MAX_PATH];
	PROJECT_FILE_PATH_TYPE ProjectFileType = m_ProjectFileManager.GetProjectFileType();

	if (ProjectFileType == PROJECT_FILE_TYPE_RELATIVE) {
		PathCombine(szFilePath, szProjectFilePath, pProjectFileInfo->GetFileSrcPath());
		PathCombine(szDestFilePath, szProjectFilePath, pProjectFileInfo->GetFileDestPath());
	} else if (ProjectFileType == PROJECT_FILE_TYPE_TOOLS) {
		TCHAR szModuleFileName[MAX_PATH];
		GetModuleFileName(NULL, szModuleFileName, _countof(szModuleFileName));
		PathRemoveFileSpec(szModuleFileName);

		PathCombine(szFilePath, szModuleFileName, pProjectFileInfo->GetFileSrcPath());
		PathCombine(szDestFilePath, szModuleFileName, pProjectFileInfo->GetFileDestPath());
	} else {
		_tcscpy_s(szFilePath, _countof(szFilePath), pProjectFileInfo->GetFileSrcPath());
		_tcscpy_s(szDestFilePath, _countof(szDestFilePath), pProjectFileInfo->GetFileDestPath());
	}

	TCHAR szSrcFile[MAX_PATH], szDestFile[MAX_PATH];

	// 获取源文件路径
	if (PathIsDirectory(szFilePath) == FILE_ATTRIBUTE_DIRECTORY)	//如果是目录，则添加文件名
		PathCombine(szSrcFile , szFilePath , pProjectFileInfo->GetFileName());
	else
		_tcscpy_s(szSrcFile , _countof(szSrcFile) , szFilePath);

	strSrcFile = szSrcFile;
	// 获取目标文件路径
	CString strDestName = pProjectFileInfo->GetDestFileName();
	if (strDestName.IsEmpty())
		strDestName = pProjectFileInfo->GetFileName();

	if (PathIsDirectory(szDestFilePath) == FILE_ATTRIBUTE_DIRECTORY)	//如果是目录，则添加文件名
		PathCombine(szDestFile , szDestFilePath , strDestName);
	else
		_tcscpy_s(szDestFile , _countof(szDestFile) , szDestFilePath);
	strDestFile = szDestFile;
}

void CProjectFile::LoadProjectFileList(LPCTSTR lpszProjectFilePath)
{
	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("FileList"));
	if (pList->GetCount() != 0)
	{
		ReleaseFileItemTagObject(pList);
		pList->RemoveAll();
	}

	TListInfoUI* pListInfo = pList->GetListInfo();
	IMarkup* pMarkup = m_ProjectFileBuilder.Create(lpszProjectFilePath, &m_ProjectFileManager);
	while(pMarkup)
	{
		CreateProjectFileItem(pList, pListInfo, pMarkup);
		pMarkup = pMarkup->GetNext();
	}
}

LRESULT CProjectFile::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT bRes = FALSE;
	bHandled = TRUE;
	switch (uMsg)
	{
		case WM_MENUCLICK:
				bRes = OnMenuClick(wParam, lParam);
			break;
		default:
			bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

LRESULT CProjectFile::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = FALSE;
	if (wParam == VK_F5)
	{
		bHandled = TRUE;
		CString strProjectFilePath = m_ProjectFileManager.GetProjectFilePath();
		if (strProjectFilePath.IsEmpty())
			return lRes;
		LoadProjectFileList(strProjectFilePath);
		return lRes;
	} else
		return WindowImplBase::OnKeyDown(uMsg, wParam, lParam, bHandled);
}

LRESULT CProjectFile::OnMenuClick(WPARAM wParam, LPARAM lParam)
{
	CControlUI* pControl = (CControlUI*)lParam;
	ASSERT(pControl);
	if (pControl == NULL)
		return FALSE;

	//获取菜单对应根节点的控件名称
	CDuiString strMenuName = pControl->GetManager()->GetRoot()->GetName();
	if (strMenuName == _T("History"))
	{
		CDuiString strSenderName = pControl->GetText();
		CString strProjectPath;
		m_pHistory->GetHistoryInfo(strSenderName, strProjectPath);
		if (PathFileExists(strProjectPath) == FALSE)
		{
			MessageBox(m_hWnd, _T("项目文件不存在，请确认后重新选择^_^"), _T("提示"), MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}

		LoadProjectFileList(strProjectPath);
	}

	return TRUE;
}

void CProjectFile::AddToHistory(LPCTSTR lpszProjectName, LPCTSTR szProjectFilePath)
{
	m_pHistory->AddToHistory(lpszProjectName, szProjectFilePath);
}