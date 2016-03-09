
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "PETool.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "PEToolView.h"
#include "PELibrary.h"
#include "LsCommon/ErrorInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE() 
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	m_pPELibrary = new CPELibrary;
}

CMainFrame::~CMainFrame()
{
	if (m_pPELibrary)
	{
		delete m_pPELibrary;
		m_pPELibrary = NULL;
	}
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// ������ִ���
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CPEToolView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	/*m_strTitle = _T("");*/
	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(TRUE);
	return 0;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

CPEToolView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CPEToolView* pView = DYNAMIC_DOWNCAST(CPEToolView, pWnd);
	return pView;
}

CLeftView* CMainFrame::GetLeftPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CLeftView* pView = DYNAMIC_DOWNCAST(CLeftView, pWnd);
	return pView;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFilePathName[MAX_PATH];
	DragQueryFile(hDropInfo,0,szFilePathName,_countof(szFilePathName));

	do 
	{
		if (m_pPELibrary->ParsePEInfo(szFilePathName) == FALSE)
		{
			MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK|MB_ICONWARNING);
			break;
		}

		if (GetLeftPane()->InsertPEHeaderInfo(m_pPELibrary) == FALSE)
			break;

	} while (FALSE);

	DragFinish(hDropInfo);
}