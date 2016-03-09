
// CaptureScreenDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CaptureScreen.h"
#include "CaptureScreenDlg.h"
#include "LsCommon/FileTools.h"
#include "LsCommon/WndFunc/WndFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	SCREEN_WIDTH		GetSystemMetrics(SM_CXSCREEN)
#define   SCREEN_HEIGHT		GetSystemMetrics(SM_CYSCREEN)

#define	TIME_TEXT_INFO		1000
// CCaptureScreenDlg �Ի���


CCaptureScreenDlg::CCaptureScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureScreenDlg::IDD, pParent)
{
/**********************/
/* ��ʼ����Ƥ���๦�� */
/**********************/
	m_RectTrack.m_nStyle = CRectTracker::resizeInside|CRectTracker::dottedLine; 
	m_RectTrack.m_rect.SetRect(0,0,0,0);

	m_TrackPos = -1;
	m_bHasDraw = FALSE;
	m_bShowInfo = TRUE;
	m_bShowToolBar = FALSE;
	m_dwToolBarID = 0;
	m_ToolBarType = TOOLBAR_BUTTON_TYPE_NORMAL;
	m_hCursor=AfxGetApp()->LoadCursor(ARROW_COLOR);

	CRect rect(0, 0,SCREEN_WIDTH,SCREEN_HEIGHT);
	HBITMAP hDeskTopBitMap = NULL;

	HINSTANCE hInstance = AfxGetApp()->m_hInstance;
	m_DotBmp = LoadBitmap(hInstance,MAKEINTRESOURCE(SC_DOT));
	m_ToolBarBackGround = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_BACKGROUND));
	m_ToolBarSave = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_SAVE));
	m_ToolBarShare = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_SHARE));
	m_ToolBarCancel = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_CANCEL));
	m_ToolBarBack = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_BACK));
	m_ToolBarComplete = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP_COMPLETE));
	//��Ļ���ֻ�ˢ
	CopyScreenToBitmap(&rect,hDeskTopBitMap);
	m_TrueColorBmp = CBitmap::FromHandle(hDeskTopBitMap);

	m_pDeskTopBmp = CoverDesktopBmp(&rect,hDeskTopBitMap,m_DotBmp);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(&m_RectSelected,sizeof(m_RectSelected));
}

CCaptureScreenDlg::~CCaptureScreenDlg()
{
}

void CCaptureScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCaptureScreenDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CCaptureScreenDlg ��Ϣ�������

BOOL CCaptureScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	SetTimer(TIME_TEXT_INFO,1000*5,NULL);
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetWindowText(_T("��Ļ��ͼ����"));

#ifdef _DEBUG
	SetWindowPos(&wndTopMost,0,0,800,600,SWP_SHOWWINDOW);
#else
	SetWindowPos(&wndTopMost,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SWP_SHOWWINDOW);
#endif

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCaptureScreenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCaptureScreenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CCaptureScreenDlg::OnEraseBkgnd(CDC* pDC) 
{
	CDC CompatibleDisplayDC;
	CompatibleDisplayDC.CreateCompatibleDC(pDC);
	CompatibleDisplayDC.SelectObject(m_pDeskTopBmp->GetSafeHandle());

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&CompatibleDisplayDC,0,0,SRCCOPY);

	return TRUE;
}

BOOL CCaptureScreenDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_TrackPos != -1)
		m_RectTrack.SetCursor(m_TrackPos);
	else
		SetCursor(m_hCursor);

	return TRUE;
}

void CCaptureScreenDlg::PaintScreenCapture()
{
	m_RectSelected.NormalizeRect();
	m_RectTrack.SetRect(&m_RectSelected);

	DrawTrackRect();
}

void CCaptureScreenDlg::DrawTrackRect()
{
	//��ȡ��ǰӦ�ó�����豸DC
	CDC* pDC=GetDC();
	////////////////////////////////////////////////////////////////////////////////////
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap MemBmp,OldBmp,BackOldBmp;
	MemBmp.CreateCompatibleBitmap(pDC,SCREEN_WIDTH,SCREEN_HEIGHT);
	MemDC.SelectObject(&MemBmp);
	/////////////////////////////////////���Ʊ���ͼ/////////////////////////////////////
	CDC pDCBackPic;
	pDCBackPic.CreateCompatibleDC(&MemDC);
	pDCBackPic.SelectObject(m_pDeskTopBmp);
	MemDC.BitBlt(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,&pDCBackPic,0,0,SRCCOPY);
	
	CPoint ptMagnify,ptColor;
	GetCursorPos(&ptMagnify);
	//�Ŵ󾵷ŵ�����
	ptColor = ptMagnify;
	ScreenToClient(&ptMagnify);

	BLENDFUNCTION BlendFunc;

	BlendFunc.BlendOp=AC_SRC_OVER;
	BlendFunc.BlendFlags=0;
	BlendFunc.SourceConstantAlpha=0x50;
	BlendFunc.AlphaFormat=0;
	CString strShowText;
	CRect TextRect;
	CFont TextFont;
	TextFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,_T("����"));
	MemDC.SelectObject((HFONT)TextFont);
	MemDC.SetBkMode(TRANSPARENT);
	MemDC.SetTextColor(RGB(255,255,255));

	/////////////////////////////////////����ѡ������/////////////////////////////////////
	if (m_RectSelected.Width() != 0 && m_RectSelected.Height() != 0)
	{
		EnsureRectVisible(&m_RectSelected,13);

		CRect Rect(m_RectSelected);
		CPen LinePen(PS_SOLID,1,RGB(0,0,255));
		MemDC.SelectObject(LinePen);

		MemDC.MoveTo(Rect.left,Rect.top-1);
		MemDC.LineTo(Rect.right,Rect.top-1);
		MemDC.LineTo(Rect.right,Rect.bottom);
		MemDC.LineTo(Rect.left-1,Rect.bottom);
		MemDC.LineTo(Rect.left-1,Rect.top-1);
		/////////////////////////////////////���Ʋ�ɫ����/////////////////////////////////////
		pDCBackPic.SelectObject(m_TrueColorBmp);
		MemDC.BitBlt(Rect.left,Rect.top,Rect.Width(),Rect.Height(),
			&pDCBackPic,Rect.left,Rect.top,SRCCOPY);

		/////////////////////////////////////����ѡ������/////////////////////////////////////

		pDCBackPic.SelectObject(m_DotBmp);
		//���Ͻ�
		MemDC.BitBlt(Rect.left-3,Rect.top-3,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���н�
		MemDC.BitBlt(Rect.left+Rect.Width()/2-2,Rect.top-3,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���Ͻ�
		MemDC.BitBlt(Rect.right-2,Rect.top-3,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���н�
		MemDC.BitBlt(Rect.left-3,Rect.top+Rect.Height()/2-2,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���н�
		MemDC.BitBlt(Rect.right-2,Rect.top+Rect.Height()/2-2,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���½�
		MemDC.BitBlt(Rect.left-3,Rect.bottom-2,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���н�
		MemDC.BitBlt(Rect.left+Rect.Width()/2-2,Rect.bottom-2,5,5,&pDCBackPic,0,0,SRCCOPY);
		//���½�
		MemDC.BitBlt(Rect.right-2,Rect.bottom-2,5,5,&pDCBackPic,0,0,SRCCOPY);

		/////////////////////////////////////������ʾ�ı�����/////////////////////////////////////

		CRect RectText(m_RectSelected.left,m_RectSelected.top-95,m_RectSelected.left+205,m_RectSelected.top-5);

		EnsureRectIsVisible(&RectText,&m_RectSelected);
	
		MemDC.AlphaBlend(RectText.left,RectText.top,RectText.Width(),RectText.Height(),&pDCBackPic,0,0,4,4,BlendFunc);

		/////////////////////////////////////���ƷŴ�////////////////////////////////////////////

		CRect RectMagnify(ptMagnify.x-10,ptMagnify.y-10,ptMagnify.x+10,ptMagnify.y+10);
		MemDC.StretchBlt(RectText.left+5,RectText.top+5,80,80,&MemDC,
			RectMagnify.left,RectMagnify.top,RectMagnify.Width(),RectMagnify.Height(),SRCCOPY);
		/////////////////////////////////////���ƷŴ�ʮ�ּ�////////////////////////////////////////////

		//��ʱ���ṩ

		/////////////////////////////////////������ʾ�ı�����/////////////////////////////////////
		
		strShowText.Format(_T("���λ��:(%d,%d)"),ptColor.x,ptColor.y);
		TextRect = CRect(RectText.left+90,RectText.top+5,RectText.left+200,RectText.top+25);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		COLORREF ColorPixel = MemDC.GetPixel(ptMagnify);
		strShowText.Format(_T("��ǰRGB:(%02X,%02X,%02X)"),GetRValue(ColorPixel),GetGValue(ColorPixel),GetBValue(ColorPixel));
		TextRect = CRect(RectText.left+90,RectText.top+25,RectText.left+200,RectText.top+45);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		strShowText.Format(_T("�����С:(%d,%d)"),m_RectSelected.Width(),m_RectSelected.Height());
		TextRect = CRect(RectText.left+90,RectText.top+45,RectText.left+200,RectText.top+65);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		strShowText.Format(_T("˫���ɿ�����ɽ�ͼ"));
		TextRect = CRect(RectText.left+90,RectText.top+65,RectText.left+200,RectText.top+85);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}
	
	////////////////////////////////////������ʾ����////////////////////////////////////////////
	if (m_bShowInfo == TRUE)
	{
		CRect RectTextInfo(ptMagnify.x+15,ptMagnify.y+25,ptMagnify.x+190,ptMagnify.y+95);

		EnsureRectIsVisible(&RectTextInfo,&m_RectSelected);
		pDCBackPic.SelectObject(m_DotBmp);
		MemDC.AlphaBlend(RectTextInfo.left,RectTextInfo.top,RectTextInfo.Width(),RectTextInfo.Height(),&pDCBackPic,0,0,4,4,BlendFunc);
		//1����ס������ѡ���������
		strShowText.Format(_T("1����ס������ѡ���������"));
		TextRect = CRect(RectTextInfo.left+5,RectTextInfo.top+5,RectTextInfo.left+180,RectTextInfo.top+25);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		//2������Ҽ���Esc�˳�����
		strShowText.Format(_T("2������Ҽ���Esc�˳�����"));
		TextRect = CRect(RectTextInfo.left+5,RectTextInfo.top+25,RectTextInfo.left+180,RectTextInfo.top+45);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		//3��F12��������
		strShowText.Format(_T("3��F12��������"));
		TextRect = CRect(RectTextInfo.left+5,RectTextInfo.top+45,RectTextInfo.left+180,RectTextInfo.top+65);
		MemDC.DrawText(strShowText,&TextRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}

	////////////////////////////////////���ƹ�����///////////////////////////////////////////////
	if (m_bShowToolBar == TRUE)
	{
		m_RectTool = CRect(m_RectSelected.right-180,m_RectSelected.bottom+10,m_RectSelected.right,m_RectSelected.bottom+40);

		EnsureRectIsVisible(&m_RectTool,&m_RectSelected);
		pDCBackPic.SelectObject(m_ToolBarBackGround);
		MemDC.BitBlt(m_RectTool.left,m_RectTool.top,173,28,&pDCBackPic,0,0,SRCCOPY);

		HBITMAP hBitMapArray[5] = {m_ToolBarSave,m_ToolBarShare,m_ToolBarCancel,m_ToolBarBack,m_ToolBarComplete};
		UINT		BmpWidth[][2] = {{24,3},{24,3},{24,3},{24,10},{51,3}};

		DRAW_TOOLBAR DrawToolBar;
		ZeroMemory(&DrawToolBar,sizeof(DrawToolBar));
		DrawToolBar.pTargetDC = &MemDC;
		DrawToolBar.pSrcDc = &pDCBackPic;
		DrawToolBar.dwBmpHeight = 22;
		DrawToolBar.dwLeftPosition = m_RectTool.left;
		DrawToolBar.dwTopPosition = m_RectTool.top +3;
		int nTotal = _countof(hBitMapArray);
		for (int i=0;i<nTotal;++i)
		{
			DrawToolBar.dwBmpBit = 1<<(nTotal-i-1);
			DrawToolBar.dwLeftPosition +=  BmpWidth[i][1];
			DrawToolBar.dwBmpWidth = BmpWidth[i][0];
			DrawToolBar.hBmp = hBitMapArray[i];
			DrawToolBarBmp(DrawToolBar);
			DrawToolBar.dwLeftPosition += BmpWidth[i][0];
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	pDC->BitBlt(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,&MemDC,0,0,SRCCOPY);
	m_pDeskTopBmp->FromHandle((HBITMAP)MemBmp.GetSafeHandle());
	
	//�ͷ���Դ
	MemDC.DeleteDC();
	MemBmp.DeleteObject();
	pDCBackPic.DeleteDC();
	ReleaseDC(pDC);
}

void CCaptureScreenDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//����������Ѿ����
	m_bLBtnDown = TRUE;
	m_bShowInfo = FALSE;
	//�����Ƿ�����Ƥ����η�Χ��
	int nHitTest;
	nHitTest=m_RectTrack.HitTest(point);
	if (nHitTest >=0 )
	{
		m_RectTrack.SetCursor(this,HTCLIENT);
		m_TrackPos = nHitTest;
		//�����ڽ��������ڵ���������ع�����
		m_bShowToolBar = FALSE;
	}

	//��¼�����������λ��
	m_ptLButtonDown = point;

	if (m_bHasDraw == FALSE)
	{
		m_RectSelected.left = m_RectSelected.right = point.x;
		m_RectSelected.top = m_RectSelected.bottom = point.y;
	}

	m_ToolBarType = TOOLBAR_BUTTON_TYPE_NORMAL;

	if (PtInRect(&m_RectTool,point))
		ChangeToolBarState(point,TOOLBAR_BUTTON_TYPE_PRESS);

	PaintScreenCapture();
	CDialog::OnLButtonDown(nFlags, point);
}

void CCaptureScreenDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	//������CDialog::OnCancel();
	OnCancel();
}

void CCaptureScreenDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 	������������ʧЧ
 	m_bLBtnDown = FALSE;
 	m_bHasDraw = TRUE;
	m_bShowInfo = FALSE;
	m_TrackPos = -1;
	m_bShowToolBar = TRUE;

	if (PtInRect(&m_RectTool,point))
		ChangeToolBarState(point,TOOLBAR_BUTTON_TYPE_HOVER);

	PaintScreenCapture();
}

void CCaptureScreenDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_dwToolBarID = 0;
	//�ж�����Ƿ���
	if ((nFlags & MK_LBUTTON) != FALSE && m_bHasDraw == FALSE)
	{
		//��ǵ�ǰ����λ��
		m_RectSelected.right = point.x;
		m_RectSelected.bottom = point.y;

		PaintScreenCapture();
	}
	else
	{
		//�����ƶ��͸ı��С
		if (m_TrackPos != -1 && m_bLBtnDown == TRUE)
		{
			m_RectSelected = m_RectTrack.m_rect;
			m_RectTrack.SetCursor(m_TrackPos);
			ChangeRect(m_TrackPos,point);
		}
		else
		{
			int nHitTest;
			nHitTest=m_RectTrack.HitTest(point);
			if (nHitTest >=0 )
				m_RectTrack.SetCursor(this,HTCLIENT);
			
			if (PtInRect(&m_RectTool,point))
				ChangeToolBarState(point,TOOLBAR_BUTTON_TYPE_HOVER);

			//���»������汳��ͼ
			PaintScreenCapture();
		}
	}

	CDialog::OnMouseMove(nFlags,point);
}

void CCaptureScreenDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_RectSelected.Width() == 0 || m_RectSelected.Height() == 0)
		return;

	int nHitTest;
	nHitTest=m_RectTrack.HitTest(point);
	if (nHitTest == -1)
		return;

	CDC* pDC=GetDC();
	////////////////////////////////////////////////////////////////////////////////////
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap MemBmp,OldBmp,BackOldBmp;
	MemBmp.CreateCompatibleBitmap(pDC,m_RectSelected.Width(),m_RectSelected.Height());
	MemDC.SelectObject(&MemBmp);
	/////////////////////////////////////���Ʊ���ͼ/////////////////////////////////////
	CDC pDCBackPic;
	pDCBackPic.CreateCompatibleDC(&MemDC);
	pDCBackPic.SelectObject(m_TrueColorBmp);

	if (m_RectSelected.Width() != 0 && m_RectSelected.Height() != 0)
		EnsureRectVisible(&m_RectSelected,13);
	CRect Rect(m_RectSelected);

	MemDC.BitBlt(0,0,Rect.Width(),Rect.Height(),
		&pDCBackPic,Rect.left,Rect.top,SRCCOPY);

	CFileDialog FileSave(FALSE);
	if (FileSave.DoModal() == IDOK)
	{
		SaveBitmapFile((HBITMAP)MemBmp.GetSafeHandle(),FileSave.GetPathName());
	}
}

void CCaptureScreenDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIME_TEXT_INFO)
	{
		m_bShowInfo = FALSE;
		PaintScreenCapture();
		KillTimer(nIDEvent);
	}
}

void CCaptureScreenDlg::ChangeRect(int nHandle,CPoint point)
{
	int xPt=  point.x - m_ptLButtonDown.x;
	int yPt = point.y - m_ptLButtonDown.y;
	m_ptLButtonDown = point;
	CRect RectTmp(m_RectSelected);
	switch(nHandle)
	{
		case DIRECTE_WNES:
				RectTmp.left += xPt;
				RectTmp.top += yPt;
			break;
		case DIRECTE_ENWS:
				RectTmp.right += xPt;
				RectTmp.top += yPt;
			break;
		case DIRECTE_ESWN:
				RectTmp.right += xPt;
				RectTmp.bottom += yPt;
			break;
		case DIRECTE_WSEN:
				RectTmp.left += xPt;
				RectTmp.bottom += yPt;
			break;
		case DIRECTE_NS:
				RectTmp.top += yPt;
			break;
		case DIRECTE_EW:
				RectTmp.right += xPt;
			break;
		case DIRECTE_SN:
				RectTmp.bottom += yPt;
			break;
		case DIRECTE_WE:
				RectTmp.left += xPt;
			break;
		case DIRECTE_ALL:
				RectTmp.left += xPt;
				RectTmp.top += yPt;
				RectTmp.right += xPt;
				RectTmp.bottom += yPt;
			break;
	}

	if (RectTmp.left>=0 && RectTmp.top>=0 && RectTmp.bottom<=SCREEN_HEIGHT && RectTmp.right <= SCREEN_WIDTH)
		m_RectSelected = RectTmp;

	PaintScreenCapture();
}

void CCaptureScreenDlg::OnCancel()
{
	if (m_RectTrack.m_rect.Width() !=0 && m_RectTrack.m_rect.Height() !=0)
	{
		m_bLBtnDown = FALSE;
		m_bHasDraw = FALSE;
		m_TrackPos = -1;
		ZeroMemory(&m_RectSelected,sizeof(m_RectSelected));
		m_bShowToolBar = FALSE;
		PaintScreenCapture();
	}
	else
		CDialog::OnCancel();
}

LPRECT CCaptureScreenDlg::GetSelectRect()
{
	return &m_RectSelected;
}

void CCaptureScreenDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	if (IsZoomed())
		PaintScreenCapture();	
}

void CCaptureScreenDlg::EnsureRectInScreen(LPRECT lpRect)
{
	if (lpRect->left<0)
		lpRect->left = 0;
	if (lpRect->top<0)
		lpRect->top = 0;
	if (lpRect->right>SCREEN_WIDTH)
		lpRect->right = SCREEN_WIDTH;
	if (lpRect->bottom>SCREEN_HEIGHT)
		lpRect->bottom = SCREEN_HEIGHT;
}

void CCaptureScreenDlg::OnToolbarCommand(UINT nID)
{

}

void CCaptureScreenDlg::DrawToolBarBmp(DRAW_TOOLBAR DrawToolBar)
{
	int nBmpOffset = 1;
	if ((m_dwToolBarID & DrawToolBar.dwBmpBit) != FALSE)
		nBmpOffset = m_ToolBarType;

	DrawToolBar.pSrcDc->SelectObject(DrawToolBar.hBmp);
	DrawToolBar.pTargetDC->BitBlt(DrawToolBar.dwLeftPosition,DrawToolBar.dwTopPosition,DrawToolBar.dwBmpWidth,
		DrawToolBar.dwBmpHeight,DrawToolBar.pSrcDc,DrawToolBar.dwBmpWidth*nBmpOffset,0,SRCCOPY);
}

void CCaptureScreenDlg::ChangeToolBarState(CPoint& point,TOOLBAR_BUTTON_TYPE ToolBarButtonType)
{
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	UINT		BmpWidth[][2] = {{24,3},{24,3},{24,3},{24,10},{51,3}};
	m_ToolBarType = TOOLBAR_BUTTON_TYPE_NORMAL;
	int nLeftOffset = 0;
	int nBtnCount = 5;
	m_dwToolBarID = 0;
	for (int i=0;i<nBtnCount;++i)
	{
		//����ToolBar�ľ�������
		nLeftOffset += BmpWidth[i][1];
		CRect RectToolBarBtn(m_RectTool.left+nLeftOffset,m_RectTool.top+3,m_RectTool.left+BmpWidth[i][0]+nLeftOffset,m_RectTool.top+3+22);
		if (PtInRect(&RectToolBarBtn,point))
		{
			m_bShowToolBar = TRUE;
			m_ToolBarType = ToolBarButtonType;
			m_dwToolBarID = 1<<(nBtnCount-i-1);
			break;
		}
		nLeftOffset +=BmpWidth[i][0];
	}
}