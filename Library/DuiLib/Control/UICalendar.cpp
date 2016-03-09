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

	CCalendarUI::CCalendarUI()
		: pDateTimeBtn(NULL)
		, m_pBtnToDay(NULL)
		, pEnabledYearSel(true)
		, pEnabledMoothSel(true)
		, m_pInfoPanelHorz(NULL)
		, m_pWeekPanelVert(NULL)
		, m_pMainTitleHoriz(NULL)
		, m_pMonthPanelHorz(NULL)
	{  
		GetLocalTime(&m_LocalTime);
		SetDateTime(m_LocalTime.wYear,m_LocalTime.wMonth,m_LocalTime.wDay);
		
		ZeroMemory(&m_CalendarStyle,sizeof(m_CalendarStyle));
		m_CalendarStyle.dwMainTitleColor = 0xFF8DE316;
		m_CalendarStyle.dwSubTitleColor = 0xFFFFFFFF;
		m_CalendarStyle.dwDayHotColor = 0xFF3721F1;
		m_CalendarStyle.dwDayHotTextColor = 0xFFFFFFFF;
		m_CalendarStyle.dwDaySelectedColor = 0xFF3721F1;
		m_CalendarStyle.dwDaySelectedTextColor = 0xFFFFFFFF;
		m_CalendarStyle.dwWeekdayColor = 0xFF000000;
		m_CalendarStyle.dwWeekendColor = 0xFF0000FF;
		m_CalendarStyle.dwNotMonthDayColor = 0xFFDEDEDE;
		m_CalendarStyle.dwStatusBkColor = 0xFFF3ED8E;
	
		//初始化日历主标题容器与元素
		m_pMainTitleHoriz		= new CHorizontalLayoutUI();
		m_pLastYear			= new CButtonUI();
		m_pMoothSelect	= new CButtonUI();
		m_pNextYear			= new CButtonUI();

		m_pLastYear->SetFixedWidth(60);
		m_pNextYear->SetFixedWidth(60);
		m_pMainTitleHoriz->SetFixedHeight(22);
		m_pMoothSelect->SetFixedWidth(80);

 		m_pLastYear->OnNotify	+= MakeDelegate(this,&CCalendarUI::Notify);
 		m_pMoothSelect->OnNotify	+= MakeDelegate(this,&CCalendarUI::Notify);
 		m_pNextYear->OnNotify	+= MakeDelegate(this,&CCalendarUI::Notify);

		m_pMainTitleHoriz->Add(m_pLastYear);
		m_pMainTitleHoriz->Add(new CControlUI());
		m_pMainTitleHoriz->Add(m_pMoothSelect);
		m_pMainTitleHoriz->Add(new CControlUI());
		m_pMainTitleHoriz->Add(m_pNextYear);

		//初始化日历副标题容器与元素
		m_pSubTitleHoriz	= new CHorizontalLayoutUI();
		m_pSubTitleHoriz->SetFixedHeight(22);
	
		for(int nWeekIndex = 0;nWeekIndex < 7;nWeekIndex++)
		{
			CButtonUI* pWeek	= new CButtonUI();
			pWeek->SetMouseEnabled(false);
			SetWeekPanel(pWeek,nWeekIndex);
			m_pSubTitleHoriz->Add(pWeek);
		}

		//初始化月份选择容器与元素
		CVerticalLayoutUI* pMonthSubPanelA = new CVerticalLayoutUI();
		CVerticalLayoutUI* pMonthSubPanelB = new CVerticalLayoutUI();
		m_pMonthPanelHorz	= new CHorizontalLayoutUI();
		m_pMonthPanelHorz->SetFloat(true);
		m_pMonthPanelHorz->SetName(MONTH_LIST);

		m_pMonthPanelHorz->Add(pMonthSubPanelA);
		m_pMonthPanelHorz->Add(pMonthSubPanelB);
		m_pMonthPanelHorz->SetFixedHeight(120);
		m_pMonthPanelHorz->SetMaxWidth(80);

		TCHAR szMonthDay[5];
		for(int nMonth = 1;nMonth < 13;nMonth++)
		{
			COptionUI* pMonth = new COptionUI();
			pMonth->SetGroup(MONTH_GROUP_NAME);

			_stprintf(szMonthDay,_T("%d"),nMonth);
			pMonth->SetText(szMonthDay);
			pMonth->SetTag(nMonth);
			pMonth->OnNotify+= MakeDelegate(this,&CCalendarUI::Notify);

			if (nMonth % 2)
				pMonthSubPanelA->Add(pMonth);
			else
				pMonthSubPanelB->Add(pMonth);
		}

		//初始化日期容器与元素
		m_pWeekPanelVert	= new CVerticalLayoutUI();
		for(int nWeekLine = 1;nWeekLine < 7;nWeekLine++)
		{
			CHorizontalLayoutUI* pWeekLine = new CHorizontalLayoutUI();
			for(int nDay = 1;nDay < 8;nDay++)
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
		CControlUI* pInfoPanel = CreateStatusPanel();
		m_pInfoPanelHorz->Add(pInfoPanel);
		m_pInfoPanelHorz->SetFixedHeight(22);
		m_pInfoPanelHorz->SetBkColor(m_CalendarStyle.dwStatusBkColor);

		Add(m_pMainTitleHoriz);
		Add(m_pSubTitleHoriz);
		Add(m_pWeekPanelVert);
		Add(m_pMonthPanelHorz);
		Add(m_pInfoPanelHorz);

		//初始化默认样式
		SetCalendarStyle(m_CalendarStyle);
		//初始化日历数据
		ShowCalendar(m_LocalTime.wYear,m_LocalTime.wMonth);
		ChangeDateTime(m_LocalTime.wYear,m_LocalTime.wMonth,m_LocalTime.wDay);
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
		m_pMainTitleHoriz->SetBkColor(CalendarStyle.dwMainTitleColor);
		m_pLastYear->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pLastYear->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pMoothSelect->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pMoothSelect->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);
		m_pNextYear->SetHotBkColor(CalendarStyle.dwDayHotColor);
		m_pNextYear->SetHotTextColor(CalendarStyle.dwDaySelectedTextColor);

		m_pSubTitleHoriz->SetBkColor(CalendarStyle.dwSubTitleColor);
		m_pMonthPanelHorz->SetBkColor(CalendarStyle.dwMainTitleColor);

		for (int n=0;n<m_pMonthPanelHorz->GetCount();++n)
		{
			CVerticalLayoutUI* pMonthPanel = (CVerticalLayoutUI*)m_pMonthPanelHorz->GetItemAt(n);
			for (int m=0;m<pMonthPanel->GetCount();++m)
			{
				COptionUI* pMonth = (COptionUI*)pMonthPanel->GetItemAt(m);
				pMonth->SetHotBkColor(CalendarStyle.dwDayHotColor);			
				pMonth->SetHotTextColor(CalendarStyle.dwDayHotTextColor);
				pMonth->SetSelectedTextColor(CalendarStyle.dwDaySelectedTextColor);
				pMonth->SetSelectedBkColor(CalendarStyle.dwDaySelectedColor);
			}
		}

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
		m_pBtnToDay->SetHotTextColor(CalendarStyle.dwDayHotTextColor);
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
			nMonth = 1;
			++nYear;
		}

		if (nMonth < 1) 
		{
			nMonth = 12;
			--nYear;
		}
	}

	void CCalendarUI::ShowCalendar(int nYear,int nMonth)
	{
		NormalizeTime(nYear,nMonth);
		UpdateMainTitle(nYear,nMonth);

		int nLastMaxDay		= nMonth == 1 ?DaysOfMonth(12,nYear-1):DaysOfMonth(nMonth-1,nYear);
		int nLastDay				= 0;
		int nAsMooth			= 0;
		int nDays					= DaysOfMonth(nMonth,nYear);
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

		nAsMooth	= nWeek == 0?0:nMonth-1;
		nLastDay		= nWeek == 0?0:nLastMaxDay-nWeek;
		for(int nWeekIndex = 0;nWeekIndex < 6;nWeekIndex++)
		{
			CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(m_pWeekPanelVert->GetItemAt(nWeekIndex));
			for(int nDayIndex = 0;nDayIndex < 7;nDayIndex++)
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
					nAsMooth = nCalDay <= nDays?nMonth:nMonth+1;
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
				pTCalendarInfo->nDay			= ((nWeek && nDayIndex < nWeek) || nCalDay > nDays)?nLastDay:nCalDay;
				pTCalendarInfo->nWeekLine	= nWeekIndex;
				pTCalendarInfo->nWeek		= nWeek;
				pTCalendarInfo->nAsMooth	= nMonth;

				if(nCalDay < 1)
					pTCalendarInfo->nAsMooth= nMonth -1;
				else if(nCalDay > nDays)
					pTCalendarInfo->nAsMooth= nMonth +1;
				
				bool bWeekend = false;
				if (nDayIndex == 0 || nDayIndex == 6)
					bWeekend = true;

				COptionUI* pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));
				SetDayPanel(pDay,bWeekend,pTCalendarInfo);
				pDay->Selected(nCalDay == mToday);
			}
			nWeek = 0;
		}
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
		if (pMsg->pSender == m_pMoothSelect)
			OnMoothSelect(pMsg);
		else if (pMsg->pSender == m_pBtnToDay)
			OnToday(pMsg);
		else if (pMsg->pSender == m_pLastYear)
			OnLastYear(pMsg);
		else if (pMsg->pSender == m_pNextYear)
			OnNextYear(pMsg);
	}

	void CCalendarUI::OnKillFocus(TNotifyUI* pMsg)
	{
		if (pMsg->pSender == m_pMoothSelect)
		{
			RECT rcPos = m_pMonthPanelHorz->GetPos();
			if (PtInRect(&rcPos,pMsg->ptMouse) == false)
				m_pMonthPanelHorz->SetVisible(false);
		}
	}

	void CCalendarUI::OnSelectChanged(TNotifyUI* pMsg)
	{
		COptionUI* pDay = (COptionUI*)pMsg->pSender->GetInterface(DUI_CTR_OPTION);
		if (pDay && _tcscmp(pDay->GetGroup(),DAY_GROUP_NAME) == 0)
			OnSelcetDay(pMsg);
		else if (pDay && _tcscmp(pDay->GetGroup(),MONTH_GROUP_NAME) == 0)
		{
			OnSelectMooth(pMsg);
			m_pMonthPanelHorz->SetVisible(false);
		}
	}

	//上一年
	void CCalendarUI::OnLastYear(TNotifyUI* pMsg)
	{
		if(GetEnabledYearSel())
			ShowCalendar(--m_LocalTime.wYear,m_LocalTime.wMonth);
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
		if(GetEnabledMoothSel())
			ShowCalendar(m_LocalTime.wYear,pMsg->pSender->GetTag());
	}

	//点击月份选择按钮，弹出月份选择列表
	void CCalendarUI::OnMoothSelect(TNotifyUI* pMsg)
	{
		if(GetEnabledMoothSel() == false)
			return;

		m_pMonthPanelHorz->SetVisible(true);
		RECT rc		= m_pMoothSelect->GetPos();
		rc.top			= rc.bottom;
		rc.bottom	= rc.top + 130;
		m_pMonthPanelHorz->SetPos(rc);
		m_pMonthPanelHorz->SetFloat(true);
	}

	//点击今天按钮
	void CCalendarUI::OnToday(TNotifyUI* pMsg)
	{
		GetLocalTime(&m_LocalTime);
		SetDateTime(m_LocalTime.wYear,m_LocalTime.wMonth,m_LocalTime.wDay);
		ShowCalendar(m_LocalTime.wYear,m_LocalTime.wMonth);
	}

	void CCalendarUI::OnSelcetDay(TNotifyUI* pMsg)
	{
		TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pMsg->pSender->GetParent()->GetTag();

		SetDateTime(pTCalendarInfo);

		if(GetEnabledMoothSel() && pTCalendarInfo && pTCalendarInfo->nMooth != pTCalendarInfo->nAsMooth)
			ShowCalendar(pTCalendarInfo->nYear,pTCalendarInfo->nAsMooth);
		else
			ChangeDateTime(pTCalendarInfo->nYear,pTCalendarInfo->nMooth,pTCalendarInfo->nDay);
	}

	void CCalendarUI::SetWeekPanel(CControlUI* pWeek,int nIndex)
	{
		static LPCTSTR lpszWeekName[] = {_T("星期天"),_T("星期一"),_T("星期二"),_T("星期三"),_T("星期四"),_T("星期五"),_T("星期六")};
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
			pDay->SetMouseEnabled(GetEnabledMoothSel());
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

	void CCalendarUI::SetEnabledMoothSel(bool bEnabled /*= true*/)
	{
		pEnabledMoothSel = bEnabled;
	}

	bool CCalendarUI::GetEnabledMoothSel()
	{
		return pEnabledMoothSel;
	}

	void CCalendarUI::UpdateMainTitle( int _Year,int _Mooth)
	{
		TCHAR szDateTime[20];

		_stprintf(szDateTime,_T("%4d-%02d"),_Year,_Mooth);
		m_pMoothSelect->SetText(szDateTime);
		
		_stprintf(szDateTime,_T("%4d"),_Year-1);
		m_pLastYear->SetText(szDateTime);

		_stprintf(szDateTime,_T("%4d"),_Year+1);
		m_pNextYear->SetText(szDateTime);
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
		pDateTimeBtn						= new CButtonUI();
		m_pBtnToDay						= new CButtonUI();

		pDateTimeBtn->SetMouseEnabled(false);
		pDateTimeBtn->SetFixedWidth(100);
		
		pControl->Add(pDateTimeBtn);
		pControl->Add(new CControlUI());
		pControl->Add(m_pBtnToDay);
		
		m_pBtnToDay->OnNotify			   += MakeDelegate(this,&CCalendarUI::Notify);
		m_pBtnToDay->SetText(TODAY_TEXT);
		m_pBtnToDay->SetName(TODAY_NAME);
		m_pBtnToDay->SetFixedWidth(60);
		
		return pControl;
	}

	void CCalendarUI::ChangeDateTime( int nYear,int nMooth,int nDay )
	{
		TCHAR szTimeDate[20];
		_stprintf(szTimeDate,_T("%4d-%02d-%02d"),nYear,nMooth,nDay);

		if(pDateTimeBtn)
			pDateTimeBtn->SetText(szTimeDate);
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
		if(pTCalendarInfo)
			SetDateTime(pTCalendarInfo->nYear,pTCalendarInfo->nMooth,pTCalendarInfo->nDay);
	}

	void CCalendarUI::SetDateTime( int nYear,int nMooth,int nDay )
	{ 
		mYear		=	nYear;
		mMonth  =	nMooth;
		mToday	=	nDay; 
	}

	CDuiString CCalendarUI::GetDateTime()
	{
		TCHAR szTimeDate[20];
		_stprintf(szTimeDate,_T("%4d-%02d-%02d"),mYear,mMonth,mToday);

		return szTimeDate;
	}
}
