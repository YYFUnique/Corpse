#include "../stdafx.h"
#include "SkinFace.h"
#include "../PCMaster.h"
namespace DuiLib
{
	CSkinFaceUI::CSkinFaceUI()
	{

	}

	LPCTSTR CSkinFaceUI::GetClass() const
	{
		return _T("CSkinFaceUI");
	}

	LPVOID CSkinFaceUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName,DUI_CTR_SKINFACE) == 0 ) return static_cast<CSkinFaceUI*>(this);
		else
			return CListContainerElementUI::GetInterface(pstrName);
	}

	UINT CSkinFaceUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	void CSkinFaceUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("bkimage")) == 0) SetBkImage(pstrValue);
		else if(_tcsicmp(pstrName,_T("selectedimage")) == 0) SetSelectedImage(pstrValue);
		else
			CListContainerElementUI::SetAttribute(pstrName,pstrValue);
	}

	const CDuiString& CSkinFaceUI::GetBkImage()
	{
		return m_strBkImage;
	}

	void CSkinFaceUI::SetBkImage(LPCTSTR lpszBkImage)
	{
		m_strBkImage = lpszBkImage;
	}

	const CDuiString& CSkinFaceUI::GetSelectedImage()
	{
		return m_strSelectedImage;
	}

	void CSkinFaceUI::SetSelectedImage(LPCTSTR lpszSelectedImage)
	{
		m_strSelectedImage = lpszSelectedImage;
	}

	void CSkinFaceUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		PaintStatusImage(hDC);
		CContainerUI::DoPaint(hDC, rcPaint);
	}

	void CSkinFaceUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if ( IsSelected() !=0 ){
			if (!m_strSelectedImage.IsEmpty()){
				if (!DrawImage(hDC,(LPCTSTR)m_strSelectedImage)){
					m_strSelectedImage.Empty();
				}
			}
		}

		if ((m_uButtonState & UISTATE_PUSHED) != 0){
			if (!m_strPushImage.IsEmpty()){
				if (!DrawImage(hDC,(LPCTSTR)m_strPushImage)){
					m_strPushImage.Empty();
				}
			}
			return;
		}

		if ((m_uButtonState & UISTATE_HOT) !=0){
			if (!m_strHotImage.IsEmpty()){
				if( !DrawImage(hDC, (LPCTSTR)m_strHotImage)){
					m_strHotImage.Empty();
				}
			}
			return;
		}
	}

	void CSkinFaceUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CListContainerElementUI::DoEvent(event);
			return;
		}

		CListContainerElementUI::DoEvent(event);
	}

	void CSkinFaceUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}


	CSkinDescribeUI::CSkinDescribeUI(CPaintManagerUI& PaintManager)
		:CTileListCtrlUI(PaintManager)
	{
		SetItemSize(CDuiSize(80,80));
	}

	LPCTSTR CSkinDescribeUI::GetClass() const
	{
		return _T("SkinDescribe");
	}

	LPVOID CSkinDescribeUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName,_T("IListOwner")) == 0) return static_cast<IListOwnerUI*>(this);
		if (_tcsicmp(pstrName,_T("SkinDescribeUI")) == 0) return static_cast<CSkinDescribeUI*>(this);
		else
			return CTileLayoutUI::GetInterface(pstrName);
	}

	bool CSkinDescribeUI::IsMultiSelect()
	{
		return false;
	}

	void CSkinDescribeUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CTileLayoutUI::DoEvent(event);
			return;
		}

		CTileLayoutUI::DoEvent(event);
	}

	bool CSkinDescribeUI::AddNode(LPARAM lParam)
	{
		const SkinFaceInfo* pSkinFaceInfo = (PSkinFaceInfo)lParam;
		CSkinFaceUI* pSkinFace = NULL;
		ASSERT(FALSE && _T("不能添加"));
		// 暂时屏蔽以下代码
		/*CPCMaster* pMainDlg = (CPCMaster*)GetMainApp();
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pSkinFace = (CSkinFaceUI*)m_DialogBuilder.Create(_T("SkinDescribe.xml"),0,pMainDlg,&m_PaintManager);
		else
			pSkinFace = (CSkinFaceUI*)m_DialogBuilder.Create(pMainDlg,&m_PaintManager);

		ASSERT(pSkinFace);*/

		CLabelUI* pLabelAuthor = (CLabelUI*)m_PaintManager.FindSubControlByName(pSkinFace,_T("LabelAuthor"));
		pLabelAuthor->SetText(pSkinFaceInfo->strAuthor);
		CTextUI* pFaceDes = (CTextUI*)m_PaintManager.FindSubControlByName(pSkinFace,_T("TextDes"));
		pFaceDes->SetText(pSkinFaceInfo->strFaceDes);

		return Add(pSkinFace);
	}
}