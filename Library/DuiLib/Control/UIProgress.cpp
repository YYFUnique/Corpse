#include "stdafx.h"
#include "UIProgress.h"

namespace DuiLib
{
	CProgressUI::CProgressUI() : m_bShowText(false), m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	LPCTSTR CProgressUI::GetClass() const
	{
		return _T("ProgressUI");
	}

	LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_PROGRESS) == 0 ) return static_cast<CProgressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	bool CProgressUI::IsShowText()
	{
		return m_bShowText;
	}

	void CProgressUI::SetShowText(bool bShowText)
	{
		if( m_bShowText == bShowText ) return;
		m_bShowText = bShowText;
		if(!m_bShowText) SetText(_T(""));
		UpdateText();
	}

	bool CProgressUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		if(nValue == m_nValue || nValue<m_nMin || nValue > m_nMax) {
			return;
		}
		m_nValue = nValue;
		Invalidate();
		UpdateText();
	}

	void CProgressUI::SetForeImage(LPCTSTR lpszForeImage)
	{
		if (m_sForeImage == lpszForeImage)
			return;

		m_sForeImage = lpszForeImage;
		Invalidate();
	}

	void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("showtext")) == 0) SetShowText(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
		else if( _tcscmp(pstrName, _T("isstretchfore"))==0) SetStretchForeImage(_tcscmp(pstrValue, _T("true")) == 0? true : false);
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintStatusImage(HDC hDC)
	{
		PaintForeColor(hDC);
		PaintForeImage(hDC);
	}

	void CProgressUI::PaintForeColor(HDC hDC)
	{
		if(m_dwForeColor == 0) return;

		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = m_rcItem;
		if( m_bHorizontal ) {
			rc.right = m_rcItem.left + (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
		}
		else {
			rc.bottom = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);

		}

		CRenderEngine::DrawColor(hDC, rc, GetAdjustColor(m_dwForeColor));
	}

	void CProgressUI::PaintForeImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}

		if( !m_sForeImage.IsEmpty() ) {
			m_sForeImageModify.Empty();
			if (m_bStretchForeImage)
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom);
			else
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'")
				, rc.left, rc.top, rc.right, rc.bottom
				, rc.left, rc.top, rc.right, rc.bottom);

			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage, (LPCTSTR)m_sForeImageModify) ) m_sForeImage.Empty();
			else return;
		}
	}

	bool CProgressUI::IsStretchForeImage()
	{
		return m_bStretchForeImage;
	}

	void CProgressUI::SetStretchForeImage( bool bStretchForeImage /*= true*/ )
	{
		if (m_bStretchForeImage==bStretchForeImage)	
			return;
		m_bStretchForeImage=bStretchForeImage;
		Invalidate();
	}

	void CProgressUI::UpdateText()
	{
		if(m_bShowText) {
			CDuiString sText;
			sText.Format(_T("%.0f%%"), (m_nValue - m_nMin) * 100.0f / (m_nMax - m_nMin));
			SetText(sText);
		}
	}
}
