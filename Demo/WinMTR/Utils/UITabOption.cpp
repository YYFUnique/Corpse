#include "StdAfx.h"
#include "UITabOption.h"

CTabOptionUI::CTabOptionUI()
	: m_bSelected(false)
	, m_dwSelectedTextColor(0)
	, m_dwSelectedBkColor(0xFFF9F9F9)
{

}

CTabOptionUI::~CTabOptionUI()
{
	if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
}

LPCTSTR CTabOptionUI::GetClass() const
{
	return _T("TabOptionUI");
}

LPVOID CTabOptionUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, DUI_CTR_TABOPTION) == 0)
		return static_cast<CTabOptionUI*>(this);
	else
		return CButtonUI::GetInterface(pstrName);
}

void CTabOptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
	else if( _tcscmp(pstrName, _T("selected")) == 0 ) Selected(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcsicmp(pstrName, _T("head-normal")) == 0)		SetHeadNormalImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("head-hot")) == 0)	SetHeadHotImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("head-select")) == 0)  SetHeadSelectedImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("normal")) == 0)    SetNormalImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("normal-hot")) == 0)  SetNormalHotImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("pre-select")) == 0)   SetPreSelectedImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("pre-select-hot")) == 0)  SetPreSelectedHotImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("next-select")) == 0)  SetNextSelectedImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("next-select-hot")) == 0)  SetNextSelectedHotImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("select")) == 0)  SetSelectedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
	else if( _tcscmp(pstrName, _T("selectedbkcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetSelectedBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("selectedtextcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetSelectedTextColor(clrColor);
	}
	else
		CButtonUI::SetAttribute(pstrName, pstrValue);
}

bool CTabOptionUI::Activate()
{
	if( !CButtonUI::Activate() ) return false;
	if( !m_sGroupName.IsEmpty() ) Selected(true);
	else Selected(!m_bSelected);

	return true;
}

void CTabOptionUI::SetEnabled(bool bEnable /*= true*/)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() ) {
		if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
		else m_uButtonState = 0;
	}
}

void CTabOptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit /*= true*/)
{
	CControlUI::SetManager(pManager, pParent, bInit);
	if( bInit && !m_sGroupName.IsEmpty() ) {
		if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
	}
}

LPCTSTR CTabOptionUI::GetGroup() const
{
	return m_sGroupName;
}

void CTabOptionUI::SetGroup(LPCTSTR pStrGroupName /*= NULL*/)
{
	if( pStrGroupName == NULL ) {
		if( m_sGroupName.IsEmpty() ) return;
		m_sGroupName.Empty();
	}
	else {
		if( m_sGroupName == pStrGroupName ) return;
		if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		m_sGroupName = pStrGroupName;
	}

	if( !m_sGroupName.IsEmpty() ) {
		if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
	}
	else {
		if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	Selected(m_bSelected);
}

bool CTabOptionUI::IsSelected() const
{
	return m_bSelected;
}

void CTabOptionUI::Selected(bool bSelected)
{
	if( m_bSelected == bSelected ) return;
	m_bSelected = bSelected;
	if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
	else m_uButtonState &= ~UISTATE_SELECTED;

	if( m_pManager != NULL ) {
		if( !m_sGroupName.IsEmpty() ) {
			if( m_bSelected ) {
				CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
				for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
					CTabOptionUI* pControl = static_cast<CTabOptionUI*>(aOptionGroup->GetAt(i));
					if( pControl != this ) {
						pControl->Selected(false);
					}
				}
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}
		else {
			m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
		}
	}

	Invalidate();
}

SIZE CTabOptionUI::EstimateSize(SIZE szAvailable)
{
	return CButtonUI::EstimateSize(szAvailable);
}

void CTabOptionUI::PaintText(HDC hDC)
{
	if( (m_uButtonState & UISTATE_SELECTED) != 0 )
	{
		DWORD oldTextColor = m_dwTextColor;
		if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
			NULL, NULL, nLinks, m_uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
			m_iFont, m_uTextStyle);

		m_dwTextColor = oldTextColor;
	}
	else
		CButtonUI::PaintText(hDC);
}

void CTabOptionUI::SetSelectedBkColor( DWORD dwBkColor )
{
	m_dwSelectedBkColor = dwBkColor;
}

DWORD CTabOptionUI::GetSelectBkColor()
{
	return m_dwSelectedBkColor;
}

LPCTSTR CTabOptionUI::GetForeImage()
{
	return m_sForeImage;
}

void CTabOptionUI::SetForeImage(LPCTSTR pStrImage)
{
	m_sForeImage = pStrImage;
	Invalidate();
}

void CTabOptionUI::SetHeadNormalImage(LPCTSTR lpszHeadNormalImage)
{
	if (m_strHeadNormalImage == lpszHeadNormalImage)
		return;

	m_strHeadNormalImage = lpszHeadNormalImage;
	Invalidate();
}

void CTabOptionUI::SetHeadHotImage(LPCTSTR lpszHeadHotImage)
{
	if (m_strHeadHotImage == lpszHeadHotImage)
		return;

	m_strHeadHotImage = lpszHeadHotImage;
	Invalidate();
}

void CTabOptionUI::SetHeadSelectedImage(LPCTSTR lpszHeadSelectedImage)
{
	if (m_strHeadSelectedImage == lpszHeadSelectedImage)
		return;

	m_strHeadSelectedImage = lpszHeadSelectedImage;
	Invalidate();
}

void CTabOptionUI::SetNormalImage(LPCTSTR lpszNormalImage)
{
	if (m_strNormalImage == lpszNormalImage)
		return;

	m_strNormalImage = lpszNormalImage;
	Invalidate();
}

void CTabOptionUI::SetNormalHotImage(LPCTSTR lpszNormalHotImage)
{
	if (m_strNormalHotImage == lpszNormalHotImage)
		return;

	m_strNormalHotImage = lpszNormalHotImage;
	Invalidate();
}

void CTabOptionUI::SetPreSelectedImage(LPCTSTR lpszPreSelectedImage)
{
	if (m_strPreSelectedImage = lpszPreSelectedImage)
		return;

	m_strPreSelectedImage = lpszPreSelectedImage;
	Invalidate();
}

void CTabOptionUI::SetPreSelectedHotImage(LPCTSTR lpszPreSelectedHotImage)
{
	if (m_strPreSelectedHotImage == lpszPreSelectedHotImage)
		return;

	m_strPreSelectedHotImage = lpszPreSelectedHotImage;
	Invalidate();
}

void CTabOptionUI::SetNextSelectedImage(LPCTSTR lpszNextSelectedImage)
{
	if (m_strNextSelectedImage == lpszNextSelectedImage)
		return;

	m_strNextSelectedImage = lpszNextSelectedImage;
	Invalidate();
}

void CTabOptionUI::SetNextSelectedHotImage(LPCTSTR lpszNextSelectedImage)
{
	if (m_strNextSelectedHotImage == lpszNextSelectedImage)
		return;

	m_strNextSelectedHotImage = lpszNextSelectedImage;
	Invalidate();
}

void CTabOptionUI::SetSelectedImage(LPCTSTR lpszSelectedImage)
{
	if (m_strSelectedImage == lpszSelectedImage)
		return;

	m_strSelectedImage = lpszSelectedImage;
	Invalidate();
}

void CTabOptionUI::SetSelectedTextColor(DWORD dwTextColor)
{
	m_dwSelectedTextColor = dwTextColor;
}

DWORD CTabOptionUI::GetSelectedTextColor()
{
	if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
	return m_dwSelectedTextColor;
}

void CTabOptionUI::PaintStatusImage(HDC hDC)
{
	/*CContainerUI* pParent = (CContainerUI*)m_pParent;
	int nIndex = pParent->GetItemIndex(this);

	m_uButtonState &= ~UISTATE_PUSHED;
	if ((m_uButtonState & UISTATE_HOT) != 0 && IsSelected() && !m_sSelectedHotImage.IsEmpty()) {
		if( !DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage) )
			m_sSelectedHotImage.Empty();
		else goto Label_ForeImage;
	}
	else if ((m_uButtonState & UISTATE_SELECTED) != 0 ) {
		if( !m_sSelectedImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sSelectedImage) ) m_sSelectedImage.Empty();
			else goto Label_ForeImage;
		}
		else if(m_dwSelectedBkColor != 0) {
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
			return;
		}
	}
	else if ((m_uButtonState & UISTATE_HOT) != 0) {
		if (nIndex == 0) {
			if( !m_strHeadHot.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_strHeadHot) ) m_strHeadHot.Empty();
				else goto Label_ForeImage;
			}
		}
		else {
			CTabOptionUI* pTabOption = (CTabOptionUI*)pParent->GetItemAt(nIndex+1);
			if (pTabOption->IsSelected()) {
				if( !m_strPreSelectHot.IsEmpty() ) {
					if( !DrawImage(hDC, (LPCTSTR)m_strPreSelectHot) ) m_strPreSelectHot.Empty();
					else goto Label_ForeImage;
				}
			}
			else{
				if( !m_strNormalHot.IsEmpty() ) {
					if( !DrawImage(hDC, (LPCTSTR)m_strNormalHot) ) m_strNormalHot.Empty();
					else goto Label_ForeImage;
				}
			}
		}
	}
	else
		COptionUI::PaintStatusImage(hDC);

Label_ForeImage:
	if( !m_sForeImage.IsEmpty() ) {
		if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
	}*/
	CButtonUI::PaintStatusImage(hDC);
}

void CTabOptionUI::SetOwner(IOwner* pOwner)
{
	if (pOwner != NULL)
		m_pOwner = pOwner;
}