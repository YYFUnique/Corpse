#pragma once

#include "UIRichText.h"
#include "UIBubbleLayout.h"

class CBubbleMgr
{
public:
	CBubbleMgr();
public:
	void AddTipMsg(CRichTextUI* pRichText, LPCTSTR lpszRichText);

	void EnterBubble(CRichTextUI* pRich);
	void LeaveBubble(CBubbleLayoutUI* pBubble);

	void SetStart(DWORD dwStart);
	void SetEnd(DWORD dwEnd);

	void AddCharFormat();
public:
	DWORD			m_dwStart;
	DWORD			m_dwEnd;
	DWORD			m_dwNickLen;
	CRichTextUI*  m_pRichText;
};