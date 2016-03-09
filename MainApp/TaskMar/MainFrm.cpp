
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "TaskMar.h"

#include "MainFrm.h"
#include "TaskMarDoc.h"
#include "TaskMarView.h"
#include "ProcessInfoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
#define  ID_PROCESS_INFO		1000					//ϵͳ������ϸ��Ϣ

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_RUN_SYSTEM_DLG, &CMainFrame::OnRunSystemDlg)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
	ID_PROCESS_INFO,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
	ReleaseProcessErrorInfo();
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
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (m_wndSplitter.CreateStatic(this, 2, 1) == FALSE)		//2��1��
		return FALSE;

	CRect rc;
	GetClientRect(&rc);

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTaskMarView), CSize(rc.Width(), rc.Height()*2/3), pContext) ||
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CProcessInfoView),CSize(rc.Width(), rc.Height()*2/3), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	CenterWindow();
	return TRUE;
}

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

void CMainFrame::OnRunSystemDlg()
{
	// TODO: �ڴ���������������
	HINSTANCE hInst = LoadLibrary(_T("shell32.dll"));   
	if( hInst )   
	{   
		typedef void (__stdcall *FUNC)(HWND,HICON,LPCTSTR,LPCTSTR,LPCTSTR,UINT);   
		FUNC func;   
		func = (FUNC)GetProcAddress(hInst,(char*)61);   
		
		if( func )
		{
			HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
			func(m_hWnd,hIcon,NULL,NULL,NULL,0x02 );   
		}

		FreeLibrary( hInst );   
	}  
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType,cx,cy);
	
	if (m_wndSplitter.IsWindowVisible() == TRUE)
	{
		CRect rect;
		GetClientRect(&rect);
		int x = rect.Width();
		int y = rect.Height();

		m_wndSplitter.SetRowInfo(0, rect.Height()*6/10,100);
		m_wndSplitter.SetRowInfo(1, rect.Height()*4/10,100);

		m_wndSplitter.RecalcLayout();
	}
}