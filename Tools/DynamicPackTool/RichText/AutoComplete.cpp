#include "StdAfx.h"
#include "../Utils/Platform.h"
#include "AutoComplete.h"
#include "Define.h"
#include "CharacterSet.h"

AutoComplete::AutoComplete()
	: m_bActive(FALSE)
	, m_chSeparator(',')
	, m_chTypesep('?')
	, m_bIgnoreCase(FALSE)
	, m_bChooseSingle(FALSE)
	, m_nPosStart(0)
	, m_nStartLen(0)
	, m_bCancelAtStartPos(TRUE)
	, m_bAutoHide(TRUE)
	, m_bDropRestOfWord(FALSE)
	, m_nIgnoreCaseBehaviour(SC_CASEINSENSITIVEBEHAVIOUR_RESPECTCASE)
	, m_nWidthLBDefault(100)
	, m_nHeightLBDefault(100)
{
	m_chStopChars[0] = '\0';
	m_chFillUpChars[0] = '\0';
}

AutoComplete::~AutoComplete()
{
	if (m_lb) {
		m_lb->Destroy();
		delete m_lb;
		m_lb = NULL;
	}
}

BOOL AutoComplete::IsActive() const
{
	return m_bActive;
}

void AutoComplete::Start(Window &parent, int nCtrlID, int nPos, POINT pt, int nStartLen, int nLineHeight, BOOL bUnicodeMode, int nTechnology)
{
	if (m_bActive)
		Cancel();

	m_lb->Create(parent, nCtrlID, pt, nLineHeight, bUnicodeMode, nTechnology);
	m_lb->Clear();
	m_bActive = TRUE;
	m_nStartLen = nStartLen;
	m_nPosStart = nPos;
}

void AutoComplete::SetStopChars(const CHAR* chStopChars)
{
	strncpy(m_chStopChars, chStopChars, sizeof(m_chStopChars));
	m_chStopChars[sizeof(m_chStopChars) - 1] = '\0';
}

BOOL AutoComplete::IsStopChar(CHAR chChar)
{
	return chChar && strchr(m_chStopChars, chChar);
}

void AutoComplete::SetFillupChars(const CHAR *chFillupChars)
{
	strncpy(m_chFillUpChars, chFillupChars, sizeof(m_chFillUpChars));
	m_chFillUpChars[sizeof(m_chFillUpChars) - 1] = '\0';
}

BOOL AutoComplete::IsFillUpChars(CHAR chChar)
{
	return chChar && strchr(m_chFillUpChars, chChar);
}

void AutoComplete::SetSeparator(CHAR chSeparator)
{
	m_chSeparator = chSeparator;
}

CHAR AutoComplete::GetSeparator() const
{
	return m_chSeparator;
}

void AutoComplete::SetTypesep(CHAR chSeparator)
{
	m_chTypesep = chSeparator;
}

CHAR AutoComplete::GetTypesep() const
{
	return m_chTypesep;
}

void AutoComplete::SetList(const CHAR *chList)
{
	m_lb->SetList(chList, m_chSeparator, m_chTypesep);
}

int AutoComplete::GetSelection() const
{
	return m_lb->GetSelection();
}

std::string AutoComplete::GetValue(int nItem) const
{
	CHAR chVal[maxItemLen];
	m_lb->GetValue(nItem, chVal, _countof(chVal));
	return std::string(chVal);
}

void AutoComplete::Show(BOOL bShow /*= TRUE*/)
{
	m_lb->Show(bShow);
	if (bShow)
		m_lb->Select(0);
}

void AutoComplete::Cancel()
{
	if (m_lb->IsCreated()) {
		m_lb->Clear();
		m_lb->Destroy();
		m_bActive = FALSE;
	}
}

void AutoComplete::Move(int nDelta)
{
	int nCount = m_lb->GetLength();
	int nCur = m_lb->GetSelection();
	nCur += nDelta;
	if (nCur >= nCount)
		nCur = nCount - 1;
	if (nCur < 0)
		nCur = 0;
	m_lb->Select(nCur);
}

void AutoComplete::Select(const CHAR *chWord)
{
	size_t nWordLen = strlen(chWord);
	int nPos = -1;
	int nStart = 0;		// lower bound of the api array block to search
	int nEnd = m_lb->GetLength() - 1; // upper bound of the api array block to search
	while ((nStart <= nEnd) && (nPos == -1)) { // Binary searching loop
		int nPivot = (nStart + nEnd) / 2;
		CHAR chItem[maxItemLen];
		m_lb->GetValue(nPivot, chItem, _countof(chItem));
		int nCond;
		if (m_bIgnoreCase)
			nCond = CompareNCaseInsensitive(chWord, chItem, nWordLen);
		else
			nCond = strncmp(chWord, chItem, nWordLen);

		if (nCond == 0) {
			// Find first match
			while (nPivot > nStart) {
				m_lb->GetValue(nPivot - 1, chItem, _countof(chItem));
				if (m_bIgnoreCase)
					nCond = CompareNCaseInsensitive(chWord, chItem, nWordLen);
				else
					nCond = strncmp(chWord, chItem, nWordLen);
				if (nCond != 0)
					break;

				--nPivot;
			}

			nPos = nPivot;
			if (m_bIgnoreCase && m_nIgnoreCaseBehaviour == SC_CASEINSENSITIVEBEHAVIOUR_RESPECTCASE) {
				// Check for exact-case match
				for (; nPivot = nEnd; ++nPivot) {
					m_lb->GetValue(nPivot, chItem, _countof(chItem));
					if (strncmp(chWord, chItem, nWordLen) == 0) {
						nPos = nPivot;
						break;
					}
					if (CompareNCaseInsensitive(chWord, chItem, nWordLen))
						break;
				}
			}
		} else if (nCond < 0)
			nEnd = nPivot - 1;
		else if (nCond > 0)
			nStart = nPivot + 1;
	}
	if (nPos == -1 && m_bAutoHide)
		Cancel();
	else
		m_lb->Select(nPos);
}