
// MainFrm.cpp : CMainFrame 类的实现
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

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
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
	// 创建拆分窗口
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
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

// CMainFrame 诊断

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


// CMainFrame 消息处理程序

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
			MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK|MB_ICONWARNING);
			break;
		}

		if (GetLeftPane()->InsertPEHeaderInfo(m_pPELibrary) == FALSE)
			break;

	} while (FALSE);

	DragFinish(hDropInfo);
}