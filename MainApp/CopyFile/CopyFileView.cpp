// CopyFileView.cpp : CCopyFileView 类的实现
//

#include "stdafx.h"
#include "CopyFile.h"

#include "CopyFileDoc.h"
#include "CopyFileView.h"
#include "LsCommon/ErrorInfo.h"
#include "LsCommon/FuncCallBack.h"
#include "LsCommon/CommonFunc.h"
#include "LsCommon/TextTools.h"
#include <uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI ThreadProcCopyFile(LPVOID lParam);
DWORD WINAPI ThreadCopyFileByMenu(LPVOID lParam);
//int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
//

FINDREPLACE FindReplace;
TCHAR szFindItemName[MAX_PATH];

const int WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

UINT uMessID = 0;

IMPLEMENT_DYNCREATE(CCopyFileView, CListView)

BEGIN_MESSAGE_MAP(CCopyFileView, CListView)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_LISTVIEW_EDIT, &CCopyFileView::OnListviewEdit)
	ON_COMMAND(ID_LISTVIEW_DELETE, &CCopyFileView::OnListviewDelete)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnHeaderClick)
	ON_NOTIFY_REFLECT(NM_RCLICK,OnRClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK,OnLButtonDblClk)
	ON_COMMAND(ID_MENU_OPEN_PATH, &CCopyFileView::OnMenuLookupTargetFile)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw) 
	ON_COMMAND(ID_MENU_FILE_PROPERTY, &CCopyFileView::OnMenuFileProperty)
	ON_COMMAND(ID_MENU_LOOKUP_FILE, &CCopyFileView::OnMenuLookupFile)
	ON_COMMAND(ID_MENU_REFRESH, &CCopyFileView::OnMenuRefresh)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE,&CCopyFileView::FindItemName)
	ON_COMMAND(ID_COPY_ALL_SELECTED, &CCopyFileView::OnCopyAllSelected)
END_MESSAGE_MAP()

// CCopyFileView 构造/析构

CCopyFileView::CCopyFileView()
{
	// TODO: 在此处添加构造代码
	m_nLastSortSubItem = 0;
	m_bRefreshByKey = FALSE;
	m_ImgList.Create(16,16,ILC_COLOR32|ILC_MASK,10,10);
}

int CCopyFileView::OnCreate(LPCREATESTRUCT lpcs)
{
	CWnd::DragAcceptFiles();
	CListView::OnCreate(lpcs);

	return TRUE;
}

CCopyFileView::~CCopyFileView()
{
	if (m_ImgList.GetImageCount()>0)
		m_ImgList.DeleteImageList();
}

BOOL CCopyFileView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CCopyFileView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
	//  从而可以用项填充 ListView。
	CListCtrl& ListCtrl = GetListCtrl();

	SetWindowTheme(m_hWnd,L"Explorer",NULL);
	ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
	ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP
		|LVS_EX_INFOTIP|LVS_EX_DOUBLEBUFFER|LVS_EX_SINGLEROW);

	if (m_ImgList.GetImageCount()>0)
		m_ImgList.DeleteImageList();

	m_ImgList.Add(LoadIcon(NULL,IDI_APPLICATION));

	InitListView();
}

//取得配置文件路径
void CCopyFileView::GetIniFilePath(TCHAR* szIniFilePath)
{
	GetModuleFileName(NULL , szIniFilePath ,MAX_PATH);

	PathRenameExtension(szIniFilePath,_T(".ini"));
}

//设置源文件信息
void CCopyFileView::SetOriginalItemText(int iItemIndex , LPCTSTR strFilePath)
{
	CFileStatus FileStatusInfo;
	if(GetFileStatus(strFilePath , FileStatusInfo) == FALSE)
		return;
	CString strFileInfo;
	strFileInfo.Format(_T("%4d/%02d/%02d %02d:%02d:%02d") ,
		FileStatusInfo.m_mtime.GetYear() ,
		FileStatusInfo.m_mtime.GetMonth() ,
		FileStatusInfo.m_mtime.GetDay() ,
		FileStatusInfo.m_mtime.GetHour() ,
		FileStatusInfo.m_mtime.GetMinute() ,
		FileStatusInfo.m_mtime.GetSecond()
		);
	GetListCtrl().SetItemText(iItemIndex , 3 , strFileInfo);

	//GetFileSizeAsString(FileStatusInfo.m_size , strFileInfo);
	TCHAR szFileSize[MAX_PATH];
	LPCTSTR lpszFileSize = StrFormatByteSize64(FileStatusInfo.m_size,szFileSize,_countof(szFileSize));
	GetListCtrl().SetItemText(iItemIndex , 4 , lpszFileSize);
}

//设置目标文件信息
void CCopyFileView::SetTargetItemText(int iItemCount , LPCTSTR strFilePath)
{
	CFileStatus FileStatusInfo;
	if(GetFileStatus(strFilePath , FileStatusInfo) == FALSE)
		return;
	CString strFileInfo;
	strFileInfo.Format(_T("%4d/%02d/%02d %02d:%02d:%02d") ,
		FileStatusInfo.m_mtime.GetYear() ,
		FileStatusInfo.m_mtime.GetMonth() ,
		FileStatusInfo.m_mtime.GetDay() ,
		FileStatusInfo.m_mtime.GetHour() ,
		FileStatusInfo.m_mtime.GetMinute() ,
		FileStatusInfo.m_mtime.GetSecond()
		);
	//目标文件时间
	GetListCtrl().SetItemText(iItemCount , 5 , strFileInfo);

	TCHAR szFileSize[MAX_PATH];
	LPCTSTR lpszFileSize = StrFormatByteSize64(FileStatusInfo.m_size,szFileSize,_countof(szFileSize));
	//GetFileSizeAsString(FileStatusInfo.m_size , strFileInfo);
	//目标文件大小
	GetListCtrl().SetItemText(iItemCount , 6 , lpszFileSize);
}

BOOL CCopyFileView::GetFileStatus(LPCTSTR strFilePath , CFileStatus& FileStatusInfo)
{
	if (PathFileExists(strFilePath) == FALSE)
		return FALSE;

	return CFile::GetStatus(strFilePath , FileStatusInfo);
}

/************************************************************************/
/* 函数名：IsFileExist                                                                     */
/* 作   用：判断传入的目录是否存在，如果不存在自动创建					*/
/* 参数一：保存操作信息																	*/
/* 参数二：当前目录                                                                       */
/* 参数三：要创建目录的指定路径，相对于当前目录							*/
/* 返回值：该函数无返回值，返回信息都存放在参数一中					*/
/************************************************************************/
void CCopyFileView::FileExists(		LPCTSTR strCurrentDir , 
							   LPCTSTR strDesFilePath ,
							   CString& strCopyInfo 	
							   )
{
	if (PathFileExists(strDesFilePath) == FALSE)
	{
		strCopyInfo=_T("目标目录不存在，自动创建。");

		TCHAR szDecFilePath[MAX_PATH];
		PathCombine(szDecFilePath , strCurrentDir , strDesFilePath);
		int iRetVal = SHCreateDirectoryEx(m_hWnd , szDecFilePath , NULL);
		if (iRetVal != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("创建目录出错"));
			strCopyInfo += GetThreadErrorInfoString();
		}
	}
}

/************************************************************************/
/* 函数名：CopyFileToDirectory                                                      */
/* 功   能：拷贝文件，如果拷贝失败返回错误信息								*/
/* 参数一：保存操作信息																	*/
/* 参数二：源文件路径																	*/
/*	参数三：目标文件路径																*/
/* 返回值：成功返回非0，失败0														*/
/************************************************************************/

BOOL CCopyFileView::CopyFileToDirectory(	LPCTSTR strSrcPath , 
										LPCTSTR strDesFilePath ,
										CString& strCopyInfo , 
										int iItemIndex
										)
{
	BOOL iCopyFaild=FALSE;

	if(::CopyFile(strSrcPath , strDesFilePath , FALSE) == FALSE )
	{
		if (_tcsicmp(strSrcPath , strDesFilePath) == FALSE)
			strCopyInfo +=_T("目标和源相同，终止复制");
		else
		{
			SetErrorInfo(SYSTEM_ERROR , GetLastError() , _T("复制失败"));
			strCopyInfo += GetThreadErrorInfoString();
		}
	}
	else
	{
		strCopyInfo+=_T(" 文件拷贝成功。");
		iCopyFaild = TRUE;
	}

	return iCopyFaild;
}

//响应拖拽文件
void CCopyFileView::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFilePathName[MAX_PATH];
	UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0); //文件的个数
	CListCtrl& ListCtrl = GetListCtrl();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	TCHAR szTargetFilePath[MAX_PATH];
	pMainFrame->GetTargetFilePath(szTargetFilePath , sizeof(szTargetFilePath));
	for ( UINT nIndex=0 ; nIndex< nNumOfFiles; ++nIndex )
	{
		//拒绝文件夹
		DragQueryFile(hDropInfo, nIndex, szFilePathName, sizeof(szFilePathName)); //得到文件路径
		if(PathIsDirectory(szFilePathName) == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		InsertListItem(szFilePathName , szTargetFilePath);	//添加一条记录
	}

	DragFinish(hDropInfo);

	pMainFrame->ChangeModifyStatus(MODIFY_NOT_SAVE);
}

//鼠标左键在列表中的有效双击
void CCopyFileView::OnLButtonDblClk(NMHDR* pNMHDR,LPARAM* pResult)
{
	NMLISTVIEW* pLVCD = reinterpret_cast<NMLISTVIEW*>( pNMHDR ); 

	if (pLVCD->iItem >=0)
		OpenDesFilePath(pLVCD->iItem);
}

//打开指定目标目录所在文件夹
BOOL CCopyFileView::OpenDesFilePath(int iItemIndex)
{
	CString strDesFilePath;
	strDesFilePath = GetListCtrl().GetItemText(iItemIndex , 2);
	if(strDesFilePath.IsEmpty())	//如果是存放到当前目录，怎么办
		return FALSE;
	ShellExecute(m_hWnd,_T("Open"),strDesFilePath,_T("explorer.exe"),NULL,SW_SHOW);
	return TRUE;
}

//检测点击是否有效，无效返回-1，
int CCopyFileView::GetCheckLine(UINT nFlags, CPoint point)
{
	GetListCtrl().SetSelectionMark(-1);
	int i = SendMessage(WM_LBUTTONDOWN , nFlags , MAKELPARAM(point.x,point.y));
	int iItemIndex = GetListCtrl().GetSelectionMark();
	if(iItemIndex ==-1)
		return -1;
	else
		return iItemIndex;
}
//鼠标右键点击
void CCopyFileView::OnRClick(NMHDR* pNMHdr,LPARAM* lResult)
{
	NMLISTVIEW* pLVCD = (NMLISTVIEW*)pNMHdr;
	if (pLVCD->iItem < 0 )
		return;

	CMenu Menu;
	Menu.LoadMenu(IDR_LIST_MENU);
	CMenu* pMenu;
	pMenu = Menu.GetSubMenu(1);

	pMenu->EnableMenuItem(ID_MENU_OPEN_PATH,MF_ENABLED);
	SetForegroundWindow();

	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

// CCopyFileView 诊断

#ifdef _DEBUG
void CCopyFileView::AssertValid() const
{
	CListView::AssertValid();
}

void CCopyFileView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CCopyFileDoc* CCopyFileView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCopyFileDoc)));
	return (CCopyFileDoc*)m_pDocument;
}
#endif //_DEBUG


// CCopyFileView 消息处理程序

//编辑列表框中目标目录
void CCopyFileView::OnListviewEdit()
{
	// TODO: 在此添加命令处理程序代码
	LPITEMIDLIST pDlist;
	BROWSEINFO BrowseInfo;

	ZeroMemory(&BrowseInfo, sizeof(BrowseInfo));

	BrowseInfo.hwndOwner = m_hWnd;
	BrowseInfo.lpszTitle = _T("选择一个文件夹作为目标目录: ");
	BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	TCHAR szModulePath[MAX_PATH];
	GetModuleFileName(NULL , szModulePath , sizeof(szModulePath));
	PathRemoveFileSpec(szModulePath);

	pDlist = SHBrowseForFolder(&BrowseInfo);
	if (pDlist)
	{
		TCHAR szFolderPath[MAX_PATH];
		SHGetPathFromIDList(pDlist , szFolderPath);
		CListCtrl& ListCtrl = GetListCtrl();
		int iTotalItem = ListCtrl.GetItemCount();
		TCHAR szOutFolder[MAX_PATH];
		BOOL bSuccess = PathRelativePathTo(
			szOutFolder,
			szModulePath,
			FILE_ATTRIBUTE_DIRECTORY,
			szFolderPath,
			FILE_ATTRIBUTE_DIRECTORY);
		if (bSuccess)
			_tcscpy_s(szFolderPath,szOutFolder);

		for (int iItem=0; iItem<iTotalItem ;++iItem)
		{
			if(ListCtrl.GetItemState(iItem , LVIS_SELECTED))
				ListCtrl.SetItemText(iItem,2,szFolderPath);
		}
		TCHAR szIniFilePath[MAX_PATH];
		GetIniFilePath(szIniFilePath);
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->SetTargetFilePath(szFolderPath,szIniFilePath);
		pMainFrame->ChangeModifyStatus(MODIFY_NOT_SAVE);
	}
}

//删除一条信息
void CCopyFileView::OnListviewDelete()
{
	// TODO: 在此添加命令处理程序代码
	CListCtrl& ListCtrl = GetListCtrl();
	int iSelectItem = ListCtrl.GetItemCount();
	do 
	{
		if(ListCtrl.GetItemState(iSelectItem , LVIS_SELECTED))
			ListCtrl.DeleteItem(iSelectItem);
		--iSelectItem;
	} while (iSelectItem>=0);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->ChangeModifyStatus(MODIFY_NOT_SAVE);
}

//点击列表框头，激活排序
BOOL CCopyFileView::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pLV = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
	CListCtrl& ListCtrl = GetListCtrl();
	int iSubItem = pLV->iSubItem;

	LIST_SORT_ITEM_INFO ListSortItem;

	HDITEM hditem={0};
	hditem.mask = HDI_FORMAT|HDI_LPARAM;
	ListCtrl.GetHeaderCtrl()->GetItem( iSubItem, &hditem );

	if (m_bRefreshByKey == TRUE)
		ListSortItem.bSortAscending = hditem.fmt;
	else if (m_nLastSortSubItem == iSubItem)
		ListSortItem.bSortAscending = (hditem.fmt & HDF_SORTUP) == 0 ? TRUE : FALSE;
	else
		ListSortItem.bSortAscending = TRUE;

	ListSortItem.hListWnd = m_hWnd;
	ListSortItem.nDataType= (LIST_CTRL_COLUMN_FORMAT)hditem.lParam;
	ListSortItem.nSortColumnId = iSubItem;

	ListCtrl.SortItemsEx(&ListCompare , (LPARAM)&ListSortItem);

	m_nLastSortSubItem = iSubItem;
	int nItemCount = ListCtrl.GetHeaderCtrl()->GetItemCount();

	for (int iIndex=0; iIndex<nItemCount; ++iIndex)
	{
		ListCtrl.GetHeaderCtrl()->GetItem( iIndex, &hditem );

		hditem.fmt &= ~(HDF_SORTDOWN|HDF_SORTUP);
		if(iIndex == iSubItem)
			hditem.fmt |= ListSortItem.bSortAscending ? HDF_SORTUP:HDF_SORTDOWN;
		ListCtrl.GetHeaderCtrl()->SetItem( iIndex, &hditem );
	}

	return TRUE;	
}
/*
void CCopyFileView::OnMenuOpenPath()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos;
	pos = GetListCtrl().GetFirstSelectedItemPosition();
	while(pos)
	{
		int iItemIndex = GetListCtrl().GetNextSelectedItem(pos);
		OpenDesFilePath(iItemIndex);
	}
}
*/
//拷贝文件并填写响应信息
DWORD WINAPI ThreadProcCopyFile(LPVOID lParam)
{
	CMainFrame* pMainFrame = (CMainFrame*)lParam;
	CCopyFileView* pCopyView = (CCopyFileView*)pMainFrame->GetActiveView();
	CListCtrl& ListCtrl = pCopyView->GetListCtrl();

	int iItemCount=0,iItemFaild=0,iTemNoCopy=0;
	int iItemTotal = ListCtrl.GetItemCount();

	for (int iIndex = 0; iIndex<iItemTotal; ++iIndex)
	{
		CString strCopyInfo;
		TCHAR szSrcPath[MAX_PATH],szTargetFilePath[MAX_PATH];
		if (pCopyView->GetSrcAndTargetFilePath(iIndex,szSrcPath,szTargetFilePath,strCopyInfo) == FALSE)
			return 0;

		//拷贝文件
		int nCopyCondition = pCopyView->CopyFileCondition(szSrcPath , szTargetFilePath);
		if (nCopyCondition & NEED_COPY_FILE)	//目标文件最新且有效，不需要覆盖
		{
			if(pCopyView->CopyFileToDirectory(szSrcPath , szTargetFilePath , strCopyInfo , iItemCount) == FALSE)
				++iItemFaild;

			//拷贝结果
			ListCtrl.SetItemText(iItemCount , 7 , strCopyInfo);
		}
		if (nCopyCondition & FIRST_IS_NEW || nCopyCondition & SECOND_IS_NEW)
		{
			CString strCopyInfo;
			if ((nCopyCondition & FIRST_IS_NEW) == FIRST_IS_NEW)
				strCopyInfo.Format(_T("源文件是最新文件，需要复制"));
			else
				strCopyInfo.Format(_T("目标文件是最新文件，不需要复制"));

			ListCtrl.SetItemText(iItemCount , 7 , strCopyInfo);
		}
		if ((nCopyCondition & SECOND_IS_NEW) != SECOND_IS_NEW)
			++iTemNoCopy;

		CString strSrcFileLastModifyTime = ListCtrl.GetItemText(iItemCount,3);
		if (strSrcFileLastModifyTime.IsEmpty() == TRUE)
			pCopyView->SetTargetItemText(iItemCount,szSrcPath);

		CString strTargetFileLastModifyTime = ListCtrl.GetItemText(iItemCount , 5);
		if (strTargetFileLastModifyTime.IsEmpty() == TRUE)
			pCopyView->SetTargetItemText(iItemCount,szTargetFilePath);

		++iItemCount;
	}

	CString strCopyInfo;
	CString strStatusInfo;
	if (iTemNoCopy==0)
		strCopyInfo = _T("所有文件不需要复制");
	else
		strCopyInfo.Format(_T("%d个文件需要复制"),iTemNoCopy);
	strStatusInfo.Format(_T("统计：总共执行%d个任务，成功%d个任务，%s，失败%d个任务") , iItemCount , iItemCount-iItemFaild , strCopyInfo , iItemFaild);

	TCHAR szStatusInfo[MAX_PATH];
	_tcscpy_s(szStatusInfo,strStatusInfo);
	pMainFrame->SendMessage(WM_UPDATE_STSTUS , (WPARAM)szStatusInfo , 0);

	AdjustListWidth(ListCtrl);
	ListCtrl.RedrawWindow();
	return TRUE;
}

DWORD WINAPI ThreadCopyFileByMenu(LPVOID lParam)
{
	CMainFrame* pMainFrame = (CMainFrame*)lParam;
	CCopyFileView* pCopyView = (CCopyFileView*)pMainFrame->GetActiveView();
	CListCtrl& ListCtrl = pCopyView->GetListCtrl();

	int iItemCount=0,iItemFaild=0,iTemNoCopy=0;
	int iItemTotal = ListCtrl.GetItemCount();

	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();

	while(pos)
	{
		int iIndex = ListCtrl.GetNextSelectedItem(pos);
		CString strCopyInfo;
		TCHAR szSrcPath[MAX_PATH],szTargetFilePath[MAX_PATH];
		if (pCopyView->GetSrcAndTargetFilePath(iIndex,szSrcPath,szTargetFilePath,strCopyInfo) == FALSE)
			return 0;

		//拷贝文件
		int nCopyCondition = pCopyView->CopyFileCondition(szSrcPath , szTargetFilePath);
		if (nCopyCondition & NEED_COPY_FILE)	//目标文件最新且有效，不需要覆盖
		{
			if(pCopyView->CopyFileToDirectory(szSrcPath , szTargetFilePath , strCopyInfo , iIndex) == FALSE)
				++iItemFaild;

			//拷贝结果
			ListCtrl.SetItemText(iIndex , 7 , strCopyInfo);
		}
		if (nCopyCondition & FIRST_IS_NEW || nCopyCondition & SECOND_IS_NEW)
		{
			CString strCopyInfo;
			if ((nCopyCondition & FIRST_IS_NEW) == FIRST_IS_NEW)
				strCopyInfo.Format(_T("源文件是最新文件，需要复制"));
			else
				strCopyInfo.Format(_T("目标文件是最新文件，不需要复制"));

			ListCtrl.SetItemText(iIndex , 7 , strCopyInfo);
		}
		if ((nCopyCondition & SECOND_IS_NEW) != SECOND_IS_NEW)
			++iTemNoCopy;

		CString strSrcFileLastModifyTime = ListCtrl.GetItemText(iIndex,3);
		if (strSrcFileLastModifyTime.IsEmpty() == TRUE)
			pCopyView->SetTargetItemText(iIndex,szSrcPath);

		CString strTargetFileLastModifyTime = ListCtrl.GetItemText(iIndex , 5);
		if (strTargetFileLastModifyTime.IsEmpty() == TRUE)
			pCopyView->SetTargetItemText(iIndex,szTargetFilePath);

		++iItemCount;
	}

	CString strCopyInfo;
	CString strStatusInfo;
	if (iTemNoCopy==0)
		strCopyInfo = _T("所有文件不需要复制");
	else
		strCopyInfo.Format(_T("%d个文件需要复制"),iTemNoCopy);
	strStatusInfo.Format(_T("统计：总共执行%d个任务，成功%d个任务，%s，失败%d个任务") , iItemCount , iItemCount-iItemFaild , strCopyInfo , iItemFaild);

	TCHAR szStatusInfo[MAX_PATH];
	_tcscpy_s(szStatusInfo,strStatusInfo);
	pMainFrame->SendMessage(WM_UPDATE_STSTUS , (WPARAM)szStatusInfo , 0);

	AdjustListWidth(ListCtrl);
	ListCtrl.RedrawWindow();
	return TRUE;
}

//比较两个数的大小
//int GetCompareByNumberic(const LARGE_INTEGER& LarFirst,const LARGE_INTEGER& LarSecond)
//{
//	if (LarFirst.QuadPart-LarSecond.QuadPart > 0)
//		return 1;
//	else if (LarFirst.QuadPart-LarSecond.QuadPart == 0)
//		return 0;
//	else
//		return -1;
//}

//BOOL GetFileTime(LPCTSTR strTime,SYSTEMTIME& systime)
//{
//	ZeroMemory(&systime,sizeof(systime));
//	_stscanf_s(strTime,_T("%d/%d/%d %d:%d:%d"),&systime.wYear,&systime.wMonth,&systime.wDay,&systime.wHour,&systime.wMinute,&systime.wSecond);
//	return TRUE;
//}

//BOOL GetFileSize(LPCTSTR strFilePath,CFileStatus& FileStatusInfo)
INT64 GetFileSize(const CString& strFileSize)
{
	INT64 iFileSize = _ttoi64(strFileSize);
	if (strFileSize.IsEmpty())		//如果内容为空
		return -1;
	TRACE(_T("%s"),strFileSize.Right(2));
	if (strFileSize.Right(2).Compare(_T("KB")) == NULL)
		return iFileSize * 1024;
	else if (strFileSize.Right(2).Compare(_T("MB")) == NULL)
		return iFileSize * 1024 * 1024;
	else if (strFileSize.Right(2).Compare(_T("GB")) == NULL)
		return iFileSize * 1024 * 1024;
	else //不考虑超过GB的单文件
		return iFileSize;
}
/*
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	LIST_SORT_ITEM_INFO* pListSortInfo = (LIST_SORT_ITEM_INFO*) lParamSort;
	CListCtrl* pListCtrl = (CListCtrl*)CWnd::FromHandle(pListSortInfo->hListWnd);

	CString strItemPrev,strItemNext;
	int iSubItem = pListSortInfo->nSortColumnId;

	strItemPrev = pListCtrl->GetItemText(lParam1,iSubItem);
	strItemNext = pListCtrl->GetItemText(lParam2,iSubItem);

	int AscOrder;
	AscOrder = pListSortInfo->bSortAscending == 0 ? -1 : 1 ;
	LARGE_INTEGER LarFirst,LarSecond;
	switch (pListSortInfo->nDataType)
	{
	case STRING_TEXT:
		return _tcsicmp(strItemPrev , strItemNext) * AscOrder;
	case FILE_TIME:
		SYSTEMTIME SystemTime;

		FILETIME FileTimePrev,FileTimeNext;
		memset(&FileTimePrev,0,sizeof(FileTimePrev));
		memset(&FileTimeNext,0,sizeof(FileTimeNext));
		if (strItemPrev.IsEmpty() == FALSE)
		{
			GetFileTime(strItemPrev,SystemTime);
			SystemTimeToFileTime(&SystemTime,&FileTimePrev);
		}

		if (strItemNext.IsEmpty() == FALSE)
		{
			GetFileTime(strItemNext,SystemTime);
			SystemTimeToFileTime(&SystemTime,&FileTimeNext);
		}

		LarFirst.LowPart = FileTimePrev.dwLowDateTime;
		LarFirst.HighPart = FileTimePrev.dwHighDateTime;

		LarSecond.LowPart =FileTimeNext.dwLowDateTime;
		LarSecond.HighPart = FileTimeNext.dwHighDateTime;

		return GetCompareByNumberic(LarFirst,LarSecond) * AscOrder;
	case FILE_SIZE:
		LarFirst.QuadPart = GetFileSize(strItemPrev);
		LarSecond.QuadPart = GetFileSize(strItemNext);
		return GetCompareByNumberic(LarFirst,LarSecond) * AscOrder;
	default:
		return -1;
	}
}
*/
void CCopyFileView::CopyFileToDestation()
{
	HANDLE hThread;
	hThread = CreateThread(NULL , NULL , ThreadProcCopyFile , AfxGetMainWnd() , 0 ,NULL);
	if(hThread)
		CloseHandle(hThread);
}

void CCopyFileView::ShowUpdate(LPCTSTR lpszTargetFilePath)
{
	CListCtrl& ListCtrl = GetListCtrl();

	int nTotal = ListCtrl.GetItemCount();
	for (int i=0 ; i<nTotal ; ++i)
	{
		ListCtrl.SetItemText(i,2,lpszTargetFilePath);
	}
}

BOOL CCopyFileView::CompareFileProperty(LPCTSTR lpszSrcFile,LPCTSTR lpszDesFile)
{
	BOOL bSuccess = FALSE;

	do 
	{
		CFileStatus FileSrcStatus,FileDesStatus;
		if (CFile::GetStatus(lpszSrcFile , FileSrcStatus) == FALSE)
			break;
		if (CFile::GetStatus(lpszDesFile,FileDesStatus) == FALSE)
			break;
		if (FileSrcStatus.m_size != FileDesStatus.m_size)
			break;
		if (FileSrcStatus.m_mtime != FileDesStatus.m_mtime)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

int CCopyFileView::CopyFileCondition(LPCTSTR lpszSrcFile , LPCTSTR lpszDesFile)
{
	TCHAR szIniFilePath[MAX_PATH];
	GetIniFilePath(szIniFilePath);
	UINT nCoverState = GetPrivateProfileInt(_T("BaseInfo"),_T("CoverWhenEver"),0,szIniFilePath);		//获取拷贝文件是否覆盖操作

	if (nCoverState==0)
		return NEED_COPY_FILE;

	if (PathFileExists(lpszDesFile) == FALSE)
		return NEED_COPY_FILE;

	FILETIME SrcLastWriteFileTime , DesLastWriteFileTime;
	HANDLE hFileSrc , hFileDes;

	hFileSrc = CreateFile(lpszSrcFile , GENERIC_READ , FILE_SHARE_READ , NULL ,OPEN_EXISTING , 0 , NULL);
	hFileDes = CreateFile(lpszDesFile , GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , 0 , NULL);
	if (hFileDes == INVALID_HANDLE_VALUE || hFileSrc == INVALID_HANDLE_VALUE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开源文件或目标文件失败"));
		return NEED_COPY_FILE;
	}

	GetFileTime(hFileSrc , NULL ,NULL , &SrcLastWriteFileTime);
	GetFileTime(hFileDes , NULL , NULL , &DesLastWriteFileTime);

	CloseHandle(hFileSrc);
	CloseHandle(hFileDes);

	LARGE_INTEGER LarIntSrc;
	LARGE_INTEGER LarIntDes;
	LarIntSrc.LowPart  = SrcLastWriteFileTime.dwLowDateTime;
	LarIntSrc.HighPart = SrcLastWriteFileTime.dwHighDateTime;

	LarIntDes.LowPart = DesLastWriteFileTime.dwLowDateTime;
	LarIntDes.HighPart = DesLastWriteFileTime.dwHighDateTime;

	return LarIntSrc.QuadPart>LarIntDes.QuadPart ? FIRST_IS_NEW | NEED_COPY_FILE : SECOND_IS_NEW;		//保留最新的文件
}

BOOL CCopyFileView::InsertListItem(LPCTSTR lpszSrcFilePath , LPCTSTR lpszTargetFilePath)
{
	CListCtrl& ListCtrl = GetListCtrl();
	CString strFilePath(lpszSrcFilePath);

	UINT nIconIndex = InsertIconToImageList(strFilePath);
	int iItemIndex = ListCtrl.InsertItem(0x7FFF , PathFindFileName(lpszSrcFilePath) , nIconIndex);		//插入文件名

	if (lpszTargetFilePath != NULL)
		ListCtrl.SetItemText(iItemIndex , 2 , lpszTargetFilePath);

	TCHAR szCurrentDir[MAX_PATH];
	GetModuleFileName(NULL,szCurrentDir,sizeof(szCurrentDir));
	PathRemoveFileSpec(szCurrentDir);

	DWORD dwFileAttribute = FILE_ATTRIBUTE_DIRECTORY;

	TCHAR szRelativeFilePath[MAX_PATH];
	BOOL bSuccess = PathRelativePathTo(szRelativeFilePath,
		szCurrentDir,
		FILE_ATTRIBUTE_DIRECTORY,
		strFilePath,
		FILE_ATTRIBUTE_DIRECTORY);

	if(bSuccess)
		ListCtrl.SetItemText(iItemIndex , 1 , szRelativeFilePath);
	else
		ListCtrl.SetItemText(iItemIndex , 1 , strFilePath);

	return TRUE;
}

UINT CCopyFileView::InsertIconToImageList(LPCTSTR lpszSrcFilePath)
{
	if (PathFileExists(lpszSrcFilePath) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("文件%s不存在"),lpszSrcFilePath);
		return FALSE;
	}

	if (PathIsDirectory(lpszSrcFilePath) == FILE_ATTRIBUTE_DIRECTORY)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("路径%s是一个目录"),lpszSrcFilePath);
		return FALSE;
	}

	SHFILEINFO ShFileInfo;
	memset(&ShFileInfo , 0 , sizeof(SHFILEINFO));
	DWORD dwRet = SHGetFileInfo(lpszSrcFilePath , NULL , &ShFileInfo , sizeof(SHFILEINFO) , SHGFI_ICON|SHGFI_SMALLICON);
	if (dwRet == 0)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取文件图标失败"));
		return FALSE;
	}

	m_ImgList.Add(ShFileInfo.hIcon);
	DestroyIcon(ShFileInfo.hIcon);

	return m_ImgList.GetImageCount()-1;
}

void CCopyFileView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)    
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );   
	*pResult = CDRF_DODEFAULT;  
	CListCtrl& ListCtrl = GetListCtrl();

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )  
	{  
		*pResult = CDRF_NOTIFYITEMDRAW;  
	}  
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )  
	{  
		*pResult = CDRF_NOTIFYSUBITEMDRAW;  
	}  
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )  
	{
		COLORREF clrNewTextColor, clrNewBkColor; 
		int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );  
		CString strSrcFilePath = ListCtrl.GetItemText(nItem,1);

		TCHAR szModuleFilePath[MAX_PATH],szSrcFilePath[MAX_PATH],szTargetFilePath[MAX_PATH];
		GetIniFilePath(szModuleFilePath);
		PathRemoveFileSpec(szModuleFilePath);
		PathCombine(szSrcFilePath,szModuleFilePath,strSrcFilePath);

		CString strTargetFilePath(ListCtrl.GetItemText(nItem,2));
		strTargetFilePath.TrimRight(_T("\\"));
		CString strFileName;
		strFileName.Format(_T("%s\\%s"),strTargetFilePath,ListCtrl.GetItemText(nItem,0));

		PathCombine(szTargetFilePath,szModuleFilePath,strFileName);
		int nCopyCondition = CopyFileCondition(szSrcFilePath , szTargetFilePath);

		if (PathFileExists(szSrcFilePath) == FALSE || PathFileExists(szTargetFilePath) == FALSE)	//文件不存在
		{
			clrNewTextColor = RGB(255,0,0);
			clrNewBkColor = RGB(255,255,255);
		}
		else if(CompareFileProperty(szSrcFilePath,szTargetFilePath) == FALSE)	//文件属性不同
		{
			clrNewTextColor = RGB(255,0,0);
			clrNewBkColor = RGB(255,255,255);
		}
		else	//默认拷贝
		{
			clrNewTextColor = RGB(0,0,0);
			clrNewBkColor = RGB(255,255,255);
		}

		pLVCD->clrText = clrNewTextColor;  
		pLVCD->clrTextBk = clrNewBkColor;  

		*pResult = CDRF_DODEFAULT;  
	}  
}
void CCopyFileView::OnMenuFileProperty()
{
	// TODO: 在此添加命令处理程序代码
	CListCtrl& ListCtrl = GetListCtrl();
	int nSelectColumn = ListCtrl.GetSelectionMark();
	TCHAR szFilePath[MAX_PATH],szModuleFilePath[MAX_PATH];
	GetIniFilePath(szModuleFilePath);
	PathRemoveFileSpec(szModuleFilePath);
	CString strFile = ListCtrl.GetItemText(nSelectColumn,1);
	PathCombine(szFilePath,szModuleFilePath,strFile);

	if (PathFileExists(szFilePath) == FALSE)
	{
		CString strTipInfo;
		strTipInfo.Format(_T("文件%s不存在！"),szFilePath);
		MessageBox(strTipInfo,_T("提示"),MB_OK);
		return;
	}

	SHELLEXECUTEINFO ShellInfo; 
	//ZeroMemory(&i,sizeof(i)); 
	ShellInfo.fMask=SEE_MASK_INVOKEIDLIST|SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI; 
	ShellInfo.lpVerb=_T("properties"); 
	ShellInfo.lpFile=szFilePath; 
	ShellInfo.lpParameters=0; 
	ShellInfo.lpDirectory=0; 

	ShellInfo.lpIDList=NULL; 
	ShellInfo.cbSize=sizeof(ShellInfo); 
	ShellExecuteEx(&ShellInfo); 
}


BOOL CCopyFileView::OpenFolderAndSelectFile(TCHAR* szFilePath)
{
	if (PathFileExists(szFilePath) == FALSE)
	{
		CString strTipInfo;
		strTipInfo.Format(_T("文件%s不存在！"),szFilePath);
		MessageBox(strTipInfo,_T("提示"),MB_OK);
		return FALSE;
	}

	LPSHELLFOLDER pDesktopFolder;
	if ( SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	{  
		LPITEMIDLIST     pidl;
		ULONG             chEaten;
		ULONG             dwAttributes;
		HRESULT             hr;

		hr = pDesktopFolder->ParseDisplayName(
			NULL, NULL, szFilePath, &chEaten, &pidl, &dwAttributes);
		if (FAILED(hr))
		{
			pDesktopFolder->Release();
			return FALSE;
		}
		LPCITEMIDLIST pidlFolder = pidl;
		CoInitialize( NULL);
		hr = SHOpenFolderAndSelectItems( pidl, 0, NULL, 0);
		pDesktopFolder->Release();
		if ( hr == S_OK)
			return TRUE;
	}
	return FALSE;
}

void CCopyFileView::OnMenuLookupFile()
{
	// TODO: 在此添加命令处理程序代码
	CListCtrl& ListCtrl = GetListCtrl();
	int nSelectColumn = ListCtrl.GetSelectionMark();
	TCHAR szFilePath[MAX_PATH],szModuleFilePath[MAX_PATH];
	GetIniFilePath(szModuleFilePath);
	PathRemoveFileSpec(szModuleFilePath);
	CString strFile = ListCtrl.GetItemText(nSelectColumn,1);
	PathCombine(szFilePath,szModuleFilePath,strFile);

	OpenFolderAndSelectFile(szFilePath);
}

void CCopyFileView::OnMenuLookupTargetFile()
{
	CListCtrl& ListCtrl = GetListCtrl();
	int nSelectColumn = ListCtrl.GetSelectionMark();
	TCHAR szFilePath[MAX_PATH],szModuleFilePath[MAX_PATH],szTargetFilePath[MAX_PATH];
	GetIniFilePath(szModuleFilePath);
	PathRemoveFileSpec(szModuleFilePath);
	CString strFile = ListCtrl.GetItemText(nSelectColumn,2);
	PathCombine(szFilePath,szModuleFilePath,strFile);
	CString strFileName = ListCtrl.GetItemText(nSelectColumn,0);
	PathCombine(szTargetFilePath,szFilePath,strFileName);

	OpenFolderAndSelectFile(szTargetFilePath);
}

BOOL CCopyFileView::GetSrcAndTargetFilePath(int iIndex,LPTSTR lpSrcFilePath,LPTSTR lpTargetFilePath,CString& strCopyInfo)
{
	CString strFileName,strFilePath,strDesFilePath;
	CListCtrl& ListCtrl = GetListCtrl();

	TCHAR szCurrentDir[MAX_PATH];
	GetModuleFileName(NULL,szCurrentDir,sizeof(szCurrentDir));
	PathRemoveFileSpec(szCurrentDir);

	strFileName = ListCtrl.GetItemText(iIndex,0);
	strFilePath = ListCtrl.GetItemText(iIndex,1);
	strDesFilePath = ListCtrl.GetItemText(iIndex ,2);

	//获取目标文件目录
	TCHAR szDesFilePath[MAX_PATH];
	if(strDesFilePath.IsEmpty())
		strDesFilePath =szCurrentDir;		//原值已经为空了直接赋值覆盖
	_tcscpy_s(szDesFilePath,strDesFilePath);
	//将目标目录的相对路径转换为绝对路径，
	//否则拷贝到当前目录的上级目录时pathcombine会出现问题(去掉了上级目录)
	TCHAR szDesFileAbsolutePath[MAX_PATH];
	//_tcscpy_s(szDesFileAbsolutePath)
	PathCombine(szDesFileAbsolutePath , szCurrentDir , szDesFilePath);
	FileExists(szCurrentDir , szDesFileAbsolutePath , strCopyInfo);

	//TCHAR szTargetFilePath[MAX_PATH];
	PathCombine(lpTargetFilePath , szDesFileAbsolutePath , strFileName);	//目标文件的绝对路径

	//获取源文件目录
	if(strFilePath.IsEmpty())
		strFilePath = szCurrentDir;
	//TCHAR szSrcPath[MAX_PATH];
	TCHAR szSrcFileAbsolutePath[MAX_PATH];

	PathCombine(szSrcFileAbsolutePath ,szCurrentDir , strFilePath);
	if (PathIsDirectory(szSrcFileAbsolutePath) == FILE_ATTRIBUTE_DIRECTORY)	//如果是目录，则添加文件名
		PathCombine(lpSrcFilePath , szSrcFileAbsolutePath , strFileName);
	else
		_tcscpy_s(lpSrcFilePath , MAX_PATH , szSrcFileAbsolutePath);

	return TRUE;
}

BOOL CCopyFileView::ShowFileInfo(int iIndex)
{
	TCHAR szSrcFilePath[MAX_PATH],szTargetFilePath[MAX_PATH];
	CString strCopyInfo;
	if (GetSrcAndTargetFilePath(iIndex,szSrcFilePath,szTargetFilePath,strCopyInfo) == FALSE)
		return FALSE;

	SetOriginalItemText(iIndex , szSrcFilePath);
	SetTargetItemText(iIndex , szTargetFilePath);

	return TRUE;
}

void CCopyFileView::UpdateFileInfo()
{
	int nTotal = GetListCtrl().GetItemCount();
	for (int n=0;n<nTotal;++n)
		ShowFileInfo(n);
}

void CCopyFileView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_F5:
		Refresh();
		break;
	//case VK_CONTROL&0x46:
	case VK_F3:
		FindBox();
		break;
	case VK_F2:
		break;
	default:
		CListView::OnKeyDown(nChar,nRepCnt,nFlags);
	}
}

BOOL CCopyFileView::InitListView()
{
	CListCtrl& ListCtrl = GetListCtrl();
	int nColumnCount = ListCtrl.GetHeaderCtrl()->GetItemCount();

	if (nColumnCount == 0)
	{
		ListCtrl.SetImageList(&m_ImgList , LVSIL_SMALL);

		ListCtrl.InsertColumn(0,_T("源文件名"),LVCFMT_LEFT,100);
		ListCtrl.InsertColumn(1,_T("源文件目录"),LVCFMT_LEFT,200);
		ListCtrl.InsertColumn(2,_T("目标文件目录"),LVCFMT_LEFT,200);
		ListCtrl.InsertColumn(3,_T("源文件修改时间"),LVCFMT_LEFT,150);
		ListCtrl.InsertColumn(4,_T("源文件大小"),LVCFMT_RIGHT,80);
		ListCtrl.InsertColumn(5,_T("目标文件修改时间"),LVCFMT_LEFT,150);
		ListCtrl.InsertColumn(6,_T("目标文件大小"),LVCFMT_RIGHT,100);
		ListCtrl.InsertColumn(7,_T("操作结果"),LVCFMT_LEFT,400);

		int nTotalCount = ListCtrl.GetHeaderCtrl()->GetItemCount();

		for (int nIndex=0;nIndex<nTotalCount;++nIndex)
		{
			HDITEM hditem={0};
			hditem.mask = HDI_LPARAM;

			if (nIndex == 3 || nIndex == 5)
				hditem.lParam = FILE_TIME;
			else if (nIndex == 4 || nIndex == 6)
				hditem.lParam = FILE_SIZE;
			else
				hditem.lParam = STRING_TEXT;

			ListCtrl.GetHeaderCtrl()->SetItem(nIndex,&hditem);
		}
	}
	else
		//重新复制时，删除内容
		ListCtrl.DeleteAllItems();

	TCHAR szFilePath[MAX_PATH];

	GetIniFilePath(szFilePath);
	CString strIniFilePath(szFilePath);

	if(PathFileExists(strIniFilePath) == FALSE)
	{
		AfxMessageBox(_T("没有找到对应的配置文件"));
		return FALSE;
	}	

	TCHAR szFileBuff[SHRT_MAX];
	GetPrivateProfileString(NULL , _T("LookUpSection") , NULL , szFileBuff , SHRT_MAX , strIniFilePath);
	TCHAR* pszFileBuff = szFileBuff;

	TCHAR szModuleFilePath[MAX_PATH];
	GetModuleFileName(NULL,szModuleFilePath,MAX_PATH);
	PathRemoveFileSpec(szModuleFilePath);

	SetRedraw(FALSE);

	while(*pszFileBuff)
	{
		TCHAR szSrcFilePath[MAX_PATH];
		TCHAR szDesFilePath[MAX_PATH];
		GetPrivateProfileString(pszFileBuff , _T("SrcPath") , NULL , szSrcFilePath ,MAX_PATH , strIniFilePath);
		GetPrivateProfileString(pszFileBuff , _T("DesPath") , NULL ,szDesFilePath ,MAX_PATH , strIniFilePath);

		TCHAR szTargetFile[MAX_PATH];
		PathCombine(szTargetFile , szModuleFilePath , szSrcFilePath);

		if (PathIsDirectory(szTargetFile))
			PathAppend(szTargetFile,pszFileBuff);

		CString strFileName(pszFileBuff);
		if(strFileName.FindOneOf(_T(".")) !=-1 )
		{
			UINT nIconIndex = InsertIconToImageList(szTargetFile);
			int iIndex = ListCtrl.InsertItem(0x7FFF,pszFileBuff , nIconIndex);
			ListCtrl.SetItemText(iIndex , 1 , szSrcFilePath);
			ListCtrl.SetItemText(iIndex , 2 , szDesFilePath);

			ShowFileInfo(iIndex);			
		}
		//指向下一个节
		pszFileBuff += _tcslen(pszFileBuff) + 1;
	}
	AdjustListWidth(ListCtrl);

	NMLISTVIEW nmHDR;
	memset(&nmHDR,0,sizeof(NMLISTVIEW));
	nmHDR.iSubItem = m_nLastSortSubItem;

	OnHeaderClick((NMHDR*)&nmHDR , NULL);

	SetRedraw(TRUE);
	return TRUE;
}

void CCopyFileView::Refresh()
{
	m_bRefreshByKey = TRUE;
	InitListView();
	m_bRefreshByKey = FALSE;
}

void CCopyFileView::OnMenuRefresh()
{
	// TODO: 在此添加命令处理程序代码
	Refresh();
}

void CCopyFileView::FindBox()
{
	szFindItemName[0] = _T('\0');
	ZeroMemory(&FindReplace,sizeof(FINDREPLACE));
	FindReplace.lStructSize = sizeof(FINDREPLACE);
	FindReplace.hwndOwner = m_hWnd;
	FindReplace.hInstance = NULL;
	FindReplace.Flags = FR_FINDNEXT|FR_DOWN;
	FindReplace.lpstrFindWhat = szFindItemName;
	FindReplace.wFindWhatLen = _countof(szFindItemName);

	FindText(&FindReplace);
}

LRESULT CCopyFileView::FindItemName(WPARAM wParam,LPARAM lParam)
{
	FINDREPLACE* FindReplace = (FINDREPLACE*)lParam;
	
	SerarchTextInList(FindReplace,szFindItemName,GetListCtrl());

	return TRUE;
}
void CCopyFileView::OnCopyAllSelected()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hThread;
	hThread = CreateThread(NULL , NULL , ThreadCopyFileByMenu, AfxGetMainWnd() , 0 ,NULL);
	if(hThread)
		CloseHandle(hThread);
}
