#include "StdAfx.h"
#include "Document.h"
#include "PerLine.h"
#include "Define.h"
#include "UniConversion.h"
#include "CharacterSet.h"
#include "../Utils/PlatWin.h"
#include "ILexInterface.h"

Document::Document()
	: m_nRefCount(0)
	, m_pcf(NULL)
	, m_nEolMode(SC_EOL_CRLF)
	, m_nDBCSCodePage(0)
	, m_nStylingBits(5)
	, m_nStylingBitsMask(0x1F)
	, m_chStylingMask(0)
	, m_nEndStyled(0)
	, m_nStyleClock(0)
	, m_nEnteredModification(0)
	, m_nEnteredStyling(0)
	, m_nEnteredReadOnlyCount(0)
	, m_nTabInChars(8)
	, m_nIndentInChars(0)
	, m_nActualIndentInChars(8)
	, m_bUseTabs(TRUE)
	, m_bTabIndents(TRUE)
	, m_bBackspaceUnindents(FALSE)
	, m_pWatchers(NULL)
	, m_nWatcherLen(0)
	, m_bMatchesValid(FALSE)
	, m_pRegex(NULL)
{
	UTF8BytesOfLeadInitialise();

	m_pPerLineData[LINE_DATA_MARKERS] = new LineMarkers;
	m_pPerLineData[LINE_DATA_LEVELS] = new LineLevels;
	m_pPerLineData[LINE_DATA_STATES] = new LineState;
	m_pPerLineData[LINE_DATA_MARGIN] = new LineAnnotation;
	m_pPerLineData[LINE_DATA_ANNOTATION] = new LineAnnotation;

	m_cb.SetPerLine(this);
	m_pLexInterface = NULL;
}

Document::~Document()
{
	for (int n=0; n < m_nWatcherLen; ++n) 
		m_pWatchers[n].GetDocWatcher()->NotifyDeleted(this, m_pWatchers[n].GetUserData());

	delete[] m_pWatchers;
	for (int m=0; m<LINE_DATA_SIZE; ++m) {
		delete m_pPerLineData[m];
		m_pPerLineData[m] = NULL;
	}

	m_pWatchers = NULL;
	m_nWatcherLen = 0;
	delete m_pRegex;
	m_pRegex = NULL;
	delete m_pLexInterface;
	m_pLexInterface = NULL;
	delete m_pcf;
	m_pcf = NULL;
}

void Document::Init()
{
	for (int m=0; m<LINE_DATA_SIZE; ++m)
		if (m_pPerLineData[m] != NULL)
			m_pPerLineData[m]->Init();
}

BOOL Document::SetDBCSCodePage(int nDBCSCodePage)
{
	if (m_nDBCSCodePage != nDBCSCodePage) {
		m_nDBCSCodePage = nDBCSCodePage;
		SetCaseFolder(NULL);
		return TRUE;
	} 
	return FALSE;
}

void Document::InsertLine(int nLine)
{
	for (int m=0; m<LINE_DATA_SIZE; ++m)
		if (m_pPerLineData[m] != NULL)
			m_pPerLineData[m]->InsertLine(nLine);
}

void Document::RemoveLine(int nLine)
{
	for (int m=0; m<LINE_DATA_SIZE; ++m)
		if (m_pPerLineData[m] != NULL)
			m_pPerLineData[m]->RemoveLine(nLine);
}

// Increase reference count and return its previous value.
int Document::AddRef() {
	return m_nRefCount++;
}

// Decrease reference count and return its previous value.
// Delete the document if reference count reaches zero.
int Document::Release() 
{
	int nRefCount = --m_nRefCount;
	if (nRefCount == 0)
		delete this;
	return nRefCount;
}

void Document::SetSavePoint()
{
	m_cb.SetSavePoint();
	NotifySavePoint(TRUE);
}

BOOL Document::IsSavePoint() const
{
	return m_cb.IsSavePoint();
}

const CHAR* Document::BufferPointer()
{
	return m_cb.BufferPointer();
}

const CHAR* Document::RangePointer(int nPos, int nLen)
{
	return m_cb.RangePointer(nPos, nLen);
}

int Document::GetMark(int nLine) const
{
	return static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->GetMarkValue(nLine);
}

int Document::MarkerNext(int nLineStart, int nMask) const
{
	return static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->GetMarkerNext(nLineStart, nMask);
}

int Document::AddMark(int nLine, int nMarkerNum)
{
	if (nLine >= 0 && nLine <= LinesTotal()) {
		int nPrev = static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->AddMark(nLine, nMarkerNum, LinesTotal());
		DocModification DocMh(SC_MOD_CHANGEMARKER, LineStart(nLine), 0, 0, 0, nLine);
		NotifyModified(DocMh);

		return nPrev;
	}

	return 0;
}

void Document::AddMarkSet(int nLine, int nValueSet)
{
	if (nLine < 0 || nLine > LinesTotal())
		return;

	UINT m = nValueSet;
	for (int n=0; m; ++n, m >>= 1) 
		if (m & 1)
			static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->AddMark(nLine, n, LinesTotal());
	
	DocModification DocMh(SC_MOD_CHANGEMARKER, LinesTotal(), 0, 0, 0, nLine);
	NotifyModified(DocMh);
}

void Document::DeleteMark(int nLine, int nMarkerNum)
{
	static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->DeleteMark(nLine, nMarkerNum, FALSE);

	DocModification DocMh(SC_MOD_CHANGEMARKER, LinesTotal(), 0, 0, 0, nLine);
	NotifyModified(DocMh);
}

void Document::DeleteMarkFromHandle(int nMarkerHandle)
{
	static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->DeleteMardFromHandle(nMarkerHandle);

	DocModification DocMh(SC_MOD_CHANGEMARKER, 0, 0, 0, 0);
	DocMh.SetLine(-1);
	NotifyModified(DocMh);
}

void Document::DeleteAllMarks(int nMarkerNum)
{
	BOOL bChange = FALSE;
	for (int nLine = 0; nLine < LinesTotal(); ++nLine){
		LineMarkers* pLineMarkers = static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS]);
		if (pLineMarkers != NULL && pLineMarkers->DeleteMark(nLine, nMarkerNum, TRUE) != FALSE)
			bChange = TRUE;
	}

	if (bChange) {
		DocModification DocMh(SC_MOD_CHANGEMARKER, 0, 0, 0, 0);
		DocMh.SetLine(-1);
		NotifyModified(DocMh);
	}
}

int Document::LineFromHandle(int nMarkerHandle)
{
	return static_cast<LineMarkers*>(m_pPerLineData[LINE_DATA_MARKERS])->LineFromHandle(nMarkerHandle);
}

int Document::LineStart(int nLine) const
{
	return m_cb.LineStart(nLine);
}

int Document::LineEnd(int nLine) const
{
	if (nLine == LinesTotal() - 1)
		return LineStart(nLine + 1);
	else
	{
		int nPos = LineStart(nLine + 1) - 1;
		// When line terminator is CR+LF, may need to go back one more
		if ((nPos > LineStart(nLine)) && (m_cb.CharAt(nPos - 1) == '\r'))
			--nPos;

		return nPos;
	}
}

void Document::SetErrorStatus(int nStatus)
{
	// Tell the watchers the lexer has changed.
	for (int n=0; n<m_nWatcherLen; ++n)
		m_pWatchers[n].GetDocWatcher()->NotifyErrorOccurred(this, m_pWatchers[n].GetUserData(), nStatus);
}

int Document::LineFromPosition(int nPos) const
{
	return m_cb.LineFromPosition(nPos);
}

int Document::LineEndPosition(int nPos) const
{
	return LineEnd(LineFromPosition(nPos));
}

BOOL Document::IsLineEndPosition(int nPos) const
{
	return LineEnd(LineFromPosition(nPos)) == nPos;
}

int Document::VCHomePosition(int nPos) const
{
	int nLine = LineFromPosition(nPos);
	int nStartPos = LineStart(nLine);
	int nEndLine = LineEnd(nLine);
	int nStartText = nStartPos;
	while(nStartText < nEndLine && (m_cb.CharAt(nStartText) == ' ' || m_cb.CharAt(nStartText) == '\t'))
		++nStartText;

	return nStartText;
}

int Document::SetLevel(int nLine, int nLevel)
{
	int nPrev = static_cast<LineLevels*>(m_pPerLineData[LINE_DATA_LEVELS])->SetLevel(nLine, nLevel, LinesTotal());
	if (nPrev != nLevel) {
		DocModification DocMh(SC_MOD_CHANGEFOLD | SC_MOD_CHANGEMARKER, LineStart(nLine), 0, 0, 0, nLine);

		DocMh.SetFoldLevelNow(nLevel);
		DocMh.SetFoldLevelPrev(nPrev);

		NotifyModified(DocMh);
	}

	return nPrev;
}

int Document::GetLevel(int nLine) const
{
	return static_cast<LineLevels*>(m_pPerLineData[LINE_DATA_LEVELS])->GetLevel(nLine);
}

void Document::ClearLevels()
{
	static_cast<LineLevels*>(m_pPerLineData[LINE_DATA_LEVELS])->ClearLevels();
}

static BOOL IsSubordinate(int nLevelStart, int nLevelTry) {
	if (nLevelTry & SC_FOLDLEVELWHITEFLAG)
		return TRUE;
	else
		return (nLevelStart & SC_FOLDLEVELNUMBERMASK) < (nLevelStart & SC_FOLDLEVELNUMBERMASK);
}

int Document::GetLastChild(int nLineParent, int nLevel /*= -1*/, int nLastLine /*= -1*/) 
{
	if (nLevel == -1)
		nLevel = GetLevel(nLineParent) & SC_FOLDLEVELNUMBERMASK;
	int nMaxLine = LinesTotal();
	int nLastVisibleLine = (nLastLine != -1) ? MIN(LinesTotal() - 1, nLastLine) : -1;
	int nLineMaxSubord = nLineParent;
	while(nLineMaxSubord < nMaxLine - 1) {
		EnsureStyledTo(LineStart(nLineMaxSubord + 2));
		if (IsSubordinate(nLevel, GetLevel(nLineMaxSubord+ 1)) == FALSE)
			break;
		if ((nLastVisibleLine != -1) && (nLineMaxSubord >= nLastVisibleLine) && !(GetLevel(nLineMaxSubord) & SC_FOLDLEVELWHITEFLAG))
			break;
		++nLineMaxSubord;
	}

	if (nLineMaxSubord > nLineParent) {
		if (nLevel > (GetLevel(nLineMaxSubord + 1) & SC_FOLDLEVELNUMBERMASK)) {
			// Have chewed up some whitespace that belongs to a parent so seek back
			if (GetLevel(nLineMaxSubord) & SC_FOLDLEVELWHITEFLAG)
				--nLineMaxSubord;
		}
	}

	return nLineMaxSubord;
}

int Document::GetFoldParent(int nLine) const
{
	int nLevel = GetLevel(nLine) & SC_FOLDLEVELNUMBERMASK;
	int nLineLook = nLine - 1;
	while ((nLineLook > 0) && (
			(!(GetLevel(nLineLook) & SC_FOLDLEVELHEADERFLAG)) ||
			((GetLevel(nLineLook) & SC_FOLDLEVELNUMBERMASK) >= nLevel))
			)
	{
		--nLineLook;
	}

	if ((GetLevel(nLineLook) & SC_FOLDLEVELHEADERFLAG) &&
		((GetLevel(nLineLook) & SC_FOLDLEVELNUMBERMASK) < nLevel))
	{
		return nLineLook;
	}

	return -1;
}

void Document::GetHighlightDelimiters(HighlightDelimiter &hDelimite, int nLine, int nLastLine) 
{
	int nLevle = GetLevel(nLine);
	int nLastVisibleLine = MAX(nLine, nLastLine) + 1;

	int nLookLine = nLine;
	int nLookLineLevel = nLevle;
	int nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK;
	while ((nLookLine > 0) && ((nLookLineLevel & SC_FOLDLEVELWHITEFLAG) ||
		((nLookLineLevel & SC_FOLDLEVELHEADERFLAG) && 
		(nLookLineLevelNum >= (GetLevel(nLookLine + 1) & SC_FOLDLEVELNUMBERMASK))))) 
	{
		nLookLineLevel = GetLevel(--nLookLine);
		nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK;
	}

	int nBeginFoldBlock = (nLookLineLevel & SC_FOLDLEVELHEADERFLAG) ? nLookLine : GetFoldParent(nLookLine);
	if (nBeginFoldBlock == -1) {
		hDelimite.Clear();
		return;
	}

	int nEndFoldBlock = GetLastChild(nBeginFoldBlock, -1, nLastVisibleLine);
	int nFirstChangeableLineBefore = -1;
	if (nEndFoldBlock < nLine) {
		nLookLine = nBeginFoldBlock - 1;
		nLookLineLevel = GetLevel(nLookLine);
		nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK;
		while ((nLookLine >= 0) && (nLookLineLevelNum >= SC_FOLDLEVELBASE)) {
			if (nLookLineLevel & SC_FOLDLEVELHEADERFLAG) { 
				if (GetLastChild(nLookLine, -1, nLastVisibleLine) == nLine) {
					nBeginFoldBlock = nLookLine;
					nEndFoldBlock = nLine;
					nFirstChangeableLineBefore = nLine - 1;
				}
			}
			if ((nLookLine > 0) && (nLookLineLevelNum == SC_FOLDLEVELBASE) && ((GetLevel(nLookLine - 1) & SC_FOLDLEVELNUMBERMASK) > nLookLineLevelNum))
				break;

			nLookLineLevel = GetLevel(--nLookLine);
			nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK;
		}
	}
	if (nFirstChangeableLineBefore == -1) {
		nLookLine = nLine - 1;
		nLookLineLevel = GetLevel(nLookLine);
		nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK;
		for (; nLookLine >= nBeginFoldBlock;
			nLookLineLevel = GetLevel(--nLookLine), nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK) 
		{
			if ((nLookLineLevel & SC_FOLDLEVELWHITEFLAG) || (nLookLineLevelNum > (nLevle & SC_FOLDLEVELNUMBERMASK))) {
				nFirstChangeableLineBefore = nLookLine;
				break;
			}
		}
	}

	if (nFirstChangeableLineBefore == -1) 
		nFirstChangeableLineBefore = nBeginFoldBlock - 1;

	int nFirstChangeableLineAfter = -1;
	nLookLine = nLine + 1;
	nLookLineLevel = GetLevel(nLookLine);
	nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK; 
	for (; nLookLine <= nEndFoldBlock;
			nLookLineLevel = GetLevel(++nLookLine), nLookLineLevelNum = nLookLineLevel & SC_FOLDLEVELNUMBERMASK) 
	{
		if ((nLookLineLevel & SC_FOLDLEVELHEADERFLAG) && (nLookLineLevelNum < (GetLevel(nLookLine + 1) & SC_FOLDLEVELNUMBERMASK))) {
			nFirstChangeableLineAfter = nLookLine;
			break;
		}
	}

	if (nFirstChangeableLineAfter = -1) 
		nFirstChangeableLineAfter = nEndFoldBlock + 1;

	hDelimite.SetBeginFoldBlock(nBeginFoldBlock);
	hDelimite.SetEndFoldBlock(nEndFoldBlock);
	hDelimite.SetFirstChangeableLineBefore(nFirstChangeableLineBefore);
	hDelimite.SetFirstChangeableLineAfter(nFirstChangeableLineAfter);
}

int Document::ClampPositionIntoDocument(int nPos)
{
	return Platfrom::Clamp(nPos, 0, Length());
}

BOOL Document::IsCrLf(int nPos) const
{
	if (nPos < 0)
		return FALSE;
	if (nPos >= Length() - 1)
		return FALSE;
	return (m_cb.CharAt(nPos) == '\r') && (m_cb.CharAt(nPos + 1) == '\n');
}

int Document::LenChar(int nPos)
{
	if (nPos < 0)
		return 1;
	else if (IsCrLf(nPos))
		return 2;
	else if (SC_CP_UTF8 == m_nDBCSCodePage) {
		const BYTE nLeadByte = static_cast<BYTE>(m_cb.CharAt(nPos));
		const int nCharBytesWidth = UTF8BytesOfLead[nLeadByte];

		int nDocLen = Length();
		if ((nPos + nCharBytesWidth) > nDocLen)
			return nDocLen - nPos;
		else
			return nCharBytesWidth;
	} else if (m_nDBCSCodePage)
		return IsDBCSLeadByte(m_cb.CharAt(nPos)) ? 2 : 1;
	else
		return 1;
}

BOOL Document::IsInGoodUTF8(int nPos, int& nStart, int& nEnd) const
{
	int nTrail = nPos;
	while((nTrail > 0) && (nPos - nTrail < UTF8MaxBytes) && UTF8IsTrailByte(static_cast<BYTE>(m_cb.CharAt(nTrail - 1))))
		--nTrail;

	nStart = (nTrail > 0) ? nTrail - 1 : nTrail;

	const BYTE bLeadByte = static_cast<BYTE>(m_cb.CharAt(nStart));
	const int nCharBytesWidth = UTF8BytesOfLead[bLeadByte];
	
	if (nCharBytesWidth == 1)
		return FALSE;
	else {
		int nTrailBytes = nCharBytesWidth - 1;
		int nLen = nPos - nStart;

		// pos too far from lead
		if (nLen > nTrailBytes)
			return FALSE;

		CHAR chBytes[UTF8MaxBytes] = {static_cast<char>(bLeadByte),0,0,0};
		for (int n=1; n<nCharBytesWidth && ((nStart + n) < Length()); ++n)
			chBytes[n] = m_cb.CharAt(static_cast<int>(nStart + n));
		int nUTF8Status = UTF8Classify(reinterpret_cast<const BYTE*>(chBytes), nCharBytesWidth);
		if (nUTF8Status & UTF8MaskInvalid)
			return FALSE;
		nEnd = nStart = nCharBytesWidth;
		return TRUE;
	}
}

// Normalise a position so that it is not halfway through a two byte character.
// This can occur in two situations -
// When lines are terminated with \r\n pairs which should be treated as one character.
// When displaying DBCS text such as Japanese.
// If moving, move the position in the indicated direction.
int Document::MovePositionOutsideChar(int nPos, int nMoveDir, BOOL bCheckLineEnd /*= TRUE*/)
{
	// If out of range, just return minimum/maximum value.
	if (nPos <= 0)
		return 0;
	if (nPos >= Length())
		return Length();

	if (bCheckLineEnd && IsCrLf(nPos - 1)) {
		if (nMoveDir > 0)
			return nPos + 1;
		else
			return nPos - 1;
	}
	if (m_nDBCSCodePage) {
		if (SC_CP_UTF8 == m_nDBCSCodePage) {
			BYTE bch = static_cast<BYTE>(m_cb.CharAt(nPos));
			// If ch is not a trail byte then pos is valid intercharacter position
			if (UTF8IsTrailByte(bch)) { 
				int nStartUTF = nPos;
				int nEndUTF = nPos;
				if (IsInGoodUTF8(nPos, nStartUTF, nEndUTF)) {
					// ch is a trail byte within a UTF-8 character
					if (nMoveDir > 0)
						nPos = nEndUTF;
					else
						nPos = nStartUTF;
				}
				// Else invalid UTF-8 so return position of isolated trail byte
			}
		} else {
			// Anchor DBCS calculations at start of line because start of line can
			// not be a DBCS trail byte.
			int nPosStartLine = LineStart(LineFromPosition(nPos));
			if (nPos == nPosStartLine)
				return nPos;

			// Step back until a non-lead-byte is found.
			int nPosCheck = nPos;
			while((nPosCheck > nPosStartLine) && IsDBCSLeadByte(m_cb.CharAt(nPosCheck - 1)))
				--nPosCheck;

			// Check from known start of character.
			while(nPosCheck < nPos) {
				int nMbSize = IsDBCSLeadByte(m_cb.CharAt(nPosCheck)) ? 2 : 1;
				if (nPosCheck + nMbSize == nPos)
					return nPos;
				else if (nPosCheck + nMbSize > nPos) {
					if (nMoveDir > 0)
						return nPosCheck + nMbSize;
					else
						return nPosCheck;
				}

				nPosCheck += nMbSize;
			}
		}
	}

	return nPos;
}

// NextPosition moves between valid positions - it can not handle a position in the middle of a
// multi-byte character. It is used to iterate through text more efficiently than MovePositionOutsideChar.
// A \r\n pair is treated as two characters.

int Document::NextPosition(int nPos, int nMoveDir) const
{
	// If out of range, just return minimum/maximum value.
	int nIncrement = (nMoveDir > 0) ? 1 : -1;
	if (nPos + nIncrement <= 0)
		return 0;
	if (nPos + nIncrement >= Length())
		return Length();

	if (m_nDBCSCodePage) {
		if (SC_CP_UTF8 == m_nDBCSCodePage) { 
			if (nIncrement == 1) {
				// Simple forward movement case so can avoid some checks
				const BYTE bLeadByte = static_cast<BYTE>(m_cb.CharAt(nPos));
				// Single byte character or invalid
				if (UTF8IsAscii(bLeadByte))
					++nPos;
				else {
					const int nCharBytesWidth = UTF8BytesOfLead[bLeadByte];
					CHAR chBytes[UTF8MaxBytes] = {static_cast<char>(bLeadByte),0,0,0};
					for (int n=0; n<nCharBytesWidth; ++n)
						chBytes[n] = m_cb.CharAt(static_cast<int>(nPos + n));

					int nUTF8Status = UTF8Classify(reinterpret_cast<const BYTE*>(chBytes), nCharBytesWidth);
					if (nUTF8Status & UTF8MaskInvalid)
						++nPos;
					else
						nPos += nUTF8Status & UTF8MaskWidth;
				}
			} else {
				// Examine byte before position
				--nPos;
				BYTE bch = static_cast<BYTE>(m_cb.CharAt(nPos));
				// If ch is not a trail byte then pos is valid intercharacter position
				if (UTF8IsTrailByte(bch)) {
					// If ch is a trail byte in a valid UTF-8 character then return start of character
					int nStartUTF = nPos;
					int nEndUTF = nPos;
					if (IsInGoodUTF8(nPos, nStartUTF, nEndUTF))
						nPos = nStartUTF;
					// Else invalid UTF-8 so return position of isolated trail byte
				}
			}
		} else {
			if (nMoveDir > 0 ) {
				int nMbSize = IsDBCSLeadByte(m_cb.CharAt(nPos)) ? 2 : 1;
				nPos += nMbSize;
				if (nPos > Length())
					nPos = Length();
			} else {
				// Anchor DBCS calculations at start of line because start of line can
				// not be a DBCS trail byte.
				int nPosStartLine = LineStart(LineFromPosition(nPos));
				// See http://msdn.microsoft.com/en-us/library/cc194792%28v=MSDN.10%29.aspx
				// http://msdn.microsoft.com/en-us/library/cc194790.aspx
				if ((nPos - 1) < nPosStartLine)
					return nPosStartLine - 1;
				else if (IsDBCSLeadByte(m_cb.CharAt(nPos - 1))) {
					// Must actually be trail byte
					return nPos - 2;
				} else {
					// Otherwise, step back until a non-lead-byte is found.
					int nPosTmp = nPos - 1;
					while(nPosStartLine <= --nPosTmp && IsDBCSLeadByte((m_cb.CharAt(nPosTmp))))
						;
					// Now posTemp+1 must point to the beginning of a character,
					// so figure out whether we went back an even or an odd
					// number of bytes and go back 1 or 2 bytes, respectively.
					return (nPosTmp - 1 - ((nPos - nPosTmp) & 1));
				}
			}
		}
	} else
		nPos += nIncrement;

	return nPos;
}

BOOL Document::NextCharacter(int &nPos, int nMoveDir)
{
	// Returns true if pos changed
	int nPosNext = NextPosition(nPos, nMoveDir);
	if (nPosNext == nPos)
		return FALSE;

	nPos = nPosNext;
	return TRUE;
}

int Document::CodePage() const
{
	return m_nDBCSCodePage;
}

BOOL Document::IsDBCSLeadByte(CHAR chByte) const
{
	// Byte ranges found in Wikipedia articles with relevant search strings in each case
	BYTE bch = static_cast<BYTE>(chByte);
	switch (m_nDBCSCodePage)
	{
		case 932:	// Shift_jis
			// Lead bytes F0 to FC may be a Microsoft addition. 
			return ((bch >= 0x81) && (bch <= 0x9F)) || ((bch >= 0xE0) && (bch <= 0xFC));
		case 936:	//GBK
			return (bch >= 0x81) && (bch <= 0xFE);
		case 949:	// Korean Wansung KS C-5601-1987
			return (bch >= 0x81) && (bch <= 0xFE);
		case 950:	//Big5
			return (bch >= 0x81) && (bch <= 0xFE);
		case 1361:
			return ((bch >= 0x84) && (bch <= 0xD3)) || ((bch >= 0xD8) && (bch <= 0xDD)) || ((bch >= 0xE0) && (bch <= 0xF9));
	}

	return FALSE;
}

// Need to break text into segments near lengthSegment but taking into
// account the encoding to not break inside a UTF-8 or DBCS character
// and also trying to avoid breaking inside a pair of combining characters.
// The segment length must always be long enough (more than 4 bytes)
// so that there will be at least one whole character to make a segment.
// For UTF-8, text must consist only of valid whole characters.
// In preference order from best to worst:
//   1) Break after space
//   2) Break before punctuation
//   3) Break after whole character
int Document::SafeSegment(const CHAR* chData, int nLen, int nLenSegment)
{
	if (nLen <= nLenSegment)
		return nLen;

	int nLastSpaceBreak = -1;
	int nLastPunctuationBreak = -1;
	int nLastEncodingAllowedBreak = -1;
	for (int n = 0; n< nLenSegment;) {
		BYTE bch = static_cast<BYTE>(chData[n]);
		if (n > 0) {
			if (IsSpaceOrTab(chData[n - 1]) && IsSpaceOrTab(chData[n]) == FALSE)
				nLastSpaceBreak = n;
			if (bch < 'A')
				nLastPunctuationBreak = n;
		}

		nLastEncodingAllowedBreak = n;
		if (m_nDBCSCodePage == SC_CP_UTF8)
			n += UTF8BytesOfLead[bch];
		else if (m_nDBCSCodePage)
			n += IsDBCSLeadByte(bch) ? 2 : 1;
		else
			++n;
	}
	if (nLastSpaceBreak >= 0)
		return nLastSpaceBreak;
	else if (nLastPunctuationBreak >= 0)
		return nLastPunctuationBreak;

	return nLastEncodingAllowedBreak;
}

void Document::ModifiedAt(int nPos)
{
	if (m_nEndStyled > nPos)
		m_nEndStyled = nPos;
}

void Document::CheckReadOnly()
{
	if (m_cb.IsReadOnly() && m_nEnteredReadOnlyCount == 0) { 
		++m_nEnteredReadOnlyCount;
		NotifyModifyAttempt();
		--m_nEnteredReadOnlyCount;
	}
}

// Document only modified by gateways DeleteChars, InsertString, Undo, Redo, and SetStyleAt.
// SetStyleAt does not change the persistent state of a document
BOOL Document::DeleteChars(int nPos, int nLen)
{
	if (nLen <= 0)
		return FALSE;
	if ((nPos + nLen) > Length())
		return FALSE;

	CheckReadOnly();
	if (m_nEnteredReadOnlyCount != 0)
		return FALSE;
	else {
		++m_nEnteredReadOnlyCount;
		if (m_cb.IsReadOnly() == FALSE) {
			DocModification DocMh(SC_MOD_BEFOREDELETE | SC_PERFORMED_USER, nPos, nLen, 0, 0);

			NotifyModified(DocMh);
			int nPrevLineTotal = LinesTotal();
			BOOL bStartSavePoint = m_cb.IsSavePoint();
			BOOL bStartSequence = FALSE;
			const CHAR* chText = m_cb.DeleteChars(nPos, nLen, bStartSequence);
			if (bStartSavePoint && m_cb.IsCollectingUndo())
				NotifySavePoint(bStartSavePoint == FALSE);
			if ((nPos < Length()) || nPos == 0)
				ModifiedAt(nPos);
			else
				ModifiedAt(nPos - 1);
			DocModification DocMn(SC_MOD_DELETETEXT | SC_PERFORMED_USER | (bStartSequence ? SC_STARTACTION : 0), nPos, nLen, LinesTotal() - nPrevLineTotal, chText);
			NotifyModified(DocMn);
		}
		--m_nEnteredModification;
	}

	return m_cb.IsReadOnly() == FALSE;
}

/**
 * Insert a string with a length.
 */

BOOL Document::InsertString(int nPos, const CHAR* chText, int nLen )
{
	if (nLen <= 0)
		return FALSE;
	CheckReadOnly();
	if (m_nEnteredModification != 0)
		return FALSE;
	else {
		++m_nEnteredModification;
		if (m_cb.IsReadOnly() == FALSE) {
			DocModification DocMh(SC_MOD_BEFOREINSERT | SC_PERFORMED_USER, nPos, nLen, 0, chText);
			NotifyModified(DocMh);

			int nPrevLineTotal = LinesTotal();
			BOOL bStartSequence = FALSE;
			const CHAR* chData = m_cb.InsertString(nPos, chText, nLen, bStartSequence);
			if (bStartSequence && m_cb.IsCollectingUndo())
				NotifySavePoint(bStartSequence == FALSE);
			ModifiedAt(nPos);
			DocModification DocMn(SC_MOD_INSERTTEXT | SC_PERFORMED_USER | (bStartSequence ? SC_STARTACTION : 0), nPos, nLen, LinesTotal() - nPrevLineTotal, chData);
			NotifyModified(DocMn);
		}

		--m_nEnteredModification;
	}

	return m_cb.IsReadOnly() == FALSE;
}

int Document::AddData(char *chData, int nLen)
{
	int nPos = Length();
	InsertString(nPos, chData, nLen);

	return 0;
}

void* Document::ConvertToDocument()
{
	return this;
}

BOOL Document::CanUndo()
{
	return m_cb.CanUndo();
}

BOOL Document::CanRedo()
{
	return m_cb.CanRedo();
}

void Document::DeleteUndoHistory()
{
	return m_cb.DeleteUndoHistory();
}

BOOL Document::SetUndoCollection(BOOL bUndoCollecting /*= TRUE*/)
{
	return m_cb.SetUndoCollection(bUndoCollecting);
}

BOOL Document::IsCollectingUndo() const
{
	return m_cb.IsCollectingUndo();
}

void Document::BeginUndoAction()
{
	m_cb.BeginUndoAction();
}

void Document::EndUndoAction()
{
	m_cb.EndUndoAction();
}

void Document::AddUndoAction(int nToken, BOOL bMayCoalesce)
{
	m_cb.AddUndoAction(nToken, bMayCoalesce);
}

int Document::Undo()
{
	int nNewPos = -1;
	CheckReadOnly();
	if (m_nEnteredModification == 0) {
		++m_nEnteredModification;
		if (m_cb.IsCollectingUndo() == FALSE) {
			BOOL bStartSavePoint = m_cb.IsSavePoint();
			BOOL bMultiLine = FALSE;
			int nSteps = m_cb.StartUndo();
			int nCoalescedRemovePos = -1;
			int nCoalescedRemoveLen = 0;
			int nPrevRemoveActionPos = -1;
			int nPrevRemoveActionLen = 0;
			for (int n=0; n<nSteps; ++n) {
				const int nPrevLinesTotal = LinesTotal();
				const Action& Act = m_cb.GetUndoStep();
				if (Act.GetActionType() == removeAction){
					NotifyModified(DocModification(SC_MOD_BEFOREINSERT | SC_PERFORMED_UNDO, Act));
				} else if (Act.GetActionType() == containerAction) {
					DocModification DocMn(SC_MOD_CONTAINER | SC_PERFORMED_UNDO);
					DocMn.SetToken(Act.GetPos());
					NotifyModified(DocMn);
					if (Act.IsMayCoalesce() == FALSE) {
						nCoalescedRemovePos = -1;
						nCoalescedRemoveLen = 0;
						nPrevRemoveActionPos = -1;
						nPrevRemoveActionLen = 0;
					}
				} else {
					NotifyModified(DocModification(SC_MOD_BEFOREDELETE | SC_PERFORMED_UNDO, Act));
				}

				m_cb.PerformUndoStep();
				if (Act.GetActionType() != containerAction) {
					ModifiedAt(Act.GetPos());
					nNewPos = Act.GetPos();
				}

				int nModFlags = SC_PERFORMED_UNDO;
				// With undo, an insertion action becomes a deletion notification
				if (Act.GetActionType() == removeAction) {
					nNewPos += Act.GetDataLength();
					nModFlags |= SC_MOD_INSERTTEXT;
					if ((nCoalescedRemoveLen > 0) && 
						(Act.GetPos() == nPrevRemoveActionPos || 
						Act.GetPos() == (nPrevRemoveActionPos + nPrevRemoveActionLen))) 
					{
						nCoalescedRemoveLen += Act.GetDataLength();
						nNewPos = nCoalescedRemovePos + nCoalescedRemoveLen;
					} else {
						nCoalescedRemovePos = Act.GetPos();
						nCoalescedRemoveLen = Act.GetDataLength();
					}

					nPrevRemoveActionPos = Act.GetPos();
					nPrevRemoveActionLen = Act.GetDataLength();
				} else if (Act.GetActionType() == insertAction) {
					nModFlags |= SC_MOD_DELETETEXT;
					nCoalescedRemovePos = -1;
					nCoalescedRemoveLen = 0;
					nPrevRemoveActionPos = -1;
					nPrevRemoveActionLen = 0;
				}

				if (nSteps > 1)
					nModFlags |= SC_MULTISTEPUNDOREDO;

				const int nLinesAdded = LinesTotal() - nPrevLinesTotal;
				if (nLinesAdded != 0)
					bMultiLine = TRUE;
				if (n == nSteps - 1){
					nModFlags |= SC_LASTSTEPINUNDOREDO;
					if (bMultiLine)
						nModFlags |= SC_MULTILINEUNDOREDO;
				}

				DocModification DocMn(nModFlags, Act.GetPos(), Act.GetDataLength(), nLinesAdded, Act.GetData());
				NotifyModified(DocMn);
			}

			BOOL bEndSavePoint = m_cb.IsSavePoint();
			if (bStartSavePoint != bEndSavePoint)
				NotifySavePoint(bEndSavePoint);
		}
		--m_nEnteredModification;
	}

	return nNewPos;
}

int Document::Redo()
{
	int nNewPos = -1;
	CheckReadOnly();
	if (m_nEnteredModification == 0) {
		++m_nEnteredModification;
		if (m_cb.IsReadOnly() == FALSE) {
			BOOL bStartSavePoint = m_cb.IsSavePoint();
			BOOL bMultiLine = FALSE;
			int nSteps = m_cb.StartRedo();
			for (int n=0; n<nSteps; ++n) {
				const int nPrevLinesTotal = LinesTotal();
				const Action& Act = m_cb.GetRedoStep();
				if (Act.GetActionType() == insertAction) {
					DocModification DocMn(SC_MOD_BEFOREINSERT | SC_PERFORMED_REDO, Act);
					NotifyModified(DocMn);
				} else if (Act.GetActionType() == containerAction) {
					DocModification DocMn(SC_MOD_CONTAINER | SC_PERFORMED_REDO);
					DocMn.SetToken(Act.GetPos());
					NotifyModified(DocMn);
				} else {
					NotifyModified(DocModification(SC_MOD_BEFOREDELETE | SC_PERFORMED_REDO, Act));
				}

				m_cb.PerformRedoStep();
				if (Act.GetActionType() != containerAction) {
					ModifiedAt(Act.GetPos());
					nNewPos = Act.GetPos();
				}

				int nModFlags = SC_PERFORMED_REDO;
				if (Act.GetActionType() == insertAction) {
					nNewPos += Act.GetDataLength();
					nModFlags |= SC_MOD_BEFOREINSERT;
				} else if (Act.GetActionType() == removeAction) {
					nModFlags |= SC_MOD_BEFOREDELETE;
				}
				if (nSteps > 1)
					nModFlags |= SC_MULTISTEPUNDOREDO;

				const int nLinesAdded = LinesTotal() - nPrevLinesTotal;
				if (nLinesAdded != 0)
					bMultiLine = TRUE;
				if (n == nSteps - 1) {
					nModFlags |= SC_LASTSTEPINUNDOREDO;
					if (bMultiLine)
						nModFlags |= SC_MULTILINEUNDOREDO;
				}
				NotifyModified(
							DocModification(nModFlags, Act.GetPos(), Act.GetDataLength(), nLinesAdded, Act.GetData()));
			}

			BOOL bEndSavePoint = m_cb.IsSavePoint();
			if (bStartSavePoint != bEndSavePoint)
				NotifySavePoint(bEndSavePoint);
		}
		--m_nEnteredModification;
	}

	return nNewPos;
}

/**
 * Insert a single character.
 */
BOOL Document::InsertChar(int nPos ,CHAR chData)
{
	CHAR chs[1];
	chs[0] = chData;
	return InsertString(nPos, chs, _countof(chs));
}

/**
 * Insert a null terminated string.
 */
BOOL Document::InsertCString(int nPos, const CHAR *chData)
{
	return InsertString(nPos, chData, static_cast<int>(chData ? strlen(chData) : 0));
}

void Document::ChangeChar(int nPos, CHAR chData)
{
	DeleteChars(nPos, 1);
	InsertChar(nPos, chData);
}

void Document::DeleteChar(int nPos)
{
	DeleteChars(nPos, LenChar(nPos));
}

void Document::DeleteCharBack(int nPos)
{
	if (nPos <= 0)
		return;
	else if (IsCrLf(nPos - 2))
		DeleteChars(nPos - 2, 2);
	else if (m_nDBCSCodePage) {
		int nStartChar = NextPosition(nPos, -1);
		DeleteChars(nStartChar, nPos - nStartChar);
	} else
		DeleteChars(nPos - 1, 1);
}

CHAR Document::CharAt(int nPos) const
{
	return m_cb.CharAt(nPos);
}

void Document::GetCharRange(CHAR* pBuffer, int nPos, int nLen) const
{
	m_cb.GetCharRange(pBuffer, nPos, nLen);
}

CHAR Document::StyleAt(int nPos) const
{
	return m_cb.StyleAt(nPos);
}

void Document::GetStyleRange(BYTE* pBuffer, int nPos, int nLen) const
{
	m_cb.GetStyleRange(pBuffer, nPos, nLen);
}

static int NextTab(int pos, int tabSize) {
	return ((pos / tabSize) + 1) * tabSize;
}

static std::string CreateIndentation(int indent, int tabSize, bool insertSpaces) {
	std::string indentation;
	if (!insertSpaces) {
		while (indent >= tabSize) {
			indentation += '\t';
			indent -= tabSize;
		}
	}
	while (indent > 0) {
		indentation += ' ';
		indent--;
	}
	return indentation;
}

int Document::GetLineIndentation(int nLine) const
{
	int nIndent = 0;
	if ((nLine >= 0) && (nLine < LinesTotal())) {
		int nLineStart = LineStart(nLine);
		int nLen = Length();
		for (int n=nLineStart; n<nLen; ++n) {
			CHAR ch = m_cb.CharAt(n);
			if (ch == ' ')
				++nIndent;
			else if (ch == '\t')
				nIndent = NextTab(nIndent, m_nTabInChars);
			else
				return nIndent;
		}
	}

	return nIndent;
}

void Document::SetLineIndentation(int nLine, int nIndent)
{
	int nIndentOfLine = GetLineIndentation(nLine);
	if (nIndent < 0)
		nIndent = 0;
	if (nIndent != nIndentOfLine) {
		std::string strLineBuf = CreateIndentation(nIndent, m_nTabInChars, !m_bUseTabs);
		int nThisLineStart = LineStart(nLine);
		int nIndentPos = GetLineIndentPosition(nLine);
		UndoGroup ug(this);
		DeleteChars(nThisLineStart, nIndentPos - nThisLineStart);
		InsertCString(nThisLineStart, strLineBuf.c_str());
	}
}

int Document::GetLineIndentPosition(int nLine) const
{
	if (nLine < 0)
		return 0;
	int nPos = LineStart(nLine);
	int nLen = Length();
	while ((nPos < nLen) && IsSpaceOrTab(m_cb.CharAt(nPos)))
		++nPos;
	return nPos;
}

int Document::GetColumn(int nPos)
{
	int nColumn = 0;
	int nLine = LineFromPosition(nPos);
	if ((nLine >= 0) && (nLine < LinesTotal())) {
		for (int n=LineStart(nLine); n<nPos;) {
			CHAR ch = m_cb.CharAt(n);
			if (ch == '\t') {
				nColumn = NextTab(nColumn, m_nTabInChars);
				++n;
			} else if (ch == '\r')
				return nColumn;
			else if (ch == '\n')
				return nColumn;
			else {
				++nColumn;
				n = NextPosition(n, 1);
			}
		}
	}

	return nColumn;
}

int Document::CountCharacters(int nStartPos, int nEndPos)
{
	nStartPos = MovePositionOutsideChar(nStartPos, 1, FALSE);
	nEndPos = MovePositionOutsideChar(nEndPos, -1, FALSE);

	int nCount = 0;
	int n = nStartPos;
	while (n < nEndPos) {
		++nCount;
		if (IsCrLf(n))
			++n;
		n = NextPosition(n, 1);
	}

	return nCount;
}

int Document::FindColumn(int nLine, int nColumn)
{
	int nPos = LineStart(nLine);
	if ((nLine >= 0) && (nLine < LinesTotal())) {
		int nColumnCurrent = 0;
		while ((nColumnCurrent < nColumn) && nPos < Length()) {
			CHAR ch = m_cb.CharAt(nPos);
			if (ch == '\t') {
				nColumnCurrent = NextTab(nColumnCurrent, m_nTabInChars);
				if (nColumnCurrent > nColumn)
					return nPos;
				++nPos;
			} else if (ch == '\r')
				return nPos;
			else if (ch == '\n')
				return nPos;
			else {
				++nColumnCurrent;
				nPos = NextPosition(nPos, 1);
			}
		}
	}

	return nPos;
}

void Document::Indent(BOOL bForward, int nLineBottom, int nLineTop)
{
	// Dedent - suck white space off the front of the line to dedent by equivalent of a tab
	for (int nLine = nLineBottom; nLine >= nLineTop; --nLine) {
		int nIndentOfLine = GetLineIndentation(nLine);
		if (bForward) {
			if (LineStart(nLine) < LineEnd(nLine))
				SetLineIndentation(nLine, nIndentOfLine + GetIndentSize());
		} else
			SetLineIndentation(nLine, nIndentOfLine - GetIndentSize());
	}
}

// Convert line endings for a piece of text to a particular mode.
// Stop at len or when a NUL is found.
// Caller must delete the returned pointer.
CHAR* Document::TransformLineEnds(int* pLenOut, const CHAR* chText, size_t nLen, int nEolModWanted)
{
	CHAR* pDest = new CHAR[2*nLen + 1];
	const CHAR* sPtr = chText;
	CHAR* dPtr = pDest;
	for (size_t n = 0; (n < nLen) && (*sPtr != '\0'); ++n) {
		if (*sPtr == '\n' || *sPtr == '\r') {
			if (nEolModWanted == SC_EOL_CR)
				*dPtr++ = '\r';
			else if (nEolModWanted == SC_EOL_LF)
				*dPtr++ = '\n';
			else { // eolModeWanted == SC_EOL_CRLF
				*dPtr++ = '\r';
				*dPtr++ = '\n';
			}

			if ((*sPtr == '\r') && (n + 1 < nLen) && (*(sPtr + 1) == '\n')) {
				n++;
				sPtr++;
			}

			sPtr++;
		} else
			*dPtr++ = *sPtr++;
	}
	*dPtr++ = '\0';
	*pLenOut = (dPtr - pDest) - 1;
	return pDest;
}

void Document::ConvertLineEnds(int nEolModeSet)
{
	UndoGroup ug(this);

	for (int nPos = 0; nPos < Length(); ++nPos) {
		if (m_cb.CharAt(nPos == '\r')) {
			if (m_cb.CharAt(nPos + 1) == '\n') {
				// CRLF
				if (nEolModeSet == SC_EOL_CR)
					DeleteChars(nPos + 1, 1);	// Delete the LF
				else if (nEolModeSet == SC_EOL_LF)
					DeleteChars(nPos, 1);	//Delete ths CR
				else
					++nPos;
			} else {
				// CR
				if (nEolModeSet == SC_EOL_CRLF) {
					InsertString(nPos + 1, "\n", 1);	//Insert LF
					nPos++;
				}else if (nEolModeSet == SC_EOL_LF) {
					InsertString(nPos, "\n", 1);	// Insert LF
					DeleteChars(nPos + 1, 1);	// Delete CR
				}
			}
		} else if (m_cb.CharAt(nPos) == '\n') {
			// LF
			if (nEolModeSet == SC_EOL_CRLF) {
				InsertString(nPos, "\r", 1);	// Insert CR;
				++nPos;
			} else if (nEolModeSet == SC_EOL_CR) {
				InsertString(nPos, "\r", 1);	// Insert CR
				DeleteChars(nPos + 1, 1);	// Delete LF
			}
		}
	}
}

BOOL Document::IsWhiteLine(int nLine) const
{
	int nCurrentChar = LineStart(nLine);
	int nEndLine = LineEnd(nLine);
	while (nCurrentChar < nEndLine) {
		if (m_cb.CharAt(nCurrentChar) != ' ' && m_cb.CharAt(nCurrentChar) != '\t')
			return FALSE;
		++nCurrentChar;
	}

	return TRUE;
}

int Document::ParaUp(int nPos)
{
	int nLine = LineFromPosition(nPos);
	--nLine;
	while (nLine >= 0 && IsWhiteLine(nLine))	// skip empty lines
		--nLine;

	while(nLine >= 0 && !IsWhiteLine(nLine))	// skip non-empty lines
		--nLine;

	++nLine;
	return LineStart(nLine);
}

int Document::ParaDown(int nPos)
{
	int nLine = LineFromPosition(nPos);
	while (nLine < LinesTotal() && !IsWhiteLine(nLine))		// skip non-empty lines
		++nLine;
	while (nLine < LinesTotal() && IsWhiteLine(nLine))		// skip empty lines
		++nLine;
	if (nLine < LinesTotal())
		return LineStart(nLine);
	else
		return LineEnd(nLine - 1);
}

int Document::GetIndentSize() const
{
	return m_nActualIndentInChars;
}

CHAR_TYPE Document::GetWordCharClass(BYTE chData) const
{
	if ((SC_CP_UTF8 == m_nTabInChars) && (!UTF8IsAscii(chData)))
		return CHAR_TYPE_WORD;
	return m_CharClass.GetClass(chData);
}

/**
 * Used by commmands that want to select whole words.
 * Finds the start of word at pos when delta < 0 or the end of the word when delta >= 0.
 */
int Document::ExtendWordSelect(int nPos, int nDelta, BOOL bOnlyWordCharacter /* = FALSE */)
{
	CHAR_TYPE ctStart = CHAR_TYPE_WORD;
	if (nDelta < 0) {
		if (bOnlyWordCharacter == FALSE)
			ctStart = GetWordCharClass(m_cb.CharAt(nPos - 1));
		while (nPos > 0 && (GetWordCharClass(m_cb.CharAt(nPos - 1)) == ctStart))
			--nPos;
	} else {
		if (bOnlyWordCharacter == FALSE && nPos < Length())
			ctStart = GetWordCharClass(m_cb.CharAt(nPos));
		while(nPos < Length() && GetWordCharClass(m_cb.CharAt(nPos)) == ctStart)
			++nPos;
	}

	return MovePositionOutsideChar(nPos, nDelta, TRUE);
}

/**
 * Find the start of the next word in either a forward (delta >= 0) or backwards direction
 * (delta < 0).
 * This is looking for a transition between character classes although there is also some
 * additional movement to transit white space.
 * Used by cursor movement by word commands.
 */
int Document::NextWordStart(int nPos, int nDelta)
{
	if (nDelta < 0) {
		while (nPos > 0 && (GetWordCharClass(m_cb.CharAt(nPos -1)) == CHAR_TYPE_SPACE))
			--nPos;
		if (nPos > 0) {
			CHAR_TYPE ctStart = GetWordCharClass(m_cb.CharAt(nPos - 1));
			while (nPos > 0 && (GetWordCharClass(m_cb.CharAt(nPos - 1)) == ctStart))
				--nPos;
		} else {
			CHAR_TYPE ctStart = GetWordCharClass(m_cb.CharAt(nPos));
			while (nPos < Length() && (GetWordCharClass(m_cb.CharAt(nPos)) == ctStart))
				++nPos;
			while (nPos < Length() && (GetWordCharClass(m_cb.CharAt(nPos)) == CHAR_TYPE_SPACE))
				++nPos;
		}
	}

	return nPos;
}

/**
 * Find the end of the next word in either a forward (delta >= 0) or backwards direction
 * (delta < 0).
 * This is looking for a transition between character classes although there is also some
 * additional movement to transit white space.
 * Used by cursor movement by word commands.
 */
int Document::NextWordEnd(int nPos, int nDelta)
{
	if (nDelta < 0) {
		if (nPos > 0) {
			CHAR_TYPE ctStart = GetWordCharClass(m_cb.CharAt(nPos - 1));
			if (ctStart != CHAR_TYPE_SPACE) {
				while (nPos > 0 && GetWordCharClass(m_cb.CharAt(nPos - 1)) == ctStart)
					--nPos;
			}
			while (nPos > 0 && GetWordCharClass(m_cb.CharAt(nPos - 1)) == CHAR_TYPE_SPACE)
				--nPos;
		}
	} else {
		while (nPos < Length() && GetWordCharClass(m_cb.CharAt(nPos)) == CHAR_TYPE_SPACE)
			++nPos;
		if (nPos < Length()) {
			CHAR_TYPE ctStart = GetWordCharClass(m_cb.CharAt(nPos));
			while (nPos < Length() && GetWordCharClass(m_cb.CharAt(nPos)) == ctStart)
				++nPos;
		}
	}

	return nPos;
}

int Document::GetLength() const
{
	return m_cb.GetLength();
}

/**
 * Check that the character at the given position is a word or punctuation character and that
 * the previous character is of a different character class.
 */
BOOL Document::IsWordStartAt(int nPos) const
{
	if (nPos > 0){
		CHAR_TYPE ctPos = GetWordCharClass(CharAt(nPos));
		return (ctPos == CHAR_TYPE_WORD || ctPos == CHAR_TYPE_PUNCTUATION) && (ctPos != GetWordCharClass(CharAt(nPos - 1)));
	}

	return TRUE;
}

/**
 * Check that the character at the given position is a word or punctuation character and that
 * the next character is of a different character class.
 */
BOOL Document::IsWordEndAt(int nPos) const
{
	if (nPos < Length()) {
		CHAR_TYPE ctPrev = GetWordCharClass(CharAt(nPos - 1));
		return (ctPrev == CHAR_TYPE_WORD || ctPrev == CHAR_TYPE_SPACE) && (ctPrev != GetWordCharClass(CharAt(nPos)));
	}

	return TRUE;
}

/**
 * Check that the given range is has transitions between character classes at both
 * ends and where the characters on the inside are word or punctuation characters.
 */
BOOL Document::IsWordAt(int nStart, int nEnd) const
{
	return IsWordStartAt(nStart) && IsWordEndAt(nEnd);
}

BOOL Document::IsMatchesWordOptions(BOOL bWord, BOOL bWordStart, int nPos, int nLen) const
{
	return (!bWord && !bWordStart) ||
				(bWord && IsWordAt(nPos, nPos + nLen)) ||
				(bWordStart && IsWordStartAt(nPos));
}

BOOL Document::HasCaseFolder() const
{
	return m_pcf != NULL;
}

void Document::SetCaseFolder(CaseFolder *pcf)
{
	if (m_pcf != NULL)
		delete m_pcf;
	m_pcf = pcf;
}

/**
 * Find text in document, supporting both forward and backward
 * searches (just pass minPos > maxPos to do a backward search)
 * Has not been tested with backwards DBCS searches yet.
 */
LONG Document::FindText(int nMinPos, int nMaxPos, const char *chSearch, BOOL bCaseSensitive, BOOL bWord, BOOL bWordStart, BOOL bRegExp, int nFlags, int *nLen)
{
	if (*nLen <= 0)
		return nMinPos;
	if (bRegExp) {
		if (m_pRegex == NULL)
			m_pRegex = CreateRegexSearch(&m_CharClass);
		return m_pRegex->FindText(this, nMinPos, nMaxPos, chSearch, bCaseSensitive, bWord, bWordStart, nFlags, nLen);
	} else {
		const BOOL bForward = nMinPos <= nMaxPos;
		const int nIncrement = bForward ? 1 : -1;

		// Range endpoints should not be inside DBCS characters, but just in case, move them.
		const int nStartPos = MovePositionOutsideChar(nMinPos, nIncrement, FALSE);
		const int nEndPos = MovePositionOutsideChar(nMaxPos, nIncrement, FALSE);

		// Compute actual search ranges needed
		const int nLenFind = *nLen;

		const int nLimitPos = MAX(nStartPos, nEndPos);
		int nPos = nStartPos;

		// back all of a character
		if (bForward == FALSE)
			nPos = NextPosition(nPos, nIncrement);

		if (bCaseSensitive) {
			const int nEndSearch = (nStartPos <= nEndPos) ? nEndPos  - nLenFind + 1 : nEndPos;
			const CHAR chStartSearch = chSearch[0];
			while (bForward ? (nPos < nEndSearch) : (nPos >= nEndSearch)) {
				if (CharAt(nPos) == chStartSearch) {
					BOOL bFound = (nPos + nLenFind) <= nLimitPos;
					for (int nIndexSearch = 1; (nIndexSearch < nLenFind) && bFound; ++nIndexSearch) 
						bFound = CharAt(nPos + nIndexSearch) == chSearch[nIndexSearch];

					if (bFound && IsMatchesWordOptions(bWord, bWordStart, nPos, nLenFind))
						return nPos;
				}
				if (NextCharacter(nPos, nIncrement) == FALSE)
					break;
			}
		} else if (SC_CP_UTF8 == m_nDBCSCodePage) {
			const size_t nMaxFoldingExpansion = 4;
			std::vector<CHAR> chSearchThing(nLenFind * UTF8MaxBytes * nMaxFoldingExpansion + 1);
			const int nLenSearch = static_cast<int>(
				m_pcf->Fold(&chSearchThing[0], chSearchThing.size(), chSearch, nLenFind));
			CHAR chBytes[UTF8MaxBytes + 1];
			CHAR chFolded[UTF8MaxBytes * nMaxFoldingExpansion + 1];
			while (bForward ? (nPos < nEndPos) : (nPos >= nEndPos)) {
				int nWidthFirstCharacter = 0;
				int nPosIndexDocument = nPos;
				int nIndexSearch = 0;
				BOOL bCharacterMatches = TRUE;

				while(TRUE) {
					const BYTE bLeadByte = static_cast<BYTE>(m_cb.CharAt(nPosIndexDocument));
					chBytes[0] = bLeadByte;
					int nWidthChar = 1;
					if (!UTF8IsAscii(bLeadByte)) {
						const int nCharBytesWidth = UTF8BytesOfLead[bLeadByte];
						for (int n=1; n<nCharBytesWidth; ++n)
							chBytes[n] = m_cb.CharAt(nPosIndexDocument + n);
						nWidthChar = UTF8Classify(reinterpret_cast<const BYTE*>(chBytes), nCharBytesWidth) & UTF8MaskWidth;
					}

					if (nWidthFirstCharacter == 0)
						nWidthFirstCharacter = nWidthChar;
					if ((nPosIndexDocument + nWidthChar) > nLimitPos)
						break;
					
					const int nLenFlat = static_cast<int>(m_pcf->Fold(chFolded, _countof(chFolded), chBytes, nWidthChar));
					chFolded[nLenFlat] = 0;
					// Does folded match the buffer
					bCharacterMatches = 0 == memcmp(chFolded, &chSearchThing[0] + nIndexSearch, nLenFlat);

					if (bCharacterMatches == 0)
						break;
					nPosIndexDocument += nWidthChar;
					nIndexSearch += nLenFlat;
					if (nIndexSearch >= nLenSearch)
						break;
				}
				if (bCharacterMatches && (nIndexSearch == static_cast<int>(nLenSearch))) {
					if (IsMatchesWordOptions(bWord, bWordStart, nPos, nPosIndexDocument - nPos)) {
						*nLen = nPosIndexDocument - nPos;
						return nPos;
					}
				}
				if (bForward)
					nPos += nWidthFirstCharacter;
				else if (NextCharacter(nPos, nIncrement) == FALSE)
					break;
			}
		} else if (m_nDBCSCodePage) {
			const size_t nMaxByteCharacter = 2;
			const size_t nMaxFoldingExpasion = 4;
			std::vector<CHAR> chSearchThing(nLenFind * nMaxByteCharacter * nMaxFoldingExpasion + 1);
			const int nLenSearch = static_cast<int>(
				m_pcf->Fold(&chSearchThing[0], chSearchThing.size(), chSearch, nLenFind));

			while (bForward ? (nPos < nEndPos) : (nPos >= nEndPos)) {
				int nIndexDocument = 0;
				int nIndexSearch = 0;
				BOOL bCharacterMatches = TRUE;
				while (bCharacterMatches && ((nPos + nIndexDocument) < nLimitPos) && (nIndexSearch < nLenSearch)) {
					CHAR chBytes[nMaxByteCharacter + 1];
					chBytes[0] = m_cb.CharAt(nPos + nIndexDocument);
					const int nWidthChar = IsDBCSLeadByte(chBytes[0]) ? 2 : 1;

					if (nWidthChar == 2)
						chBytes[1] = m_cb.CharAt(nPos + nIndexDocument + 1);
					if ((nPos + nIndexDocument + nWidthChar) > nLimitPos)
						break;

					CHAR chFolded[nMaxByteCharacter * nMaxFoldingExpasion + 1];
					const int nLenFlat = static_cast<int>(m_pcf->Fold(chFolded, sizeof(chFolded), chBytes, nWidthChar));
					chFolded[nLenFlat] = 0;
					// Does folded match the buffer
					bCharacterMatches = 0 == memcmp(chFolded, &chSearchThing[0] + nIndexSearch, nLenFlat);
					nIndexDocument += nWidthChar;
					nIndexSearch += nLenFlat;
				}

				if (bCharacterMatches && (nIndexSearch == static_cast<int>(nLenSearch))) {
					if (IsMatchesWordOptions(bWord, bWordStart, nPos, nIndexDocument)) {
						*nLen = nIndexDocument;
						return nPos;
					}
				}
				if (NextCharacter(nPos, nIncrement) == FALSE)
					break;
			}
		} else {
			const int nEndSearch = (nStartPos <= nEndPos) ? nEndPos - nLenFind + 1 : nEndPos;
			std::vector<CHAR> chSearchThing(nLenFind + 1);
			m_pcf->Fold(&chSearchThing[0], chSearchThing.size(), chSearch, nLenFind);
			while (bForward ? (nPos < nEndSearch) : (nPos >= nEndSearch)) {
				BOOL bFound = (nPos + nLenFind) <= nLimitPos;
				for (int nIndexSearch = 0; (nIndexSearch < nLenFind) && bFound; ++nIndexSearch){
					CHAR ch = CharAt(nPos + nIndexSearch);
					CHAR chFolded[2];
					m_pcf->Fold(chFolded, sizeof(chFolded), &ch, 1);
					bFound = chFolded[0] == chSearchThing[nIndexSearch];
				}
				if (bFound && IsMatchesWordOptions(bWord, bWordStart, nPos, nLenFind))
					return nPos;
				if (NextCharacter(nPos, nIncrement) == NULL)
					break;
			}
		}
	}

	return -1;
}

const CHAR* Document::SubstituteByPosition(const CHAR* chData, int* nLen)
{
	if (m_pRegex)
		return m_pRegex->SubstituteByPosition(this, chData, nLen);
	return NULL;
}

int Document::LinesTotal() const
{
	return m_cb.Lines();
}

void Document::ChangeCase(Range r, BOOL bMakeUpperCase)
{
	for (int nPos = r.GetStart(); nPos < r.GetEnd();) {
		int nLen = LenChar(nPos);
		if (nLen == 1) {
			CHAR ch = CharAt(nPos);
			if (bMakeUpperCase) {
				if (IsLowerCase(ch))
					ChangeChar(nPos, static_cast<CHAR>(MakeUpperCase(ch)));
			} else {
				if (IsUpperCase(ch)) { 
					ChangeChar(nPos, static_cast<CHAR>(MakeLowerCase(ch)));
				}
			}
		}

		nPos += nLen;
	}
}

void Document::SetDefaultCharClasses(BOOL bIncludeWordClass)
{
	m_CharClass.SetDefaultCharClasses(bIncludeWordClass);
}

void Document::SetCharClasses(const BYTE* chData, CHAR_TYPE NewCharType)
{
	m_CharClass.SetCharClasses(chData, NewCharType);
}

int Document::GetCharOfClass(CHAR_TYPE chClass, BYTE* pBuffer) const
{
	return m_CharClass.GetCharsOfClass(chClass, pBuffer);
}

void Document::SetStylingBits(int nBits)
{
	m_nStylingBits = nBits;
	m_nStylingBitsMask = (1 << nBits) - 1;
}

void Document::StartStyling(int nPos, CHAR chMask)
{
	m_chStylingMask = chMask;
	m_nEndStyled = nPos;
}

BOOL Document::SetStyleFor(int nLen, CHAR chStyle)
{
	if (m_nEnteredStyling != 0)
		return FALSE;

	++m_nEnteredStyling;
	chStyle &= m_chStylingMask;
	int nPrevEndStyled = m_nEndStyled;
	if (m_cb.SetStyleFor(m_nEndStyled, nLen, chStyle, m_chStylingMask)) {
		DocModification DocMh(SC_MOD_CHANGESTYLE | SC_PERFORMED_USER, nPrevEndStyled, nLen);
		NotifyModified(DocMh);
	}

	m_nEndStyled += nLen;
	--m_nEnteredStyling;
	return TRUE;
}

BOOL Document::SetStyles(int nLen, const CHAR* chStyles)
{
	if (m_nEnteredStyling != 0)
		return FALSE;
	++m_nEnteredStyling;
	BOOL bChange = FALSE;
	int nStartMod = 0;
	int nEndMod = 0;
	for (int nPos = 0; nPos < nLen; ++nPos, ++m_nEndStyled) {
		if (m_cb.SetStyleAt(m_nEndStyled, chStyles[nPos], m_chStylingMask)) {
			if (bChange)
				nStartMod = nEndMod;
			bChange = TRUE;
			nEndMod = m_nEndStyled;
		}
	}

	if (bChange) {
		DocModification DocMh(SC_MOD_CHANGESTYLE | SC_PERFORMED_USER,
									nStartMod, nEndMod - nStartMod + 1);
		NotifyModified(DocMh);
	}
	--m_nEnteredStyling;

	return TRUE;
}

int Document::GetEndStyled() const
{
	return m_nEndStyled;
}

void Document::EnsureStyledTo(int nPos) 
{
	if ((m_nEnteredStyling == 0) && (nPos > GetEndStyled())) {
		IncrementStyleClock();
		if (m_pLexInterface && m_pLexInterface->UseContainerLexing() == FALSE) {
			int nLineEndStyled = LineFromPosition(GetEndStyled());
			int nEndStyledTo = LineStart(nLineEndStyled);
			m_pLexInterface->Colorise(nEndStyledTo, nPos);
		} else {
			// Ask the watchers to style, and stop as soon as one responds.
			for (int n=0; nPos > GetEndStyled() && n < m_nWatcherLen; ++n) 
				m_pWatchers[n].GetDocWatcher()->NotifyStyleNeeded(this, m_pWatchers[n].GetUserData(), nPos);
		}
	}
}

void Document::LexerChanged()
{
	// Tell the watchers the lexer has changed.
	for (int n=0; n<m_nWatcherLen; ++n)
		m_pWatchers[n].GetDocWatcher()->NotifyLexerChanged(this, m_pWatchers[n].GetUserData());
}

int Document::SetLineState(int nLine, int nState)
{
	int nStatePrevious = static_cast<LineState*>(m_pPerLineData[LINE_DATA_STATES])->SetLineState(nLine, nState);
	if (nState != nStatePrevious){
		DocModification mh(SC_MOD_CHANGELINESTATE, LineStart(nLine), 0, 0, 0, nLine);
		NotifyModified(mh);
	}

	return nStatePrevious;
}

int Document::GetLineState(int nLine) const
{
	return static_cast<LineState*>(m_pPerLineData[LINE_DATA_STATES])->GetLineState(nLine);
}

int Document::GetmaxLineState() const
{
	return static_cast<LineState*>(m_pPerLineData[LINE_DATA_STATES])->GetMaxLineState();
}

void Document::ChangeLexerState(int nStart, int nEnd)
{
	DocModification mh(SC_MOD_LEXERSTATE, nStart, nEnd - nStart, 0, 0, 0);
	NotifyModified(mh);
}

StyledText Document::MarginStyledText(int nLine)
{
	LineAnnotation* pLa  = static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION]);
	return StyledText(pLa->GetLength(nLine), pLa->GetText(nLine), 
			pLa->MultipleStyles(nLine), pLa->GetStyle(nLine), pLa->GetStyles(nLine));
}

void Document::MarginSetText(int nLine ,const CHAR* chText)
{
	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_MARGIN])->SetText(nLine, chText);
	DocModification mh(SC_MOD_CHANGEMARGIN, LineStart(nLine), 0, 0, 0, nLine);
	NotifyModified(mh);
}

void Document::MarginSetStyle(int nLine, int nStyle)
{
	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_MARGIN])->SetStyle(nLine, nStyle);
	NotifyModified(DocModification(SC_MOD_CHANGEMARGIN, LineStart(nLine),0,0,0,nLine));
}

void Document::MarginSetStyles(int nLine, const BYTE* chStyles)
{
	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_MARGIN])->SetStyles(nLine, chStyles);
	NotifyModified(DocModification(SC_MOD_CHANGEMARGIN, LineStart(nLine), 0, 0, 0, nLine));
}

int Document::MarginLength(int nLine) const
{
	return static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_MARGIN])->GetLength(nLine);
}


void Document::MarginClearAll()
{
	int nMaxEditorLine = LinesTotal();
	for (int n=0; n<nMaxEditorLine; ++n)
		MarginSetText(n, 0);
	// Free remaining data
	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_MARGIN])->ClearAll();
}

BOOL Document::AnnotationAny() const
{
	return	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->AnySet();
}

StyledText Document::AnnotationStyledText(int nLine)
{
	LineAnnotation* pLa = static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION]);
	return StyledText(pLa->GetLength(nLine), pLa->GetText(nLine),
		pLa->MultipleStyles(nLine), pLa->GetStyle(nLine), pLa->GetStyles(nLine));
}

void Document::AnnotationSetText(int nLine, const CHAR* chData)
{
	if (nLine >= 0 && nLine < LinesTotal()) {
		const int nLineBefore = AnnotationLines(nLine);
		
		static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->SetText(nLine, chData);
		const int nLinesAfter = AnnotationLines(nLine);
		DocModification mh(SC_MOD_CHANGEANNOTATION, LineStart(nLine),0, 0, 0, nLine);
		mh.SetAnnotationLinesAdd(nLinesAfter - nLineBefore);
		NotifyModified(mh);
	}
}

void Document::AnnotationSetStyle(int nLine, int nStyle)
{
	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->SetStyle(nLine, nStyle);
	DocModification mh(SC_MOD_CHANGEANNOTATION, LineStart(nLine), 0, 0, 0, nLine);
	NotifyModified(mh);
}

void Document::AnnotationSetStyles(int nLine, const BYTE* chStyle)
{
	if (nLine >= 0 && nLine <= LinesTotal())
		static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->SetStyles(nLine, chStyle);
}

int Document::AnnotationLength(int nLine) const
{
	return static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->GetLength(nLine);
}

int Document::AnnotationLines(int nLine) const
{
	return static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->GetLines(nLine);
}

void Document::AnnotationClearAll()
{
	int nMaxEditorLine = LinesTotal();
	for (int n=0; n<nMaxEditorLine; ++n)
		AnnotationSetText(n, 0);
	// Free remaining data
	static_cast<LineAnnotation*>(m_pPerLineData[LINE_DATA_ANNOTATION])->ClearAll();
}

void Document::IncrementStyleClock()
{
	m_nStyleClock = (m_nStyleClock + 1) % 0x100000;
}

void Document::DecorationSetCurrentIndicator(int nIndicator)
{
	m_Decorations.SetCurrentIndicator(nIndicator);
}

void Document::DecorationFillRange(int nPos, int nVal, int nLen)
{
	if (m_Decorations.FillRange(nPos, nVal, nLen)){
		DocModification mh(SC_MOD_CHANGEINDICATOR | SC_PERFORMED_USER
			, nPos, nLen);
		NotifyModified(mh);
	}
}

BOOL Document::AddWatcher(IDocWatcher* pWatcher, void* pUserData)
{
	for (int n=0; n<m_nWatcherLen; ++n) {
		if (m_pWatchers[n].GetDocWatcher() == pWatcher && m_pWatchers[n].GetUserData() == pUserData)
			return FALSE;
	}

	WatcherWithUserData* pwNew = new WatcherWithUserData[m_nWatcherLen + 1];
	for (int n=0; n<m_nWatcherLen; ++n)
		pwNew[n] = m_pWatchers[n];

	pwNew[m_nWatcherLen].SetDocWatcher(pWatcher);
	pwNew[m_nWatcherLen].SetUserData(pUserData);
	delete[] m_pWatchers;
	m_pWatchers = pwNew;
	++m_nWatcherLen;

	return TRUE;
}

BOOL Document::RemoveWatcher(IDocWatcher *pWatcher, void *pUserData)
{
	for (int n=0; n<m_nWatcherLen; ++n) {
		if ((m_pWatchers[n].GetDocWatcher() == pWatcher) && 
			m_pWatchers[n].GetUserData() == pUserData)
		{
			if (m_nWatcherLen == 1) {
				delete[] m_pWatchers;
				m_pWatchers = NULL;
				m_nWatcherLen = 0;
			} else {
				WatcherWithUserData* pwNew = new WatcherWithUserData[m_nWatcherLen];
				for (int m=0; m<m_nWatcherLen - 1; ++m)
					pwNew[m] = (m<n) ? m_pWatchers[m] : m_pWatchers[m+1];
				delete[] m_pWatchers;
				m_pWatchers = pwNew;
				--m_nWatcherLen;
			}

			return TRUE;
		}
	}

	return FALSE;
}

void Document::NotifyModifyAttempt()
{
	for (int n=0; n<m_nWatcherLen; ++n)
		m_pWatchers[n].GetDocWatcher()->NotifyModifyAttempt(this, m_pWatchers[n].GetUserData());
}

void Document::NotifySavePoint(BOOL bSavePoint /*= TRUE*/)
{
	for (int n=0; n<m_nWatcherLen; ++n)
		m_pWatchers[n].GetDocWatcher()->NotifySavePoint(this, m_pWatchers[n].GetUserData(), bSavePoint);
}

void Document::NotifyModified(DocModification &mh)
{
	if (mh.GetModificationType() & SC_MOD_INSERTTEXT)
		m_Decorations.InsertSpace(mh.GetPos(), mh.GetLength());
	else if (mh.GetModificationType() & SC_MOD_DELETETEXT)
		m_Decorations.DeleteRange(mh.GetPos(), mh.GetLength());

	for (int n=0; n<m_nWatcherLen; ++n)
		m_pWatchers[n].GetDocWatcher()->NotifyModified(this, mh, m_pWatchers[n].GetUserData());
}

BOOL Document::IsWordPartSeparator(CHAR chData) const
{
	return (GetWordCharClass(chData) == CHAR_TYPE_WORD) && IsPunctuation(chData);
}

int Document::WordPartLeft(int nPos)
{
	if (nPos > 0) {
		--nPos;
		CHAR chStartChar = m_cb.CharAt(nPos);
		if (IsWordPartSeparator(chStartChar)) {
			while(nPos >0 && IsWordPartSeparator(m_cb.CharAt(nPos)))
				--nPos;
		}
		if (nPos > 0) { 
			chStartChar = m_cb.CharAt(nPos);
			--nPos;
			if (IsLowerCase(chStartChar)) {
				while(nPos > 0 && IsLowerCase(m_cb.CharAt(nPos)))
					--nPos;
				if (!IsUpperCase(m_cb.CharAt(nPos)) && !IsLowerCase(m_cb.CharAt(nPos)))
					++nPos;
			} else if (IsUpperCase(chStartChar)) {
				while (nPos > 0 && IsUpperCase(m_cb.CharAt(nPos)))
					--nPos;
				if (!IsUpperCase(m_cb.CharAt(nPos)))
					++nPos;
			} else if (IsADigit(chStartChar)) {
				while(nPos > 0 && IsADigit(m_cb.CharAt(nPos)))
					--nPos;
				if (!IsADigit(m_cb.CharAt(nPos)))
					++nPos;
			} else if (IsPunctuation(chStartChar)) {
				while (nPos > 0 && IsPunctuation(m_cb.CharAt(nPos)))
					--nPos;
				if (!IsPunctuation(m_cb.CharAt(nPos)))
					++nPos;
			} else if (IsSpaceChar(chStartChar)) {
				while (nPos > 0 && IsSpaceChar(m_cb.CharAt(nPos)))
					--nPos;
				if (!IsSpaceChar(m_cb.CharAt(nPos)))
					++nPos;
			} else if (!isascii(chStartChar)) {
				while (nPos > 0 && isascii(m_cb.CharAt(nPos)))
					--nPos;
				if (!isascii(m_cb.CharAt(nPos)))
					++nPos;
			} else {
				++nPos;
			}
		}
	}

	return nPos;
}

int Document::WordPartRight(int nPos)
{
	CHAR chStartChar = m_cb.CharAt(nPos);
	int nLen = Length();
	if (IsWordPartSeparator(chStartChar)) {
		while(nPos >0 && IsWordPartSeparator(m_cb.CharAt(nPos)))
			++nPos;
		chStartChar = m_cb.CharAt(nPos);
	}
	
	if (!isascii(chStartChar)) {
		while (nPos < nLen && !isascii(m_cb.CharAt(nPos)))
			++nPos;
	} else if (IsLowerCase(chStartChar)) {
		while (nPos < nLen && IsLowerCase(m_cb.CharAt(nPos)))
			++nPos;
	} else if (IsUpperCase(chStartChar)) {
		if (IsLowerCase(m_cb.CharAt(nPos + 1))) {
			++nPos;
			while (nPos < nLen && IsLowerCase(m_cb.CharAt(nPos)))
				++nPos;
		} else {
			while (nPos < nLen && IsUpperCase(m_cb.CharAt(nPos)))
				++nPos;
		}
		if (IsLowerCase(m_cb.CharAt(nPos)) && IsUpperCase(m_cb.CharAt(nPos - 1)))
			--nPos;
	} else if (IsADigit(chStartChar)) {
		while (nPos < nLen && IsADigit(m_cb.CharAt(nPos)))
			++nPos;
	} else if (IsPunctuation(chStartChar)) {
		while (nPos < nLen && IsPunctuation(m_cb.CharAt(nPos)))
			++nPos;
	} else if (IsSpaceChar(chStartChar)) {
		while (nPos < nLen && IsSpaceChar(m_cb.CharAt(nPos)))
			++nPos;
	} else {
		++nPos;
	}

	return nPos;
}

BOOL Document::IsLineEndChar(CHAR chChar) const
{
	return (chChar == '\r') || (chChar == '\n');
}

int Document::ExtendStyleRange(int nPos, int nDelta, BOOL bSingleLine /*= FALSE*/)
{
	int nStart = m_cb.StyleAt(nPos)	;
	if (nDelta < 0) {
		while (nPos > 0 && (m_cb.StyleAt(nPos) == nStart) && (bSingleLine == FALSE || IsLineEndChar(m_cb.CharAt(nPos) == FALSE)))
			--nPos;
		++nPos;
	} else {
		while (nPos < Length() && (m_cb.StyleAt(nPos) == nStart) && (bSingleLine == FALSE || IsLineEndChar(m_cb.CharAt(nPos)) == FALSE))
			++nPos;
	}

	return nPos;
}

CHAR Document::BraceOpposite(CHAR chChar)
{
	switch (chChar)
	{
		case '(':
			return ')';
		case ')':
			return '(';
		case '[':
			return ']';
		case ']':
			return '[';
		case '{':
			return '}';
		case '}':
			return '{';
		case '<':
			return '>';
		case '>':
			return '<';
		default:
			return '\0';
	}
}

// TODO: should be able to extend styled region to find matching brace
int Document::BraceMatch(int nPos, int nMaxReStyle)
{
	CHAR chBrace = CharAt(nPos);
	CHAR chSeek = BraceOpposite(chBrace);
	if (chSeek == '\0')
		return -1;
	CHAR chStyleBrace = static_cast<CHAR>(StyleAt(nPos) & m_nStylingBitsMask);
	int nDirection = -1;
	if (chBrace == '(' || chBrace == '[' || chBrace == '{' || chBrace == '<')
		nDirection = 1;
	int nDepth = 1;
	nPos = NextPosition(nPos, nDirection);
	while ((nPos >= 0) && (nPos < Length())) {
		CHAR chAtPos = CharAt(nPos);
		CHAR chStyleAtPos = static_cast<CHAR>(StyleAt(nPos) & m_nStylingBitsMask);
		if ((nPos > GetEndStyled()) || (chStyleAtPos == chStyleBrace)) {
			if (chAtPos == chBrace)
				++nDepth;
			if (chAtPos == chSeek)
				--nDepth;
			if (nDepth == 0)
				return nPos;
		}

		int nPosBeforeMove = nPos;
		nPos = NextPosition(nPos, nDirection);
		if (nPos == nPosBeforeMove)
			break;
	}

	return -1;
}

BOOL Document::IsPunctuation(char ch) {
	return isascii(ch) && ispunct(ch);
}