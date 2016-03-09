#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CScrollBarUI : public CControlUI
	{
	public:
		CScrollBarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CContainerUI* GetOwner() const;
		void SetOwner(CContainerUI* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);

		int GetScrollRange() const;
		void SetScrollRange(int nRange);

		int GetScrollPos() const;
		void SetScrollPos(int nPos);

		int GetLineSize() const;
		void SetLineSize(int nSize);
		
		bool GetShowButton1();
		void SetShowButton1(bool bShow);

		LPCTSTR GetButton1NormalImage();
		void SetButton1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage();
		void SetButton1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage();
		void SetButton1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage();
		void SetButton1DisabledImage(LPCTSTR pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		LPCTSTR GetButton2NormalImage();
		void SetButton2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage();
		void SetButton2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage();
		void SetButton2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage();
		void SetButton2DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetThumbNormalImage();
		void SetThumbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage();
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage();
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage();
		void SetThumbDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage();
		void SetRailNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage();
		void SetRailHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage();
		void SetRailPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage();
		void SetRailDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetBkNormalImage();
		void SetBkNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetBkHotImage();
		void SetBkHotImage(LPCTSTR pStrImage);
		LPCTSTR GetBkPushedImage();
		void SetBkPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetBkDisabledImage();
		void SetBkDisabledImage(LPCTSTR pStrImage);

		//////////////////////////////////////////////////////////////////////////
		/// added by gechunping on 2014-4-15
		DWORD GetButton1NormalColor();
		void SetButton1NormalColor(DWORD dwColor);
		DWORD GetButton1HotColor();
		void SetButton1HotColor(DWORD dwColor);
		DWORD GetButton1PushedColor();
		void SetButton1PushedColor(DWORD dwColor);
		DWORD GetButton1DisableColor();
		void SetButton1DisableColor(DWORD dwColor);

		DWORD GetButton2NormalColor();
		void SetButton2NormalColor(DWORD dwColor);
		DWORD GetButton2HotColor();
		void SetButton2HotColor(DWORD dwColor);
		DWORD GetButton2PushedColor();
		void SetButton2PushedColor(DWORD dwColor);
		DWORD GetButton2DisableColor();
		void SetButton2DisableColor(DWORD dwColor);

		DWORD GetThumbNormalColor();
		void SetThumbNormalColor(DWORD dwColor);
		DWORD GetThumbHotColor();
		void SetThumbHotColor(DWORD dwColor);
		DWORD GetThumbPushedColor();
		void SetThumbPushedColor(DWORD dwColor);
		DWORD GetThumbDisableColor();
		void SetThumbDisableColor(DWORD dwColor);

		SIZE GetButton1Round();
		void SetButton1Round(SIZE szRound);
		SIZE GetButton2Round();
		void SetButton2Round(SIZE szRound);
		SIZE GetThumbRound();
		void SetThumbRound(SIZE szRound);

		DWORD GetButton1MarkColor();
		void SetButton1MarkColor(DWORD dwColor);
		DWORD GetButton2MarkColor();
		void SetButton2MarkColor(DWORD dwColor);
		//////////////////////////////////////////////////////////////////////////

		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		CContainerUI* m_pOwner;
		POINT ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

		CDuiString m_sBkNormalImage;
		CDuiString m_sBkHotImage;
		CDuiString m_sBkPushedImage;
		CDuiString m_sBkDisabledImage;

		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
		CDuiString m_sButton1NormalImage;
		CDuiString m_sButton1HotImage;
		CDuiString m_sButton1PushedImage;
		CDuiString m_sButton1DisabledImage;

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
		CDuiString m_sButton2NormalImage;
		CDuiString m_sButton2HotImage;
		CDuiString m_sButton2PushedImage;
		CDuiString m_sButton2DisabledImage;

		RECT m_rcThumb;
		UINT m_uThumbState;
		CDuiString m_sThumbNormalImage;
		CDuiString m_sThumbHotImage;
		CDuiString m_sThumbPushedImage;
		CDuiString m_sThumbDisabledImage;

		CDuiString m_sRailNormalImage;
		CDuiString m_sRailHotImage;
		CDuiString m_sRailPushedImage;
		CDuiString m_sRailDisabledImage;

		CDuiString m_sImageModify;

		//////////////////////////////////////////////////////////////////////////
		///added by gechunping on 2014-4-15
		DWORD m_dwButton1NormalColor;
		DWORD m_dwButton1HotColor;
		DWORD m_dwButton1PushedColor;
		DWORD m_dwButton1DisableColor;

		DWORD m_dwButton2NormalColor;
		DWORD m_dwButton2HotColor;
		DWORD m_dwButton2PushedColor;
		DWORD m_dwButton2DisableColor;

		DWORD m_dwThumbNormalColor;
		DWORD m_dwThumbHotColor;
		DWORD m_dwThumbPushedColor;
		DWORD m_dwThumbDisableColor;

		SIZE m_szButton1Round;
		SIZE m_szButton2Round;
		SIZE m_szThumbRound;

		DWORD m_dwButton1MarkColor;
		DWORD m_dwButton2MarkColor;

		//////////////////////////////////////////////////////////////////////////
	};
}

#endif // __UISCROLLBAR_H__