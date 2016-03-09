#include "StdAfx.h"
#include "UISlider.h"

namespace DuiLib
{
	CSliderUI::CSliderUI() : m_uButtonState(0), m_nStep(1)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
		m_dwThumbColor=0;
		m_dwThumbHotColor=0;
		m_dwThumbPushedColor=0;
		m_dwThumbBorderColor=0;
		m_dwThumbBorderHotColor=0;
		m_dwThumbBorderPushedColor=0;
		m_szThumbBorderRound.cx=0;
		m_szThumbBorderRound.cy=0;
		m_nBackSize=10;
		
	}

	LPCTSTR CSliderUI::GetClass() const
	{
		return _T("SliderUI");
	}

	UINT CSliderUI::GetControlFlags() const
	{
		if( IsEnabled() ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID CSliderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_SLIDER) == 0 ) return static_cast<CSliderUI*>(this);
		return CProgressUI::GetInterface(pstrName);
	}

	void CSliderUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CSliderUI::GetChangeStep()
	{
		return m_nStep;
	}

	void CSliderUI::SetChangeStep(int step)
	{
		m_nStep = step;
	}

	void CSliderUI::SetThumbSize(SIZE szXY)
	{
		m_szThumb = szXY;
	}

	RECT CSliderUI::GetThumbRect() const
	{
		if( m_bHorizontal ) {
			int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	LPCTSTR CSliderUI::GetThumbImage() const
	{
		return m_sThumbImage;
	}

	void CSliderUI::SetThumbImage(LPCTSTR pStrImage)
	{
		m_sThumbImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbHotImage() const
	{
		return m_sThumbHotImage;
	}

	void CSliderUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		m_sThumbHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbPushedImage() const
	{
		return m_sThumbPushedImage;
	}

	void CSliderUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		m_sThumbPushedImage = pStrImage;
		Invalidate();
	}

	void CSliderUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CProgressUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				RECT rcThumb = GetThumbRect();
				if( ::PtInRect(&rcThumb, event.ptMouse) ) {
					m_uButtonState |= UISTATE_CAPTURED;
					Invalidate();
				}
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if(!IsEnabled())
				return;
			int nValue;
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				Invalidate();
			}
			if( m_bHorizontal ) {
				if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue = m_nMax;
				else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
				else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
			}
			else {
				if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
				else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue = m_nMax;
				else nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
			}
			if(m_nValue !=nValue && nValue>=m_nMin && nValue<=m_nMax){
				m_nValue =nValue;
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_SCROLLWHEEL ) 
		{
			switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			SetValue(GetValue() + GetChangeStep());
			m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
			return;
		case SB_LINEDOWN:
			SetValue(GetValue() - GetChangeStep());
			m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
			return;
			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( m_bHorizontal ) {
					int nValue=0;
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue = m_nMax;
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
					else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
					if(nValue!=m_nValue){
						m_nValue =nValue;
						m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
					}
				}
				else {
					int nValue=0;
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue = m_nMax;
					else nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
					if(nValue!=m_nValue){
						m_nValue =nValue;
						m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
					}
					Invalidate();
				}
			}
			
			if( IsEnabled() ) {
				RECT rcThumb = GetThumbRect();
				if(::PtInRect(&rcThumb, event.ptMouse))
					m_uButtonState |= UISTATE_HOT;
				else
					m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcThumb = GetThumbRect();
			if( IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if(event.Type==UIEVENT_MOUSELEAVE)
		{
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		CControlUI::DoEvent(event);
	}


	void CSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("thumbimage")) == 0 ) SetThumbImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbsize")) == 0 ) {
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if( _tcscmp(pstrName, _T("step")) == 0 ) {
			SetChangeStep(_ttoi(pstrValue));
		}
		//////////////////////////////////////////////////////////////////////////
		//Added by gechunping  on 2014-2-15
		else if( _tcscmp(pstrName, _T("thumbcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbhotcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbHotColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbpushedcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbPushedColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbbordercolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbBorderColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbborderhotcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbBorderHotColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbborderpushedcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbBorderPushedColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbborderround")) == 0 ) {
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbBorderRound(szXY);
		}
		else if( _tcscmp(pstrName, _T("thumbbordersize")) == 0 ) {
			SetThumbBorderSize(_ttoi(pstrValue));
		}
		else if( _tcscmp(pstrName, _T("backsize")) == 0 ) {
			SetBackSize(_ttoi(pstrValue));
		}
		//////////////////////////////////////////////////////////////////////////
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderUI::PaintStatusImage(HDC hDC)
	{
		CProgressUI::PaintStatusImage(hDC);

		RECT rcThumb = GetThumbRect();
		RECT rcThumbCopy=rcThumb;
		rcThumb.left -= m_rcItem.left;
		rcThumb.top -= m_rcItem.top;
		rcThumb.right -= m_rcItem.left;
		rcThumb.bottom -= m_rcItem.top;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {

			//////////////////////////////////////////////////////////////////////////
			//Added by gechunping  on 2014-2-15
			if(m_dwThumbPushedColor==0) m_dwThumbPushedColor=m_dwThumbColor;
			if(m_dwThumbBorderPushedColor==0) m_dwThumbBorderPushedColor=m_dwThumbBorderColor;
			if(m_dwThumbPushedColor!=0)
			{
#ifdef RENDER_GDIPLUS
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbPushedColor),m_szThumbBorderRound);
				if(m_dwThumbBorderPushedColor!=0)
					CRenderEngine::DrawRoundRect(hDC,rcThumbCopy,m_nThumbBorderSize,m_szThumbBorderRound.cx,m_szThumbBorderRound.cy,GetAdjustColor(m_dwThumbBorderPushedColor));
#else
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbPushedColor));
				if(m_dwThumbBorderPushedColor!=0)
					CRenderEngine::DrawRect(hDC,rcThumbCopy,m_nThumbBorderSize,GetAdjustColor(m_dwThumbBorderPushedColor));
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			if(m_sThumbPushedImage.IsEmpty())
				m_sThumbPushedImage=m_sThumbImage;
			if( !m_sThumbPushedImage.IsEmpty() ) {
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, (LPCTSTR)m_sThumbPushedImage, (LPCTSTR)m_sImageModify) ) m_sThumbPushedImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			//////////////////////////////////////////////////////////////////////////
			//Added by gechunping  on 2014-2-15
			if(m_dwThumbBorderHotColor==0) m_dwThumbBorderHotColor=m_dwThumbBorderColor;
			if(m_dwThumbHotColor==0) m_dwThumbHotColor=m_dwThumbColor;
			if(m_dwThumbHotColor!=0)
			{
#ifdef RENDER_GDIPLUS
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbHotColor),m_szThumbBorderRound);
				if(m_dwThumbBorderHotColor!=0)
					CRenderEngine::DrawRoundRect(hDC,rcThumbCopy,m_nThumbBorderSize,m_szThumbBorderRound.cx,m_szThumbBorderRound.cy,GetAdjustColor(m_dwThumbBorderHotColor));
#else
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbHotColor));
				if(m_dwThumbBorderHotColor!=0)
					CRenderEngine::DrawRect(hDC,rcThumbCopy,m_nThumbBorderSize,GetAdjustColor(m_dwThumbBorderHotColor));
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			if(m_sThumbHotImage.IsEmpty())
				m_sThumbHotImage=m_sThumbImage;
			if( !m_sThumbHotImage.IsEmpty() ) {
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, (LPCTSTR)m_sThumbHotImage, (LPCTSTR)m_sImageModify) ) m_sThumbHotImage.Empty();
				else return;
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//Added by gechunping  on 2014-2-15
			if(m_dwThumbColor!=0)
			{
#ifdef RENDER_GDIPLUS
				
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbColor),m_szThumbBorderRound);
				if(m_dwThumbBorderColor!=0)
					CRenderEngine::DrawRoundRect(hDC,rcThumbCopy,m_nThumbBorderSize,m_szThumbBorderRound.cx,m_szThumbBorderRound.cy,GetAdjustColor(m_dwThumbBorderColor));
#else
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbColor));
				if(m_dwThumbBorderColor!=0)
					CRenderEngine::DrawRect(hDC,rcThumbCopy,m_nThumbBorderSize,GetAdjustColor(m_dwThumbBorderColor));
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			if( !m_sThumbImage.IsEmpty() ) {
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, (LPCTSTR)m_sThumbImage, (LPCTSTR)m_sImageModify) ) m_sThumbImage.Empty();
				else return;
			}
		}
		
	}

	//////////////////////////////////////////////////////////////////////////
	//Added by gechunping  on 2014-2-15

	void CSliderUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

		// »æÖÆÑ­Ðò£º±³¾°ÑÕÉ«->±³¾°Í¼->ÎÄ±¾->×´Ì¬Í¼->±ß¿ò
		if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
#ifndef RENDER_GDIPLUS
			CRenderClip roundClip;
			CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
#endif
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
			PaintStatusImage(hDC);
		}
		else {
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
			PaintStatusImage(hDC);
		}
	}

	void CSliderUI::PaintBkColor(HDC hDC)
	{
		RECT rcBack=m_rcItem;
		if(m_bHorizontal){
			int deltSize=(m_rcItem.bottom-m_rcItem.top-m_nBackSize)/2;
			rcBack.top+=deltSize;
			rcBack.bottom-=deltSize;
		}
		else{
			int deltSize=(m_rcItem.right-m_rcItem.left-m_nBackSize)/2;
			rcBack.left+=deltSize;
			rcBack.right-=deltSize;
		}
		if( m_dwBackColor != 0 ) 
#ifdef RENDER_GDIPLUS
			if( m_dwBackColor2 != 0 ) {
				if( m_dwBackColor3 != 0 ) {
					DWORD colors[]={GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2),GetAdjustColor(m_dwBackColor3)};
					float positions[]={0,0.5,1};
					CRenderEngine::DrawGradient(hDC,rcBack,colors,positions,3,true,m_cxyBorderRound);
				}
				else 
				{
					DWORD colors[]={GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2)};
					float positions[]={0,1};
					CRenderEngine::DrawGradient(hDC,rcBack,colors,positions,2,true,m_cxyBorderRound);
				}
			}
#else
			if( m_dwBackColor2 != 0 ) {
				if( m_dwBackColor3 != 0 ) {
					RECT rc = rcBack;
					rc.bottom = (rc.bottom + rc.top) / 2;
					CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 8);
					rc.top = rc.bottom;
					rc.bottom = rcBack.bottom;
					CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3), true, 8);
				}
				else 
					CRenderEngine::DrawGradient(hDC, rcBack, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 16);
			}
#endif
			else if( m_dwBackColor >= 0xFF000000 ) 
			{
#ifdef RENDER_GDIPLUS
				CRenderEngine::DrawColor(hDC, rcBack, GetAdjustColor(m_dwBackColor),m_cxyBorderRound);
#else
				CRenderEngine::DrawColor(hDC, rcBack, GetAdjustColor(m_dwBackColor));
#endif
			}
			else 
			{
#ifdef RENDER_GDIPLUS
				CRenderEngine::DrawColor(hDC, rcBack, GetAdjustColor(m_dwBackColor),m_cxyBorderRound);
#else
				CRenderEngine::DrawColor(hDC, rcBack, GetAdjustColor(m_dwBackColor));
#endif
			}
	}

	void CSliderUI::PaintBorder(HDC hDC)
	{
		RECT rcBack=m_rcItem;
		if(m_bHorizontal){
			int deltSize=(m_rcItem.bottom-m_rcItem.top-m_nBackSize)/2;
			rcBack.top+=deltSize;
			rcBack.bottom-=deltSize;
		}
		else{
			int deltSize=(m_rcItem.right-m_rcItem.left-m_nBackSize)/2;
			rcBack.left+=deltSize;
			rcBack.right-=deltSize;
		}

		if(m_dwBorderColor != 0 || m_dwFocusBorderColor != 0)
		{
			if(m_nBorderSize > 0 && ( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ))//»­Ô²½Ç±ß¿ò
			{
				if (IsFocused() && m_dwFocusBorderColor != 0)
					CRenderEngine::DrawRoundRect(hDC, rcBack, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor));
				else
					CRenderEngine::DrawRoundRect(hDC, rcBack, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor));
			}
			else
			{
				if (IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize > 0)
					CRenderEngine::DrawRect(hDC, rcBack, m_nBorderSize, GetAdjustColor(m_dwFocusBorderColor));
				else if(m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0)
				{
					RECT rcBorder;

					if(m_rcBorderSize.left > 0){
						rcBorder		= rcBack;
						rcBorder.right	= rcBack.left;
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.left,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
					if(m_rcBorderSize.top > 0){
						rcBorder		= rcBack;
						rcBorder.bottom	= rcBack.top;
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.top,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
					if(m_rcBorderSize.right > 0){
						rcBorder		= rcBack;
						rcBorder.left	= rcBack.right;
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.right,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
					if(m_rcBorderSize.bottom > 0){
						rcBorder		= rcBack;
						rcBorder.top	= rcBack.bottom;
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.bottom,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
					}
				}
				else if(m_nBorderSize > 0)
					CRenderEngine::DrawRect(hDC, rcBack, m_nBorderSize, GetAdjustColor(m_dwBorderColor));
			}
		}
	}

	DWORD CSliderUI::GetThumbColor(){
		return m_dwThumbColor;
	}

	void CSliderUI::SetThumbColor(DWORD color){
		m_dwThumbColor=color;
		Invalidate();
	}

	DWORD CSliderUI::GetThumbHotColor(){
		return m_dwThumbHotColor;
	}
	void CSliderUI::SetThumbHotColor(DWORD color){
		m_dwThumbHotColor=color;
		Invalidate();
	}

	DWORD CSliderUI::GetThumbPushedColor(){
		return m_dwThumbPushedColor;
	}

	void CSliderUI::SetThumbPushedColor(DWORD color){
		m_dwThumbPushedColor=color;
		Invalidate();
	}

	DWORD CSliderUI::GetThumbBorderColor(){
		return m_dwThumbBorderColor;
	}

	void CSliderUI::SetThumbBorderColor(DWORD color){
		m_dwThumbBorderColor=color;
		Invalidate();
	}

	DWORD CSliderUI::GetThumbBorderHotColor(){
		return m_dwThumbBorderHotColor;
	}

	void CSliderUI::SetThumbBorderHotColor(DWORD color){
		m_dwThumbBorderHotColor=color;
		Invalidate();
	}

	DWORD CSliderUI::GetThumbBorderPushedColor(){
		return m_dwThumbBorderPushedColor;
	}

	void CSliderUI::SetThumbBorderPushedColor(DWORD color){
		m_dwThumbBorderPushedColor=color;
		Invalidate();
	}

	void CSliderUI::SetThumbBorderRound(SIZE thumbBorderRound){
		m_szThumbBorderRound=thumbBorderRound;
		Invalidate();
	}

	SIZE CSliderUI::GetThumbBorderRound(){
		return m_szThumbBorderRound;
	}

	void CSliderUI::SetThumbBorderSize(int thumbBorderSize){
		m_nThumbBorderSize=thumbBorderSize;
		Invalidate();
	}

	int CSliderUI::GetThumbBorderSize(){
		return m_nThumbBorderSize;
	}
	int CSliderUI::GetBackSize()
	{
		return m_nBackSize;
	}
	void CSliderUI::SetBackSize(int backSize)
	{
		m_nBackSize=backSize;
		Invalidate();
	}
	/// add by gechunping
	//////////////////////////////////////////////////////////////////////////
}
