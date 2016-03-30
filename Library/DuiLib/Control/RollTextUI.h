#pragma once

namespace DuiLib
{
	typedef enum _ROLLTEXT_DIRECTION
	{
		ROLLTEXT_DIRECTION_LEFT		=0,		//文字向左移动
		ROLLTEXT_DIRECTION_RIGHT	=1,		//文字向右移动
		ROLLTEXT_DIRECTION_UP 			=2,		//文字向上移动
		ROLLTEXT_DIRECTION_DOWN	=3,		//文字向下移动
	}ROLLTEXT_DIRECTION;

	#define ROLLTEXT_TIMERID						20		//文字移动定时器ID
	#define ROLLTEXT_TIMERID_SPAN			100U		//文字移动定时器时间间隔

	class UILIB_API CRollTextUI : public CLabelUI
	{
	public:
		CRollTextUI(void);
		~CRollTextUI(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		virtual void DoInit();

	public:	
		virtual void SetText(LPCTSTR pstrText);
		virtual void PaintText(HDC hDC);
		virtual void DoEvent(TEventUI& event);
		virtual void SetPos(RECT rc);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		
	public:
		void BeginRoll();
		void EndRoll();
		
		void SetRollText(BOOL bRollText);
		void SetRollTextStep(int nStep);
		void SetOverStop(BOOL bOverStop);
		void SetRollType(LPCTSTR lpszRollType);
	protected:

	private:    
		//每次移动文字的步长
		int m_nStep;      
		int m_nScrollYPos;      
		int m_nScrollXPos;      
		//是否使用RollText滚动显示文字
		BOOL m_bRollText;  
		//当鼠标悬浮在滚动文字上时，是否需要停止
		BOOL m_bOverStop;
		//文字的累计高度(垂直移动时)，
		int m_nTextHeight;      
		//文字最宽宽度(垂直移动时)
		int m_nTextWidth;       
		//文字是否已经改变，如果改变需要重新计算显示宽带和高度
		BOOL m_bTextChanged;
		BOOL m_bTextPosed;
		//文字移动方向
		ROLLTEXT_DIRECTION m_nRollDirection;   
	};
}	// namespace DuiLib