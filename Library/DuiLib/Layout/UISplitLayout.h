#pragma once

namespace DuiLib
{
	class UILIB_API CSplitLayoutUI : public CContainerUI
	{
		public:
			CSplitLayoutUI();

			LPCTSTR GetClass() const;
			LPVOID GetInterface(LPCTSTR pstrName);
			UINT GetControlFlags() const;

		public:
			void SetPos(RECT rc);
			bool IsSepImmMode() const;
			void SetVertical(bool bVertical);
			void DoEvent(TEventUI& event);
			void SetSepWidth(int nSepWidth);
			void SetSepImmMode(bool bImmediately);
			void DoPostPaint(HDC hDC, const RECT& rcPaint);
			void SetSepLineColor(DWORD dwSepLineColor);
			void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
			
		protected:
		
			void SetPosInHori(RECT rc);
			void SetPosInVeri(RECT rc);
			bool EstimateFixedLength(CControlUI* pControl, const RECT& rcSeparator, int& nFixed);
		protected:
			bool m_bImmMode;
			int	m_iSepIndex;
			int	 m_uButtonState;		//鼠标指针状态
			int    m_iSepWidth;			//分隔条宽带和高度
			bool m_bVertical;			//分隔条是垂直还是水平
			RECT m_rcSeparator;		//可以拖动范围（除去padding和inset等外部范围）
			RECT m_rcNewPos;
			POINT ptLastMouse;
			DWORD m_dwSepLineColor;
			//	用于存放该类的空白间隙，通常用于指示显示鼠标指针
			TStdValArray<RECT>  m_rcColRowInfo;
	};
}