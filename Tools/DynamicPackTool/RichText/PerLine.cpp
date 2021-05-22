#include "StdAfx.h"
#include "PerLine.h"
#include "Define.h"

MarkerHandleSet::MarkerHandleSet()
{
	m_pRoot = NULL;
}

MarkerHandleSet::~MarkerHandleSet()
{
	MarkerHandleNumber* pMhn = m_pRoot;
	while(pMhn) {
		MarkerHandleNumber* pMhnToFree = pMhn;
		pMhn = pMhn->pNext;
		delete pMhnToFree;
	}

	m_pRoot = NULL;
}

int MarkerHandleSet::GetLength() const
{
	int n = 0;
	MarkerHandleNumber* pMhn = m_pRoot;
	while(pMhn) {
		++n;
		pMhn = pMhn->pNext;
	}

	return n;
}

int MarkerHandleSet::NumberFromHandle(int nHandle) const
{
	MarkerHandleNumber* pMhn = m_pRoot;
	while(pMhn) {
		if (pMhn->handle == nHandle)
			return pMhn->number;
		pMhn = pMhn->pNext;
	}

	return -1;
}

int MarkerHandleSet::MarkValue() const
{
	UINT n = 0;
	MarkerHandleNumber* pMhn = m_pRoot;
	while(pMhn) {
		n |= (1 << pMhn->number);
		pMhn = pMhn->pNext;
	}

	return n;
}

BOOL MarkerHandleSet::IsContains(int nHandle) const
{
	MarkerHandleNumber* pMhn = m_pRoot;
	while(pMhn) {
		if (pMhn->handle == nHandle)
			return TRUE;

		pMhn = pMhn->pNext;
	}

	return FALSE;
}

BOOL MarkerHandleSet::InsertHandle(int nHandle, int nMarkNum)
{
	MarkerHandleNumber* pMhn = new MarkerHandleNumber;
	if (pMhn == NULL)
		return FALSE;

	// 将节点插入到头部
	pMhn->handle = nHandle;
	pMhn->number = nMarkNum;
	pMhn->pNext = m_pRoot;
	m_pRoot = pMhn;

	return TRUE;
}

void MarkerHandleSet::RemoveHandle(int nHandle)
{
	MarkerHandleNumber* pMhn = m_pRoot;
	MarkerHandleNumber* pPrevMhn = m_pRoot;
	while(pMhn) {
		if (pMhn->handle = nHandle) {
			// 将前一个节点的后节点指向被删除节点的下一个节点
			pPrevMhn->pNext = pMhn->pNext;
			
			delete pMhn;
			return;
		}

		pPrevMhn = pMhn;
		pMhn = pMhn->pNext;
	}
}

BOOL MarkerHandleSet::RemoveNumber(int nMarkNum, BOOL bAll)
{
	BOOL bPerformedDelete = FALSE;
	MarkerHandleNumber* pMhn = m_pRoot;
	MarkerHandleNumber* pPrevMhn = m_pRoot;
	while(pMhn) {
		if (pMhn->number == nMarkNum) {
			pPrevMhn->pNext = pMhn->pNext;
			delete pMhn;
			bPerformedDelete = TRUE;
			if (bAll == FALSE)
				break;
		}

		pPrevMhn = pMhn;
		pMhn = pMhn->pNext;
	}

	return bPerformedDelete;
}

void MarkerHandleSet::CombineWith(MarkerHandleSet *pOther)
{
	MarkerHandleNumber* pMhn = m_pRoot;
	while(pMhn)
		pMhn = pMhn->pNext;

	pMhn = pOther->m_pRoot;

	// ?
	// pOther->m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////
LineMarkers::LineMarkers()
	: m_nHandleCurrent(0)
{

}

LineMarkers::~LineMarkers()
{
	Init();
}

void LineMarkers::Init()
{
	for (int nLine = 0; nLine < m_pMarkers.Length(); ++ nLine) {
		delete m_pMarkers[nLine];
		m_pMarkers[nLine] = 0;
	}

	m_pMarkers.DeleteAll();
}

void LineMarkers::InsertLine(int nLine)
{
	if (m_pMarkers.Length())
		m_pMarkers.Insert(nLine, 0);
}

void LineMarkers::RemoveLine(int nLine)
{
	// Retain the markers from the deleted line by oring them into the previous line
	if (m_pMarkers.Length()) {
		if (nLine > 0)
			MergeMarkers(nLine - 1);
		m_pMarkers.Delete(nLine);
	}
}

int LineMarkers::LineFromHandle(int nMarkerHandle)
{
	if (m_pMarkers.Length()) {
		for (int nLine = 0; nLine < m_pMarkers.Length(); ++nLine) {
			if (m_pMarkers[nLine]) {
				if (m_pMarkers[nLine]->IsContains(nMarkerHandle))
					return nLine;
			}
		}
	}

	return -1;
}

void LineMarkers::MergeMarkers(int nPos)
{
	if (m_pMarkers[nPos + 1] != NULL) {
		if (m_pMarkers[nPos] == NULL)
			m_pMarkers[nPos] = new MarkerHandleSet;
		m_pMarkers[nPos]->CombineWith(m_pMarkers[nPos + 1]);
		delete m_pMarkers[nPos + 1];
		m_pMarkers[nPos + 1] = NULL;
	}
}

int LineMarkers::GetMarkValue(int nLine) const
{
	if (m_pMarkers.Length() && nLine >= 0 && (nLine < m_pMarkers.Length()) && m_pMarkers[nLine])
		return m_pMarkers[nLine]->MarkValue();
	else
		return 0;
}

int LineMarkers::GetMarkerNext(int nLineStart, int nMask) const
{
	if (nLineStart < 0)
		return 0;
	int nLen = m_pMarkers.Length();
	for (int iLine = nLineStart; iLine < nLen; ++iLine) {
		MarkerHandleSet* pOnline = m_pMarkers[iLine];
		if (pOnline && ((pOnline->MarkValue() & nMask) != 0))
			return iLine;
	}

	return -1;
}

int LineMarkers::AddMark(int nLine, int nMarker, int nLines)
{
	++m_nHandleCurrent;
	if (m_pMarkers.Length() == 0)
		m_pMarkers.InsertValue(0, nLines, 0);

	if (nLine >= m_pMarkers.Length())
		return -1;

	if (m_pMarkers[nLine] == NULL) {
		// Need new structure to hold marker handle
		m_pMarkers[nLine] = new MarkerHandleSet;
		if (m_pMarkers[nLine] == NULL)
			return -1;
	}

	m_pMarkers[nLine]->InsertHandle(m_nHandleCurrent, nMarker);

	return m_nHandleCurrent;
}

BOOL LineMarkers::DeleteMark(int nLine, int nMarkNum, BOOL bAll)
{
	BOOL bChanges = FALSE;
	if (m_pMarkers.Length() && (nLine >=0) && (nLine < m_pMarkers.Length()) && m_pMarkers[nLine]) {
		if (nMarkNum == -1){
			bChanges = TRUE;
			delete m_pMarkers[nLine];
			m_pMarkers[nLine] = NULL;
		} else {
			bChanges = m_pMarkers[nLine]->RemoveNumber(nMarkNum, bAll);
			if (m_pMarkers[nLine]->GetLength() == 0) {
				delete m_pMarkers[nLine];
				m_pMarkers[nLine] = NULL;
			}
		}
	}

	return bChanges;
}

void LineMarkers::DeleteMardFromHandle(int nMarkHandle)
{
	int nLine = LineFromHandle(nMarkHandle);
	if (nLine >= 0) {
		m_pMarkers[nLine]->RemoveHandle(nMarkHandle);
		if (m_pMarkers[nLine]->GetLength() == 0) {
			delete m_pMarkers[nLine];
			m_pMarkers[nLine] = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////
LineLevels::LineLevels()
{

}

LineLevels::~LineLevels()
{

}

void LineLevels::Init()
{
	m_nLevels.DeleteAll();
}

void LineLevels::InsertLine(int nLine)
{
	if (m_nLevels.Length()) {
		int nLevel = (nLine < m_nLevels.Length()) ? m_nLevels[nLine] : SC_FOLDLEVELBASE;
		m_nLevels.InsertValue(nLine, 1, nLine);
	}
}

void LineLevels::RemoveLine(int nLine)
{
	if (m_nLevels.Length()) {
		// Move up following lines but merge header flag from this line
		// to line before to avoid a temporary disappearence causing expansion.
		int nFirstheader = m_nLevels[nLine] & SC_FOLDLEVELHEADERFLAG;
		m_nLevels.Delete(nLine);

		// Last line loses the header flag
		if (nLine == m_nLevels.Length() - 1)
			m_nLevels[nLine - 1] &= ~SC_FOLDLEVELHEADERFLAG;
		else if (nLine >0 )
			m_nLevels[nLine - 1] |= nFirstheader;
	}
}

void LineLevels::ExpandLevels(int nSizeNew /*= -1*/)
{
	m_nLevels.InsertValue(m_nLevels.Length(), nSizeNew - m_nLevels.Length(), SC_FOLDLEVELBASE);
}

void LineLevels::ClearLevels()
{
	m_nLevels.DeleteAll();
}

int LineLevels::SetLevel(int nLine, int nLevel, int nLines)
{
	int nPre = 0;
	if ((nLine >= 0) && nLine < nLines) {
		if (m_nLevels.Length() == 0)
			ExpandLevels(nLines + 1);
		nPre = m_nLevels[nLine];
		if (nPre != nLevel)
			m_nLevels[nLine] = nLevel;
	}

	return nPre;
}

int LineLevels::GetLevel(int nLine) const
{
	if (m_nLevels.Length() && (nLine >= 0) && (nLine < m_nLevels.Length()))
		return m_nLevels[nLine];
	else
		return SC_FOLDLEVELBASE;
}

/////////////////////////////////////////////////////////////////
LineState::LineState()
{

}

LineState::~LineState()
{

}

void LineState::Init()
{
	m_nLineStates.DeleteAll();
}

void LineState::InsertLine(int nLine)
{
	if (m_nLineStates.Length()) {
		m_nLineStates.EnsureLength(nLine);
		int nVal = (nLine < m_nLineStates.Length()) ? m_nLineStates.Length() : 0;
		m_nLineStates.Insert(nLine, nVal);
	}
}

void LineState::RemoveLine(int nLine)
{
	if (m_nLineStates.Length() > nLine)
		m_nLineStates.Delete(nLine);
}

int LineState::SetLineState(int nLine, int nState)
{
	m_nLineStates.EnsureLength(nLine + 1);
	int nStateOld = m_nLineStates[nLine];
	m_nLineStates[nLine] = nState;

	return nStateOld;
}

int LineState::GetLineState(int nLine)
{
	if (nLine < 0)
		return 0;

	m_nLineStates.EnsureLength(nLine + 1);
	return m_nLineStates[nLine];
}

int LineState::GetMaxLineState() const
{
	return m_nLineStates.Length();
}

// Each allocated LineAnnotation is a char array which starts with an AnnotationHeader
// and then has text and optional styles.

static const int IndividualStyles = 0x100;

typedef struct tagAnnotationHeader {
	short style;	// Style IndividualStyles implies array of styles
	short lines;
	int length;
}AnnotationHeader;


LineAnnotation::LineAnnotation()
{

}

LineAnnotation::~LineAnnotation()
{
	ClearAll();
}

void LineAnnotation::Init()
{
	ClearAll();
}

void LineAnnotation::InsertLine(int nLine)
{
	if (m_chAnnotations.Length()) {
		m_chAnnotations.EnsureLength(nLine);
		m_chAnnotations.Insert(nLine, 0);
	}
}

void LineAnnotation::RemoveLine(int nLine)
{
	if (m_chAnnotations.Length() && (nLine < m_chAnnotations.Length())) {
		delete m_chAnnotations[nLine];
		m_chAnnotations.Delete(nLine);
	}
}

BOOL LineAnnotation::AnySet() const
{
	return m_chAnnotations.Length() > 0;
}

BOOL LineAnnotation::MultipleStyles(int nLine) const
{
	if (m_chAnnotations.Length() && (nLine >= 0) && (nLine < m_chAnnotations.Length()) && m_chAnnotations[nLine])
		return reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLine])->style == IndividualStyles;
	else
		return 0;
}

int LineAnnotation::GetStyle(int nLine) const
{
	if (m_chAnnotations.Length() && (nLine >= 0) && (nLine < m_chAnnotations.Length()) && m_chAnnotations[nLine])
		return reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLine])->style;
	else
		return 0;
}

const CHAR* LineAnnotation::GetText(int nLine) const
{
	if (m_chAnnotations.Length() && (nLine >= 0) && (nLine < m_chAnnotations.Length()) && m_chAnnotations[nLine])
		return m_chAnnotations[nLine] + sizeof(AnnotationHeader);
	else
		return 0;
}

const BYTE* LineAnnotation::GetStyles(int nLine) const
{
	if (m_chAnnotations.Length() && (nLine >= 0) && (nLine < m_chAnnotations.Length()) && m_chAnnotations[nLine])
		return reinterpret_cast<BYTE*>(m_chAnnotations[nLine] + sizeof(AnnotationHeader) + GetLength(nLine));
	else
		return 0;
}

void LineAnnotation::SetText(int nLine, const CHAR *chText)
{
	if (chText && (nLine >= 0)) {
		m_chAnnotations.EnsureLength(nLine + 1);
		int nStyle = GetStyle(nLine);
		if (m_chAnnotations[nLine])
			delete[] m_chAnnotations[nLine];

		m_chAnnotations[nLine] = AllocateAnnotation(static_cast<int>(strlen(chText)), nStyle);
		AnnotationHeader* pAh = reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLine]);
		pAh->style = static_cast<short>(nStyle);
		pAh->length = static_cast<int>(strlen(chText));
		pAh->lines = static_cast<short>(NumberLines(chText));
		memcpy(m_chAnnotations[nLine] + sizeof(AnnotationHeader), chText, pAh->length);
	} else {
		if (m_chAnnotations.Length() && (nLine >= 0) && (nLine < m_chAnnotations.Length()) && m_chAnnotations[nLine]) {
			delete[] m_chAnnotations[nLine];
			m_chAnnotations[nLine] = NULL;
		}
	}
}

void LineAnnotation::ClearAll()
{
	for (int nLine = 0; nLine < m_chAnnotations.Length(); ++ nLine) {
		delete[] m_chAnnotations[nLine];
		m_chAnnotations[nLine] = NULL;
	}

	m_chAnnotations.DeleteAll();
}

void LineAnnotation::SetStyle(int nLine, int nStyle)
{
	m_chAnnotations.EnsureLength(nLine + 1);
	if (m_chAnnotations[nLine] == NULL)
		m_chAnnotations[nLine] = AllocateAnnotation(0, nStyle);

	reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLine])->style = static_cast<short>(nStyle);
}

void LineAnnotation::SetStyles(int nLine, const BYTE* chStyles)
{
	if (nLine >= 0) {
		m_chAnnotations.EnsureLength(nLine + 1);
		if (m_chAnnotations[nLine] == NULL)
			m_chAnnotations[nLine] = AllocateAnnotation(0, IndividualStyles);
		else
		{
			AnnotationHeader* pAhSource = reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLine]);
			if (pAhSource->style != IndividualStyles) {
				CHAR* pAllocation = AllocateAnnotation(pAhSource->length, IndividualStyles);
				AnnotationHeader* pAhAlloc = reinterpret_cast<AnnotationHeader*>(pAllocation);

				pAhAlloc->length = pAhSource->length;
				pAhAlloc->lines = pAhSource->lines;
				memcpy(pAllocation + sizeof(AnnotationHeader), m_chAnnotations[nLine] + sizeof(AnnotationHeader), pAhSource->length);

				delete[] m_chAnnotations[nLine];
				m_chAnnotations[nLine] = pAllocation;
			}
		}

		AnnotationHeader* pAh = reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLine]);
		pAh->style = IndividualStyles;
		memcpy(m_chAnnotations[nLine] + sizeof(AnnotationHeader) + pAh->length, chStyles, pAh->length);
	}
}

int LineAnnotation::GetLength(int nLen) const
{
	if (m_chAnnotations.Length() && (nLen >= 0) && (nLen < m_chAnnotations.Length()) && m_chAnnotations[nLen])
		return reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLen])->length;
	else
		return 0;
}

int LineAnnotation::GetLines(int nLen) const
{
	if (m_chAnnotations.Length() && (nLen >= 0) && (nLen < m_chAnnotations.Length()) && m_chAnnotations[nLen])
		return reinterpret_cast<AnnotationHeader*>(m_chAnnotations[nLen])->lines;
	else
		return 0;
}

int LineAnnotation::NumberLines(const CHAR* chData) 
{
	if (chData) {
		int nLine = 0;
		while (*chData) {
			if (*chData == '\n')
				nLine++;
			chData++;
		}
		return nLine+1;
	} else {
		return 0;
	}
}

CHAR* LineAnnotation::AllocateAnnotation(int nLen, int nStyle)
{
	size_t dwLen = sizeof(AnnotationHeader) + nLen + ((nStyle == IndividualStyles) ? nLen : 0);
	CHAR *chRet = new CHAR[dwLen];
	memset(chRet, 0, dwLen);
	return chRet;
}