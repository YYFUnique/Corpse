#include "stdafx.h"
#include "UICalendar.h"

namespace DuiLib
{
	CCalendarUI::CCalendarUI(CPaintManagerUI& PaintManager,SIZE& ItemSize)
		:CListToolUI(PaintManager,ItemSize)
		,m_nYear(1970)
		,m_nMonth(1)
		,m_nDay(1)
	{

	}

	CCalendarUI::~CCalendarUI()
	{
		
	}

	LPCTSTR CCalendarUI::GetClass() const
	{
		return _T("CalendarUI");
	}

	LPVOID CCalendarUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_CALENDARLAYOUT) == 0 ) return static_cast<CCalendarUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CCalendarUI::GetControlFlags() const
	{
		return 0;
	}

	void CCalendarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName,_T("year")) == 0) SetCurrentYear(_ttoi(pstrValue));
		else if (_tcscmp(pstrName,_T("month")) == 0) SetCurrentMonth(_ttoi(pstrValue));
		else if (_tcscmp(pstrName,_T("day")) == 0) SetCurrentDay(_ttoi(pstrValue));
		else
			CListToolUI::SetAttribute(pstrName,pstrValue);
	}

	void CCalendarUI::SetCurrentYear(UINT nYear)
	{
		if (m_nYear == nYear)
			return;
		m_nYear = nYear;
		//检查是否是闰年
		CheckYearIsLeap(nYear);
	}

	UINT CCalendarUI::GetCurrentYear()
	{
		return m_nYear;
	}

	void CCalendarUI::SetCurrentMonth(UINT nMonth)
	{
		if (m_nMonth == nMonth)
			return;
		m_nMonth = nMonth;
	}

	UINT CCalendarUI::GetCurrentMonth()
	{
		return m_nMonth;
	}

	void CCalendarUI::SetCurrentDay(UINT nDay)
	{
		m_nDay = nDay;
	}

	UINT CCalendarUI::GetCurrentDay()
	{
		return m_nDay;
	}

	bool CCalendarUI::CheckYearIsLeap(UINT nYear /*=0 */)
	{
		if (nYear == 0)
			nYear = m_nYear;

		m_bIsLeap = true;
		return true;
	}

	bool CCalendarUI::IsLeapYear()
	{
		return m_bIsLeap;
	}

	UINT CCalendarUI::GetFirstDayWeek()
	{
		if (m_nYear == 0 || m_nMonth == 0)
			return 0;
		
		SYSTEMTIME SystemTime;
		SystemTime.wYear = m_nYear;
		SystemTime.wMonth = m_nMonth;
		SystemTime.wDay = 1;
		SystemTime.wHour = SystemTime.wMinute = SystemTime.wSecond = 0;
		CTime Time(SystemTime);
		return Time.GetDayOfWeek();
	}

	UINT CCalendarUI::GetDayCount(UINT nMonth/* = 0*/)
	{
		if (nMonth == 0)
			nMonth = m_nMonth;
		switch (nMonth)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				return 31;
			case 2:
			{
				if (m_bIsLeap)
					return 28;
				else
					return 29;
			}
			default:
				return 30;
		}
	}

	bool CCalendarUI::AddNode(LPARAM lParam,LPCTSTR lpszXmlFile)
	{
		CListToolItemUI* pItemList = NULL;
		WindowImplBase* pImplBase = (WindowImplBase*)GetMainApp();
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pItemList = (CListToolItemUI*)m_DialogBuilder.Create(lpszXmlFile,0,pImplBase,&m_PaintManager,NULL);
		else
			pItemList = (CListToolItemUI*)m_DialogBuilder.Create(pImplBase,&m_PaintManager);

		ASSERT(pItemList);

		PCALENDAR_TIME pCalendarTime = (PCALENDAR_TIME)lParam;
		CLabelUI* pSolarDay = (CLabelUI*)m_PaintManager.FindSubControlByName(pItemList,_T("Solar"));
		TCHAR szDay[10];
		_stprintf_s(szDay,_countof(szDay),_T("%d"),pCalendarTime->nDay);
		pSolarDay->SetText(szDay);
		if (pCalendarTime->bInMonth == false)
			pSolarDay->SetTextColor(0xFFCDCDCD);
		else if (pCalendarTime->bWeekend)
			pSolarDay->SetTextColor(0xFFE02D2D);
		CLabelUI* pLunarDay = (CLabelUI*)m_PaintManager.FindSubControlByName(pItemList,_T("Lunar"));
		//计算阴历
		pLunarDay->SetText(_T(""));

		return Add(pItemList);
	}
}