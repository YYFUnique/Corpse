// CopyFileView.cpp : CCopyFileView ���ʵ��
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

// CCopyFileView ����/����

CCopyFileView::CCopyFileView()
{
	// TODO: �ڴ˴���ӹ������
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

void CCopyFileView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: ���� GetListCtrl() ֱ�ӷ��� ListView ���б�ؼ���
	//  �Ӷ������������ ListView��
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

//ȡ�������ļ�·��
void CCopyFileView::GetIniFilePath(TCHAR* szIniFilePath)
{
	GetModuleFileName(NULL , szIniFilePath ,MAX_PATH);

	PathRenameExtension(szIniFilePath,_T(".ini"));
}

//����Դ�ļ���Ϣ
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

//����Ŀ���ļ���Ϣ
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
	//Ŀ���ļ�ʱ��
	GetListCtrl().SetItemText(iItemCount , 5 , strFileInfo);

	TCHAR szFileSize[MAX_PATH];
	LPCTSTR lpszFileSize = StrFormatByteSize64(FileStatusInfo.m_size,szFileSize,_countof(szFileSize));
	//GetFileSizeAsString(FileStatusInfo.m_size , strFileInfo);
	//Ŀ���ļ���С
	GetListCtrl().SetItemText(iItemCount , 6 , lpszFileSize);
}

BOOL CCopyFileView::GetFileStatus(LPCTSTR strFilePath , CFileStatus& FileStatusInfo)
{
	if (PathFileExists(strFilePath) == FALSE)
		return FALSE;

	return CFile::GetStatus(strFilePath , FileStatusInfo);
}

/************************************************************************/
/* ��������IsFileExist                                                                     */
/* ��   �ã��жϴ����Ŀ¼�Ƿ���ڣ�����������Զ�����					*/
/* ����һ�����������Ϣ																	*/
/* ����������ǰĿ¼                                                                       */
/* ��������Ҫ����Ŀ¼��ָ��·��������ڵ�ǰĿ¼							*/
/* ����ֵ���ú����޷���ֵ��������Ϣ������ڲ���һ��					*/
/************************************************************************/
void CCopyFileView::FileExists(		LPCTSTR strCurrentDir , 
							   LPCTSTR strDesFilePath ,
							   CString& strCopyInfo 	
							   )
{
	if (PathFileExists(strDesFilePath) == FALSE)
	{
		strCopyInfo=_T("Ŀ��Ŀ¼�����ڣ��Զ�������");

		TCHAR szDecFilePath[MAX_PATH];
		PathCombine(szDecFilePath , strCurrentDir , strDesFilePath);
		int iRetVal = SHCreateDirectoryEx(m_hWnd , szDecFilePath , NULL);
		if (iRetVal != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("����Ŀ¼����"));
			strCopyInfo += GetThreadErrorInfoString();
		}
	}
}

/************************************************************************/
/* ��������CopyFileToDirectory                                                      */
/* ��   �ܣ������ļ����������ʧ�ܷ��ش�����Ϣ								*/
/* ����һ�����������Ϣ																	*/
/* ��������Դ�ļ�·��																	*/
/*	��������Ŀ���ļ�·��																*/
/* ����ֵ���ɹ����ط�0��ʧ��0														*/
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
			strCopyInfo +=_T("Ŀ���Դ��ͬ����ֹ����");
		else
		{
			SetErrorInfo(SYSTEM_ERROR , GetLastError() , _T("����ʧ��"));
			strCopyInfo += GetThreadErrorInfoString();
		}
	}
	else
	{
		strCopyInfo+=_T(" �ļ������ɹ���");
		iCopyFaild = TRUE;
	}

	return iCopyFaild;
}

//��Ӧ��ק�ļ�
void CCopyFileView::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFilePathName[MAX_PATH];
	UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0); //�ļ��ĸ���
	CListCtrl& ListCtrl = GetListCtrl();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	TCHAR szTargetFilePath[MAX_PATH];
	pMainFrame->GetTargetFilePath(szTargetFilePath , sizeof(szTargetFilePath));
	for ( UINT nIndex=0 ; nIndex< nNumOfFiles; ++nIndex )
	{
		//�ܾ��ļ���
		DragQueryFile(hDropInfo, nIndex, szFilePathName, sizeof(szFilePathName)); //�õ��ļ�·��
		if(PathIsDirectory(szFilePathName) == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		InsertListItem(szFilePathName , szTargetFilePath);	//���һ����¼
	}

	DragFinish(hDropInfo);

	pMainFrame->ChangeModifyStatus(MODIFY_NOT_SAVE);
}

//���������б��е���Ч˫��
void CCopyFileView::OnLButtonDblClk(NMHDR* pNMHDR,LPARAM* pResult)
{
	NMLISTVIEW* pLVCD = reinterpret_cast<NMLISTVIEW*>( pNMHDR ); 

	if (pLVCD->iItem >=0)
		OpenDesFilePath(pLVCD->iItem);
}

//��ָ��Ŀ��Ŀ¼�����ļ���
BOOL CCopyFileView::OpenDesFilePath(int iItemIndex)
{
	CString strDesFilePath;
	strDesFilePath = GetListCtrl().GetItemText(iItemIndex , 2);
	if(strDesFilePath.IsEmpty())	//����Ǵ�ŵ���ǰĿ¼����ô��
		return FALSE;
	ShellExecute(m_hWnd,_T("Open"),strDesFilePath,_T("explorer.exe"),NULL,SW_SHOW);
	return TRUE;
}

//������Ƿ���Ч����Ч����-1��
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
//����Ҽ����
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

// CCopyFileView ���

#ifdef _DEBUG
void CCopyFileView::AssertValid() const
{
	CListView::AssertValid();
}

void CCopyFileView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CCopyFileDoc* CCopyFileView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCopyFileDoc)));
	return (CCopyFileDoc*)m_pDocument;
}
#endif //_DEBUG


// CCopyFileView ��Ϣ�������

//�༭�б����Ŀ��Ŀ¼
void CCopyFileView::OnListviewEdit()
{
	// TODO: �ڴ���������������
	LPITEMIDLIST pDlist;
	BROWSEINFO BrowseInfo;

	ZeroMemory(&BrowseInfo, sizeof(BrowseInfo));

	BrowseInfo.hwndOwner = m_hWnd;
	BrowseInfo.lpszTitle = _T("ѡ��һ���ļ�����ΪĿ��Ŀ¼: ");
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

//ɾ��һ����Ϣ
void CCopyFileView::OnListviewDelete()
{
	// TODO: �ڴ���������������
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

//����б��ͷ����������
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
	// TODO: �ڴ���������������
	POSITION pos;
	pos = GetListCtrl().GetFirstSelectedItemPosition();
	while(pos)
	{
		int iItemIndex = GetListCtrl().GetNextSelectedItem(pos);
		OpenDesFilePath(iItemIndex);
	}
}
*/
//�����ļ�����д��Ӧ��Ϣ
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

		//�����ļ�
		int nCopyCondition = pCopyView->CopyFileCondition(szSrcPath , szTargetFilePath);
		if (nCopyCondition & NEED_COPY_FILE)	//Ŀ���ļ���������Ч������Ҫ����
		{
			if(pCopyView->CopyFileToDirectory(szSrcPath , szTargetFilePath , strCopyInfo , iItemCount) == FALSE)
				++iItemFaild;

			//�������
			ListCtrl.SetItemText(iItemCount , 7 , strCopyInfo);
		}
		if (nCopyCondition & FIRST_IS_NEW || nCopyCondition & SECOND_IS_NEW)
		{
			CString strCopyInfo;
			if ((nCopyCondition & FIRST_IS_NEW) == FIRST_IS_NEW)
				strCopyInfo.Format(_T("Դ�ļ��������ļ�����Ҫ����"));
			else
				strCopyInfo.Format(_T("Ŀ���ļ��������ļ�������Ҫ����"));

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
		strCopyInfo = _T("�����ļ�����Ҫ����");
	else
		strCopyInfo.Format(_T("%d���ļ���Ҫ����"),iTemNoCopy);
	strStatusInfo.Format(_T("ͳ�ƣ��ܹ�ִ��%d�����񣬳ɹ�%d������%s��ʧ��%d������") , iItemCount , iItemCount-iItemFaild , strCopyInfo , iItemFaild);

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

		//�����ļ�
		int nCopyCondition = pCopyView->CopyFileCondition(szSrcPath , szTargetFilePath);
		if (nCopyCondition & NEED_COPY_FILE)	//Ŀ���ļ���������Ч������Ҫ����
		{
			if(pCopyView->CopyFileToDirectory(szSrcPath , szTargetFilePath , strCopyInfo , iIndex) == FALSE)
				++iItemFaild;

			//�������
			ListCtrl.SetItemText(iIndex , 7 , strCopyInfo);
		}
		if (nCopyCondition & FIRST_IS_NEW || nCopyCondition & SECOND_IS_NEW)
		{
			CString strCopyInfo;
			if ((nCopyCondition & FIRST_IS_NEW) == FIRST_IS_NEW)
				strCopyInfo.Format(_T("Դ�ļ��������ļ�����Ҫ����"));
			else
				strCopyInfo.Format(_T("Ŀ���ļ��������ļ�������Ҫ����"));

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
		strCopyInfo = _T("�����ļ�����Ҫ����");
	else
		strCopyInfo.Format(_T("%d���ļ���Ҫ����"),iTemNoCopy);
	strStatusInfo.Format(_T("ͳ�ƣ��ܹ�ִ��%d�����񣬳ɹ�%d������%s��ʧ��%d������") , iItemCount , iItemCount-iItemFaild , strCopyInfo , iItemFaild);

	TCHAR szStatusInfo[MAX_PATH];
	_tcscpy_s(szStatusInfo,strStatusInfo);
	pMainFrame->SendMessage(WM_UPDATE_STSTUS , (WPARAM)szStatusInfo , 0);

	AdjustListWidth(ListCtrl);
	ListCtrl.RedrawWindow();
	return TRUE;
}

//�Ƚ��������Ĵ�С
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
	if (strFileSize.IsEmpty())		//�������Ϊ��
		return -1;
	TRACE(_T("%s"),strFileSize.Right(2));
	if (strFileSize.Right(2).Compare(_T("KB")) == NULL)
		return iFileSize * 1024;
	else if (strFileSize.Right(2).Compare(_T("MB")) == NULL)
		return iFileSize * 1024 * 1024;
	else if (strFileSize.Right(2).Compare(_T("GB")) == NULL)
		return iFileSize * 1024 * 1024;
	else //�����ǳ���GB�ĵ��ļ�
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
	UINT nCoverState = GetPrivateProfileInt(_T("BaseInfo"),_T("CoverWhenEver"),0,szIniFilePath);		//��ȡ�����ļ��Ƿ񸲸ǲ���

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
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("��Դ�ļ���Ŀ���ļ�ʧ��"));
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

	return LarIntSrc.QuadPart>LarIntDes.QuadPart ? FIRST_IS_NEW | NEED_COPY_FILE : SECOND_IS_NEW;		//�������µ��ļ�
}

BOOL CCopyFileView::InsertListItem(LPCTSTR lpszSrcFilePath , LPCTSTR lpszTargetFilePath)
{
	CListCtrl& ListCtrl = GetListCtrl();
	CString strFilePath(lpszSrcFilePath);

	UINT nIconIndex = InsertIconToImageList(strFilePath);
	int iItemIndex = ListCtrl.InsertItem(0x7FFF , PathFindFileName(lpszSrcFilePath) , nIconIndex);		//�����ļ���

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
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("�ļ�%s������"),lpszSrcFilePath);
		return FALSE;
	}

	if (PathIsDirectory(lpszSrcFilePath) == FILE_ATTRIBUTE_DIRECTORY)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 ,_T("·��%s��һ��Ŀ¼"),lpszSrcFilePath);
		return FALSE;
	}

	SHFILEINFO ShFileInfo;
	memset(&ShFileInfo , 0 , sizeof(SHFILEINFO));
	DWORD dwRet = SHGetFileInfo(lpszSrcFilePath , NULL , &ShFileInfo , sizeof(SHFILEINFO) , SHGFI_ICON|SHGFI_SMALLICON);
	if (dwRet == 0)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ȡ�ļ�ͼ��ʧ��"));
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

		if (PathFileExists(szSrcFilePath) == FALSE || PathFileExists(szTargetFilePath) == FALSE)	//�ļ�������
		{
			clrNewTextColor = RGB(255,0,0);
			clrNewBkColor = RGB(255,255,255);
		}
		else if(CompareFileProperty(szSrcFilePath,szTargetFilePath) == FALSE)	//�ļ����Բ�ͬ
		{
			clrNewTextColor = RGB(255,0,0);
			clrNewBkColor = RGB(255,255,255);
		}
		else	//Ĭ�Ͽ���
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
	// TODO: �ڴ���������������
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
		strTipInfo.Format(_T("�ļ�%s�����ڣ�"),szFilePath);
		MessageBox(strTipInfo,_T("��ʾ"),MB_OK);
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
		strTipInfo.Format(_T("�ļ�%s�����ڣ�"),szFilePath);
		MessageBox(strTipInfo,_T("��ʾ"),MB_OK);
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
	// TODO: �ڴ���������������
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

	//��ȡĿ���ļ�Ŀ¼
	TCHAR szDesFilePath[MAX_PATH];
	if(strDesFilePath.IsEmpty())
		strDesFilePath =szCurrentDir;		//ԭֵ�Ѿ�Ϊ����ֱ�Ӹ�ֵ����
	_tcscpy_s(szDesFilePath,strDesFilePath);
	//��Ŀ��Ŀ¼�����·��ת��Ϊ����·����
	//���򿽱�����ǰĿ¼���ϼ�Ŀ¼ʱpathcombine���������(ȥ�����ϼ�Ŀ¼)
	TCHAR szDesFileAbsolutePath[MAX_PATH];
	//_tcscpy_s(szDesFileAbsolutePath)
	PathCombine(szDesFileAbsolutePath , szCurrentDir , szDesFilePath);
	FileExists(szCurrentDir , szDesFileAbsolutePath , strCopyInfo);

	//TCHAR szTargetFilePath[MAX_PATH];
	PathCombine(lpTargetFilePath , szDesFileAbsolutePath , strFileName);	//Ŀ���ļ��ľ���·��

	//��ȡԴ�ļ�Ŀ¼
	if(strFilePath.IsEmpty())
		strFilePath = szCurrentDir;
	//TCHAR szSrcPath[MAX_PATH];
	TCHAR szSrcFileAbsolutePath[MAX_PATH];

	PathCombine(szSrcFileAbsolutePath ,szCurrentDir , strFilePath);
	if (PathIsDirectory(szSrcFileAbsolutePath) == FILE_ATTRIBUTE_DIRECTORY)	//�����Ŀ¼��������ļ���
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

		ListCtrl.InsertColumn(0,_T("Դ�ļ���"),LVCFMT_LEFT,100);
		ListCtrl.InsertColumn(1,_T("Դ�ļ�Ŀ¼"),LVCFMT_LEFT,200);
		ListCtrl.InsertColumn(2,_T("Ŀ���ļ�Ŀ¼"),LVCFMT_LEFT,200);
		ListCtrl.InsertColumn(3,_T("Դ�ļ��޸�ʱ��"),LVCFMT_LEFT,150);
		ListCtrl.InsertColumn(4,_T("Դ�ļ���С"),LVCFMT_RIGHT,80);
		ListCtrl.InsertColumn(5,_T("Ŀ���ļ��޸�ʱ��"),LVCFMT_LEFT,150);
		ListCtrl.InsertColumn(6,_T("Ŀ���ļ���С"),LVCFMT_RIGHT,100);
		ListCtrl.InsertColumn(7,_T("�������"),LVCFMT_LEFT,400);

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
		//���¸���ʱ��ɾ������
		ListCtrl.DeleteAllItems();

	TCHAR szFilePath[MAX_PATH];

	GetIniFilePath(szFilePath);
	CString strIniFilePath(szFilePath);

	if(PathFileExists(strIniFilePath) == FALSE)
	{
		AfxMessageBox(_T("û���ҵ���Ӧ�������ļ�"));
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
		//ָ����һ����
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
	HANDLE hThread;
	hThread = CreateThread(NULL , NULL , ThreadCopyFileByMenu, AfxGetMainWnd() , 0 ,NULL);
	if(hThread)
		CloseHandle(hThread);
}
