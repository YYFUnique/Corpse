#pragma once

#include "Partitioning.h"

typedef enum tagactionType 
{ 
	insertAction, 
	removeAction, 
	startAction, 
	containerAction 
}actionType;

// Interface to per-line data that wants to see each line insertion and deletion
class IPerLine
{
public:
	virtual ~IPerLine() {}
	virtual void Init() = 0;
	virtual void InsertLine(int nLine) = 0;
	virtual void RemoveLine(int nLine) = 0;
};

/**
 * The line vector contains information about each of the lines in a cell buffer.
 */

class LineVector
{
public:
	LineVector();
	~LineVector();
public:
	void Init();
	void SetPerLine(IPerLine* pPl);

	void InsertText(int nLine, int nDelta);
	void InsertLine(int nLine, int nPos, BOOL bLineStart);
	void SetLineStart(int nLine, int nPos);
	void RemoveLine(int nLine);
	int GetLines() const;

	int GetLineFromPosition(int nPos) const;
	int GetLineStart(int nLine) const;

	int MarkValue(int nLine);
	int AddMark(int nLine, int nMarker);
	void MergeMarkers(int nPos);
	void DeleteMark(int nLine, int nMarkerNum, BOOL bAll = TRUE);
	void DeleteMarkFromHandle(int nMarkHandel);
	int GetLineFromHandle(int nMarkerHandle);

	void ClearLevels();
	int SetLevel(int nLine, int nLevel);
	int GetLevel(int nLine) const;

	int SetLineState(int nLine, int nState);
	int GetLineState(int nLine) const;
	int GetMaxLineState() const;
private:
	Partitioning m_Starts;
	IPerLine*		m_pPerLine;
};


/**
 * Actions are used to store all the information required to perform one undo/redo step.
 */

class Action
{
public:
	Action();
	~Action();

public:
	void Create(actionType at_, int nPos = 0, CHAR* chData = 0, int nDataLen = 0, BOOL bMayCoalesce = TRUE );
	void Destrory();
	void Grab(Action* pSource);

public:
	const actionType& GetActionType() const;
	BOOL	IsMayCoalesce() const;
	void SetMayCoalesce(BOOL bMayCoalesce = TRUE);
	int GetPos() const;
	int GetDataLength() const;
	CHAR* GetData() const;
protected:
	actionType  m_ActionType;
	int				m_nPos;
	CHAR*			m_chData;
	int				m_nLen;
	BOOL			m_bMayCoalesce;
};

class Document;

class UndoGroup
{
public:
	UndoGroup(Document* pDoc, BOOL bGroupNeed = TRUE);
	~UndoGroup();
public:
	BOOL IsGroupNeeded() const;
private:
	Document*		m_pDoc;
	BOOL				m_bGroupNeeded;
};

class UndoHistory
{
public:
	UndoHistory();
	~UndoHistory();
public:
	void AppendAction(actionType at, int nPos, CHAR* chData, int nLen, BOOL& bStartSequence, BOOL bMayCoalesce = TRUE);

	void BeginUndoAction();
	void EndUndoAction();
	void DropUndoSequence();
	void DeleteUndoHistory();

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	BOOL IsSavePoint() const;

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	BOOL CanUndo() const;
	int StartUndo();
	const Action& GetUndoStep() const;
	void CompletedUndoStep();
	BOOL CanRedo() const;
	int StartRedo();
	const Action& GetRedoStep() const;
	void CompletedRedoStep();
protected:
	void EnsureUndoRoom();
protected:
	Action*	m_pActions;
	int			m_nLenActions;
	int			m_nMaxAction;
	int			m_nCurrentAction;
	int			m_nUndoSequenceDepth;
	int			m_nSavePoint;
};


/**
 * Holder for an expandable array of characters that supports undo and line markers.
 * Based on article "Data Structures in a Bit-Mapped Text Editor"
 * by Wilfred J. Hansen, Byte January 1987, page 183.
 */

class CellBuffer
{
public:
	CellBuffer();
	~CellBuffer();

public:
	/// Retrieving positions outside the range of the buffer works and returns 0
	CHAR CharAt(int nPos) const;
	void GetCharRange(CHAR* chBuffer, int nPos, int nLen) const;
	CHAR StyleAt(int nPos) const;
	void GetStyleRange(BYTE* pBuffer, int nPos, int nLen) const;
	const char* BufferPointer();
	const char* RangePointer(int nPos, int nRangeLen);
	int GapPosition() const;

	int GetLength() const;
	void Allocate(int nNewSize);
	void SetPerLine(IPerLine* pPl);
	int Lines() const;
	int LineStart(int nLine) const;
	int LineFromPosition(int nPos) const{
		return m_lv.GetLineFromPosition(nPos);
	}
	void InsertLine(int nLine, int nPos, BOOL bLineStart);
	void RemoveLine(int nLine);
	const CHAR* InsertString(int nPos, const CHAR* stext, int nLen, BOOL& bStartSequence);

	/// Setting styles for positions outside the range of the buffer is safe and has no effect.
	/// @return true if the style of a character is changed.
	BOOL SetStyleAt(int nPos, CHAR chStyleValue, CHAR chMask = '\377');
	BOOL SetStyleFor(int nPos, int nLen, CHAR chStyleValue, CHAR chMask);

	const char* DeleteChars(int nPos, int nLen, BOOL& bStartSequence);

	BOOL IsReadOnly() const;
	void SetReadOnly(BOOL bReadOnly = TRUE);

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	BOOL IsSavePoint() const;

	BOOL SetUndoCollection(BOOL bCollectUndo = TRUE);
	BOOL IsCollectingUndo() const;
	void BeginUndoAction();
	void EndUndoAction();
	void AddUndoAction(int nToken, BOOL bMayCoalesce);
	void DeleteUndoHistory();

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	BOOL CanUndo() const;
	int StartUndo();
	const Action& GetUndoStep() const;
	void PerformUndoStep();
	BOOL CanRedo() const;
	int StartRedo();
	const Action& GetRedoStep() const;
	void PerformRedoStep();
protected:
	void BasicInsertString(int nPos, const CHAR* text, int nLen);
	void BasicDeleteChars(int nPos, int nLen);
private:
	SplitVector<char> m_Substance;
	SplitVector<char> m_Style;
	BOOL m_bReadOnly;

	BOOL m_bCollectingUndo;
	UndoHistory m_uh;
	LineVector    m_lv;
};