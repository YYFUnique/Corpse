#pragma once

namespace DuiLib
{
	class UILIB_API CAnimationTabLayoutUI : public CTabLayoutUI, public CUIAnimation
	{
	public:
		CAnimationTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool SelectItem( int iIndex );
		void DoEvent(TEventUI& event);
		void SetPos(RECT rc);
		void OnTimer( int nTimerID );

		virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop);
		virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop(INT nAnimationID);

		void SetAnimationFrame(UINT nFrame);
		void SetAnimationElapse(int nElapse);
		void SetShowAnimate(bool bShowAnimate);
		void SetAnimationDirection(bool bVertical);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		
		void DoAnimation();
	protected:
		bool m_bVertical;					//ˮƽ���Ǵ�ֱ����Ķ���
		bool m_bAnimate;					//�Ƿ���ʾ����
		int  m_nStep;							//����
		int m_nTabElapse;					//ʱ����
		RECT m_rcItemOld;					//ԭʼλ��
		RECT m_rcCurPos;					//��ǰ��λ��
		RECT m_rcLastPos;					//ԭʼ��λ��
		CControlUI* m_pControl;		//��ǰ��Ҫ��ʾ�Ŀؼ�
		CControlUI* m_pLast;				//֮ǰ��ʾ�Ŀؼ�
		UINT m_nFrameCount;			//����֡��
	};	
}
