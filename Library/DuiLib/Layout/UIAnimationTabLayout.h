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
		bool m_bVertical;					//水平还是垂直方向的东西
		bool m_bAnimate;					//是否显示动画
		int  m_nStep;							//步长
		int m_nTabElapse;					//时间间隔
		RECT m_rcItemOld;					//原始位置
		RECT m_rcCurPos;					//当前项位置
		RECT m_rcLastPos;					//原始项位置
		CControlUI* m_pControl;		//当前需要显示的控件
		CControlUI* m_pLast;				//之前显示的控件
		UINT m_nFrameCount;			//动画帧数
	};	
}
