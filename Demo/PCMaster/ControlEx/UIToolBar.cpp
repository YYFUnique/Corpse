#include "../StdAfx.h"
#include "UIToolBar.h"
#include <Shlwapi.h>

CToolBarUI::CToolBarUI()
	:m_ObjType(OBJ_TYPE_FILE)
	,m_hFileIcon(NULL)
	,m_hMemDC(NULL)
{
	ZeroMemory(&m_ShFileInfo,sizeof(SHFILEINFO));
	ZeroMemory(&m_szLeftTop,sizeof(SIZE));
	ZeroMemory(&m_BMInf,sizeof(BITMAP));

	m_uTextStyle = DT_WORDBREAK|DT_EDITCONTROL|DT_CENTER;
}

LPCTSTR CToolBarUI::GetClass() const
{
	return _T("CToolBarUI");
}

LPVOID CToolBarUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName,_T("ListItem")) == 0) return static_cast<IListItemUI*>(this);
	else if (_tcsicmp(pstrName,_T("ToolBar")) == 0) return static_cast<CToolBarUI*>(this);
	else
		return static_cast<CButtonUI*>(this);
}

void CToolBarUI::DoEvent(DuiLib::TEventUI &event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CButtonUI::DoEvent(event);
		return;
	}
	
	if( event.Type == UIEVENT_BUTTONDOWN){
		if( IsEnabled() ){
			Select();
			m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			Invalidate();
		}
		return;
	}else if (event.Type == UIEVENT_RBUTTONUP){
		if (IsEnabled()){
			Select();
			m_pManager->SendNotify(this,DUI_MSGTYPE_MENU);
			Invalidate();
		}
		return;
	}else if (event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RETURN){
		if (IsEnabled()){
			Select();
			m_pManager->SendNotify(this,DUI_MSGTYPE_ITEMCLICK);
			Invalidate();
		}
		return;
	}else if( event.Type == UIEVENT_BUTTONUP ){
		if (IsEnabled()){
			m_uButtonState &= ~UISTATE_PUSHED;
			Invalidate();
		}
		return;
	}else if (event.Type == UIEVENT_MOUSEENTER){
		if (IsEnabled()){
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}
		return;
	}else if (event.Type == UIEVENT_MOUSELEAVE){
		if (IsEnabled()){
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}else if (event.Type == UIEVENT_DBLCLICK){
		if (IsEnabled()){
			Select();
			m_pManager->SendNotify(this,DUI_MSGTYPE_ITEMDBCLICK);
			Invalidate();
		}
		return;
	}

	if (m_pOwner)	
		m_pOwner->DoEvent(event); 
	else 
		CButtonUI::DoEvent(event);
}

void CToolBarUI::PaintStatusImage(HDC hDC)
{
	if (m_pOwner == NULL)
		return;

	TListInfoUI* pListInfo = m_pOwner->GetListInfo();

	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if ((m_uButtonState & UISTATE_DISABLED) !=0 ){
		if (!pListInfo->sDisabledImage.IsEmpty()){
			if (!DrawImage(hDC,pListInfo->sDisabledImage)) pListInfo->sDisabledImage.Empty();
			return;
		}
	}

	if ( IsSelected() !=0 ){
		if (!pListInfo->sSelectedImage.IsEmpty()){
			if (!DrawImage(hDC,(LPCTSTR)pListInfo->sSelectedImage))  pListInfo->sSelectedImage.Empty();
		}
		else if(pListInfo->dwSelectedBkColor != 0) 
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(pListInfo->dwSelectedBkColor));
	}

	if ((m_uButtonState & UISTATE_PUSHED) != 0){
		if (!pListInfo->sSelectedImage.IsEmpty()){
			if (!DrawImage(hDC,(LPCTSTR)pListInfo->sSelectedImage)) 	pListInfo->sSelectedImage.Empty();
		}
	}

	if ((m_uButtonState & UISTATE_HOT) !=0){
		if (!pListInfo->sHotImage.IsEmpty())
			if(!DrawImage(hDC, (LPCTSTR)pListInfo->sHotImage))	pListInfo->sHotImage.Empty();
	}

	PaintForeImage(hDC);
}

bool CToolBarUI::Select(bool bSelect /* = true */)
{
	if( !IsEnabled() ) return false;
	if( bSelect == m_bSelected ) return true;
	m_bSelected = bSelect;
	//由于删除控件后，控件对象的索引和容器元素索引对应不起来
	//需要更新后才能继续操作
	CContainerUI* pContainer = static_cast<CContainerUI*>(m_pParent->GetInterface(DUI_CTR_CONTAINER));
	if (pContainer == NULL)		return false;

	m_iIndex = pContainer->GetItemIndex(this);
	if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex);
	Invalidate();
	
	return true;
}

void CToolBarUI::SetObjType(OBJ_TYPE ObjType)
{
	if (m_ObjType == ObjType)
		return;

	ReleaseResource();
	m_ObjType = ObjType;
	Invalidate();
}

CDuiString CToolBarUI::GetFilePath()
{
	return m_sFilePath;
}

void CToolBarUI::SetFilePath(LPCTSTR lpszFilePath)
{
	if (m_sFilePath == lpszFilePath && m_ObjType == OBJ_TYPE_FILE)
		return;

	m_sFilePath = lpszFilePath;
	Invalidate();
}

void CToolBarUI::PaintForeImage(HDC hDC)
{
	if (m_ObjType == OBJ_TYPE_HBITMAP)
		DrawHBitmap(hDC);
	else if (m_ObjType == OBJ_TYPE_ICON)
		DrawIcon(hDC);
	else if (m_ObjType == OBJ_TYPE_FILE)
		DrawFileIcon(hDC);
}

void CToolBarUI::DrawHBitmap(HDC hDC)
{

}

void CToolBarUI::DrawIcon(HDC hDC)
{
	if (m_hFileIcon == NULL)
	{
		m_hFileIcon = (HICON)LoadImage(NULL,m_sFilePath,IMAGE_ICON,0,0,LR_LOADFROMFILE);
		if (m_hFileIcon == NULL)
			return;
	}
	GetIconPosition(m_ShFileInfo.hIcon);
	DrawIconEx(hDC,m_szLeftTop.cx,m_szLeftTop.cy,m_hFileIcon,0,0,0,NULL,DI_NORMAL|DI_COMPAT);
}

void CToolBarUI::DrawFileIcon(HDC hDC)
{
	if (m_ShFileInfo.hIcon == NULL)
	{
		if (PathFileExists(m_sFilePath) == FALSE)
			return;

		DWORD dwRet = SHGetFileInfo(m_sFilePath,0,&m_ShFileInfo,sizeof(SHFILEINFO),SHGFI_ICON|SHGFI_LARGEICON);
		if (dwRet == 0)
			return;
	}
	GetIconPosition(m_ShFileInfo.hIcon);
	DrawIconEx(hDC,m_szLeftTop.cx,m_szLeftTop.cy,m_ShFileInfo.hIcon,0,0,0,NULL,DI_NORMAL|DI_COMPAT);
}

void CToolBarUI::PaintText(HDC hDC)
{
	RECT rcTextRect = {0};
	rcTextRect.left = m_rcItem.left+m_rcTextPadding.left;
	rcTextRect.top = m_szLeftTop.cy + m_BMInf.bmHeight + m_rcTextPadding.top;
	rcTextRect.right = m_rcItem.right - m_rcTextPadding.right;
	rcTextRect.bottom = m_rcItem.bottom - m_rcTextPadding.bottom;

	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

	DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

	if (m_pOwner && m_iFont == -1) m_iFont = m_pOwner->GetListInfo()->nFont;
	CRenderEngine::DrawText(hDC, m_pManager, rcTextRect, m_sText, clrColor,m_iFont, m_uTextStyle|DT_END_ELLIPSIS);
}

void CToolBarUI::ReleaseResource()
{
	if (m_hFileIcon != NULL)
		DestroyIcon(m_hFileIcon);
	if (m_ShFileInfo.hIcon != NULL)
		DestroyIcon(m_ShFileInfo.hIcon);
}

void CToolBarUI::GetHBitmapPosition(HBITMAP hBitmap)
{
	GetObject(hBitmap, sizeof(BITMAP), &m_BMInf);
	GetPicPosition(m_BMInf.bmWidth,m_BMInf.bmHeight);
}

void CToolBarUI::GetIconPosition(HICON hIcon)
{
	ICONINFO IconInfo;
	GetIconInfo(hIcon,&IconInfo);
	if (IconInfo.hbmMask != NULL)
		GetHBitmapPosition(IconInfo.hbmMask);
}

void CToolBarUI::GetPicPosition(int nPicWidth,int nPicHeight)
{
	m_szLeftTop.cx = m_rcItem.left;
	m_szLeftTop.cy = m_rcItem.top;

	if (m_uTextStyle & DT_RIGHT)
		m_szLeftTop.cx += m_rcItem.right - m_rcItem.left - nPicWidth;
	else if (m_uTextStyle & DT_CENTER)
		m_szLeftTop.cx += (m_rcItem.right - m_rcItem.left - nPicWidth) / 2;
	
	if (m_uTextStyle & DT_BOTTOM)
		m_szLeftTop.cy += m_rcItem.bottom - m_rcItem.bottom - nPicHeight - m_rcPadding.bottom;
	else if (m_uTextStyle & DT_VCENTER)
		m_szLeftTop.cy += (m_rcItem.bottom - m_rcItem.top - nPicHeight) /2;
	else
		m_szLeftTop.cy += m_rcPadding.top;
}

void CToolBarUI::SetPadding(RECT rcPad)
{
	SetRect(&m_rcPadding,rcPad.left,rcPad.top,rcPad.right,rcPad.left);
}