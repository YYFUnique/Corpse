#pragma once
#include "UITileCtrl.h"
namespace DuiLib
{
	#define		DUI_CTR_LISTTOOLITEM				_T("ListToolItem")
	#define		DUI_MSGTYPE_ADD_CONTROL		_T("AddControl")
	class CListToolItemUI : public CListContainerElementUI
	{
	public:
		CListToolItemUI();

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

	class CListToolUI : public CTileCtrlUI
	{
	public:
		CListToolUI(CPaintManagerUI& PaintManager,SIZE& ItemSize);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	public:
		virtual void DoEvent(TEventUI& event);
		virtual bool AddNode(LPARAM lParam,LPCTSTR lpszXmlFile) = 0;
		void SelectItemByKey(TCHAR szKey);
	};
}