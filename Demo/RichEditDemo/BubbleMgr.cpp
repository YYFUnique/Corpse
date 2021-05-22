#include "StdAfx.h"
#include "BubbleMgr.h"

CBubbleMgr::CBubbleMgr()
{
	m_pRichText = NULL;
}

void CBubbleMgr::SetStart(DWORD dwStart)
{
	m_dwStart = dwStart;
}

void CBubbleMgr::SetEnd(DWORD dwEnd)
{
	m_dwEnd = dwEnd;
}

void CBubbleMgr::AddTipMsg(CRichTextUI* pRichText, LPCTSTR lpszRichText)
{
	CBubbleLayoutUI* pBubble = new CBubbleLayoutUI;
	EnterBubble(pRichText);
	pBubble->SetBkColor(0xFFFF0000);
	pRichText->AppendText(lpszRichText);

	LeaveBubble(pBubble);

	pRichText->SetSel(m_dwStart, m_dwEnd);
	
	PARAFORMAT2 pf;
	pf.cbSize        = sizeof(PARAFORMAT2);
	pf.dwMask        = PFM_ALIGNMENT;
	pf.wAlignment = PFA_CENTER;

	LRESULT lRes = FALSE;
	pRichText->GetTextServices()->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
}

void CBubbleMgr::EnterBubble(CRichTextUI* pRich)
{
	if (pRich == NULL)
		return;

	m_pRichText = pRich;
	m_pRichText->SetSel(-1,-1);
	CHARRANGE chRange;
	m_pRichText->GetSel(chRange);
	m_dwStart = chRange.cpMax;
}

void CBubbleMgr::LeaveBubble(CBubbleLayoutUI* pBubble)
{
	m_pRichText->SetSel(-1,-1);
	CHARRANGE chRange;
	m_pRichText->GetSel(chRange);
	m_dwEnd = chRange.cpMax;

	m_pRichText->AppendText(_T("\r\n"));
	pBubble->SetNickNameLen(m_dwNickLen);
	pBubble->SetRichTextRange(m_dwStart, m_dwEnd);

	m_pRichText->Add(pBubble);
	m_pRichText->Invalidate();
	m_pRichText = NULL;
}