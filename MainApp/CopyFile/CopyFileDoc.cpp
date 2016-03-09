
// CopyFileDoc.cpp : CCopyFileDoc ���ʵ��
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


// CCopyFileDoc ����/����

CCopyFileDoc::CCopyFileDoc()
{
	// TODO: �ڴ����һ���Թ������
	
}

CCopyFileDoc::~CCopyFileDoc()
{
}

BOOL CCopyFileDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	
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
// CCopyFileDoc ���л�

void CCopyFileDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CCopyFileDoc ���

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


// CCopyFileDoc ����
void CCopyFileDoc::OnAddItemToList()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	CFileDialog FileDialog(TRUE,NULL,NULL, 
		OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLEHOOK,NULL,NULL);

	FileDialog.m_ofn.nMaxFile=MAX_PATH*30;
	TCHAR szFileBuffer[MAX_PATH*30] = {0};
	FileDialog.m_ofn.lpstrFile = szFileBuffer;
	FileDialog.m_ofn.lpstrTitle  = _T("��ѡ��Ҫ�򿪵��ļ�");
	FileDialog.m_ofn.lStructSize = sizeof(FileDialog.m_ofn);

	if (FileDialog.DoModal() == IDOK)
	{
		pMainFrame->ChangeModifyStatus(MODIFY_NOT_SAVE);	//������Ҫ�����־

		TCHAR szTargetFilePath[MAX_PATH];
		pMainFrame->GetTargetFilePath(szTargetFilePath , sizeof(szTargetFilePath));
		CCopyFileView* pView = (CCopyFileView*)pMainFrame->GetActiveView();

		POSITION Pos = FileDialog.GetStartPosition();
		while(Pos)
		{
			CString strFileName = FileDialog.GetNextPathName(Pos);
			pView->InsertListItem(strFileName , szTargetFilePath);	//���һ����¼
		}

		//pView->SetImageList();
	}
}