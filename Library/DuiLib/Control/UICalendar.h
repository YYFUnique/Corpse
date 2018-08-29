#pragma once

namespace DuiLib
{
	typedef struct tagCalendarStyle
	{
		DWORD dwWeekendColor;					//周末文字颜色
		DWORD dwWeekdayColor;					//工作日文字颜色
		CDuiString strHotImage;							//日期获取焦点背景图片
		DWORD dwDayHotColor;						//日期获取焦点背景颜色
		DWORD dwDayHotTextColor;				//日期获取焦点文字颜色
		CDuiString strPushedImage;					//日期被按下的背景图片
		DWORD dwDayPushedColor;					//日期被按下的背景颜色
		CDuiString strSelectedImage;					//日期被选中的背景颜色
		DWORD dwDaySelectedColor;				//日期被选中的背景颜色
		DWORD dwDaySelectedTextColor;		//日期被选中的文字颜色
		DWORD dwDayDisabledColor;				//日期被禁用的背景颜色
		DWORD dwNotMonthDayColor;			//非本月日期背景颜色
		//DWORD dwStatusBkColor;						//底部信息栏背景色
	}TCalendarStyle;
	
	typedef struct tag_CalendarInfo
	{
		int		nYear;
		int		nMooth;
		int		nDay;
		int		nAsMooth;									// 是否是本月日期（日历中可能显示上一月或下一月的日期）
		int		nWeek;										//	是否是工作日
		int		nWeekLine;									// 以周日为索引0的星期
	}TCalendarInfo;

	class  CCalendarUI : public CVerticalLayoutUI
	{
	public:
		CCalendarUI(void);
		~CCalendarUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CControlUI* CreateStatusPanel();
		void SetWeekPanel(CControlUI* pControl,int nWeek);
		void SetDayPanel(COptionUI* pControl,bool bWeekEnd,TCalendarInfo* pCalendarInfo);
		void SetDayPanel(COptionUI* pControl,bool bWeekEnd,int nYear,int nMooth,int nDay,int nWeek,int nWeekLine,int nAsMooth);
		void UpdateMainTitle(int nYear,int nMooth);
	public:
		bool IsLeapYear(int nYear);
		int  DaysOfMonth(int nMooth,int nYear = -1);
		void ShowCalendar(int nYear,int nMooth);
		void SetDateTime(TCalendarInfo* pTCalendarInfo);
		void SetDateTime(int nYear,int nMooth,int nDay);
		CDuiString GetDateTime();
	public:
		void SetCalendarStyle(TCalendarStyle& CalendarStyle);

	public:
		void OnLastYear(TNotifyUI* pMsg);
		void OnMoothSelect(TNotifyUI* pMsg);
		void OnLastMonth(TNotifyUI* pMsg);
		void OnNextMonth(TNotifyUI* pMsg);
		void OnNextYear(TNotifyUI* pMsg);
		void OnSelectMooth(TNotifyUI* pMsg);
		void OnSelcetDay(TNotifyUI* pMsg);
		void OnToday(TNotifyUI* pMsg);

		bool Notify(LPVOID lParam);
		void OnClick(TNotifyUI* pMsg);
		void OnKillFocus(TNotifyUI* pMsg);
		void OnSelectChanged(TNotifyUI* pMsg);
	public:
		void SetEnabledMonthSel(bool bEnabled = true);
		bool GetEnabledMonthSel();
		void SetEnabledYearSel(bool bEnabled = true);
		bool GetEnabledYearSel();
		void SetMainTitleHeight(int nHeight);
		int GetMainTitleHeight();
		void SetSubTitleHeight(int nHeight);
		int GetSubTitleHeight();
		void SetStatusInfoHeight(int nHeight);
		int GetStatusInfoHeight();

		void ParseCalendarStyle(LPCTSTR lpszStyle,TCalendarStyle* pCalendarStyle = NULL);
	protected:
		void NormalizeTime(int& nYear,int& nMonth);
	private:
		CButtonUI*					m_pLastYear;					//去年
		CButtonUI*					m_pLastMonth;				// 上一月
		CLabelUI*					m_pMonthSelect;			//月份选择按钮
		CButtonUI*					m_pNextMonth;				//下一月
		CButtonUI*					m_pNextYear;					//明年
		CHorizontalLayoutUI*	m_pMainTitleHoriz;			//主标题，显示年份
		CHorizontalLayoutUI*	m_pSubTitleHoriz;			//副标题，显示星期

		CVerticalLayoutUI*		m_pWeekPanelVert;		//日期显示栏
		CHorizontalLayoutUI*	m_pInfoPanelHorz;			//底部信息栏
		CButtonUI*					m_pBtnToDay;					//

		int								m_nYear;								// 日期年份
		int								m_nMonth;							//	日期月份
		int								m_nToday;							// 日期号数
		bool								pEnabledYearSel;
		bool								pEnabledMonthSel;

		SYSTEMTIME				m_LocalTime;
		TCalendarStyle			m_CalendarStyle;
	};
}
