#include "StdAfx.h"
#include "UIAnimButton.h"
#include "DllCore/Utils/TextTools.h"
#include <atlstr.h>

namespace DuiLib
{
	CString GetSkinFile(LPCTSTR strFileItem)
	{
		LPCTSTR lpszFileItem[][2] = {
			{_T("hover1"),_T("../image/phone/hover1.png")},
			{_T("hover2"),_T("../image/phone/hover2.png")},
			{_T("hover3"),_T("../image/phone/hover3.png")},
			{_T("hover4"),_T("../image/phone/hover4.png")},
			{_T("hover5"),_T("../image/phone/hover5.png")},
			{_T("close_hover1"),_T("../image/close/hover1.png")},
			{_T("close_hover2"),_T("../image/close/hover2.png")},
			{_T("close_hover3"),_T("../image/close/hover3.png")},
			{_T("close_hover4"),_T("../image/close/hover4.png")},
			{_T("close_hover5"),_T("../image/close/hover5.png")},
			{_T("accprot_hover1"),_T("../image/AccProt/hover1.png")},
			{_T("accprot_hover2"),_T("../image/AccProt/hover2.png")},
			{_T("accprot_hover3"),_T("../image/AccProt/hover3.png")},
			{_T("accprot_hover4"),_T("../image/AccProt/hover4.png")},
			{_T("skin_hover1"),_T("../image/skin/hover1.png")},
			{_T("skin_hover2"),_T("../image/skin/hover2.png")},
			{_T("skin_hover3"),_T("../image/skin/hover3.png")},
			{_T("skin_hover4"),_T("../image/skin/hover4.png")},
			{_T("imsmenu_hover1"),_T("../image/ImsMenu/hover1.png")},
			{_T("imsmenu_hover2"),_T("../image/ImsMenu/hover2.png")},
			{_T("imsmenu_hover3"),_T("../image/ImsMenu/hover3.png")},
			{_T("imsmenu_hover4"),_T("../image/ImsMenu/hover4.png")},
			{_T("mini_hover1"),_T("../image/mini/hover1.png")},
			{_T("mini_hover2"),_T("../image/mini/hover2.png")},
			{_T("mini_hover3"),_T("../image/mini/hover3.png")},
			{_T("mini_hover4"),_T("../image/mini/hover4.png")},

			{_T("trojanscan_circle_1"),_T("../image/TrojanScan/Circle/1.png")},
			{_T("trojanscan_circle_2"),_T("../image/TrojanScan/Circle/2.png")},
			{_T("trojanscan_circle_3"),_T("../image/TrojanScan/Circle/3.png")},
			{_T("trojanscan_circle_4"),_T("../image/TrojanScan/Circle/4.png")},
			{_T("trojanscan_circle_5"),_T("../image/TrojanScan/Circle/5.png")},
			{_T("trojanscan_circle_6"),_T("../image/TrojanScan/Circle/6.png")},
			{_T("trojanscan_circle_7"),_T("../image/TrojanScan/Circle/7.png")},
			{_T("trojanscan_circle_8"),_T("../image/TrojanScan/Circle/8.png")},
			{_T("trojanscan_circle_9"),_T("../image/TrojanScan/Circle/9.png")},
			{_T("trojanscan_circle_10"),_T("../image/TrojanScan/Circle/10.png")},
			{_T("trojanscan_circle_11"),_T("../image/TrojanScan/Circle/11.png")},
			{_T("trojanscan_circle_12"),_T("../image/TrojanScan/Circle/12.png")},
			{_T("trojanscan_circle_13"),_T("../image/TrojanScan/Circle/13.png")},
			{_T("trojanscan_circle_14"),_T("../image/TrojanScan/Circle/14.png")},
			{_T("trojanscan_circle_15"),_T("../image/TrojanScan/Circle/15.png")},
			{_T("trojanscan_circle_16"),_T("../image/TrojanScan/Circle/16.png")},
			{_T("trojanscan_circle_17"),_T("../image/TrojanScan/Circle/17.png")},
			{_T("trojanscan_circle_18"),_T("../image/TrojanScan/Circle/18.png")},
			{_T("trojanscan_circle_19"),_T("../image/TrojanScan/Circle/19.png")},
			{_T("trojanscan_circle_20"),_T("../image/TrojanScan/Circle/20.png")},
			{_T("trojanscan_circle_21"),_T("../image/TrojanScan/Circle/21.png")},
			{_T("trojanscan_circle_22"),_T("../image/TrojanScan/Circle/22.png")},
			{_T("trojanscan_circle_23"),_T("../image/TrojanScan/Circle/23.png")},
			{_T("trojanscan_circle_24"),_T("../image/TrojanScan/Circle/24.png")},
			{_T("trojanscan_circle_25"),_T("../image/TrojanScan/Circle/25.png")},
			{_T("trojanscan_circle_26"),_T("../image/TrojanScan/Circle/26.png")},
			{_T("trojanscan_circle_27"),_T("../image/TrojanScan/Circle/27.png")},
			{_T("trojanscan_circle_28"),_T("../image/TrojanScan/Circle/28.png")},
			{_T("trojanscan_circle_29"),_T("../image/TrojanScan/Circle/29.png")},
			{_T("trojanscan_circle_30"),_T("../image/TrojanScan/Circle/30.png")},
		};

		CString strFilePath;
		for (int n=0;n<_countof(lpszFileItem);++n)
		{
			if (_tcsicmp(lpszFileItem[n][0],strFileItem) == 0)
				strFilePath = lpszFileItem[n][1];
		}

		return strFilePath;
	}

	CAnimButtonUI::CAnimButtonUI()
	{
		m_CursorType = IDC_HAND;
		m_uElapse = 30;
		m_nAnimateType = ANIMATE_TYPE_NONE;
		m_uButtonState = 0;
		m_nHoveState = 0;
	}

	LPCTSTR CAnimButtonUI::GetClass() const
	{
		return _T("AnimButtonUI");
	}

	LPVOID CAnimButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("AnimButton")) == 0 ) return static_cast<CAnimButtonUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	UINT CAnimButtonUI::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void CAnimButtonUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_MOUSEENTER){
			if (IsEnabled()) {
				m_uButtonState = UISTATE_HOT;
				m_nAnimateType = ANIMATE_TYPE_ENTER;
				m_pManager->SetTimer(this, FRAME_ANIMATE_TIME_ID, m_uElapse);
				Invalidate();
			}
		} else if (event.Type == UIEVENT_MOUSELEAVE){
			if (m_uButtonState & UISTATE_HOT)
			{
				m_uButtonState &= ~UISTATE_HOT;
				m_nAnimateType = ANIMATE_TYPE_LEAVE;
				m_pManager->SetTimer(this, FRAME_ANIMATE_TIME_ID, m_uElapse);
				Invalidate();
			}
		} else if (event.Type == UIEVENT_TIMER) {
			if ( event.wParam == FRAME_ANIMATE_TIME_ID ){

				m_nHoveState += m_nAnimateType;
				Invalidate();

				int nItem = m_SkinFile.GetCount();

				if ((m_nAnimateType == ANIMATE_TYPE_ENTER && m_nHoveState >= nItem - 1) || 
					(m_nAnimateType == ANIMATE_TYPE_LEAVE && m_nHoveState < 0))
				{
					if (m_nHoveState >= nItem - 1)
						m_nHoveState = nItem - 1;
					if (m_nHoveState < 0)
						m_nHoveState = 0;

					if (m_nHoveState == 0)
						m_nAnimateType = ANIMATE_TYPE_NONE;
					m_pManager->KillTimer(this, FRAME_ANIMATE_TIME_ID);
				}
			}
		} else
			CButtonUI::DoEvent(event);
	}

	void CAnimButtonUI::PaintStatusImage(HDC hDC)
	{
		if (m_nAnimateType == ANIMATE_TYPE_NONE)
		{
			CButtonUI::PaintStatusImage(hDC);
		}
		else
		{
			const TImageInfo* data = static_cast<TImageInfo*>(m_ImageSkin.Find(m_SkinFile.GetAt(m_nHoveState)));
			if (data == NULL)
				return;

			RECT rcBmpPart = {0};

			if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
				rcBmpPart.right = data->nX;
				rcBmpPart.bottom = data->nY;
			}
			if (rcBmpPart.right > data->nX) rcBmpPart.right = data->nX;
			if (rcBmpPart.bottom > data->nY) rcBmpPart.bottom = data->nY;

			RECT rcCorner = {0};
			CRenderEngine::DrawImage(hDC, data->hBitmap, m_rcItem, m_rcItem, rcBmpPart, rcCorner, data->bAlpha);
		}
	}

	void CAnimButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("elapse")) == 0)	SetElapse(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("imagelist")) == 0) SetImageList(pstrValue);
		else 
			CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void CAnimButtonUI::SetImageList(LPCTSTR pstrValue)
	{
		CStdArray strFileSkin;
		AnalyseFormattedTextInfoItem(pstrValue, strFileSkin, _T(';'));

		m_SkinFile.RemoveAll();

		for (int n=0; n<strFileSkin.GetSize(); ++n)
		{
			const CString& strSkinFile = strFileSkin.GetAt(n);

			int nPos = strSkinFile.Find(_T(':'));
			CString strFileResType = strSkinFile.Left(nPos);
			CString strFileItem = strSkinFile.Right(strSkinFile.GetLength() - nPos - 1);

			CString strFilePath = GetSkinFile(strFileItem);

			m_SkinFile.Add(strFilePath);

			TImageInfo* data = NULL;
			data = CRenderEngine::LoadImage((STRINGorID)strFilePath, NULL, 0);

			if( !data ) return;
			data->sResType = strFileResType;
			data->dwMask = 0;
			if( !m_ImageSkin.Insert(strFilePath, data) ) {
				::DeleteObject(data->hBitmap);
				delete data;
			}
		}
	}

	void CAnimButtonUI::SetElapse(UINT uElapse)
	{
		if (m_uElapse == uElapse)
			return;

		m_uElapse = uElapse;
	}

	void CAnimButtonUI::StopAnimate()
	{
		m_nHoveState = 0;
		m_pManager->KillTimer(this, FRAME_ANIMATE_TIME_ID);
		//m_nHoveAnim = 0;
	}
}