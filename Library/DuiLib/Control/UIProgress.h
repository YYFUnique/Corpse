#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__
//////////////////////////////////////////////////////////////////////////
///Added by gechunping on 2014-3-18
//////////////////////////////////////////////////////////////////////////
#pragma once

namespace DuiLib
{
	class UILIB_API CProgressUI : public CLabelUI
	{
	public:
		CProgressUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool IsShowText();
		void SetShowText(bool bShowText = true);
		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		bool IsStretchForeImage();
		void SetStretchForeImage(bool bStretchForeImage = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);

		void SetForeImage(LPCTSTR lpszForeImage);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);
		void PaintForeColor(HDC hDC);
		void PaintForeImage(HDC hDC);
	
		virtual void UpdateText();
	protected:

		bool m_bHorizontal;
		bool m_bStretchForeImage;
		bool m_bShowText;
		int m_nMax;
		int m_nMin;
		int m_nValue;

		SIZE m_szThumb;
		UINT m_uButtonState;

		CDuiString m_sForeImageModify;
	};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
