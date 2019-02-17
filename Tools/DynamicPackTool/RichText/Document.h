#pragma once

#include "ILexer.h"
#include "CellBuffer.h"
#include "HighlightDelimiter.h"
#include "FolderTable.h"
#include "CharClassify.h"
#include "IRegexSearch.h"
#include "IDocWatcher.h"
#include "DocModification.h"
#include "WatcherWithUserData.h"
#include "Decoration.h"

class ILexInterface;

typedef LONG Position;
const Position invalidPosition = -1;

class Range
{
public:
	Range(Position lPos = 0)
		: m_lStart(lPos) , m_lEnd(lPos)
	{

	}
	Range(Position lStart, Position lEnd)
		: m_lStart(lStart), m_lEnd(lEnd)
	{

	}

	BOOL IsValid() const{
		return (m_lStart != invalidPosition) && (m_lEnd != invalidPosition);
	}

	// Is the position within the range?
	BOOL IsContains(Position lPos) const {
		if (m_lStart < m_lEnd) {
			return (lPos >= m_lStart && lPos <= m_lEnd);
		} else {
			return (lPos <= m_lStart && lPos >= m_lEnd);
		}
	}

	// Is the character after pos within the range?
	BOOL IsContainsCharacter(Position lPos) const {
		if (m_lStart < m_lEnd) {
			return (lPos >= m_lStart && lPos < m_lEnd);
		} else {
			return (lPos < m_lStart && lPos >= m_lEnd);
		}
	}

	BOOL IsContains(Range& other) const{
		return IsContains(other.m_lStart) && IsContains(other.m_lEnd);
	}

	// 是否具有重叠
	BOOL IsOverlaps(Range& other) const {
		return IsContains(other.m_lStart) || IsContains(other.m_lEnd) ||
				  other.IsContains(m_lStart) || other.IsContains(m_lEnd);
	}

	Position GetStart() const {
		return m_lStart;
	}

	Position GetEnd() const {
		return m_lEnd;
	}
protected:
	Position m_lStart;
	Position m_lEnd;
};

typedef struct tagStyledText 
{
	size_t length;
	size_t style;
	const char* text;
	BOOL bMultipleStyles;
	const BYTE* pStyles;
	
	tagStyledText(size_t length_, const char* text_, BOOL bMultipleStyles_, size_t style_, const BYTE* pStyles_) :
		length(length_), text(text_), bMultipleStyles(bMultipleStyles_), style(style_), pStyles(pStyles_) {
	}

	// Return number of bytes from start to before '\n' or end of text.
	// Return 1 when start is outside text
	size_t LineLength(size_t nStart) const {
		size_t nCur = nStart;
		while((nCur < length) && (text[nCur] != '\n'))
			++nCur;
		return nCur - nStart;
	}

	size_t StyleAt(size_t nPos) const {
		return bMultipleStyles ? pStyles[nPos] : style;
	}
}StyledText;


typedef enum tagLineData
{
	LINE_DATA_MARKERS		=	0,
	LINE_DATA_LEVELS				=	1,
	LINE_DATA_STATES			=	2,
	LINE_DATA_MARGIN			=	3,
	LINE_DATA_ANNOTATION	=	4,
	LINE_DATA_SIZE					=	5,
}LINE_DATA;

class Document : public IPerLine, public IDocument, public ILoader
{
public:
	/** Used to pair watcher pointer with user data. */
	Document();
	virtual ~Document();
public:
	int AddRef();
	int Release();

	virtual void Init();
	BOOL SetDBCSCodePage(int nDBCSCodePage);
	virtual void InsertLine(int nLine);
	virtual void RemoveLine(int nLine);

	int GetVersion() const;
	void SetErrorStatus(int nStatus);
	int LineFromPosition(int nPos) const;
	int ClampPositionIntoDocument(int nPos);
	BOOL IsCrLf(int nPos) const;
	/*BOOL Document::IsCrLf(int nPos) const*/
	int	   LenChar(int nPos);
	BOOL IsInGoodUTF8(int nPos, int& nStart, int& nEnd) const;
	int MovePositionOutsideChar(int nPos, int nMoveDir, BOOL bCheckLineEnd = TRUE);
	int NextPosition(int nPos, int nMoveDir) const;
	// Returns true if pos changed;
	BOOL NextCharacter(int& nPos, int nMoveDir);
	int CodePage() const;
	BOOL IsDBCSLeadByte(CHAR chByte) const;
	int SafeSegment(const CHAR* chData, int nLen, int nLenSegment);

	// Gateways to modifying document
	void ModifiedAt(int nPos);
	void CheckReadOnly();
	BOOL DeleteChars(int nPos, int nLen);
	BOOL InsertString(int nPos, const char* chData, int nLen);
	int AddData(char* chData, int nLen);
	void* ConvertToDocument();
	int Undo();
	int Redo();
	BOOL CanUndo();
	BOOL CanRedo();
	void DeleteUndoHistory();
	BOOL SetUndoCollection(BOOL bUndoCollecting = TRUE);
	BOOL IsCollectingUndo() const;
	void BeginUndoAction();
	void EndUndoAction();
	void AddUndoAction(int nToken, BOOL bMayCoalesce);
	void SetSavePoint();
	BOOL IsSavePoint() const;
	const CHAR* BufferPointer();
	const CHAR* RangePointer(int nPos, int nLen);
	int GetGapPosition() const;
	
	int GetLineIndentation(int nLine) const;
	void SetLineIndentation(int nLine, int nIndent);
	int GetLineIndentPosition(int nLine) const;
	int GetColumn(int nPos);
	int CountCharacters(int nStartPos, int nEndPos);
	int FindColumn(int nLine, int nColumn);
	void Indent(BOOL bForward, int nLineBottom, int nLineTop);
	static CHAR* TransformLineEnds(int* pLenOut, const CHAR* chText, size_t nLen, int nEolModWanted);
	void ConvertLineEnds(int nEolModeSet);
	void SetReadOnly(BOOL bReadOnly = TRUE);
	BOOL IsReadOnly() const;

	BOOL InsertChar(int nPos, CHAR chData);
	BOOL InsertCString(int nPos, const CHAR* chData);
	void ChangeChar(int nPos, CHAR chData);
	void DeleteChar(int nPos);
	void DeleteCharBack(int nPos);

	CHAR CharAt(int nPos) const;
	void GetCharRange(CHAR* pBuffer, int nPos, int nLen) const;
	CHAR StyleAt(int nPos) const;
	void GetStyleRange(BYTE* pBuffer, int nPos, int nLen) const;

	int GetMark(int nLine) const;
	int MarkerNext(int nLineStart, int nMask) const;
	int AddMark(int nLine, int nMarkerNum);
	void AddMarkSet(int nLine, int nValueSet);
	void DeleteMark(int nLine, int nMarkerNum);
	void DeleteMarkFromHandle(int nMarkerHandle);
	void DeleteAllMarks(int nMarkerNum);
	int LineFromHandle(int nMarkerHandle);
	int LineStart(int nLine) const;
	int LineEnd(int nLine) const;
	int LineEndPosition(int nPos) const;
	BOOL IsLineEndPosition(int nPos) const;
	int VCHomePosition(int nPos) const;

	int SetLevel(int nLine, int nLevel);
	int GetLevel(int nLine) const;
	void ClearLevels();
	int GetLastChild(int nLineParent, int nLevel = -1 ,int nLastLine = -1) ;
	int GetFoldParent(int nLine) const;
	void GetHighlightDelimiters(HighlightDelimiter& hDelimite, int nLine, int nLastLine) ;

	void Indent(BOOL bForward = TRUE);
	int ExtendWordSelect(int nPos, int nDelta, BOOL bOnlyWordCharacter = FALSE);
	int NextWordStart(int nPos, int nDelta);
	int NextWordEnd(int nPos, int nDelta);
	int GetLength() const;
	void Allocate(int nNewSize);
	BOOL IsMatchesWordOptions(BOOL bWord, BOOL bWordStart, int nPos, int nLen) const;
	BOOL HasCaseFolder() const;
	void SetCaseFolder(CaseFolder* pcf);
	LONG FindText(int nMinPos, int nMaxPos, const char* chSearch, BOOL bCaseSensitive, 
							BOOL bWord, BOOL bWordStart, BOOL bRegExp, int nFlags, int* nLen);
	const CHAR* SubstituteByPosition(const CHAR* chData, int* nLen);

	int LinesTotal() const;

	void ChangeCase(Range r, BOOL bMakeUpperCase);

	void SetDefaultCharClasses(BOOL bIncludeWordClass);
	void SetCharClasses(const BYTE* chData, CHAR_TYPE NewCharType);
	int GetCharOfClass(CHAR_TYPE chClass, BYTE* pBuffer) const; 
	void SetStylingBits(int nBits);
	void StartStyling(int nPos, CHAR chMask);
	BOOL SetStyleFor(int nLen, CHAR chStyle);
	BOOL SetStyles(int nLen, const CHAR* chStyle);
	int GetEndStyled() const;
	void EnsureStyledTo(int nPos) ;
	void LexerChanged();
	int GetStyleClock() const;
	void IncrementStyleClock();
	void DecorationSetCurrentIndicator(int nIndicator);
	void DecorationFillRange(int nPos, int nVal, int nLen);

	int SetLineState(int nLine, int nState);
	int GetLineState(int nLine) const;
	int GetmaxLineState() const;
	void ChangeLexerState(int nStart, int nEnd);

	StyledText MarginStyledText(int nLine);
	void MarginSetStyle(int nLine, int nStyle);
	void MarginSetStyles(int nLine, const BYTE* chStyles);
	void MarginSetText(int nLine, const CHAR* chData);
	int MarginLength(int nLine) const;
	void MarginClearAll();

	BOOL AnnotationAny() const;
	StyledText AnnotationStyledText(int nLine);
	void AnnotationSetText(int nLine, const CHAR* chData);
	void AnnotationSetStyle(int nLine, int nStyle);
	void AnnotationSetStyles(int nLine, const BYTE* chStyle);
	int AnnotationLength(int nLine) const;
	int AnnotationLines(int nLine) const;
	void AnnotationClearAll();

	BOOL AddWatcher(IDocWatcher* pWatcher, void* pUserData);
	BOOL RemoveWatcher(IDocWatcher* pWatcher, void* pUserData);

	const WatcherWithUserData* GetWatchers() const;
	int GetWatchersCount() const;

	CHAR_TYPE GetWordCharClass(BYTE chData) const;
	BOOL IsWordPartSeparator(CHAR chData) const;
	int WordPartLeft(int nPos);
	int WordPartRight(int nPos);
	BOOL IsLineEndChar(CHAR chChar) const;
	int ExtendStyleRange(int nPos, int nDelta, BOOL bSingleLine = FALSE);
	BOOL IsWhiteLine(int  nLine) const;
	int ParaUp(int nPos);
	int ParaDown(int nPos);
	int GetIndentSize() const;
	CHAR BraceOpposite(CHAR chChar);
	int BraceMatch(int nPos, int nMaxReStyle);
private:
	static BOOL IsPunctuation(char ch);
	BOOL IsWordStartAt(int nPos) const;
	BOOL IsWordEndAt(int nPos) const;
	BOOL IsWordAt(int nStart, int nEnd) const;

	void NotifyModifyAttempt();
	void NotifySavePoint(BOOL bSavePoint = TRUE);
	void NotifyModified(DocModification& mh);
public:
	ILexInterface*	m_pLexInterface;
	int					m_nStylingBits;
	int					m_nStylingBitsMask;

	int					m_nEolMode;
	/// Can also be SC_CP_UTF8 to enable UTF-8 mode
	int					m_nDBCSCodePage;
	int					m_nTabInChars;
	int					m_nIndentInChars;
	int					m_nActualIndentInChars;
	BOOL				m_bUseTabs;
	BOOL				m_bTabIndents;
	BOOL				m_bBackspaceUnindents;

	DecorationList	 m_Decorations;
private:
	int				m_nRefCount;
	CellBuffer	m_cb;
	CharClassify	 m_CharClass;
	CaseFolder*	m_pcf;
	CHAR			m_chStylingMask;
	int				m_nEndStyled;
	int				m_nStyleClock;
	int				m_nEnteredModification;
	int				m_nEnteredStyling;
	int				m_nEnteredReadOnlyCount;

	WatcherWithUserData*	m_pWatchers;
	int				m_nWatcherLen;

	IPerLine*		m_pPerLineData[LINE_DATA_SIZE];

	BOOL			m_bMatchesValid;
	IRegexSearch*		m_pRegex;
};
