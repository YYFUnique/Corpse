#include "stdafx.h"
#include "UICalendar.h"
#include <time.h>
#include <atltime.h>

namespace DuiLib 
{
	#define		TODAY_TEXT							_T("今天")
	#define		TODAY_NAME							_T("BtnToday")
	#define		MONTH_LIST							_T("MonthList")
	#define		DAY_GROUP_NAME				_T("DayGroupName")
	#define		MONTH_GROUP_NAME			_T("MoothGroupName")

	#define		TOOLTIP_LAST_YEAR				_T("上一年")
	#define		TOOLTIP_LAST_MONTH			_T("上一月")
	#define		TOOLTIP_NEXT_MONTH		_T("下一月")
	#define		TOOLTIP_NEXT_YEAR				_T("下一年")

	CCalendarUI::CCalendarUI()
		: m_pBtnToDay(NULL)
		, pEnabledYearSel(true)
		, pEnabledMonthSel(true)
		, m_pInfoPanelHorz(NULL)
		, m_pWeekPanelVert(NULL)
		, m_pMainTitleHoriz(NULL)
	{  
		GetLocalTime(&m_LocalTime);
		SetDateTime(m_LocalTime.wYear,m_LocalTime.wMonth,m_LocalTime.wDay);
		SetBkColor(0xFFF6F6F7);
		ZeroMemory(&m_CalendarStyle,sizeof(m_CalendarStyle));
		m_CalendarStyle.dwDayHotColor = 0xFF2D97DF;
		m_CalendarStyle.dwDayHotTextColor = 0xFFFFFFFF;
		m_CalendarStyle.dwDaySelectedColor = 0xFF1D73AB;
		m_CalendarStyle.dwDaySelectedTextColor = 0xFFFFFFFF;
		m_CalendarStyle.dwWeekdayColor = 0xFF000000;
		m_CalendarStyle.dwWeekendColor = 0xFFFF0000;
		m_CalendarStyle.dwNotMonthDayColor = 0xFFA6A5A9;
	
		//初始化日历主标题容器与元素
		m_pMainTitleHoriz		= new CHorizontalLayoutUI();
		m_pLastYear			= new CButtonUI();
		m_pLastMonth		=	new CButtonUI();
		m_pMonthSelect		= new CLabelUI();
		m_pNextMonth		=	new CButtonUI();
		m_pNextYear			= new CButtonUI();

		m_pLastYear->SetFixedWidth(30);
		m_pLastYear->SetToolTip(TOOLTIP_LAST_YEAR);
		m_pLastMonth->SetFixedWidth(30);
		m_pLastMonth->SetToolTip(TOOLTIP_LAST_MONTH);
		m_pNextMonth->SetFixedWidth(30);
		m_pNextMonth->SetToolTip(TOOLTIP_NEXT_MONTH);
		m_pNextYear->SetFixedWidth(30);
		m_pNextYear->SetToolTip(TOOLTIP_NEXT_YEAR);
		
		m_pMainTitleHoriz->SetFixedHeight(27);
		m_pMonthSelect->SetAutoCalcWidth(true);

 		m_pLastYear->OnNotify	+= MakeDelegate(this,&CCalendarUI::Notify);
		m_pLastMonth->OnNotify += MakeDelegate(this,&CCalendarUI::Notify);
 		//m_pMoothSelect->OnNotify	+= MakeDelegate(this,&CCalendarUI::Notify);
		m_pNextMonth->OnNotify += MakeDelegate(this, &CCalendarUI::Notify);
 		m_pNextYear->OnNotify	+= MakeDelegate(this,&CCalendarUI::Notify);

		m_pMainTitleHoriz->Add(m_pLastYear);
		m_pMainTitleHoriz->Add(new CControlUI());
		m_pMainTitleHoriz->Add(m_pLastMonth);
		m_pMainTitleHoriz->Add(new CControlUI());
		m_pMainTitleHoriz->Add(m_pMonthSelect);
		m_pMainTitleHoriz->Add(new CControlUI());
		m_pMainTitleHoriz->Add(m_pNextMonth);
		m_pMainTitleHoriz->Add(new CControlUI());
		m_pMainTitleHoriz->Add(m_pNextYear);

		//初始化日历副标题容器与元素
		m_pSubTitleHoriz	= new CHorizontalLayoutUI();
		m_pSubTitleHoriz->SetFixedHeight(22);
	
		for(int nWeekIndex = 0; nWeekIndex < 7; nWeekIndex++)
		{
			CButtonUI* pWeek	= new CButtonUI();
			pWeek->SetMouseEnabled(false);
			SetWeekPanel(pWeek, nWeekIndex);
			m_pSubTitleHoriz->Add(pWeek);
		}

		//初始化日期容器与元素
		m_pWeekPanelVert	= new CVerticalLayoutUI();
		for(int nWeekLine = 1; nWeekLine < 7; nWeekLine++)
		{
			CHorizontalLayoutUI* pWeekLine = new CHorizontalLayoutUI();
			for(int nDay = 1; nDay < 8; nDay++)
			{
				CHorizontalLayoutUI* pDayParent = new CHorizontalLayoutUI();

				COptionUI*	pDay = new COptionUI();
				pDay->SetGroup(DAY_GROUP_NAME);
				pDay->OnNotify+= MakeDelegate(this,&CCalendarUI::Notify);
				pDayParent->Add(pDay);
				
				TCalendarInfo* pCalendarInfo	= new TCalendarInfo();
				pDayParent->SetTag((UINT_PTR)pCalendarInfo);
				pWeekLine->Add(pDayParent);
			}
			m_pWeekPanelVert->Add(pWeekLine);
		}

		//初始化信息栏容器与元素
		m_pInfoPanelHorz	= new CHorizontalLayoutUI();
		m_pBtnToDay = new CButtonUI();
		m_pBtnToDay->OnNotify += MakeDelegate(this, &CCalendarUI::Notify);
		m_pBtnToDay->SetFixedWidth(135);
		m_pInfoPanelHorz->Add(m_pBtnToDay);
		m_pBtnToDay->SetFloatAlign(DT_CENTER|DT_VCENTER);
		m_pInfoPanelHorz->SetFixedHeight(25);

		Add(m_pMainTitleHoriz);
		Add(m_pSubTitleHoriz);
		Add(m_pWeekPanelVert);
		Add(m_pInfoPanelHorz);

		//初始化默认样式
		SetCalendarStyle(m_CalendarStyle);
		//初始化日历数据
		ShowCalendar(m_LocalTime.wYear,m_LocalTime.wMonth);
	}

	CCalendarUI::~CCalendarUI(void)
	{

	}
	 
	LPCTSTR CCalendarUI::GetClass() const
	{ 
		return _T("CalendarUI"); 
	}

	LPVOID CCalendarUI::GetInterface( LPCTSTR pstrName )
	{ 
		if( _tcscmp(pstrName, DUI_CTR_CALENDAR) == 0 ) return static_cast<CCalendarUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CCalendarUI::SetCalendarStyle(TCalendarStyle& CalendarStyle)
	{
		m_pLastYear->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pLastYear->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pLastYear->SetPushedBkColor(CalendarStyle.dwDayHotColor);
		m_pLastMonth->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pLastMonth->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pLastMonth->SetPushedBkColor(CalendarStyle.dwDayHotColor);
		m_pNextMonth->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pNextMonth->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pNextMonth->SetPushedBkColor(CalendarStyle.dwDayHotColor);
		m_pNextYear->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pNextYear->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pNextYear->SetPushedBkColor(CalendarStyle.dwDayHotColor);

		for(int nWeekIndex = 0;nWeekIndex < 6;nWeekIndex++)
		{
			CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(m_pWeekPanelVert->GetItemAt(nWeekIndex));
			if (pWeekLine == NULL)
				continue;

			for(int nDayIndex = 0;nDayIndex < 7;nDayIndex++)
			{
				CVerticalLayoutUI* pDayParent = static_cast<CVerticalLayoutUI*>(pWeekLine->GetItemAt(nDayIndex));
				if (pDayParent == NULL)
					continue;

				COptionUI* pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));
				if (pDay == NULL)
					continue;

				if (nDayIndex == 0 || nDayIndex == 6)
					pDay->SetTextColor(CalendarStyle.dwWeekendColor);
				else
					pDay->SetTextColor(CalendarStyle.dwWeekdayColor);

				pDay->SetHotBkColor(CalendarStyle.dwDayHotColor);
				pDay->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
				pDay->SetSelectedBkColor(CalendarStyle.dwDaySelectedColor);
				pDay->SetSelectedTextColor(CalendarStyle.dwDaySelectedTextColor);
			}
		}

		m_pBtnToDay->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pBtnToDay->SetPushedBkColor(CalendarStyle.dwDayHotColor);
		m_pBtnToDay->SetPushedTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pBtnToDay->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
	}

	bool CCalendarUI::IsLeapYear( int nYear )
	{
		return ((nYear % 4 == 0 && nYear % 100 != 0) || nYear % 400 == 0);
	}

	int CCalendarUI::DaysOfMonth( int _Mooth,int _Year /*= -1*/ )
	{
		int nYear = _Year == -1?m_LocalTime.wYear :_Year;

		switch (_Mooth)
		{
			case 1 :
			case 3 :
			case 5 :
			case 7 :
			case 8 :
			case 10 :
			case 12 : return 31 ;
			case 4 :
			case 6 :
			case 9 :
			case 11 : return 30 ;
			case 2 : 
				return IsLeapYear(nYear)?29:28;
		}  
		return 0 ;
	}

	void CCalendarUI::NormalizeTime(int& nYear,int& nMonth)
	{
		if (nMonth > 12)
		{
			m_LocalTime.wMonth = nMonth = 1;
			m_LocalTime.wYear = ++nYear;
		}

		if (nMonth < 1) 
		{
			m_LocalTime.wMonth = nMonth = 12;
			m_LocalTime.wYear =--nYear;
		}
	}

	void CCalendarUI::ShowCalendar(int nYear, int nMonth)
	{
		NormalizeTime(nYear, nMonth);
		UpdateMainTitle(nYear, nMonth);

		int nLastMaxDay		= nMonth == 1 ?DaysOfMonth(12,nYear-1):DaysOfMonth(nMonth-1,nYear);
		int nLastDay				= 0;
		int nAsMooth			= 0;													// 
		int nDays					= DaysOfMonth(nMonth,nYear);	// 本月有多少天数
		int nWeek				= 0;
		int nCalDay				= 0;

		SYSTEMTIME SystemTime = {0};
		SystemTime.wYear = nYear;
		SystemTime.wMonth = nMonth;
		SystemTime.wDay = 1;
		SystemTime.wHour = SystemTime.wMinute = SystemTime.wSecond = 0;
		//尽早替换该方法
		CTime Time(SystemTime);
		nWeek = Time.GetDayOfWeek() - 1;

		nAsMooth	= nWeek == 0 ? 0 : nMonth-1;						// 计算上一月需要显示天数
		nLastDay		= nWeek == 0 ? 0 : nLastMaxDay-nWeek;		// 计算上一月最后一天的号数

		COptionUI* pShowDay = NULL;	//显示选中的日期
		for(int nWeekIndex = 0; nWeekIndex < 6; nWeekIndex++)
		{
			CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(m_pWeekPanelVert->GetItemAt(nWeekIndex));
			// 计算一周内的序号
			for(int nDayIndex = 0;nDayIndex < 7; nDayIndex++)
			{
				CVerticalLayoutUI* pDayParent = static_cast<CVerticalLayoutUI*>(pWeekLine->GetItemAt(nDayIndex));
				if (pDayParent == NULL)
					continue;

				TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pDayParent->GetTag();
				if (pTCalendarInfo == NULL)
					continue;

				if (!(nWeek && nDayIndex < nWeek && nCalDay <= nDays))
				{
					nCalDay++;
					nAsMooth = nCalDay <= nDays ? nMonth : nMonth + 1;
				}

				if ((nLastDay <= nLastMaxDay && nAsMooth != nMonth) || nCalDay >= nDays){
					++nLastDay;
					nAsMooth	= nMonth+1;
				}

				if (nCalDay == nDays) {
					nLastDay = 0;
					nAsMooth	= nMonth;
				}
				
				pTCalendarInfo->nYear			= nYear;
				pTCalendarInfo->nMooth		= nMonth;
				pTCalendarInfo->nDay			= ((nWeek && nDayIndex < nWeek) || nCalDay > nDays) ? nLastDay : nCalDay;
				pTCalendarInfo->nWeekLine	= nWeekIndex;
				pTCalendarInfo->nWeek		= nWeek;
				pTCalendarInfo->nAsMooth	= nMonth;

				if (nCalDay < 1)
					pTCalendarInfo->nAsMooth= nMonth -1;
				else if (nCalDay > nDays)
					pTCalendarInfo->nAsMooth= nMonth +1;
				
				bool bWeekend = false;
				if (nDayIndex == 0 || nDayIndex == 6)
					bWeekend = true;

				COptionUI* pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));
				SetDayPanel(pDay, bWeekend, pTCalendarInfo);

				if (nDays <= m_nToday && nDays == nCalDay)
					pShowDay = pDay;
				else if (nDays > m_nToday && m_nToday == nCalDay)
					pShowDay = pDay;
			}
			nWeek = 0;
		}

		if (pShowDay != NULL)
			pShowDay->Selected(true);
	}

	bool CCalendarUI::Notify(LPVOID lParam)
	{
		TNotifyUI* pMsg = (TNotifyUI*)lParam;
		if (pMsg->sType == DUI_MSGTYPE_CLICK)
			OnClick(pMsg);
		else if (pMsg->sType == DUI_MSGTYPE_KILLFOCUS)
			OnKillFocus(pMsg);
		else if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
			OnSelectChanged(pMsg);

		return true;
	}

	void CCalendarUI::OnClick(TNotifyUI* pMsg)
	{
		if (pMsg->pSender == m_pMonthSelect)
			OnMoothSelect(pMsg);
		else if (pMsg->pSender == m_pBtnToDay)
			OnToday(pMsg);
		else if (pMsg->pSender == m_pLastYear)
			OnLastYear(pMsg);
		else if (pMsg->pSender == m_pLastMonth)
			OnLastMonth(pMsg);
		else if (pMsg->pSender == m_pNextMonth)
			OnNextMonth(pMsg);
		else if (pMsg->pSender == m_pNextYear)
			OnNextYear(pMsg);
	}

	void CCalendarUI::OnKillFocus(TNotifyUI* pMsg)
	{
		
	}

	void CCalendarUI::OnSelectChanged(TNotifyUI* pMsg)
	{
		COptionUI* pDay = (COptionUI*)pMsg->pSender->GetInterface(DUI_CTR_OPTION);
		
		if (pDay && _tcscmp(pDay->GetGroup(), DAY_GROUP_NAME) == 0)
		{
			// 由于选择月份或者年份时，可能导致日期选择变动，需要判断是否是手动选择日期
			RECT rcPos = pDay->GetPos();
			if (PtInRect(&rcPos, pMsg->ptMouse))
				OnSelcetDay(pMsg);
		}
	}

	//上一年
	void CCalendarUI::OnLastYear(TNotifyUI* pMsg)
	{
		if(GetEnabledYearSel())
			ShowCalendar(--m_LocalTime.wYear,m_LocalTime.wMonth);
	}

	//上一月
	void CCalendarUI::OnLastMonth(TNotifyUI* pMsg)
	{
		if(GetEnabledYearSel())
			ShowCalendar(m_LocalTime.wYear, --m_LocalTime.wMonth);
	}

	// 下一月
	void CCalendarUI::OnNextMonth(TNotifyUI* pMsg)
	{
		if(GetEnabledYearSel())
			ShowCalendar(m_LocalTime.wYear, ++m_LocalTime.wMonth);
	}

	//下一年
	void CCalendarUI::OnNextYear(TNotifyUI* pMsg)
	{
		if(GetEnabledYearSel())
			ShowCalendar(++m_LocalTime.wYear,m_LocalTime.wMonth);
	}

	//在弹出的月份列表中选择月份
	void CCalendarUI::OnSelectMooth(TNotifyUI* pMsg)
	{
		if(GetEnabledMonthSel())
			ShowCalendar(m_LocalTime.wYear,pMsg->pSender->GetTag());
	}

	//点击月份选择按钮，弹出月份选择列表
	void CCalendarUI::OnMoothSelect(TNotifyUI* pMsg)
	{
		if (GetEnabledMonthSel() == false)
			return;
	}

	//点击今天按钮
	void CCalendarUI::OnToday(TNotifyUI* pMsg)
	{
		GetLocalTime(&m_LocalTime);
		SetDateTime(m_LocalTime.wYear, m_LocalTime.wMonth, m_LocalTime.wDay);
		ShowCalendar(m_LocalTime.wYear, m_LocalTime.wMonth);
	}

	void CCalendarUI::OnSelcetDay(TNotifyUI* pMsg)
	{
		TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pMsg->pSender->GetParent()->GetTag();

		SetDateTime(pTCalendarInfo);

		if (GetEnabledMonthSel() && pTCalendarInfo && pTCalendarInfo->nMooth != pTCalendarInfo->nAsMooth)
			ShowCalendar(pTCalendarInfo->nYear, pTCalendarInfo->nAsMooth);
	}

	void CCalendarUI::SetWeekPanel(CControlUI* pWeek,int nIndex)
	{
		static LPCTSTR lpszWeekName[] = {_T("日"), _T("一"), _T("二"), _T("三"), _T("四"), _T("五"), _T("六")};
		pWeek->SetText(lpszWeekName[nIndex]);
		if (nIndex ==0 || nIndex == 6)
		{
			CButtonUI* pBtnWeek = (CButtonUI*)pWeek->GetInterface(DUI_CTR_BUTTON);
			if (pBtnWeek)
				pBtnWeek->SetTextColor(m_CalendarStyle.dwWeekendColor);
		}
	}

	void CCalendarUI::SetDayPanel(COptionUI* pDay,bool bWeekEnd,TCalendarInfo* pCalendarInfo)
	{ 
		SetDayPanel(pDay,bWeekEnd,pCalendarInfo->nYear,pCalendarInfo->nMooth,
							pCalendarInfo->nDay,pCalendarInfo->nWeek,pCalendarInfo->nWeekLine,pCalendarInfo->nAsMooth);
	}

	void CCalendarUI::SetDayPanel(COptionUI* pDay,bool bWeekEnd,int nYear,int nMooth,int nDay,int nWeek,int nWeekLine,int nAsMooth)
	{
		if(nAsMooth != nMooth)
			pDay->SetTextColor(m_CalendarStyle.dwNotMonthDayColor);
		else if (bWeekEnd)
			pDay->SetTextColor(m_CalendarStyle.dwWeekendColor);
		else 
			pDay->SetTextColor(m_CalendarStyle.dwWeekdayColor);

		if(nMooth != nAsMooth)
			pDay->SetMouseEnabled(GetEnabledMonthSel());
		else 
			pDay->SetMouseEnabled(true);
		
		TCHAR szDayText[10];
		_stprintf(szDayText,_T("%d"),nDay);
		pDay->SetText(szDayText);
	}

	void CCalendarUI::SetEnabledYearSel(bool bEnabled /*= true*/)
	{
		pEnabledYearSel = bEnabled;
	}

	bool CCalendarUI::GetEnabledYearSel()
	{
		return pEnabledYearSel;
	}

	void CCalendarUI::SetEnabledMonthSel(bool bEnabled /*= true*/)
	{
		pEnabledMonthSel = bEnabled;
	}

	bool CCalendarUI::GetEnabledMonthSel()
	{
		return pEnabledMonthSel;
	}

	void CCalendarUI::UpdateMainTitle( int _Year, int _Mooth)
	{
		TCHAR szDateTime[50];
		_stprintf(szDateTime,_T("%4d-%02d"), _Year, _Mooth);
		m_pMonthSelect->SetText(szDateTime);

		static BOOL bInit = FALSE;
		if (bInit == FALSE)
		{
			m_pLastYear->SetText(_T("<<"));
			m_pLastMonth->SetText(_T("<"));
			m_pNextMonth->SetText(_T(">"));
			m_pNextYear->SetText(_T(">>"));
			SYSTEMTIME SysTime;
			GetLocalTime(&SysTime);
			_stprintf_s(szDateTime, _countof(szDateTime), _T("今日：%d年%d月%d日"), 
								SysTime.wYear, SysTime.wMonth, SysTime.wDay);
			m_pBtnToDay->SetText(szDateTime);
		}
	}

	void CCalendarUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("enableyearsel")) == 0 ) SetEnabledYearSel(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("enablemoothsel")) == 0 ) SetEnabledYearSel(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("mtheight")) == 0 ) SetMainTitleHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("stheight")) == 0 ) SetSubTitleHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("itheight")) == 0 ) SetStatusInfoHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("style")) == 0 ) ParseCalendarStyle(pstrValue);
		else CVerticalLayoutUI::SetAttribute(pstrName,pstrValue);
	}

	CControlUI* CCalendarUI::CreateStatusPanel()
	{
		CHorizontalLayoutUI* pControl	= new CHorizontalLayoutUI();
		m_pBtnToDay						= new CButtonUI();
		
		pControl->Add(new CControlUI());
		pControl->Add(m_pBtnToDay);
		
		m_pBtnToDay->OnNotify			   += MakeDelegate(this,&CCalendarUI::Notify);
		m_pBtnToDay->SetText(TODAY_TEXT);
		m_pBtnToDay->SetName(TODAY_NAME);
		m_pBtnToDay->SetFixedWidth(60);
		
		return pControl;
	}

	void CCalendarUI::SetMainTitleHeight(int nHeight)
	{
		m_pMainTitleHoriz->SetFixedHeight(nHeight);
	}

	int CCalendarUI::GetMainTitleHeight()
	{
		return m_pMainTitleHoriz->GetFixedHeight();
	}

	void CCalendarUI::SetSubTitleHeight(int nHeight)
	{
		m_pSubTitleHoriz->SetFixedHeight(nHeight);
	}

	int CCalendarUI::GetSubTitleHeight()
	{
		return m_pSubTitleHoriz->GetFixedHeight();
	}

	void CCalendarUI::SetStatusInfoHeight(int nHeight)
	{
		m_pInfoPanelHorz->SetFixedHeight(nHeight);
	}

	int CCalendarUI::GetStatusInfoHeight()
	{
		return m_pInfoPanelHorz->GetFixedHeight();
	}

	void CCalendarUI::ParseCalendarStyle(LPCTSTR lpszStyle,TCalendarStyle* pCalendarStyle /*= NULL*/)
	{
		CDuiString sItem;
		CDuiString sValue;
		LPTSTR pstr = NULL;

		TCalendarStyle* pCalendarInfo = pCalendarStyle?pCalendarStyle:&m_CalendarStyle;

		while( *lpszStyle != _T('\0') ) {
			sItem.Empty();
			sValue.Empty();
			while( *lpszStyle > _T('\0') && *lpszStyle <= _T(' ') ) lpszStyle = ::CharNext(lpszStyle);
			while( *lpszStyle != _T('\0') && *lpszStyle != _T('=') && *lpszStyle > _T(' ') ) {
				LPTSTR pstrTemp = ::CharNext(lpszStyle);
				while( lpszStyle < pstrTemp) {
					sItem += *lpszStyle++;
				}
			}
			while( *lpszStyle > _T('\0') && *lpszStyle <= _T(' ') ) lpszStyle = ::CharNext(lpszStyle);
			if( *lpszStyle++ != _T('=') ) break;
			while( *lpszStyle > _T('\0') && *lpszStyle <= _T(' ') ) lpszStyle = ::CharNext(lpszStyle);
			if( *lpszStyle++ != _T('\'') ) break;
			while( *lpszStyle != _T('\0') && *lpszStyle != _T('\'') ) {
				LPTSTR pstrTemp = ::CharNext(lpszStyle);
				while( lpszStyle < pstrTemp) {
					sValue += *lpszStyle++;
				}
			}
			if (*lpszStyle++ != _T('\'')) break;
			if (sValue.IsEmpty() == false) {
				/*if( sItem == _T("bbcolor") ) pCalendarInfo->nCalenderBorderColor = sValue;
				else if( sItem == _T("bkcolor") ) pCalendarInfo->nCalendarBkColor = sValue;
				else if( sItem == _T("tbkcolor") ) pCalendarInfo->nMainTitleBkColor = sValue;
				else if( sItem == _T("sbkcolor") ) pCalendarInfo->nSubTitleBkColor = sValue;
				else if( sItem == _T("webkcolora") ) pCalendarInfo->nWeekendColorA = sValue;
				else if( sItem == _T("webkcolorb") ) pCalendarInfo->nWeekendColorB = sValue;
				else if( sItem == _T("dayhotcolor") ) pCalendarInfo->nDayHotColor = sValue;
				else if( sItem == _T("daypushedcolor") ) pCalendarInfo->nDayPushedColor = sValue;
				else if( sItem == _T("dayselcolor") ) pCalendarInfo->nDaySelectColor = sValue;
				else if( sItem == _T("daydiscolor") ) pCalendarInfo->nDayDisabledColor = sValue;
				else if( sItem == _T("nocurmonthdaycolor") ) pCalendarInfo->nNoCurMonthDayColor = sValue;
				else if( sItem == _T("weekicolora") ) pCalendarInfo->nWeekIntervalColorA = sValue;
				else if( sItem == _T("weekicolorb") ) pCalendarInfo->nWeekIntervalColorB = sValue;
				else if( sItem == _T("ibkcolor") ) pCalendarInfo->nStatusBkColor = sValue;*/
			}
			if( *lpszStyle++ != _T(' ') ) break;
		}
		SetCalendarStyle(*pCalendarInfo);
	}

	void CCalendarUI::SetDateTime(TCalendarInfo* pTCalendarInfo)
	{
		if (pTCalendarInfo)
			SetDateTime(pTCalendarInfo->nYear, pTCalendarInfo->nMooth, pTCalendarInfo->nDay);
	}

	void CCalendarUI::SetDateTime(int nYear,int nMooth,int nDay)
	{ 
		m_nYear		=	nYear;
		m_nMonth  =	nMooth;
		m_nToday	=	nDay; 
	}

	CDuiString CCalendarUI::GetDateTime()
	{
		TCHAR szTimeDate[20];
		_stprintf(szTimeDate,_T("%4d-%02d-%02d"), m_nYear, m_nMonth, m_nToday);

		return szTimeDate;
	}
}
