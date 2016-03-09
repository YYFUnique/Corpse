#pragma once
#include "UITileListCtrl.h"
namespace DuiLib
{
	#define DUI_CTR_LISTCTRLITEM		 _T("ListCtrlItem")

	class IListCtrlItemUI : public IListItemUI
	{
	public:
		IListCtrlItemUI();
		virtual void SetOwner(CControlUI* pOwner);
		virtual IListOwnerUI* GetOwner();
		virtual int GetIndex() const;
		virtual void SetIndex(int iIndex);
		virtual bool IsSelected() const;
		virtual bool Select(bool bSelect = true) = 0;
		virtual bool IsExpanded() const;
		virtual bool Expand(bool bExpand = true);
		virtual void DrawItemText(HDC hDC, const RECT& rcItem);
	protected:
		int m_iIndex;
		bool m_bSelected;
		IListOwnerUI* m_pOwner;
	};

	class CListCtrlUI : public CTileListCtrlUI
	{
	public:
		CListCtrlUI(CPaintManagerUI& PaintManager);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		virtual void DoEvent(TEventUI& event);
		TListInfoUI* GetListInfo();
		void SelectItemByKey(TCHAR szKey);
	
		void SetItemFont(int iFont);
		void SetHotImage(LPCTSTR lpszHotImage);
		void SetPushedImage(LPCTSTR lpszPushedImage);
		void SetSelectedImage(LPCTSTR lpszSelectedImage);
	protected:
		TListInfoUI m_ListInfo;
	};
}