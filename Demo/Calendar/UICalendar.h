#ifndef  __UI__CALENDAR__H
#define __UI__CALENDAR__H

#pragma once
#include "UIListTool.h"
#include <atltime.h>
namespace DuiLib
{
	#define		DUI_CTR_CALENDARLAYOUT			_T("Calendar")
	typedef struct _tagCalendarTime{
		UINT nYear;
		UINT nMonth;
		UINT nDay;
		bool bWeekend;
		bool bInMonth;
	}CALENDAR_TIME,*PCALENDAR_TIME;

	class CCalendarUI : public CListToolUI
	{
	public:
		CCalendarUI(CPaintManagerUI& PaintManager,SIZE& ItemSize);
		virtual ~CCalendarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		bool AddNode(LPARAM lParam,LPCTSTR lpszXmlFile);

		void SetCurrentYear(UINT nYear);
		UINT GetCurrentYear();
		void SetCurrentMonth(UINT nMonth);
		UINT GetCurrentMonth();
		void SetCurrentDay(UINT nDay);
		UINT GetCurrentDay();

		bool CheckYearIsLeap(UINT nYear=0);
		bool IsLeapYear();
		UINT GetFirstDayWeek();
		UINT GetLastMonthDayCount();
		UINT GetDayCount(UINT nMonth = 0);
	protected:
		UINT m_nYear;
		UINT m_nMonth;
		UINT m_nDay;
		bool m_bIsLeap;
	};
}

#endif //__UI__CALENDAR__H