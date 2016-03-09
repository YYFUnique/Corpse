#pragma once
#include "UITileListCtrl.h"
namespace DuiLib
{
	typedef struct _tagSkinFaceInfo
	{
		CDuiString strAuthor;
		CDuiString strFaceDes;
	}SkinFaceInfo,*PSkinFaceInfo;

#define   DUI_CTR_SKINFACE			_T("SkinFace")

	class CSkinFaceUI : public CListContainerElementUI
	{
	public:
		CSkinFaceUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;		
	public:
		virtual void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetOwner(CControlUI* pOwner);
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void PaintStatusImage(HDC hDC);
	public:
		const CDuiString& GetBkImage();
		void SetBkImage(LPCTSTR lpszBkImage);
		const CDuiString& GetSelectedImage();
		void SetSelectedImage(LPCTSTR lpszSelectedImage);
	protected:
		CDuiString m_strBkImage;
		CDuiString m_strSelectedImage;
		CDuiString m_strHotImage;
		CDuiString m_strPushImage;
	};

	class CSkinDescribeUI : public CTileListCtrlUI
	{
	public:
		CSkinDescribeUI(CPaintManagerUI& PaintManager);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

	public:
		virtual void DoEvent(TEventUI& event);
		bool AddNode(LPARAM lParam);
	};
}