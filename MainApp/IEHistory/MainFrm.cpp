
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "IEHistory.h"

#include "IEHistoryDoc.h"
#include "MainFrm.h"

#include "IEHistoryView.h"
#include "IEHistoryModule.h"
#include "IEHistoryDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
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
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (m_wndModuleSplitter.CreateStatic(this, 1, 2) == FALSE)		//2��1��
		return FALSE;

	CRect rc;
	GetClientRect(&rc);

	m_wndModuleSplitter.CreateView(0, 0, RUNTIME_CLASS(CIEHistoryModule), CSize(rc.Width()/4, rc.Height()), pContext);

	if(!m_wndDetailSplitter.CreateStatic(&m_wndModuleSplitter,2,1,WS_CHILD|WS_VISIBLE,  m_wndModuleSplitter.IdFromRowCol(0, 1)))
	{
		m_wndModuleSplitter.DestroyWindow();
		return FALSE;  
	}

	if (!m_wndDetailSplitter.CreateView(0,0,RUNTIME_CLASS(CIEHistoryDetail),CSize(rc.Width()/4*3,rc.Height()/4),pContext) ||
		!m_wndDetailSplitter.CreateView(1,0,RUNTIME_CLASS(CIEHistoryView),CSize(rc.Width()/4*3,rc.Height()/4*3),pContext))
	{
		m_wndModuleSplitter.DestroyWindow();
		m_wndDetailSplitter.DestroyWindow();
		return FALSE;
	}

	CenterWindow();
	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType,cx,cy);

	if (IsWindow(m_wndModuleSplitter.GetSafeHwnd()) == TRUE &&
		IsWindow(m_wndDetailSplitter.GetSafeHwnd()) == TRUE)
	{
		CRect rc;
		GetClientRect(&rc);

		m_wndModuleSplitter.SetColumnInfo(0,cx/4,0);
		m_wndModuleSplitter.SetColumnInfo(1,cx/4*3,0);

		m_wndDetailSplitter.SetRowInfo(0,cy/4,0);
		m_wndDetailSplitter.SetRowInfo(1,cy/4*3,0);

		m_wndModuleSplitter.RecalcLayout();

	}
}