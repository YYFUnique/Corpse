#include "stdafx.h"
#include "CRectTrackerInheritance.h"


CRectTrackerInheritance::CRectTrackerInheritance()
{

}

CRectTrackerInheritance::~CRectTrackerInheritance()
{
}

#include "ScreenSpyDlg.h"
void CRectTrackerInheritance::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,CDC* pDC, CWnd* pWnd )
{	    
	//���´���,����TRACKʱ�����߿�
	((CScreenSpyDlg *)pWnd)->PaintWindow();
	((CScreenSpyDlg *)pWnd)->SendMessage(WM_MOUSEMOVE);		
}

