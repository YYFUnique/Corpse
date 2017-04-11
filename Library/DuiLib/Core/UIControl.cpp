#include "StdAfx.h"

namespace DuiLib {

CControlUI::CControlUI() : 
m_pManager(NULL), 
m_pParent(NULL), 
m_bUpdateNeeded(true),
m_bMenuUsed(false),
m_bVisible(true), 
m_bInternVisible(true),
m_bFocused(false),
m_bEnabled(true),
m_bMouseEnabled(true),
m_bKeyboardEnabled(true),
m_bFloat(false),
m_uFloatAlign(0),
m_bSetPos(false),
m_chShortcut('\0'),
m_pTag(NULL),
m_dwBackColor(0),
m_dwBackColor2(0),
m_dwBackColor3(0),
m_dwForeColor(0),
m_dwBorderColor(0),
m_dwFocusBorderColor(0),
m_bColorHSL(false),
m_nBorderSize(0),
m_nBorderStyle(PS_SOLID),
m_nTooltipWidth(300),
m_bDragEnabled(false),
m_DropEffect(DROPEFFECT_COPY),
m_dwCaretColor(0xFF000000)
{
	m_CursorType = IDC_ARROW;
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 9999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcPadding, sizeof(RECT));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_rcBorderSize,sizeof(RECT));
    ::ZeroMemory(&m_tRelativePos, sizeof(TRelativePosUI));
	//////////////////////////////////////////////////////////////////////////
	//Added by gechunping  on 2014-3-14
	m_rcRoundSelect.left=m_rcRoundSelect.top=m_rcRoundSelect.right=m_rcRoundSelect.bottom=1;
	//////////////////////////////////////////////////////////////////////////
}

CControlUI::~CControlUI()
{
    if( OnDestroy ) OnDestroy(this);
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

CDuiString CControlUI::GetName() const
{
    return m_sName;
}

void CControlUI::SetName(LPCTSTR pstrName)
{
    m_sName = pstrName;
}

LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_CONTROL) == 0 ) return this;
    return NULL;
}

LPCTSTR CControlUI::GetClass() const
{
    return _T("ControlUI");
}

UINT CControlUI::GetControlFlags() const
{
    return 0;
}

bool CControlUI::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

CPaintManagerUI* CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
{
    m_pManager = pManager;
    m_pParent = pParent;
    if( bInit && m_pParent ) Init();
}

CControlUI* CControlUI::GetParent() const
{
    return m_pParent;
}

CDuiString CControlUI::GetText() const
{
    return m_sText;
}

void CControlUI::SetText(LPCTSTR pstrText)
{
    if( m_sText == pstrText ) return;

    m_sText = pstrText;
    Invalidate();
}

DWORD CControlUI::GetBkColor() const
{
    return m_dwBackColor;
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
    if( m_dwBackColor == dwBackColor ) return;

    m_dwBackColor = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor2() const
{
    return m_dwBackColor2;
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
    if( m_dwBackColor2 == dwBackColor ) return;

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
    return m_dwBackColor3;
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
    if( m_dwBackColor3 == dwBackColor ) return;

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetForeColor() const
{
	return m_dwForeColor;
}

void CControlUI::SetForeColor(DWORD dwForeColor)
{
	if( m_dwForeColor == dwForeColor ) return;

	m_dwForeColor = dwForeColor;
	Invalidate();
}

LPCTSTR CControlUI::GetBkImage()
{
    return m_sBkImage;
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
    if( m_sBkImage == pStrImage ) return;

    m_sBkImage = pStrImage;
    Invalidate();
}

DWORD CControlUI::GetBorderColor() const
{
    return m_dwBorderColor;
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
    if( m_dwBorderColor == dwBorderColor ) return;

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
{
    if( m_dwFocusBorderColor == dwBorderColor ) return;

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}

bool CControlUI::IsColorHSL() const
{
    return m_bColorHSL;
}

void CControlUI::SetColorHSL(bool bColorHSL)
{
    if( m_bColorHSL == bColorHSL ) return;

    m_bColorHSL = bColorHSL;
    Invalidate();
}

int CControlUI::GetBorderSize() const
{
    return m_nBorderSize;
}

void CControlUI::SetBorderSize(int nSize)
{
    if( m_nBorderSize == nSize ) return;

    m_nBorderSize = nSize;
    Invalidate();
}

//************************************
// ��������: SetBorderSize
// ��������: void
// ������Ϣ: RECT rc
// ����˵��: 
//************************************
void CControlUI::SetBorderSize( RECT rc )
{
	m_rcBorderSize = rc;
	Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

void CControlUI::SetCaretColor(DWORD dwCaretColor)
{
	m_dwCaretColor = dwCaretColor;
}

DWORD CControlUI::GetCaretColor()
{
	return m_dwCaretColor;
}

//bool CControlUI::GetSel(CHARRANGE& CharRange) const
//{
//	return false;
//}

bool CControlUI::DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    return CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
}

const RECT& CControlUI::GetPos() const
{
    return m_rcItem;
}

void CControlUI::SetPos(RECT rc)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;

    CDuiRect InvalidateRc = m_rcItem;
    if( ::IsRectEmpty(&InvalidateRc) ) InvalidateRc = rc;

    m_rcItem = rc;
    if( m_pManager == NULL ) return;

    if( !m_bSetPos ) {
        m_bSetPos = true;
        if( OnSize ) OnSize(this);
        m_bSetPos = false;
    }
    
    if( m_bFloat ) {
        CControlUI* pParent = GetParent();
        if( pParent != NULL ) {
            RECT rcParentPos = pParent->GetPos();
            if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
            else m_cXY.cx = m_rcItem.right - rcParentPos.right;
            if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
            else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
            m_cxyFixed.cx = m_rcItem.right - m_rcItem.left;
            m_cxyFixed.cy = m_rcItem.bottom - m_rcItem.top;
        }
    }

    m_bUpdateNeeded = false;
    InvalidateRc.Join(m_rcItem);

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while (pParent = pParent->GetParent())
    {
        rcTemp = InvalidateRc;
        rcParent = pParent->GetPos();
        if (!::IntersectRect(&InvalidateRc, &rcTemp, &rcParent)) 
            return;
    }
    m_pManager->Invalidate(InvalidateRc);
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int CControlUI::GetX() const
{
    return m_rcItem.left;
}

int CControlUI::GetY() const
{
    return m_rcItem.top;
}

RECT CControlUI::GetPadding() const
{
    return m_rcPadding;
}

void CControlUI::SetPadding(RECT rcPadding)
{
    m_rcPadding = rcPadding;
    NeedParentUpdate();
}

SIZE CControlUI::GetFixedXY() const
{
    return m_cXY;
}

void CControlUI::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedWidth() const
{
    return m_cxyFixed.cx;
}

void CControlUI::SetFixedWidth(int cx)
{
    if( cx < 0 ) return; 
    m_cxyFixed.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedHeight() const
{
    return m_cxyFixed.cy;
}

void CControlUI::SetFixedHeight(int cy)
{
    if( cy < 0 ) return; 
    m_cxyFixed.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxWidth() const
{
    return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
    if( m_cxyMin.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMin.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxHeight() const
{
    return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
    if( m_cxyMax.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMax.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

void CControlUI::SetFloatAlign(UINT uAlign)
{
	if (m_uFloatAlign == uAlign)
		return;

	if (IsFloat() == false)
		SetFloat(true);

	m_uFloatAlign = uAlign;
	NeedParentUpdate();
}

UINT CControlUI::GetFloatAlign() const
{
	return m_uFloatAlign;
}

void CControlUI::SetRelativePos(SIZE szMove,SIZE szZoom)
{
    m_tRelativePos.bRelative = TRUE;
    m_tRelativePos.nMoveXPercent = szMove.cx;
    m_tRelativePos.nMoveYPercent = szMove.cy;
    m_tRelativePos.nZoomXPercent = szZoom.cx;
    m_tRelativePos.nZoomYPercent = szZoom.cy;
}

void CControlUI::SetRelativeParentSize(SIZE sz)
{
    m_tRelativePos.szParent = sz;
}

TRelativePosUI CControlUI::GetRelativePos() const
{
    return m_tRelativePos;
}

bool CControlUI::IsRelativePos() const
{
    return m_tRelativePos.bRelative;
}

CDuiString CControlUI::GetToolTip() const
{
    return m_sToolTip;
}

void CControlUI::SetToolTip(LPCTSTR pstrText)
{
	CDuiString strTemp(pstrText);
	strTemp.Replace(_T("<n>"),_T("\r\n"));
	m_sToolTip=strTemp;
}

void CControlUI::SetToolTipWidth( int nWidth )
{
	m_nTooltipWidth=nWidth;
}

int CControlUI::GetToolTipWidth( void )
{
	return m_nTooltipWidth;
}

TCHAR CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(TCHAR ch)
{
    m_chShortcut = ch;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const CDuiString& CControlUI::GetUserData()
{
    return m_sUserData;
}

void CControlUI::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR CControlUI::GetTag() const
{
    return m_pTag;
}

void CControlUI::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

bool CControlUI::IsVisible() const
{
    return m_bVisible && m_bInternVisible;
}

void CControlUI::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this);
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
    return Proc(this, pData);
}

void CControlUI::Invalidate()
{
    if( !IsVisible() ) return;

	RECT InvalidateRc = m_rcItem;

	CControlUI* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->GetParent() )
	{
		rcTemp = InvalidateRc;
		rcParent = pParent->GetPos();
		if( !::IntersectRect(&InvalidateRc, &rcTemp, &rcParent) ) return;
	}

	if (m_pManager != NULL) m_pManager->Invalidate(InvalidateRc);
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void CControlUI::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

DWORD CControlUI::GetAdjustColor(DWORD dwColor)
{
    if( !m_bColorHSL ) return dwColor;
    short H, S, L;
    CPaintManagerUI::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void CControlUI::Init()
{
    DoInit();
    if( OnInit ) OnInit(this);
}

void CControlUI::DoInit()
{

}

void CControlUI::Event(TEventUI& event)
{
    if( OnEvent(&event) ) DoEvent(event);
}

void CControlUI::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETCURSOR && IsEnabled())
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(m_CursorType)));
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            return;
        }
    }
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}


void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
{
	m_sVirtualWnd = pstrValue;
	m_pManager->UsedVirtualWnd(true);
}

CDuiString CControlUI::GetVirtualWnd() const
{
	CDuiString str;
	if( !m_sVirtualWnd.IsEmpty() ){
		str = m_sVirtualWnd;
	}
	else{
		CControlUI* pParent = GetParent();
		if( pParent != NULL){
			str = pParent->GetVirtualWnd();
		}
		else{
			str = _T("");
		}
	}
	return str;
}

void CControlUI::SetStyle(LPCTSTR pstrValue)
{
	if(m_pManager != NULL) {
		LPCTSTR pStyle = m_pManager->GetStyleAttributeList(pstrValue);
		if( pStyle != NULL) {
			ApplyAttributeList(pStyle);
			return;
		}
	}
}

void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("pos")) == 0 ) {
        RECT rcPos = { 0 };
        LPTSTR pstr = NULL;
        rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
        SetFixedXY(szXY);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( _tcscmp(pstrName, _T("relativepos")) == 0 ) {
        SIZE szMove,szZoom;
        LPTSTR pstr = NULL;
        szMove.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        szMove.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        szZoom.cx = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        szZoom.cy = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr); 
        SetRelativePos(szMove,szZoom);
    }
	else if (_tcsicmp(pstrName, _T("floatalign")) == 0){
		UINT uAlign = GetFloatAlign();
		CDuiString strValue = pstrValue;
		if (strValue.Find(_T("left")) != -1){
			uAlign &= ~(DT_CENTER | DT_RIGHT);
			uAlign |= DT_LEFT;
		}else if (strValue.Find(_T("center")) != -1){
			uAlign &= ~(DT_LEFT | DT_RIGHT);
			uAlign |= DT_CENTER;
		}else if (strValue.Find(_T("right")) != -1){
			uAlign &= ~(DT_LEFT | DT_CENTER);
			uAlign |= DT_RIGHT;
		}

		if (strValue.Find(_T("top")) != -1){
			uAlign &= ~(DT_VCENTER | DT_BOTTOM);
			uAlign |= DT_TOP;
		}else if (strValue.Find(_T("vcenter")) != -1){
			uAlign &= ~(DT_TOP | DT_BOTTOM);
			uAlign |= DT_VCENTER;
		}else if (strValue.Find(_T("bottom")) != -1){
			uAlign &= ~(DT_TOP | DT_VCENTER);
			uAlign |= DT_BOTTOM;
		}

		SetFloatAlign(uAlign);
	}
    else if( _tcscmp(pstrName, _T("padding")) == 0 ) {
        RECT rcPadding = { 0 };
        LPTSTR pstr = NULL;
        rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetPadding(rcPadding);
    }
	//////////////////////////////////////////////////////////////////////////
	//Added by gechunping  on 2014-3-14 
	else if( _tcscmp(pstrName, _T("roundselect")) == 0 ) {
		RECT rcRoundSelect = { 0 };
		LPTSTR pstr = NULL;
		rcRoundSelect.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcRoundSelect.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcRoundSelect.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcRoundSelect.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetRoundSelect(rcRoundSelect);
	}
	//////////////////////////////////////////////////////////////////////////
    else if( _tcscmp(pstrName, _T("bkcolor")) == 0 || _tcscmp(pstrName, _T("bkcolor1")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor2")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor2(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor3")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor3(clrColor);
    }
	else if( _tcsicmp(pstrName, _T("forecolor")) == 0 ) {
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetForeColor(clrColor);
	}
    else if( _tcscmp(pstrName, _T("bordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("focusbordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetFocusBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("bordersize")) == 0 ) {
		CDuiString nValue = pstrValue;
		if(nValue.Find(',') < 0)
		{
			SetBorderSize(_ttoi(pstrValue));
			RECT rcPadding = {0};
			SetBorderSize(rcPadding);
		}
		else
		{
			RECT rcPadding = { 0 };
			LPTSTR pstr = NULL;
			rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetBorderSize(rcPadding);
		}
	}
	else if( _tcscmp(pstrName, _T("leftbordersize")) == 0 ) SetLeftBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("topbordersize")) == 0 ) SetTopBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("rightbordersize")) == 0 ) SetRightBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("bottombordersize")) == 0 ) SetBottomBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("borderround")) == 0 ) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
	else if( _tcscmp(pstrName, _T("caretcolor")) == 0) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetCaretColor(clrColor);
	}
    else if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("width")) == 0 ) SetFixedWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("height")) == 0 ) SetFixedHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("minwidth")) == 0 ) SetMinWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("minheight")) == 0 ) SetMinHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
    else if( _tcscmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
    else if( _tcscmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
    else if( _tcscmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
    else if( _tcscmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("float")) == 0 ) SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
    else if( _tcscmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("virtualwnd")) == 0 ) SetVirtualWnd(pstrValue);
	else if( _tcscmp(pstrName, _T("style")) == 0) SetStyle(pstrValue);
	else if( _tcscmp(pstrName, _T("droptarget")) == 0 ) SetDropEnabled(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("dropeffect")) == 0 ) SetDropEffect(pstrValue);
	else if( _tcscmp(pstrName, _T("cursor")) == 0 )	 SetCursor(pstrValue);
}

CControlUI* CControlUI::ApplyAttributeList(LPCTSTR pstrList)
{
    CDuiString sItem;
    CDuiString sValue;
    while( *pstrList != _T('\0') ) {
        sItem.Empty();
        sValue.Empty();
        while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('=') );
        if( *pstrList++ != _T('=') ) return this;
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return this;
        while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return this;
        SetAttribute(sItem, sValue);
        if( *pstrList++ != _T(' ') ) return this;
    }
    return this;
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

void CControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    // ����ѭ�򣺱�����ɫ->����ͼ->״̬ͼ->�ı�->�߿�
	if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) 
	{
		CRenderClip roundClip;
		CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
		PaintBkColor(hDC);
		PaintBkImage(hDC);
		PaintStatusImage(hDC);
		PaintText(hDC);
		PaintBorder(hDC);
	}
	else
	{
		PaintBkColor(hDC);
		PaintBkImage(hDC);
		PaintStatusImage(hDC);
		PaintText(hDC);
		PaintBorder(hDC);
	}
}

void CControlUI::PaintBkColor(HDC hDC)
{
    if( m_dwBackColor != 0 ) 

#ifdef RENDER_GDIPLUS
		if( m_dwBackColor2 != 0 ) {
			if( m_dwBackColor3 != 0 ) {
				DWORD colors[]={GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2),GetAdjustColor(m_dwBackColor3)};
				float positions[]={0,0.5,1};
				CRenderEngine::DrawGradient(hDC,m_rcItem,colors,positions,3,true,m_cxyBorderRound,m_rcRoundSelect.left,m_rcRoundSelect.top,m_rcRoundSelect.right,m_rcRoundSelect.bottom);
			}
			else 
			{
				DWORD colors[]={GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2)};
				float positions[]={0,1};
				CRenderEngine::DrawGradient(hDC,m_rcItem,colors,positions,2,true,m_cxyBorderRound,m_rcRoundSelect.left,m_rcRoundSelect.top,m_rcRoundSelect.right,m_rcRoundSelect.bottom);
			}
		}
#else
		if( m_dwBackColor2 != 0 ) {
			if( m_dwBackColor3 != 0 ) {
				RECT rc = m_rcItem;
				rc.bottom = (rc.bottom + rc.top) / 2;
				CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 8);
				rc.top = rc.bottom;
				rc.bottom = m_rcItem.bottom;
				CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3), true, 8);
			}
			else 
				CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 16);
		}
#endif
 		else if( m_dwBackColor >= 0xFF000000 ) 
		{
#ifdef RENDER_GDIPLUS
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor),m_cxyBorderRound,m_rcRoundSelect.left,m_rcRoundSelect.top,m_rcRoundSelect.right,m_rcRoundSelect.bottom);
#else
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor));
#endif
		}
		else 
		{
#ifdef RENDER_GDIPLUS
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor),m_cxyBorderRound,m_rcRoundSelect.left,m_rcRoundSelect.top,m_rcRoundSelect.right,m_rcRoundSelect.bottom);
#else
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor));
#endif
		}
}

void CControlUI::PaintBkImage(HDC hDC)
{
    if( m_sBkImage.IsEmpty() ) return;
    if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
}

void CControlUI::PaintStatusImage(HDC hDC)
{
    return;
}

void CControlUI::PaintForeColor(HDC hDC)
{
	CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwForeColor));
}

void CControlUI::PaintForeImage(HDC hDC)
{
	if( m_sForeImage.IsEmpty() ) return;
	if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
{
	//////////////////////////////////////////////////////////////////////////
	///added by gechunping on 2014-04-22 for 3dRect 
	if(m_nBorderStyle<0)
	{
		if(m_nBorderStyle==-1)
		{
			CRenderEngine::Draw3dRect(hDC,m_rcItem,0xffcccccc,0xff000000);
		}
		else if(m_nBorderStyle==-2)
		{
			CRenderEngine::Draw3dRect(hDC,m_rcItem,0xff000000,0xffcccccc);
		}
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if(m_dwBorderColor != 0 || m_dwFocusBorderColor != 0)
	{
		if(m_nBorderSize > 0 && ( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ))//��Բ�Ǳ߿�
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor),m_rcRoundSelect.left,m_rcRoundSelect.top,m_rcRoundSelect.right,m_rcRoundSelect.bottom);
			else
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor),m_rcRoundSelect.left,m_rcRoundSelect.top,m_rcRoundSelect.right,m_rcRoundSelect.bottom);
		}
		else
		{
			if (IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize > 0)
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, GetAdjustColor(m_dwFocusBorderColor));
			else if(m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0)
			{
				RECT rcBorder;
				//////////////////////////////////////////////////////////////////////////
				///corected by gechunping  ong 2014-04--02
				if(m_rcBorderSize.left > 0){
					rcBorder		= m_rcItem;
					rcBorder.left	= m_rcItem.left+m_rcBorderSize.left/2;//corected by gechunping  ong 2014-04--02
					rcBorder.right	= m_rcItem.left+m_rcBorderSize.left/2;//corected by gechunping  ong 2014-04--02
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.left,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.top > 0){
					rcBorder		= m_rcItem;
					rcBorder.bottom	= m_rcItem.top+m_rcBorderSize.top/2;
					rcBorder.top	= m_rcItem.top+m_rcBorderSize.top/2;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.top,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.right > 0){
					rcBorder		= m_rcItem;
					rcBorder.left	= m_rcItem.right-m_rcBorderSize.right/2-1;
					rcBorder.right	= m_rcItem.right-m_rcBorderSize.right/2-1;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.right,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.bottom > 0){
					rcBorder		= m_rcItem;
					rcBorder.top	= m_rcItem.bottom-m_rcBorderSize.bottom/2-1;
					rcBorder.bottom	= m_rcItem.bottom-m_rcBorderSize.bottom/2-1;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.bottom,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				//////////////////////////////////////////////////////////////////////////
			}
			else if(m_nBorderSize > 0)
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, GetAdjustColor(m_dwBorderColor));
		}
	}
}

void CControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
    return;
}

//************************************
// ��������: GetLeftBorderSize
// ��������: int
// ����˵��: 
//************************************
int CControlUI::GetLeftBorderSize() const
{
	return m_rcBorderSize.left;
}

//************************************
// ��������: SetLeftBorderSize
// ��������: void
// ������Ϣ: int nSize
// ����˵��: 
//************************************
void CControlUI::SetLeftBorderSize( int nSize )
{
	m_rcBorderSize.left = nSize;
	Invalidate();
}

//************************************
// ��������: GetTopBorderSize
// ��������: int
// ����˵��: 
//************************************
int CControlUI::GetTopBorderSize() const
{
	return m_rcBorderSize.top;
}

//************************************
// ��������: SetTopBorderSize
// ��������: void
// ������Ϣ: int nSize
// ����˵��: 
//************************************
void CControlUI::SetTopBorderSize( int nSize )
{
	m_rcBorderSize.top = nSize;
	Invalidate();
}

//************************************
// ��������: GetRightBorderSize
// ��������: int
// ����˵��: 
//************************************
int CControlUI::GetRightBorderSize() const
{
	return m_rcBorderSize.right;
}

//************************************
// ��������: SetRightBorderSize
// ��������: void
// ������Ϣ: int nSize
// ����˵��: 
//************************************
void CControlUI::SetRightBorderSize( int nSize )
{
	m_rcBorderSize.right = nSize;
	Invalidate();
}

//************************************
// ��������: GetBottomBorderSize
// ��������: int
// ����˵��: 
//************************************
int CControlUI::GetBottomBorderSize() const
{
	return m_rcBorderSize.bottom;
}

//************************************
// ��������: SetBottomBorderSize
// ��������: void
// ������Ϣ: int nSize
// ����˵��: 
//************************************
void CControlUI::SetBottomBorderSize( int nSize )
{
	m_rcBorderSize.bottom = nSize;
	Invalidate();
}

//************************************
// ��������: GetBorderStyle
// ��������: int
// ����˵��: 
//************************************
int CControlUI::GetBorderStyle() const
{
	return m_nBorderStyle;
}

//************************************
// ��������: SetBorderStyle
// ��������: void
// ������Ϣ: int nStyle
// ����˵��: 
//************************************
void CControlUI::SetBorderStyle( int nStyle )
{
	m_nBorderStyle = nStyle;
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////
//Added by gechunping  on 2014-3-14

RECT CControlUI::GetRoundSelect()
{
	return m_rcRoundSelect;
}
void CControlUI::SetRoundSelect(RECT rcRoundSelect)
{
	m_rcRoundSelect=rcRoundSelect;
}

void CControlUI::SetDropEnabled(bool bEnable)
{
	m_bDragEnabled = bEnable;
}

void CControlUI::SetDropEffect(LPCTSTR pstrValue)
{
	m_DropEffect = _ttoi(pstrValue);
}

DWORD CControlUI::GetDropEffect()
{
	return m_DropEffect;
}

bool CControlUI::IsDropEnabled()
{
	return m_bDragEnabled;
}

void  CControlUI::OnDragEnter( IDataObject *pDataObj, DWORD grfKeyState, POINT pt,  DWORD *pdwEffect)
{
	if (IsDropEnabled())
	{
		*pdwEffect = GetDropEffect();
	}else
	{
		*pdwEffect = DROPEFFECT_NONE;
		if( m_pParent != NULL )m_pParent->OnDragEnter(pDataObj,grfKeyState,pt,pdwEffect);
	}
}

void  CControlUI::OnDragOver(DWORD grfKeyState, POINT pt,DWORD *pdwEffect)
{
	if (IsDropEnabled())
	{
		*pdwEffect = GetDropEffect();
	}else
	{
		*pdwEffect = DROPEFFECT_NONE;
		if( m_pParent != NULL )m_pParent->OnDragOver(grfKeyState,pt,pdwEffect);
	}
}

void  CControlUI::OnDragLeave()
{
	if (IsDropEnabled())
		return;
	else 	if( m_pParent != NULL ) m_pParent->OnDragLeave();
}

void  CControlUI::OnDrop(IDataObject *pDataObj, DWORD grfKeyState, POINT pt, DWORD *pdwEffect)
{
	if (IsDropEnabled())
	{
		*pdwEffect = GetDropEffect();
	}else
	{
		*pdwEffect = DROPEFFECT_NONE;
		if( m_pParent != NULL )m_pParent->OnDrop(pDataObj,grfKeyState,pt,pdwEffect);
	}
}

void CControlUI::SetCursor(LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrValue, _T("arrow")) == 0)
		m_CursorType = IDC_ARROW;
	else if (_tcsicmp(pstrValue, _T("ibeam")) == 0)
		m_CursorType = IDC_IBEAM;
	else if (_tcsicmp(pstrValue, _T("wait")) == 0)
		m_CursorType = IDC_WAIT;
	else if (_tcsicmp(pstrValue, _T("cross")) == 0)
		m_CursorType = IDC_CROSS;
	else if (_tcsicmp(pstrValue, _T("uparrow")) == 0)
		m_CursorType = IDC_UPARROW;
	else if (_tcsicmp(pstrValue, _T("size")) == 0)
		m_CursorType = IDC_SIZE;
	else if (_tcsicmp(pstrValue, _T("icon")) == 0)
		m_CursorType = IDC_ICON;
	else if (_tcsicmp(pstrValue, _T("sizenwse")) == 0)
		m_CursorType = IDC_SIZENWSE;
	else if (_tcsicmp(pstrValue, _T("sizenesw")) == 0)
		m_CursorType = IDC_SIZENESW;
	else if (_tcsicmp(pstrValue, _T("sizewe")) == 0)
		m_CursorType = IDC_SIZEWE;
	else if (_tcsicmp(pstrValue, _T("sizens")) == 0)
		m_CursorType = IDC_SIZENS;
	else if (_tcsicmp(pstrValue, _T("sizeall")) == 0)
		m_CursorType = IDC_SIZEALL;
	else if (_tcsicmp(pstrValue, _T("no")) == 0)
		m_CursorType = IDC_NO;
	else if (_tcsicmp(pstrValue, _T("hand")) == 0)
		m_CursorType = IDC_HAND;
}
} // namespace DuiLib
