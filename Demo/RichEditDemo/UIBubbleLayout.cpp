#include "StdAfx.h"
#include "UIBubbleLayout.h"
#include "UIRichText.h"

CBubbleLayoutUI::CBubbleLayoutUI()
	: m_dwTextStart(0)
	, m_dwTextEnd(0)
	, m_dwLIndent(0)
	, m_dwRIndent(0)
	, m_bLeftAlign(TRUE)
{
	// 该控件不响应鼠标消息
	SetMouseEnabled(false);

	m_dwParaSpace = 0;
}

CBubbleLayoutUI::~CBubbleLayoutUI()
{

}

LPCTSTR CBubbleLayoutUI::GetClass() const
{
	return _T("BubbleLayoutUI");
}

LPVOID CBubbleLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, DUI_CTR_BUBBLELAYOUT) == 0) return static_cast<CBubbleLayoutUI*>(this);
	else
		return CHorizontalLayoutUI::GetInterface(pstrName);
}

void CBubbleLayoutUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	if( m_items.IsEmpty() ) return;

	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	for( int it = 0; it < m_items.GetSize(); it++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) {
			SetFloatPos(it);
		}
		else {
			pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
		}
	}
}

void CBubbleLayoutUI::SetParaSpace(DWORD dwParaSpace)
{
	m_dwParaSpace = dwParaSpace;
}

SIZE CBubbleLayoutUI::EstimateSize(SIZE szAvailable)
{
	CRichTextUI* pRich = (CRichTextUI*)m_pParent->GetInterface(DUI_CTR_RICHEDIT);
	if (pRich == NULL)
		return m_szAvailable;

	CHARRANGE chRange;
	pRich->GetSel(chRange);

	/*int iFirstLine = pRich->GetFirstVisibleLine();
	int iFirstCharIndex = pRich->GetLineIndex(iFirstLine);*/

	// 计算项目应有的宽和高
	int iHeight = 0;
	int iWidth = 0;

	// 计算文字高度
	POINT ptStart = pRich->GetPosFromChar(m_dwTextStart + m_dwNickNameLen);
	// 由于计算的是字符的右下角坐标，故需要多计算一个行
	POINT ptEnd = pRich->GetPosFromChar(m_dwTextEnd + 1);

	iHeight = ptEnd.y - ptStart.y;

	// 计算文字宽度
	LONG lTextStart = pRich->GetLineFromChar(m_dwTextStart + m_dwNickNameLen);
	LONG lTextEnd = pRich->GetLineFromChar(m_dwTextEnd);

	CString strTipInfo;
	pRich->GetTextRange(m_dwTextStart+m_dwNickNameLen, m_dwTextEnd, strTipInfo);

	CPictureUI* pHead = (CPictureUI*)FindSubControlByClassName(_T("PictureUI"));

	RECT rc;
	pRich->GetControlRect(&rc);

	int nBar = 0;
	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
		nBar = m_pVerticalScrollBar->GetFixedHeight();

	// 如果是多行文本 ,计算第一个字符x坐标
	if (lTextStart != lTextEnd) {
		pRich->SetSel(m_dwTextStart, m_dwTextEnd);
		int iLeft = pRich->GetCurTargetX();
		for (int iIndex=lTextStart; iIndex<=lTextEnd; ++iIndex)
		{
			int iTextIndex = pRich->GetLineIndex(iIndex);					// 获取指定行，首字符在文本中的位置
			int iLineLen = pRich->GetLineLength(iTextIndex);
			if (iLineLen == 0)
				continue;

			int iLast = iTextIndex + iLineLen;
			pRich->SetSel(iLast, iLast);

			int iRight = pRich->GetCurTargetX();
			if (iRight <= iLeft)
			{
				iWidth = rc.right - rc.left - nBar - pHead->GetFixedWidth();
				break;
			} else {
				iWidth = MAX(iRight, iWidth);
			}
		}
	} else {
		pRich->SetSel(m_dwTextEnd, m_dwTextEnd);
		iWidth = pRich->GetCurTargetX();
	}

	pRich->SetSel(chRange);

	CControlUI* pControl = FindSubControlByClassName(_T("ControlUI"));
	DWORD dwOffset = 10;
	int iNickHight = 0;
	if (pControl)
	{
		POINT ptStart = pRich->GetPosFromChar(m_dwTextStart);
		POINT ptEnd = pRich->GetPosFromChar(m_dwTextStart + m_dwNickNameLen);

		iNickHight = ptEnd.y - ptStart.y;
		DWORD dwParaSpace = m_dwParaSpace;//pRich->GetParaSpace();
		/*pRich->GetItemIndex()*/
		if (dwParaSpace == 0)
			dwParaSpace = dwOffset;
		else
			dwParaSpace = 2*dwOffset;

		pControl->SetFixedHeight(iNickHight - dwParaSpace);
		
		iHeight += 2*dwOffset;

		pRich->SetParaSpace(dwOffset);
	}

	int dwLabel = rc.right - rc.left - pHead->GetFixedWidth() - 10;
	if (dwLabel < iWidth)
		iWidth = dwLabel;

	CLabelUI* pLabel = (CLabelUI*)FindSubControlByClassName(_T("LabelUI"));
	if (pLabel)
	{
		pLabel->SetFixedHeight(iHeight);
		pLabel->SetFixedWidth(iWidth);
	}
	
	CVerticalLayoutUI* pBubbleInfo = (CVerticalLayoutUI*)FindSubControlByClassName(_T("VerticalLayoutUI"));

	if (pBubbleInfo)
	{
		pBubbleInfo->SetFixedHeight(iHeight + pControl->GetFixedHeight());
		pBubbleInfo->SetFixedWidth(iWidth);
		iHeight = pBubbleInfo->GetFixedHeight();
	}	

	//m_szAvailable.c = {/*pHead->GetFixedWidth()+*/iWidth+5, iHeight};
	m_szAvailable.cx = iWidth + 5;
	m_szAvailable.cy = iHeight;

	return m_szAvailable;
}

void CBubbleLayoutUI::RejustBubble()
{
	if (m_bLeftAlign)
		CalAsLeft();
	else
		CalAsRight();
}

void CBubbleLayoutUI::CalAsLeft()
{
	CRichTextUI* pRich = (CRichTextUI*)m_pParent->GetInterface(DUI_CTR_RICHEDIT);
	if (pRich == NULL)
		return;

	// 计算项目应有的宽和高
	int iHeight = 0;
	int iWidth = 0;

	// 计算文字高度
	POINT ptStart = pRich->GetPosFromChar(m_dwTextStart);
	LONG lLineEnd = pRich->GetLineFromChar(m_dwTextEnd);
	LONG lTextEnd = pRich->GetLineIndex(lLineEnd+1); // 使用下一行来计算文字高度
	POINT ptEnd = pRich->GetPosFromChar(lTextEnd);

	iHeight = ptEnd.y - ptStart.y;

	// 计算文字宽度
	LONG lTextStart = pRich->GetLineFromChar(m_dwTextStart);
	/*LONG*/ lTextEnd = pRich->GetLineFromChar(m_dwTextEnd);

	CPictureUI* pHead = (CPictureUI*)FindSubControlByClassName(_T("PictureUI"));

	RECT rc;
	pRich->GetControlRect(&rc);

	int nBar = 0;
	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
		nBar = m_pVerticalScrollBar->GetFixedHeight();

	// 如果是多行文本 ,计算第一个字符x坐标
	if (lTextStart != lTextEnd) {
		pRich->SetSel(m_dwTextStart, m_dwTextEnd);
		int iLeft = pRich->GetCurTargetX();
		for (int iIndex=lTextStart; iIndex<lTextEnd; ++iIndex)
		{
			int iTextIndex = pRich->GetLineIndex(iIndex);					// 获取所在行，首字符在文本中的位置
			int iLineLen = pRich->GetLineLength(iIndex);
			if (iLineLen == 0)
				continue;
			int iLast = iTextIndex + iLineLen;
			pRich->SetSel(iLast, iLast);
			int iRight = pRich->GetCurTargetX();
			if (iRight <= iLeft)
			{
				iWidth = rc.right - rc.left - nBar - pHead->GetFixedWidth() - 5;
				break;
			} else {
				iWidth = max(iRight - 5, iWidth);
			}
		}
	} else {
		//iWidth = MAX();
	}

	SIZE size = {iWidth, iHeight};
	SetBubbleSize(size);
}

void CBubbleLayoutUI::CalAsRight()
{

}

void CBubbleLayoutUI::AddBubbleMsg(LPCTSTR lpszNickName, LPCTSTR lpszBubbleMsg)
{
	CRichTextUI* pRich = (CRichTextUI*)m_pParent->GetInterface(_T("RichText"));
	if (pRich == NULL)
		return;

	if (lpszNickName != NULL)
	{

	}

	if (lpszBubbleMsg != NULL)
	{
		/*pRich->getcha*/
		pRich->AppendText(lpszBubbleMsg);
	}	
}

void CBubbleLayoutUI::SetNickNameLen(DWORD dwNickLen)
{
	m_dwNickNameLen = dwNickLen;
}

void CBubbleLayoutUI::SetRichTextRange(DWORD dwStart, DWORD dwEnd)
{
	if (m_dwTextStart != dwStart)
		m_dwTextStart = dwStart;

	if (m_dwTextEnd != dwEnd)
		m_dwTextEnd = dwEnd;

	Invalidate();
}

void CBubbleLayoutUI::SetRichTextRange(const CHARRANGE& CharRange)
{
	if (m_dwTextStart != CharRange.cpMin)
		m_dwTextStart = CharRange.cpMin;

	if (m_dwTextEnd != CharRange.cpMax)
		m_dwTextEnd = CharRange.cpMax;

	Invalidate();
}

void CBubbleLayoutUI::SetBubbleImage(LPCTSTR lpszBubbleImage)
{
	if (m_strBubbleImage == lpszBubbleImage)
		return;

	m_strBubbleImage = lpszBubbleImage;
	Invalidate();
}

bool CBubbleLayoutUI::OnLabelSize(LPVOID lParam)
{
	CRichTextUI* pRich = (CRichTextUI*)m_pParent->GetInterface(DUI_CTR_RICHEDIT);
	if (pRich == NULL)
		return false;

	CLabelUI* pLabel = (CLabelUI*)lParam;

	pLabel->SetFixedHeight(50);
	// 计算项目应有的宽和高
	//int iHeight = 0;
	//int iWidth = 0;

	//// 计算文字高度
	//POINT ptStart = pRich->GetPosFromChar(m_dwStart);
	//LONG lLineEnd = pRich->GetLineFromChar(m_dwEnd);
	//LONG lTextEnd = pRich->GetLineIndex(lLineEnd+1); // 使用下一行来计算文字高度
	//POINT ptEnd = pRich->GetPosFromChar(lTextEnd);

	//iHeight = ptEnd.y - ptStart.y;

	//// 计算文字宽度
	//LONG lTextStart = pRich->GetLineFromChar(m_dwStart);
	///*LONG*/ lTextEnd = pRich->GetLineFromChar(m_dwEnd);

	//CPictureUI* pHead = (CPictureUI*)FindSubControlByClassName(_T("PictureUI"));

	//RECT rc;
	//pRich->GetControlRect(&rc);

	//int nBar = 0;
	//if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
	//	nBar = m_pVerticalScrollBar->GetFixedHeight();

	//// 如果是多行文本 ,计算第一个字符x坐标
	//if (lTextStart != lTextEnd) {
	//	pRich->SetSel(m_dwStart, m_dwEnd);
	//	int iLeft = pRich->GetCurTargetX();
	//	for (int iIndex=lTextStart; iIndex<lTextEnd; ++iIndex)
	//	{
	//		int iTextIndex = pRich->GetLineIndex(iIndex);					// 获取所在行，首字符在文本中的位置
	//		int iLineLen = pRich->GetLineLength(iIndex);
	//		if (iLineLen == 0)
	//			continue;
	//		int iLast = iTextIndex + iLineLen;
	//		pRich->SetSel(iLast, iLast);
	//		int iRight = pRich->GetCurTargetX();
	//		if (iRight <= iLeft)
	//		{
	//			iWidth = rc.right - rc.left - nBar - pHead->GetFixedWidth() - 5;
	//			break;
	//		} else {
	//			iWidth = max(iRight - 5, iWidth);
	//		}
	//	}
	//} else {
	//	//iWidth = MAX();
	//}

	//pLabel->SetPos()
	return true;
}

void CBubbleLayoutUI::SetBubbleSize(SIZE size)
{
	CVerticalLayoutUI* pVerti = (CVerticalLayoutUI*)FindSubControlByClassName(_T("VerticalLayoutUI"), UIFIND_VISIBLE);
	CLabelUI* pBubble = (CLabelUI*)FindSubControlByClassName(_T("LabelUI"), UIFIND_VISIBLE);
	//pVerti->SetFixedHeight(size.cy+25);
	pVerti->SetFixedWidth(size.cx);
	pBubble->SetFixedWidth(size.cx);
	pBubble->SetFixedHeight(size.cy);
	/*m_cxyFixed.cy = size.cy;
	Invalidate();*/
	SetFixedHeight(size.cy);
}
