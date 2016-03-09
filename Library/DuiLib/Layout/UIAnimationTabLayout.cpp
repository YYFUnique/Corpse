#include "stdafx.h"
#include "UIAnimationTabLayout.h"

#define  TAB_ANIMATION_TABLAYOUT_TIMER_ID		1000

namespace DuiLib
{
	CAnimationTabLayoutUI::CAnimationTabLayoutUI() 
		: CUIAnimation()
		, m_nStep(1)
		, m_pLast(NULL)
		, m_pControl(NULL)
		, m_bVertical(false)
		, m_bAnimate(true)
		, m_nFrameCount(15)
		, m_nTabElapse(10)
	{
		m_pOwner = this;
		ZeroMemory(&m_rcItemOld,sizeof(RECT));
		ZeroMemory(&m_rcCurPos,sizeof(RECT));
		ZeroMemory(&m_rcLastPos,sizeof(RECT));
	}

	LPCTSTR CAnimationTabLayoutUI::GetClass() const
	{
		return _T("AnimationTabLayoutUI");
	}

	LPVOID CAnimationTabLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_ANIMATION_TABLAYOUT) == 0 ) 
			return static_cast<CAnimationTabLayoutUI*>(this);
		return CTabLayoutUI::GetInterface(pstrName);
	}

	bool CAnimationTabLayoutUI::SelectItem( int iIndex )
	{
		if (m_bAnimate == false)
			return CTabLayoutUI::SelectItem(iIndex);

		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;
		if( iIndex > m_iCurSel ) m_nStep = -1;
		if( iIndex < m_iCurSel ) m_nStep = 1;

		int m_nLastSelect = m_iCurSel;
		m_iCurSel = iIndex;

		if (m_pLast != NULL)
			m_pLast->SetVisible(false);

		m_pLast = GetItemAt(m_nLastSelect);
		m_pControl = GetItemAt(m_iCurSel);

		DoAnimation();

		if( m_pManager != NULL ) {
			m_pManager->SetNextTabControl();
			m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel, m_nLastSelect);
		}

		return true;
	}

	void CAnimationTabLayoutUI::DoAnimation()
	{
		StopAnimation(TAB_ANIMATION_TABLAYOUT_TIMER_ID);
		StartAnimation(m_nTabElapse,m_nFrameCount,TAB_ANIMATION_TABLAYOUT_TIMER_ID);
	}

	void CAnimationTabLayoutUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_TIMER)
			OnTimer(event.wParam);
		else
			CTabLayoutUI::DoEvent(event);
	}

	void CAnimationTabLayoutUI::SetPos(RECT rc)
	{
		if (m_bAnimate && m_pLast && m_pControl && IsRectEmpty(&m_rcCurPos) == FALSE) 
		{
			CControlUI::SetPos(rc);
			m_pControl->SetPos(m_rcCurPos);
		}
		else
			CTabLayoutUI::SetPos(rc);
	}

	void CAnimationTabLayoutUI::OnTimer(int nTimerID)
	{
		if (nTimerID == TAB_ANIMATION_TABLAYOUT_TIMER_ID)
			OnAnimationElapse(nTimerID);
	}

	void CAnimationTabLayoutUI::OnAnimationStart(INT nAnimationID, BOOL bFirstLoop)
	{
		m_rcItemOld = m_rcLastPos = m_rcItem;
		if( m_bVertical == false)
		{
			m_rcCurPos.top = m_rcItem.top;
			m_rcCurPos.bottom = m_rcItem.bottom;
			m_rcCurPos.left = m_rcItem.left - ( m_rcItem.right - m_rcItem.left ) * m_nStep;
			m_rcCurPos.right = m_rcItem.right - ( m_rcItem.right - m_rcItem.left ) * m_nStep;
		}
		else
		{
			m_rcCurPos.left = m_rcItem.left;
			m_rcCurPos.right = m_rcItem.right;
			m_rcCurPos.top = m_rcItem.top - ( m_rcItem.bottom - m_rcItem.top ) * m_nStep;
			m_rcCurPos.bottom = m_rcItem.bottom - ( m_rcItem.bottom - m_rcItem.top ) * m_nStep;		
		}

		if (m_pControl)
			m_pControl->SetVisible(true);
	}

	void CAnimationTabLayoutUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		int iStepLen = 0;
		if ( m_bVertical == false )
		{
			iStepLen = ( m_rcItemOld.right - m_rcItemOld.left ) * m_nStep / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				m_rcCurPos.left += iStepLen;
				m_rcCurPos.right += iStepLen;

				m_rcLastPos.left += iStepLen;
				m_rcLastPos.right += iStepLen;
			}
		}
		else
		{
			iStepLen = ( m_rcItemOld.bottom - m_rcItemOld.top ) * m_nStep / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				m_rcCurPos.top += iStepLen;
				m_rcCurPos.bottom += iStepLen;		

				m_rcLastPos.top += iStepLen;
				m_rcLastPos.bottom += iStepLen;
			}
		}

		if (m_pControl) m_pControl->SetPos(m_rcCurPos);
		if (m_pLast)	m_pLast->SetPos(m_rcLastPos);
	}

	void CAnimationTabLayoutUI::OnAnimationStop(INT nAnimationID) 
	{
		m_rcItem = m_rcLastPos =  m_rcCurPos = m_rcItemOld;
		if (m_pLast) {
			m_pLast->SetVisible(false);
			m_pLast = NULL;
		}
	}

	void CAnimationTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if ( _tcscmp(pstrName, _T("vertical")) == 0 )		SetAnimationDirection(_tcscmp( pstrValue, _T("true")) == 0 );
		else if ( _tcscmp(pstrName,_T("showanimate")) == 0)  SetShowAnimate(_tcscmp(pstrValue, _T("true")) == 0);
		else if ( _tcscmp(pstrName, _T("frame")) == 0)		SetAnimationFrame(_ttoi(pstrValue));
		else if ( _tcscmp(pstrName, _T("elapse")) == 0)	SetAnimationElapse(_ttoi(pstrValue));
		else
			CTabLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CAnimationTabLayoutUI::SetAnimationFrame(UINT nFrame)
	{
		m_nFrameCount = nFrame;
	}

	void CAnimationTabLayoutUI::SetAnimationElapse(int nElapse)
	{
		m_nTabElapse = nElapse;
	}

	void CAnimationTabLayoutUI::SetShowAnimate(bool bShowAnimate)
	{
		m_bAnimate = bShowAnimate;
	}

	void CAnimationTabLayoutUI::SetAnimationDirection(bool bVertical)
	{
		m_bVertical = bVertical;
	}

} // namespace DuiLib