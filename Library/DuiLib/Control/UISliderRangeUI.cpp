#include "StdAfx.h"
#include "UISliderRangeUI.h"
namespace DuiLib{
	CSliderRangeUI::CSliderRangeUI(void):m_nValue2(100),m_uButtonState2(0){
		m_dwThumbColor2=0;
		m_dwThumbHotColor2=0;
		m_dwThumbPushedColor2=0;
		m_dwThumbBorderColor2=0;
		m_dwThumbBorderHotColor2=0;
		m_dwThumbBorderPushedColor2=0;
	}

	LPCTSTR CSliderRangeUI::GetClass() const{
		return _T("SliderRangeUI");
	}

	LPVOID CSliderRangeUI::GetInterface(LPCTSTR pstrName){
		if( _tcscmp(pstrName, DUI_CTR_SLIDERRANGE) == 0 ) return static_cast<CSliderRangeUI*>(this);
		return  CSliderUI::GetInterface(pstrName);
	}

	void CSliderRangeUI::SetValue2(int nValue){
		m_nValue2 = nValue < m_nMin ? m_nMin : nValue;
		m_nValue2 = nValue > m_nMax ? m_nMax : nValue;
		Invalidate();
	}

	DWORD CSliderRangeUI::GetThumbColor2(){
		return m_dwThumbColor2;
	}

	void CSliderRangeUI::SetThumbColor2(DWORD color){
		m_dwThumbColor2=color;
		Invalidate();
	}

	DWORD CSliderRangeUI::GetThumbHotColor2(){
		return m_dwThumbHotColor2;
	}
	void CSliderRangeUI::SetThumbHotColor2(DWORD color){
		m_dwThumbHotColor2=color;
		Invalidate();
	}

	DWORD CSliderRangeUI::GetThumbPushedColor2(){
		return m_dwThumbPushedColor2;
	}

	void CSliderRangeUI::SetThumbPushedColor2(DWORD color){
		m_dwThumbPushedColor2=color;
		Invalidate();
	}

	DWORD CSliderRangeUI::GetThumbBorderColor2(){
		return m_dwThumbBorderColor2;
	}

	void CSliderRangeUI::SetThumbBorderColor2(DWORD color){
		m_dwThumbBorderColor2=color;
		Invalidate();
	}

	DWORD CSliderRangeUI::GetThumbBorderHotColor2(){
		return m_dwThumbBorderHotColor2;
	}

	void CSliderRangeUI::SetThumbBorderHotColor2(DWORD color){
		m_dwThumbBorderHotColor2=color;
		Invalidate();
	}

	DWORD CSliderRangeUI::GetThumbBorderPushedColor2(){
		return m_dwThumbBorderPushedColor2;
	}

	void CSliderRangeUI::SetThumbBorderPushedColor2(DWORD color){
		m_dwThumbBorderPushedColor2=color;
		Invalidate();
	}

	RECT CSliderRangeUI::GetThumbRect() const
	{
		if( m_bHorizontal ) {
			int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - 2*m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - 2*m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	RECT CSliderRangeUI::GetThumbRect2()const{
		if( m_bHorizontal ) {
			int left = m_rcItem.left+m_szThumb.cx + (m_rcItem.right - m_rcItem.left -2* m_szThumb.cx) * (m_nValue2 - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom -m_szThumb.cy - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - 2*m_szThumb.cy) * (m_nValue2 - m_nMin) / (m_nMax - m_nMin);
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	LPCTSTR CSliderRangeUI::GetThumbImage2() const
	{
		return m_sThumbImage2;
	}

	void CSliderRangeUI::SetThumbImage2(LPCTSTR pStrImage)
	{
		m_sThumbImage2 = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderRangeUI::GetThumbHotImage2() const
	{
		return m_sThumbHotImage2;
	}

	void CSliderRangeUI::SetThumbHotImage2(LPCTSTR pStrImage)
	{
		m_sThumbHotImage2 = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderRangeUI::GetThumbPushedImage2() const
	{
		return m_sThumbPushedImage2;
	}

	void CSliderRangeUI::SetThumbPushedImage2(LPCTSTR pStrImage)
	{
		m_sThumbPushedImage2 = pStrImage;
		Invalidate();
	}


	void CSliderRangeUI::DoEvent(TEventUI& event){
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CSliderRangeUI::DoEvent(event);
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ){
			if( IsEnabled() ) {
				RECT rcThumb = GetThumbRect();
				if( ::PtInRect(&rcThumb, event.ptMouse) && (m_uButtonState2 & UISTATE_CAPTURED)==0 ) {
					m_uButtonState |= UISTATE_CAPTURED;
					Invalidate();
				}
				rcThumb=GetThumbRect2();
				if(::PtInRect(&rcThumb, event.ptMouse) && (m_uButtonState & UISTATE_CAPTURED)==0 ){
					m_uButtonState2 |= UISTATE_CAPTURED;
					Invalidate();
				}
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if(!IsEnabled())
				return;
			bool bCapturedFlag=false;
			if((m_uButtonState & UISTATE_CAPTURED) != 0 || (m_uButtonState2 & UISTATE_CAPTURED) != 0)
				bCapturedFlag=true;
			int nValue=m_nValue,nValue2=m_nValue2;
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ){
				m_uButtonState &= ~UISTATE_CAPTURED;
				Invalidate();
			}
			if( (m_uButtonState2 & UISTATE_CAPTURED) != 0 ){
				m_uButtonState2 &= ~UISTATE_CAPTURED;
				Invalidate();
			}
			if(bCapturedFlag)
				return;
			if( m_bHorizontal ) {
				if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue2 = m_nMax;
				else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
				else{
					RECT rcThumb=GetThumbRect();
					RECT rcThumb2=GetThumbRect2();
					if(event.ptMouse.x<(rcThumb.right+rcThumb2.left)/2)
						nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - 2*m_szThumb.cx);
					else
						nValue2 = m_nMin + (m_nMax-m_nMin) * (event.ptMouse.x - m_rcItem.left - 3 * m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - 2*m_szThumb.cx);
				}
			}
			else {
				if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
				else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue2 = m_nMax;
				else{
					RECT rcThumb=GetThumbRect();
					RECT rcThumb2=GetThumbRect2();
					if(event.ptMouse.y>(rcThumb.top+rcThumb2.bottom)/2)
						nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - 2*m_szThumb.cy);
					else
						nValue2 = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - 3 * m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - 2*m_szThumb.cy);
				}
			}
			
			nValue=nValue<m_nMin?m_nMin:nValue;
			nValue=nValue>m_nValue2?m_nValue2:nValue;
			nValue2=nValue2>m_nMax?m_nMax:nValue2;
			nValue2=nValue2<m_nValue?m_nValue:nValue2;
			if(m_nValue !=nValue ||m_nValue2!=nValue2){
				m_nValue =nValue;
				m_nValue2=nValue2;
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			RECT rcThumb=GetThumbRect();
			RECT rcThumb2=GetThumbRect2();
			int nValue=m_nValue,nValue2=m_nValue2;
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 || (m_uButtonState2 & UISTATE_CAPTURED) != 0) {
				if( m_bHorizontal ) {
					if((m_uButtonState & UISTATE_CAPTURED) != 0){
						nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - 2*m_szThumb.cx);
						nValue=nValue<m_nMin?m_nMin:nValue;
						nValue=nValue>m_nValue2?m_nValue2:nValue;
						if(nValue!=m_nValue){
							m_nValue=nValue;
							m_pManager->SendNotify(this,DUI_MSGTYPE_VALUECHANGED);
						}
					}
					if((m_uButtonState2 & UISTATE_CAPTURED) != 0&&event.ptMouse.x>rcThumb.right)
					{
						nValue2 = m_nMin + (m_nMax-m_nMin) * (event.ptMouse.x - m_rcItem.left - 3 * m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - 2*m_szThumb.cx);
						nValue2=nValue2>m_nMax?m_nMax:nValue2;
						nValue2=nValue2<m_nValue?m_nValue:nValue2;
						if(nValue2!=m_nValue2){
							m_nValue2=nValue2;
							m_pManager->SendNotify(this,DUI_MSGTYPE_VALUECHANGED);
						}
					}
				}
				else {
					if((m_uButtonState & UISTATE_CAPTURED) != 0){
						nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - 2*m_szThumb.cy);
						nValue=nValue<m_nMin?m_nMin:nValue;
						nValue=nValue>m_nValue2?m_nValue2:nValue;
						if(nValue!=m_nValue){
							m_nValue=nValue;
							m_pManager->SendNotify(this,DUI_MSGTYPE_VALUECHANGED);
						}
					}
					if((m_uButtonState2 & UISTATE_CAPTURED) != 0)
					{
						nValue2 = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - 3 * m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - 2*m_szThumb.cy);
						nValue2=nValue2>m_nMax?m_nMax:nValue2;
						nValue2=nValue2<m_nValue?m_nValue:nValue2;
						if(nValue2!=m_nValue2){
							m_nValue2=nValue2;
							m_pManager->SendNotify(this,DUI_MSGTYPE_VALUECHANGED);
						}
					}
				}
			}
			
			if( IsEnabled() ) {
				RECT rcThumb = GetThumbRect();
				RECT rcThumb2 = GetThumbRect2();
				if(::PtInRect(&rcThumb, event.ptMouse))
					m_uButtonState |= UISTATE_HOT;
				else
					m_uButtonState &= ~UISTATE_HOT;
				if(::PtInRect(&rcThumb2, event.ptMouse))
					m_uButtonState2 |= UISTATE_HOT;
				else
					m_uButtonState2 &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcThumb = GetThumbRect();
			RECT rcThumb2 = GetThumbRect2();
			if( IsEnabled() && (::PtInRect(&rcThumb, event.ptMouse)||::PtInRect(&rcThumb2, event.ptMouse)) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}
		if(event.Type==UIEVENT_MOUSELEAVE)
		{
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		CControlUI::DoEvent(event);
	}

	void CSliderRangeUI::PaintStatusImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;
		if( m_nValue2 > m_nMax ) m_nValue2 = m_nMax;
		if( m_nValue2 < m_nMin ) m_nValue2 = m_nMin;
		CSliderUI::PaintStatusImage(hDC);
		RECT rcThumb = GetThumbRect2();
		RECT rcThumbCopy=rcThumb;
		rcThumb.left -= m_rcItem.left;
		rcThumb.top -= m_rcItem.top;
		rcThumb.right -= m_rcItem.left;
		rcThumb.bottom -= m_rcItem.top;
		if( (m_uButtonState2 & UISTATE_CAPTURED) != 0 ) {

			if(m_dwThumbPushedColor2==0) m_dwThumbPushedColor2=m_dwThumbColor2;
			if(m_dwThumbBorderPushedColor2==0) m_dwThumbBorderPushedColor2=m_dwThumbBorderColor2;
			if(m_dwThumbPushedColor2!=0)
			{
#ifdef RENDER_GDIPLUS
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbPushedColor2),m_szThumbBorderRound);
				if(m_dwThumbBorderPushedColor2!=0)
					CRenderEngine::DrawRoundRect(hDC,rcThumbCopy,m_nThumbBorderSize,m_szThumbBorderRound.cx,m_szThumbBorderRound.cy,GetAdjustColor(m_dwThumbBorderPushedColor2));
#else
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbPushedColor2));
				if(m_dwThumbBorderPushedColor2!=0)
					CRenderEngine::DrawRect(hDC,rcThumbCopy,m_nThumbBorderSize,GetAdjustColor(m_dwThumbBorderPushedColor2));
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			if(m_sThumbPushedImage2.IsEmpty())
				m_sThumbPushedImage2=m_sThumbImage2;
			if( !m_sThumbPushedImage2.IsEmpty() ) {
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, (LPCTSTR)m_sThumbPushedImage2, (LPCTSTR)m_sImageModify) ) m_sThumbPushedImage2.Empty();
				else return;
			}
		}
		else if( (m_uButtonState2 & UISTATE_HOT) != 0 ) {
			if(m_dwThumbBorderHotColor2==0) m_dwThumbBorderHotColor2=m_dwThumbBorderColor2;
			if(m_dwThumbHotColor2==0) m_dwThumbHotColor2=m_dwThumbColor2;
			if(m_dwThumbHotColor2!=0)
			{
#ifdef RENDER_GDIPLUS
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbHotColor2),m_szThumbBorderRound);
				if(m_dwThumbBorderHotColor2!=0)
					CRenderEngine::DrawRoundRect(hDC,rcThumbCopy,m_nThumbBorderSize,m_szThumbBorderRound.cx,m_szThumbBorderRound.cy,GetAdjustColor(m_dwThumbBorderHotColor2));
#else
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbHotColor2));
				if(m_dwThumbBorderHotColor2!=0)
					CRenderEngine::DrawRect(hDC,rcThumbCopy,m_nThumbBorderSize,GetAdjustColor(m_dwThumbBorderHotColor2));
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			if(m_sThumbHotImage2.IsEmpty())
				m_sThumbHotImage2=m_sThumbImage2;
			if( !m_sThumbHotImage2.IsEmpty() ) {
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, (LPCTSTR)m_sThumbHotImage2, (LPCTSTR)m_sImageModify) ) m_sThumbHotImage2.Empty();
				else return;
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//Added by gechunping  on 2014-2-15
			if(m_dwThumbColor2!=0)
			{
#ifdef RENDER_GDIPLUS

				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbColor2),m_szThumbBorderRound);
				if(m_dwThumbBorderColor2!=0)
					CRenderEngine::DrawRoundRect(hDC,rcThumbCopy,m_nThumbBorderSize,m_szThumbBorderRound.cx,m_szThumbBorderRound.cy,GetAdjustColor(m_dwThumbBorderColor2));
#else
				CRenderEngine::DrawColor(hDC,rcThumbCopy,GetAdjustColor(m_dwThumbColor2));
				if(m_dwThumbBorderColor2!=0)
					CRenderEngine::DrawRect(hDC,rcThumbCopy,m_nThumbBorderSize,GetAdjustColor(m_dwThumbBorderColor2));
#endif
			}
			//////////////////////////////////////////////////////////////////////////
			if( !m_sThumbImage2.IsEmpty() ) {
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, (LPCTSTR)m_sThumbImage2, (LPCTSTR)m_sImageModify) ) m_sThumbImage2.Empty();
				else return;
			}
		}
	}

	void CSliderRangeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue){
		if( _tcscmp(pstrName, _T("thumbimage2")) == 0 ) SetThumbImage2(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbhotimage2")) == 0 ) SetThumbHotImage2(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbpushedimage2")) == 0 ) SetThumbPushedImage2(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbcolor2")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbColor2(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbhotcolor2")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbHotColor2(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbpushedcolor2")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbPushedColor2(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbbordercolor2")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbBorderColor2(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbborderhotcolor2")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbBorderHotColor2(clrColor);
		}
		else if( _tcscmp(pstrName, _T("thumbborderpushedcolor2")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetThumbBorderPushedColor2(clrColor);
		}
		else CSliderUI::SetAttribute(pstrName, pstrValue);
	}
}

