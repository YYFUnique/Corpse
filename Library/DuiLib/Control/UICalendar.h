#pragma once

namespace DuiLib
{
	typedef struct tagCalendarStyle
	{
		DWORD dwMainTitleColor;					//日历主标题背景颜色
		DWORD dwSubTitleColor;						//日历副标题背景颜色
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
		DWORD dwStatusBkColor;						//底部信息栏背景色
	}TCalendarStyle;
	
	typedef struct tag_CalendarInfo
	{
		int		nYear;
		int		nMooth;
		int		nDay;
		int		nAsMooth;
		int		nWeek;
		int		nWeekLine;
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
		void ChangeDateTime(int nYear,int nMooth,int nDay);
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
		void OnNextYear(TNotifyUI* pMsg);
		void OnSelectMooth(TNotifyUI* pMsg);
		void OnSelcetDay(TNotifyUI* pMsg);
		void OnToday(TNotifyUI* pMsg);

		bool Notify(LPVOID lParam);
		void OnClick(TNotifyUI* pMsg);
		void OnKillFocus(TNotifyUI* pMsg);
		void OnSelectChanged(TNotifyUI* pMsg);
	public:
		void SetEnabledMoothSel(bool bEnabled = true);
		bool GetEnabledMoothSel();
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
		CDuiString					m_sComboTargetName;
		CButtonUI*					m_pLastYear;					//去年
		CButtonUI*					m_pMoothSelect;			//月份选择按钮
		CButtonUI*					m_pNextYear;					//明年
		CHorizontalLayoutUI*	m_pMainTitleHoriz;			//主标题，显示年份
		CHorizontalLayoutUI*	m_pSubTitleHoriz;			//副标题，显示星期

		CHorizontalLayoutUI*	m_pMonthPanelHorz;		//月份选择栏
		CVerticalLayoutUI*		m_pWeekPanelVert;		//日期显示栏
		CHorizontalLayoutUI*	m_pInfoPanelHorz;			//底部信息栏
		CButtonUI*					pDateTimeBtn;
		CButtonUI*					m_pBtnToDay;					//

		int								mYear;
		int								mMonth;
		int								mToday;
		bool								pEnabledYearSel;
		bool								pEnabledMoothSel;

		SYSTEMTIME				m_LocalTime;
		TCalendarStyle			m_CalendarStyle;
	};
}
