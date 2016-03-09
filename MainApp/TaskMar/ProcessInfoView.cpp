
// TaskMarView.cpp : CTaskMarView 类的实现
//

#include "stdafx.h"
#include "TaskMar.h"

#include "TaskMarDoc.h"
#include "ProcessInfoView.h"
#include "LsCommon/CommonFunc.h"

#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CProcessInfoView, CFormView)

BEGIN_MESSAGE_MAP(CProcessInfoView, CFormView)
	ON_NOTIFY(TCN_SELCHANGE,IDC_TAB_PROCESS_INFO,&CProcessInfoView::OnTabSelectChange)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabbedToolbarView construction/destruction

CProcessInfoView::CProcessInfoView()
	: CFormView(CProcessInfoView::IDD)
{
	for(int i=0;i<_countof(m_TabCtrlInfo);++i)
		m_TabCtrlInfo[i] = NULL;

	m_TabCtrlImgList.DeleteImageList();
	m_TabCtrlImgList.Create(16,16,ILC_COLOR32|ILC_MASK,10,1);
	CBitmap BitmapTabCtrl;
	BitmapTabCtrl.LoadBitmap(IDB_BITMAP_TABCTRLICON);
	m_TabCtrlImgList.Add(&BitmapTabCtrl,RGB(0,255,0));
}

CProcessInfoView::~CProcessInfoView()
{
	for(int i=0;i<_countof(m_TabCtrlInfo);++i)
		if (m_TabCtrlInfo[i] != NULL)
		{
			delete m_TabCtrlInfo[i];
			m_TabCtrlInfo[i] = NULL;
		}

	m_TabCtrlImgList.Detach();
	m_TabCtrlImgList.DeleteImageList();
}

void CProcessInfoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PROCESS_INFO, m_wndTabControl);
}

BOOL CProcessInfoView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CProcessInfoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout(); 

	LPCTSTR lpszTabTitle[] = {_T("基本信息"),_T("模块"),_T("窗口"),_T("内存"),_T("句柄"),_T("线程")};//,_T("服务"),_T("启动方式")};
	
	CRect Rect;
	m_wndTabControl.GetClientRect(&Rect); 
	Rect.top += 20;
	Rect.bottom -= 5;
	Rect.left +=5;
	Rect.right += 3;

	m_wndTabControl.SetImageList(&m_TabCtrlImgList);

	for (int i=0;i<_countof(m_TabCtrlInfo);++i)
	{
		m_TabCtrlInfo[i] = new CTabCtrlProcessInfo;
		m_TabCtrlInfo[i]->Create(&m_wndTabControl,i);
		//获取Tab控件图标列表
		//HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(m_TabCtrlInfo[i]->GetTabIcon()));
		//int nIndex = m_TabCtrlImgList.Add(hIcon);
		//插入Tab控件和显示图标
		m_wndTabControl.InsertItem(i,lpszTabTitle[i],i);

		m_TabCtrlInfo[i]->SetWindowPos(&wndTop,Rect.left,Rect.top,Rect.Width(),Rect.Height(),SWP_HIDEWINDOW|SWP_FRAMECHANGED);
	}
	
	m_wndTabControl.SetCurSel(0);
	
	OnTabSelectChange(NULL,NULL);
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	ResizeParentToFit(); 
}


#ifdef _DEBUG
void CProcessInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

void CProcessInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTaskMarDoc* CProcessInfoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTaskMarDoc)));
	return (CTaskMarDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// 消息处理函数
void CProcessInfoView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) 
{

}

BOOL CProcessInfoView::UpdateProcessInfo()
{
	int nItem = m_wndTabControl.GetCurSel();
	m_TabCtrlInfo[nItem]->ShowWindow(SW_SHOW);

	m_TabCtrlInfo[nItem]->ShowTabCtrlInfo();
	for (int i=0;i<_countof(m_TabCtrlInfo);++i)
		if (nItem != i)
			m_TabCtrlInfo[i]->ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL CProcessInfoView::SelectTabCtrl(UINT nItem)
{
	m_wndTabControl.SetCurSel(nItem);
	return TRUE;
}

void CProcessInfoView::OnTabSelectChange(NMHDR* pNMHDR,LRESULT* lResult)
{
	UpdateProcessInfo();
}

void CProcessInfoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (::IsWindow(m_wndTabControl) == FALSE)
		return;

	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	m_wndTabControl.SetWindowPos(&wndTop,Rect.left,Rect.top,cx,cy,SWP_NOZORDER);

	m_wndTabControl.GetClientRect(&Rect); 
	Rect.top += 20;
	Rect.bottom += 20;
	Rect.left +=5;
	Rect.right += 3;
	for (int i=0;i<_countof(m_TabCtrlInfo)&&m_TabCtrlInfo[i];++i)
		m_TabCtrlInfo[i]->MoveWindow(Rect);
}