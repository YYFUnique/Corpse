#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CSliderUI : public CProgressUI
	{
	public:
		CSliderUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		virtual RECT GetThumbRect() const;
		LPCTSTR GetThumbImage() const;
		void SetThumbImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage() const;
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage() const;
		void SetThumbPushedImage(LPCTSTR pStrImage);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);
		
		//////////////////////////////////////////////////////////////////////////
		//Added by gechunping  on 2014-2-15
		DWORD GetThumbColor();
		void SetThumbColor(DWORD color);
		DWORD GetThumbHotColor();
		void SetThumbHotColor(DWORD color);
		DWORD GetThumbPushedColor();
		void SetThumbPushedColor(DWORD color);
		DWORD GetThumbBorderColor();
		void SetThumbBorderColor(DWORD color);
		DWORD GetThumbBorderHotColor();
		void SetThumbBorderHotColor(DWORD color);
		DWORD GetThumbBorderPushedColor();
		void SetThumbBorderPushedColor(DWORD color);
		void SetThumbBorderRound(SIZE thumbBorderRound);
		SIZE GetThumbBorderRound();
		void SetThumbBorderSize(int thumbBorderSize);
		int GetThumbBorderSize();
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void PaintBkColor(HDC hDC);
		void PaintBorder(HDC hDC);
		int GetBackSize();
		void SetBackSize(int backSize);
		//////////////////////////////////////////////////////////////////////////

	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;
		int m_nStep;

		CDuiString m_sThumbImage;
		CDuiString m_sThumbHotImage;
		CDuiString m_sThumbPushedImage;



		CDuiString m_sImageModify;

		//////////////////////////////////////////////////////////////////////////
		//Added by gechunping  on 2014-2-15
		DWORD  m_dwThumbColor;
		DWORD  m_dwThumbHotColor;
		DWORD  m_dwThumbPushedColor;
		DWORD  m_dwThumbBorderColor;
		DWORD  m_dwThumbBorderHotColor;
		DWORD  m_dwThumbBorderPushedColor;
		SIZE m_szThumbBorderRound;
		int m_nThumbBorderSize;
		int m_nBackSize;//背景的宽度(垂直)或高度(水平)
		//////////////////////////////////////////////////////////////////////////
	};
}

#endif // __UISLIDER_H__