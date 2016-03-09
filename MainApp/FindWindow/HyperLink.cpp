#include "StdAfx.h"
#include "HyperLink.h"

#define IDC_TOOLTIP_HELP		0x1000		//提示工具条

CHyperLink::CHyperLink(void)
{
	m_LinkState = LINK_NORMAL;
	m_bVisited = FALSE;
	m_bEnableToolTip = TRUE;
	COLORREF ColorRGB[] = {RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0)};
	SetLinkColor(ColorRGB,_countof(ColorRGB));
}

CHyperLink::~CHyperLink(void)
{
	m_UnderlineFont.DeleteObject();
	m_StdFont.DeleteObject();
}

//派生类的消息映射
BEGIN_MESSAGE_MAP(CHyperLink,CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_MOUSEHOVER, &CHyperLink::OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, &CHyperLink::OnMouseLeave)
	
	ON_WM_ERASEBKGND()
	ON_CONTROL_REFLECT(STN_CLICKED, &CHyperLink::OnStnClicked)
END_MESSAGE_MAP()

//重载父类的控件绑定
void CHyperLink::PreSubclassWindow()
{
	DWORD dwStyle = GetStyle();
	SetWindowLong(GetSafeHwnd() ,GWL_STYLE ,dwStyle | SS_NOTIFY);

	CStatic::PreSubclassWindow();
	CreateDoubleFont();
	m_ToolTip.Create(this,TTS_ALWAYSTIP);
	CRect rcStatic;
	GetClientRect(&rcStatic);
	m_ToolTip.AddTool(this,_T(""),&rcStatic,IDC_TOOLTIP_HELP);
}

void CHyperLink::SetDlgItemText(int nID, LPCTSTR lpszString)
{
	CHyperLink* pHyperLink = (CHyperLink*)GetDlgItem(nID);
	pHyperLink->SetLinkUrl(lpszString);
	CStatic::SetDlgItemText(nID,m_strLinkUrl);
}

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}


void CHyperLink::ActiveToolTip(BOOL bFlag,UINT nDelayTime/*=5*/,LPCTSTR pStrTip/*=_T("")*/)
{
	m_bEnableToolTip = bFlag;
	if (bFlag == TRUE)
	{
		m_ToolTip.SetDelayTime(nDelayTime*100);
		SetTootTipText(pStrTip);
	}
}

void CHyperLink::SetTootTipText(LPCTSTR szToolTip)
{
	if (m_bEnableToolTip)
		m_ToolTip.UpdateTipText(szToolTip,this,IDC_TOOLTIP_HELP);
}

void CHyperLink::SetToolTipTextColor(COLORREF sToolTipText) 
{
	m_ToolTip.SetTipTextColor(sToolTipText);
}

void CHyperLink::SetToolTipBgColor(COLORREF sToolTipBgColor)
{
	m_ToolTip.SetTipBkColor(sToolTipBgColor);
}

void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT Tme;
	Tme.cbSize = sizeof(TRACKMOUSEEVENT);
	Tme.hwndTrack = m_hWnd;
	Tme.dwFlags = TME_HOVER | TME_LEAVE;
	Tme.dwHoverTime = 1;

	_TrackMouseEvent(&Tme); 
	CStatic::OnMouseMove(nFlags, point); 
}

BOOL CHyperLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCursor = LoadCursor(NULL,IDC_HAND);
	::SetCursor(hCursor);
	return TRUE;
}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_ColorLink[m_LinkState]);

	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

LRESULT CHyperLink::OnMouseHover(WPARAM wParam,LPARAM lParam)
{
	SetLinkState(LINK_HOVER);
	SetTextUnderline(m_bUnderlineText);
	RedrawWindow();
	return TRUE;
}

LRESULT CHyperLink::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	SetLinkState(m_bVisited?LINK_VISITED:LINK_NORMAL);
	SetTextUnderline(FALSE);
	RedrawWindow();
	return TRUE;
}

void CHyperLink::SetLinkColor(UINT nLinkState , COLORREF ColorRGB)
{
	if (nLinkState < _countof(m_ColorLink))
		m_ColorLink[nLinkState] = ColorRGB;
}

void CHyperLink::SetLinkColor(COLORREF* pColorRGB,int nCount)
{
	for (int i=0; i<nCount;++i)
		SetLinkColor(i,pColorRGB[i]);
}

void CHyperLink::SetLinkUrl(LPCTSTR strLinkUrl,UINT nLinkType/*=LINK_URL*/)
{
	m_strLinkUrl = strLinkUrl;
	if (nLinkType<=LINK_CHILD)
		m_LinkType = nLinkType;
}

void CHyperLink::SetLinkState(UINT nLinkState)
{
	if (nLinkState < _countof(m_ColorLink))
		m_LinkState = nLinkState;
}

UINT CHyperLink::GetLinkState()
{
	return m_LinkState;
}

BOOL CHyperLink::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(rect);
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

	return TRUE;
}

void CHyperLink::SetTextUnderline(BOOL bHoverText/*=FALSE*/)
{
	if(bHoverText)
		SetFont(&m_UnderlineFont);
	else
		SetFont(&m_StdFont);
}

void CHyperLink::CreateDoubleFont()
{
	LOGFONT LogFont;
	GetFont()->GetLogFont(&LogFont);
	//不知道该怎么添加下划线，故分别使用带有下划线和不带下划线的字体
	//如果想修改字体后生效，请相应ON_WM_ERASEBKGND,重置背景颜色
	LogFont.lfUnderline = FALSE;
	m_StdFont.CreateFontIndirect(&LogFont);
	LogFont.lfUnderline = TRUE;
	m_UnderlineFont.CreateFontIndirect(&LogFont);
}

void CHyperLink::GotoDefineUrl()
{
	ShellExecute(NULL, _T("open"), m_strLinkUrl, NULL, NULL, SW_SHOW);
}
void CHyperLink::OnStnClicked()
{
	// TODO: 在此添加控件通知处理程序代码
	switch(m_LinkType)
	{
	case LINK_URL:
		GotoDefineUrl();
		break;
	default:
		GotoDefineUrl();
		break;
	}
	m_bVisited = TRUE;
}
