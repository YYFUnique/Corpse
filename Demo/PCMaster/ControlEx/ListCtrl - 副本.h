#pragma once
#include "TileListCtrl.h"
namespace DuiLib
{
	typedef struct _tagListItem
	{
		CDuiString strListItemIcon;
		CDuiString strListItemPath;
		CDuiString strListItemName;
	}ListItemInfo,*PListItemInfo;

	class CListCtrlItemUI : public CListContainerElementUI
	{
	public:
		CListCtrlItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
	public:
		virtual void DoEvent(TEventUI& event);
		void SetOwner(CControlUI* pOwner);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		const CDuiString& GetItemPath();
		void SetItemPath(LPCTSTR lpszItemPath);

		const CDuiString& GetSelectedImage();
		void SetSelectedImage(LPCTSTR lpszSelectedImage);
		DWORD GetSelectedColor();
		void SetSelectedColor(DWORD dwSelectedColor);

		const CDuiString& GetHotImage();
		void SetHotImage(LPCTSTR lpszItemPath);

		const CDuiString& GetPushImage();
		void SetPushImage(LPCTSTR lpszItemPath);

		void DoPaint(HDC hDC, const RECT& rcPaint);
		void PaintStatusImage(HDC hDC);
	private:
		DWORD m_dwSelectedColor;
		CDuiString m_strItemPath;
		CDuiString m_strSelectedImage;
		CDuiString m_strHotImage;
		CDuiString m_strPushImage;
	};

	class 

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
		bool AddNode(LPARAM lParam);
		void SelectItemByKey(TCHAR szKey);
	
		void SetSelectedImage(LPCTSTR lpszSelectedImage);
		void SetHotImage(LPCTSTR lpszHotImage);
		/*void SetPushedImage(LPCTSTR lpszPushedImage);*/
	protected:
		TListInfoUI m_ListInfo;
	};
}