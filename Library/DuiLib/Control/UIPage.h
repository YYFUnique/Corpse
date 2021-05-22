#pragma once

#include "UIList.h"

namespace DuiLib
{
	class CListBodyUI;
	class CButtonUI;
	class UILIB_API CPageUI : public CHorizontalLayoutUI
	{
		public:
			CPageUI();

			LPCTSTR GetClass() const;
			UINT GetControlFlags() const;
			LPVOID GetInterface(LPCTSTR pstrName);
			bool Notify(LPVOID lParam);

			void DoEvent(TEventUI& event);
			void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		public:
			void SetCount(int nCount);
			void SetTotal(int nTotal);
			void SetPage(CListBodyUI* pList);
			void SetNextPage();
			void SetPrePage();
			void SetFirstPage();
			void SetLastPage();
			void SetPageTo(int nPage);

		protected:
			//每页显示条数
			UINT m_nCount;
			//当前显示页数
			UINT m_nPage;
			//总共条数
			UINT m_nTotal;
			//总共有多少页
			UINT m_nTotalPage;
			CListBodyUI* m_pList;

			CButtonUI* m_pFirstPage;
			CButtonUI* m_pLastPage;
			CButtonUI* m_pPrePage;
			CButtonUI* m_pNextPage;
			CButtonUI* m_pGoto;
	};
}