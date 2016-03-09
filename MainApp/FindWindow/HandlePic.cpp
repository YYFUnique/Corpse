#include "StdAfx.h"
#include "HandlePic.h"
#include "Resource.h"
#include "FindWindowDlg.h"

CHandlePic::CHandlePic(void)
{
	m_hWndNew = NULL;
	m_hWndOld = NULL;
}

CHandlePic::~CHandlePic(void)
{
}
BEGIN_MESSAGE_MAP(CHandlePic, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CHandlePic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SetCapture();

	HICON hMouseDownPic = LoadIcon(AfxGetApp()->m_hInstance , MAKEINTRESOURCE(IDI_ICON_WHEN_MOUSEDOWN));
	this->SetIcon(hMouseDownPic);

	HCURSOR hCursor = LoadCursor(AfxGetApp()->m_hInstance , MAKEINTRESOURCE(IDC_CURSOR_TARGET));
	::SetCursor(hCursor);

	CStatic::OnLButtonDown(nFlags, point);
}

void CHandlePic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ReleaseCapture();

	if(m_hWndNew)
	{	
		::RedrawWindow(m_hWndNew, NULL, NULL, 
			RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

		HWND hWndParent = ::GetParent(m_hWndNew);
		if(::IsWindow(hWndParent))
		{
			// 重画父窗口的整个客户区（RDW_ERASE、RDW_INVALIDATE标记），
			// 立即更新（RDW_UPDATENOW标记）， 包括所有子窗口（RDW_ALLCHILDREN标记）
			::RedrawWindow(hWndParent, NULL, NULL, 
				RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		} 
	}

	HICON hMouseDownPic = LoadIcon(AfxGetApp()->m_hInstance , MAKEINTRESOURCE(IDI_ICON_WHEN_MOUSEUP));
	this->SetIcon(hMouseDownPic);

	HCURSOR hCursor = LoadCursor(AfxGetApp()->m_hInstance , MAKEINTRESOURCE(IDC_ARROW));
	::SetCursor(hCursor);
	CStatic::OnLButtonUp(nFlags, point);
}

void CHandlePic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if(nFlags != MK_LBUTTON)
		return;
	ClientToScreen(&point);
	m_hWndNew = ::WindowFromPoint(point);

	if(m_hWndNew == this->m_hWnd || m_hWndNew == GetParent()->m_hWnd)
		return;

	if(m_hWndNew != m_hWndOld)
	{
		CFindWindowDlg* pMainWnd =(CFindWindowDlg*)AfxGetApp()->m_pMainWnd;

		pMainWnd->ShowWindowData(m_hWndNew);

		if (m_hWndOld != NULL)
		{
			HWND hWndParent = ::GetParent(m_hWndOld);

			HWND hWndRedraw = m_hWndOld;
			if (::IsWindow(hWndParent))
				hWndRedraw = hWndParent;

			::RedrawWindow(hWndRedraw, NULL, NULL, 
				RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
		m_hWndOld = m_hWndNew;
	}
	else 
		return;

	CRect RcWindow;
	::GetWindowRect(m_hWndNew , &RcWindow);
	CWindowDC dc(FromHandle(m_hWndNew));

	COLORREF WindowColor= dc.GetPixel(RcWindow.left , RcWindow.top);
	COLORREF Color = (WindowColor ^ 0xFFFFFF) & 0x00FFFFFF;
	CPen Pen(PS_SOLID , 5 , Color);
	dc.SelectStockObject(NULL_BRUSH);
	dc.SelectObject(Pen);

	dc.Rectangle(0 , 0 , RcWindow.right - RcWindow.left , RcWindow.bottom - RcWindow.top );

	CStatic::OnMouseMove(nFlags, point);	
}
