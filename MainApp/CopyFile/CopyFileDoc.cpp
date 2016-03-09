
// CopyFileDoc.cpp : CCopyFileDoc 类的实现
//

#include "stdafx.h"
#include "CopyFile.h"

#include "CopyFileDoc.h"
#include "MainFrm.h"
#include "CopyFileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCopyFileDoc

IMPLEMENT_DYNCREATE(CCopyFileDoc, CDocument)

BEGIN_MESSAGE_MAP(CCopyFileDoc, CDocument)
	ON_COMMAND(ID_ADD_ITEM , &CCopyFileDoc::OnAddItemToList)
	ON_COMMAND(ID_FILE_SAVE , &CCopyFileDoc::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS , &CCopyFileDoc::OnFileSaveAs)
END_MESSAGE_MAP()


// CCopyFileDoc 构造/析构

CCopyFileDoc::CCopyFileDoc()
{
	// TODO: 在此添加一次性构造代码
	
}

CCopyFileDoc::~CCopyFileDoc()
{
}

BOOL CCopyFileDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	
	return TRUE;
}

BOOL CCopyFileDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	OnFileSave();
	return TRUE;
}

void CCopyFileDoc::OnFileSave()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->SaveAllToIniFile();
}

void CCopyFileDoc::OnFileSaveAs()
{
	OnFileSave();
}
// CCopyFileDoc 序列化

void CCopyFileDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CCopyFileDoc 诊断

#ifdef _DEBUG
void CCopyFileDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCopyFileDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCopyFileDoc 命令
void CCopyFileDoc::OnAddItemToList()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	CFileDialog FileDialog(TRUE,NULL,NULL, 
		OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLEHOOK,NULL,NULL);

	FileDialog.m_ofn.nMaxFile=MAX_PATH*30;
	TCHAR szFileBuffer[MAX_PATH*30] = {0};
	FileDialog.m_ofn.lpstrFile = szFileBuffer;
	FileDialog.m_ofn.lpstrTitle  = _T("请选择要打开的文件");
	FileDialog.m_ofn.lStructSize = sizeof(FileDialog.m_ofn);

	if (FileDialog.DoModal() == IDOK)
	{
		pMainFrame->ChangeModifyStatus(MODIFY_NOT_SAVE);	//设置需要保存标志

		TCHAR szTargetFilePath[MAX_PATH];
		pMainFrame->GetTargetFilePath(szTargetFilePath , sizeof(szTargetFilePath));
		CCopyFileView* pView = (CCopyFileView*)pMainFrame->GetActiveView();

		POSITION Pos = FileDialog.GetStartPosition();
		while(Pos)
		{
			CString strFileName = FileDialog.GetNextPathName(Pos);
			pView->InsertListItem(strFileName , szTargetFilePath);	//添加一条记录
		}

		//pView->SetImageList();
	}
}