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
			int	 m_uButtonState;		//���ָ��״̬
			int    m_iSepWidth;			//�ָ�������͸߶�
			bool m_bVertical;			//�ָ����Ǵ�ֱ����ˮƽ
			RECT m_rcSeparator;		//�����϶���Χ����ȥpadding��inset���ⲿ��Χ��
			RECT m_rcNewPos;
			POINT ptLastMouse;
			DWORD m_dwSepLineColor;
			//	���ڴ�Ÿ���Ŀհ׼�϶��ͨ������ָʾ��ʾ���ָ��
			TStdValArray<RECT>  m_rcColRowInfo;
	};
}