#ifndef __UISLIDERRANGE_H__
#define __UISLIDERRANGE_H__
#pragma once
namespace DuiLib
{
	class CSliderRangeUI :	public CSliderUI
	{
	public:
		CSliderRangeUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		RECT GetThumbRect() const;
		RECT GetThumbRect2()const;

		DWORD GetThumbColor2();
		void SetThumbColor2(DWORD color);
		DWORD GetThumbHotColor2();
		void SetThumbHotColor2(DWORD color);
		DWORD GetThumbPushedColor2();
		void SetThumbPushedColor2(DWORD color);
		DWORD GetThumbBorderColor2();
		void SetThumbBorderColor2(DWORD color);
		DWORD GetThumbBorderHotColor2();
		void SetThumbBorderHotColor2(DWORD color);
		DWORD GetThumbBorderPushedColor2();
		void SetThumbBorderPushedColor2(DWORD color);

		LPCTSTR GetThumbImage2() const;
		void SetThumbImage2(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage2() const;
		void SetThumbHotImage2(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage2() const;
		void SetThumbPushedImage2(LPCTSTR pStrImage);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoEvent(TEventUI& event);
		void PaintStatusImage(HDC hDC);
		int GetValue2()const{return m_nValue2;}
		void SetValue2(int nValue);
	protected:
		int m_nValue2;
		UINT m_uButtonState2;

		CDuiString m_sThumbImage2;
		CDuiString m_sThumbHotImage2;
		CDuiString m_sThumbPushedImage2;


		DWORD  m_dwThumbColor2;
		DWORD  m_dwThumbHotColor2;
		DWORD  m_dwThumbPushedColor2;
		DWORD  m_dwThumbBorderColor2;
		DWORD  m_dwThumbBorderHotColor2;
		DWORD  m_dwThumbBorderPushedColor2;
	};
}
#endif
