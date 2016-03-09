#include "stdafx.h"
#include "FontInfo.h"

CFontInfo::CFontInfo()
	:m_nSize(9)
	,m_clrText(RGB(0x01,0x01,0x01))
	,m_strName(_T("ËÎÌו"))
	,m_bBold(FALSE)
	,m_bItalic(FALSE)
	,m_bUnderLine(FALSE)
{

}

CFontInfo::~CFontInfo()
{

}

BOOL CFontInfo::IsEqualFont(const CFontInfo* pFontInfo)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (pFontInfo->m_strName != m_strName)
			break;
		if (pFontInfo->m_nSize != m_nSize)
			break;
		if (pFontInfo->m_clrText != m_clrText)
			break;
		if (pFontInfo->m_bUnderLine != m_bUnderLine)
			break;
		if (pFontInfo->m_bItalic != m_bItalic)
			break;
		if (pFontInfo->m_bBold != m_bBold)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CFontInfo::Copy(const CFontInfo* pFontInfo)
{
	m_strName = pFontInfo->m_strName;
	m_nSize = pFontInfo->m_nSize;
	m_clrText = pFontInfo->m_clrText;
	m_bUnderLine = pFontInfo->m_bUnderLine;
	m_bItalic = pFontInfo->m_bItalic;
	m_bBold = pFontInfo->m_bBold;
}