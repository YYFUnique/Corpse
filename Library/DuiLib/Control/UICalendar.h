#pragma once

namespace DuiLib
{
	typedef struct tagCalendarStyle
	{
		DWORD dwMainTitleColor;					//���������ⱳ����ɫ
		DWORD dwSubTitleColor;						//���������ⱳ����ɫ
		DWORD dwWeekendColor;					//��ĩ������ɫ
		DWORD dwWeekdayColor;					//������������ɫ
		CDuiString strHotImage;							//���ڻ�ȡ���㱳��ͼƬ
		DWORD dwDayHotColor;						//���ڻ�ȡ���㱳����ɫ
		DWORD dwDayHotTextColor;				//���ڻ�ȡ����������ɫ
		CDuiString strPushedImage;					//���ڱ����µı���ͼƬ
		DWORD dwDayPushedColor;					//���ڱ����µı�����ɫ
		CDuiString strSelectedImage;					//���ڱ�ѡ�еı�����ɫ
		DWORD dwDaySelectedColor;				//���ڱ�ѡ�еı�����ɫ
		DWORD dwDaySelectedTextColor;		//���ڱ�ѡ�е�������ɫ
		DWORD dwDayDisabledColor;				//���ڱ����õı�����ɫ
		DWORD dwNotMonthDayColor;			//�Ǳ������ڱ�����ɫ
		DWORD dwStatusBkColor;						//�ײ���Ϣ������ɫ
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
		CButtonUI*					m_pLastYear;					//ȥ��
		CButtonUI*					m_pMoothSelect;			//�·�ѡ��ť
		CButtonUI*					m_pNextYear;					//����
		CHorizontalLayoutUI*	m_pMainTitleHoriz;			//�����⣬��ʾ���
		CHorizontalLayoutUI*	m_pSubTitleHoriz;			//�����⣬��ʾ����

		CHorizontalLayoutUI*	m_pMonthPanelHorz;		//�·�ѡ����
		CVerticalLayoutUI*		m_pWeekPanelVert;		//������ʾ��
		CHorizontalLayoutUI*	m_pInfoPanelHorz;			//�ײ���Ϣ��
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
