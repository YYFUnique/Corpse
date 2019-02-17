#include "StdAfx.h"
#include "CellBuffer.h"
#include "Document.h"

LineVector::LineVector()
	: m_Starts(256), m_pPerLine(NULL)
{
	Init();
}

LineVector::~LineVector()
{
	m_Starts.DeleteAll();
}

void LineVector::Init()
{
	m_Starts.DeleteAll();
	if (m_pPerLine)
		m_pPerLine->Init();
}

void LineVector::SetPerLine(IPerLine *pPl)
{
	m_pPerLine = pPl;
}

void LineVector::InsertText(int nLine, int nDelta)
{
	m_Starts.InsertText(nLine, nDelta);
}

void LineVector::InsertLine(int nLine, int nPos, BOOL bLineStart)
{
	m_Starts.InsertPartition(nLine, nPos);
	if (m_pPerLine) { 
		if ((nLine > 0) && bLineStart)
			--nLine;
		m_pPerLine->InsertLine(nLine);
	}
}

void LineVector::SetLineStart(int nLine, int nPos)
{
	m_Starts.SetPartitionStartPosition(nLine, nPos);
}

void LineVector::RemoveLine(int nLine)
{
	m_Starts.RemovePartition(nLine);
	if (m_pPerLine)
		m_pPerLine->RemoveLine(nLine);
}

int LineVector::GetLines() const
{
	return m_Starts.Partitions();
}

int LineVector::GetLineFromPosition(int nPos) const
{
	return m_Starts.PartitionFromPosition(nPos);
}

int LineVector::GetLineStart(int nLine) const
{
	return m_Starts.PositionFromPartition(nLine);
}

Action::Action()
{
	m_ActionType = startAction;
	m_nPos = 0;
	m_chData = NULL;
	m_nLen = 0;
	m_bMayCoalesce = FALSE;
}

Action::~Action()
{
	Destrory();
}

void Action::Create(actionType at_, int nPos /*= 0*/, CHAR *chData /*= 0*/, int nDataLen /*= 0*/, BOOL bMayCoalesce /*= TRUE*/)
{
	delete[] m_chData;
	m_nPos = nPos;
	m_ActionType = at_;
	m_chData = chData;
	m_nLen = nDataLen;
	m_bMayCoalesce = bMayCoalesce;
}

void Action::Destrory()
{
	delete[] m_chData;
	m_chData = NULL;
}

void Action::Grab(Action* pSource)
{
	delete[] m_chData;

	m_nPos = pSource->m_nPos;
	m_ActionType = pSource->m_ActionType;
	m_chData = pSource->m_chData;
	m_nLen = pSource->m_nLen;
	m_bMayCoalesce = pSource->m_bMayCoalesce;

	pSource->m_nPos = 0;
	pSource->m_ActionType = startAction;
	pSource->m_chData = NULL;
	pSource->m_nLen = 0;
	pSource->m_bMayCoalesce = TRUE;
}

const actionType& Action::GetActionType() const
{
	return m_ActionType;
}

BOOL Action::IsMayCoalesce() const
{
	return m_bMayCoalesce;
}

void Action::SetMayCoalesce(BOOL bMayCoalesce /*= TRUE*/)
{
	m_bMayCoalesce = bMayCoalesce;
}

int Action::GetPos() const
{
	return m_nPos;
}

int Action::GetDataLength() const
{
	return m_nLen;
}

CHAR* Action::GetData() const
{
	return m_chData;
}


UndoGroup::UndoGroup(Document* pDoc, BOOL bGroupNeed /*= TRUE*/)
	: m_pDoc(pDoc)
	, m_bGroupNeeded(bGroupNeed)
{
	if (bGroupNeed)
		m_pDoc->BeginUndoAction();
}

UndoGroup::~UndoGroup()
{
	if (m_bGroupNeeded)
		m_pDoc->EndUndoAction();
}

BOOL UndoGroup::IsGroupNeeded() const
{
	return m_bGroupNeeded;
}

// The undo history stores a sequence of user operations that represent the user's view of the
// commands executed on the text.
// Each user operation contains a sequence of text insertion and text deletion actions.
// All the user operations are stored in a list of individual actions with 'start' actions used
// as delimiters between user operations.
// Initially there is one start action in the history.
// As each action is performed, it is recorded in the history. The action may either become
// part of the current user operation or may start a new user operation. If it is to be part of the
// current operation, then it overwrites the current last action. If it is to be part of a new
// operation, it is appended after the current last action.
// After writing the new action, a new start action is appended at the end of the history.
// The decision of whether to start a new user operation is based upon two factors. If a
// compound operation has been explicitly started by calling BeginUndoAction and no matching
// EndUndoAction (these calls nest) has been called, then the action is coalesced into the current
// operation. If there is no outstanding BeginUndoAction call then a new operation is started
// unless it looks as if the new action is caused by the user typing or deleting a stream of text.
// Sequences that look like typing or deletion are coalesced into a single user operation.

UndoHistory::UndoHistory()
{
	m_nLenActions = 100;
	m_pActions = new Action[m_nLenActions];
	m_nMaxAction = 0;
	m_nCurrentAction = 0;
	m_nUndoSequenceDepth = 0;
	m_nSavePoint = 0;

	m_pActions[m_nCurrentAction].Create(startAction);
}

UndoHistory::~UndoHistory()
{
	delete[] m_pActions;
	m_pActions = NULL;
}

void UndoHistory::EnsureUndoRoom()
{
	// Have to test that there is room for 2 more actions in the array
	// as two actions may be created by the calling function
	if (m_nCurrentAction >= (m_nLenActions - 2)) { 
		// Run out of undo nodes so extend the array
		int nLenActionsNew = m_nLenActions * 2;
		Action* pActionsNew = new Action[nLenActionsNew];
		for (int nAct = 0; nAct < m_nCurrentAction; ++nAct)
			pActionsNew[nAct].Grab(&m_pActions[nAct]);

		delete[] m_pActions;
		m_nLenActions = nLenActionsNew;
		m_pActions = pActionsNew;
	}
}

void UndoHistory::AppendAction(actionType at, int nPos, CHAR *chData, int nLen, BOOL &bStartSequence, BOOL bMayCoalesce /*= TRUE*/)
{
	EnsureUndoRoom();

	if (m_nCurrentAction < m_nSavePoint)
		m_nSavePoint = -1;

	int nOldCurrentAction = m_nCurrentAction;
	if (m_nCurrentAction >= 1) {
		if (m_nUndoSequenceDepth == 0) {
			// Top level actions may not always be coalesced
			int nTargetAct = -1;
			const Action* pActPrevious = &(m_pActions[m_nCurrentAction + nTargetAct]);
			// Container actions may forward the coalesce state of Scintilla Actions.
			while((pActPrevious->GetActionType() == containerAction) && pActPrevious->IsMayCoalesce()) {
				--nTargetAct;
				pActPrevious = &(m_pActions[m_nCurrentAction + nTargetAct]);
			}

			// See if current action can be coalesced into previous action
			// Will work if both are inserts or deletes and position is same
			if (m_nCurrentAction == m_nSavePoint)
				++m_nCurrentAction;
			else if (m_pActions[m_nCurrentAction].IsMayCoalesce() == FALSE)
				++m_nCurrentAction;
			else if (bMayCoalesce == FALSE || pActPrevious->IsMayCoalesce() == FALSE)
				++m_nCurrentAction;
			else if (at == containerAction || m_pActions[m_nCurrentAction].GetActionType() == containerAction)
				;// A coalescible containerAction
			else if ((at != pActPrevious->GetActionType() ) && (pActPrevious->GetActionType() != startAction))
				++m_nCurrentAction;
			else if ((at == insertAction) && (nPos != (pActPrevious->GetPos() + pActPrevious->GetDataLength())))
				++m_nCurrentAction;// Insertions must be immediately after to coalesce
			else if (at == removeAction) {
				if ((nLen == 1) || (nLen == 2)) {
					if ((nPos + nLen) == pActPrevious->GetPos())
						; // Backspace -> OK
					else if (nPos == pActPrevious->GetPos())
						; // Delete -> OK
					else
						++m_nCurrentAction; // Removals must be at same position to coalesce
				} else {
					++m_nCurrentAction; // Removals must be of one character to coalesce
				}
			} else {
				// Action coalesced.
			}
		} else {
			// Actions not at top level are always coalesced unless this is after return to top level
			if (m_pActions[m_nCurrentAction].IsMayCoalesce())
				++m_nCurrentAction;
		}
	} else {
		++m_nCurrentAction;
	}

	bStartSequence = nOldCurrentAction != m_nCurrentAction;
	m_pActions[m_nCurrentAction].Create(at, nPos, chData, nLen, bMayCoalesce);
	++m_nCurrentAction;
	m_pActions[m_nCurrentAction].Create(startAction);
	m_nMaxAction = m_nCurrentAction;
}

void UndoHistory::BeginUndoAction()
{	
	EnsureUndoRoom();
	if (m_nUndoSequenceDepth == 0) {
		if (m_pActions[m_nCurrentAction].GetActionType() != startAction) {
			++m_nCurrentAction;
			m_pActions[m_nCurrentAction].Create(startAction);
			m_nMaxAction = m_nCurrentAction;
		}

		m_pActions[m_nCurrentAction].SetMayCoalesce(FALSE);
	}
	++m_nUndoSequenceDepth;
}

void UndoHistory::EndUndoAction()
{
	ASSERT(m_nUndoSequenceDepth > 0);
	EnsureUndoRoom();

	--m_nUndoSequenceDepth;
	if (m_nUndoSequenceDepth == 0) {
		if (m_pActions[m_nCurrentAction].GetActionType() != startAction) {
			++m_nCurrentAction;
			m_pActions[m_nCurrentAction].Create(startAction);
			m_nMaxAction = m_nCurrentAction;
		}

		m_pActions[m_nCurrentAction].SetMayCoalesce(FALSE);
	}
}

void UndoHistory::DropUndoSequence()
{
	m_nUndoSequenceDepth = 0;
}

void UndoHistory::DeleteUndoHistory()
{
	for(int n=0; n<m_nMaxAction; ++n)
		m_pActions[n].Destrory();

	m_nMaxAction = 0;
	m_nCurrentAction = 0;
	m_nSavePoint = 0;
	m_pActions[m_nCurrentAction].Create(startAction);
}

void UndoHistory::SetSavePoint()
{
	m_nSavePoint = m_nCurrentAction;
}

BOOL UndoHistory::IsSavePoint() const
{
	return m_nSavePoint == m_nCurrentAction;
}

BOOL UndoHistory::CanUndo() const
{
	return (m_nCurrentAction >0) && (m_nMaxAction > 0);
}

int UndoHistory::StartUndo()
{
	// Drop any trailing startAction
	if (m_pActions[m_nCurrentAction].GetActionType() == startAction &&
		m_nCurrentAction >0)
		--m_nCurrentAction;

	// Count the steps in this action
	int nAct = m_nCurrentAction;
	while(m_pActions[nAct].GetActionType() != startAction && nAct > 0)
		--nAct;

	return m_nCurrentAction - nAct;
}

const Action& UndoHistory::GetUndoStep() const
{
	return m_pActions[m_nCurrentAction];
}

void UndoHistory::CompletedUndoStep()
{
	--m_nCurrentAction;
}

BOOL UndoHistory::CanRedo() const
{
	return m_nMaxAction > m_nCurrentAction;
}

int UndoHistory::StartRedo()
{
	if (m_pActions[m_nCurrentAction].GetActionType() == startAction && m_nCurrentAction < m_nMaxAction)
		++m_nCurrentAction;

	// Count the steps in this action
	int nAct = m_nCurrentAction;
	while(m_pActions[m_nCurrentAction].GetActionType() != startAction && nAct < m_nMaxAction)
		++nAct;

	return nAct - m_nCurrentAction;
}

const Action& UndoHistory::GetRedoStep() const
{
	return m_pActions[m_nCurrentAction];
}

void UndoHistory::CompletedRedoStep()
{
	++m_nCurrentAction;
}

/////////////////////////////////////////////////////////////////////////

CellBuffer::CellBuffer()
{
	m_bReadOnly = FALSE;
	m_bCollectingUndo = TRUE;
}

CellBuffer::~CellBuffer()
{

}

CHAR CellBuffer::CharAt(int nPos) const
{
	return m_Substance.ValueAt(nPos);
}

void CellBuffer::GetCharRange(CHAR *chBuffer, int nPos, int nLen) const
{
	if (nLen < 0)
		return;

	if (nPos < 0)
		return;

	if ((nPos + nLen) > m_Substance.Length())
		return;

	m_Substance.GetRange(chBuffer, nPos, nLen);
}

CHAR CellBuffer::StyleAt(int nPos) const
{
	return m_Style.ValueAt(nPos);
}

void CellBuffer::GetStyleRange(BYTE *pBuffer, int nPos, int nLen) const
{
	if (nLen < 0)
		return;
	if (nPos < 0)
		return;
	if ((nPos + nLen) > m_Style.Length())
		return;

	m_Style.GetRange(reinterpret_cast<CHAR*>(pBuffer), nPos, nLen);
}

const CHAR* CellBuffer::BufferPointer()
{
	return m_Substance.BufferPointer();
}

const CHAR* CellBuffer::RangePointer(int nPos, int nLen)
{
	return m_Substance.RangePointer(nPos, nLen);
}

int CellBuffer::GapPosition() const
{
	return m_Substance.GapPosition();
}

// The char* returned is to an allocation owned by the undo history
const CHAR* CellBuffer::InsertString(int nPos, const CHAR *s, int nLen, BOOL& bStartSequence)
{
	CHAR* chData = NULL;
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	if (m_bReadOnly == FALSE) {
		if (m_bCollectingUndo) { 
			// Save into the undo/redo stack, but only the characters - not the formatting
			// This takes up about half load time
			chData = new CHAR[nLen];
			for (int n=0; n<nLen; ++n)
				chData[n] = s[n];

			m_uh.AppendAction(insertAction, nPos, chData, nLen, bStartSequence);
		}
		BasicInsertString(nPos, s, nLen);
	}

	return chData;
}

BOOL CellBuffer::SetStyleAt(int nPos, CHAR chStyleValue, CHAR chMask /*= '\377'*/)
{
	chStyleValue &= chMask;
	CHAR chVal = m_Style.ValueAt(nPos);
	if ((chVal & chMask) != chStyleValue) {
		m_Style.SetValueAt(nPos, static_cast<CHAR>((chVal & ~ chMask) | chStyleValue));
		return TRUE;
	}

	return FALSE;
}


BOOL CellBuffer::SetStyleFor(int nPos, int nLen, CHAR chStyleValue, CHAR chMask)
{
	BOOL bChanged = FALSE;
	ASSERT(nLen == 0 || (nLen > 0 && nLen + nPos <= m_Style.Length()));

	while(nLen--) {
		CHAR chVal = m_Style.ValueAt(nPos);
		if ((chVal & chMask) != chStyleValue) {
			m_Style.SetValueAt(nPos, static_cast<CHAR>((chVal & ~chMask) | chStyleValue));
			bChanged = TRUE;
		}
		++nPos;
	}

	return bChanged;
}

// The char* returned is to an allocation owned by the undo history
const CHAR* CellBuffer::DeleteChars(int nPos, int nLen, BOOL &bStartSequence)
{
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	ASSERT(nLen > 0);
	CHAR* chData = NULL;
	if (m_bReadOnly == FALSE) {
		if (m_bCollectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			chData = new CHAR[nLen];
			for (int n=0; n<nLen; ++n)
				chData[n] = m_Substance.ValueAt(nPos + n);

			m_uh.AppendAction(removeAction, nPos, chData, nLen, bStartSequence);
		}

		BasicDeleteChars(nPos, nLen);
	}

	return chData;
}

int CellBuffer::GetLength() const
{
	return m_Substance.Length();
}

void CellBuffer::Allocate(int nNewSize)
{
	m_Substance.ReAllocate(nNewSize);
	m_Style.ReAllocate(nNewSize);
}

void CellBuffer::SetPerLine(IPerLine *pPl)
{
	m_lv.SetPerLine(pPl);
}

int CellBuffer::Lines() const
{
	return m_lv.GetLines();
}

int CellBuffer::LineStart(int nLine) const
{
	if (nLine < 0)
		return 0;
	else if (nLine >= Lines())
		return GetLength();
	else
		return m_lv.GetLineStart(nLine);
}

BOOL CellBuffer::IsReadOnly() const
{
	return m_bReadOnly;
}

void CellBuffer::SetReadOnly(BOOL bReadOnly /*= TRUE*/)
{
	m_bReadOnly = bReadOnly;
}

void CellBuffer::SetSavePoint() 
{
	m_uh.SetSavePoint();
}

BOOL CellBuffer::IsSavePoint() const
{
	return m_uh.IsSavePoint();
}

// Without undo
void CellBuffer::InsertLine(int nLine, int nPos, BOOL bLineStart)
{
	m_lv.InsertLine(nLine, nPos, bLineStart);
}

void CellBuffer::RemoveLine(int nLine)
{
	m_lv.RemoveLine(nLine);
}

void CellBuffer::BasicInsertString(int nPos, const CHAR *s, int nLen)
{
	if (nLen == 0)
		return;

	ASSERT(nLen > 0);

	m_Substance.InsertFromArray(nPos, s, 0, nLen);
	m_Style.InsertValue(nPos, nLen, 0);

	int nLineInsert = m_lv.GetLineFromPosition(nPos) + 1;
	BOOL bAtLineStart = m_lv.GetLineStart(nLineInsert - 1) == nPos;
	// Point all the lines after the insertion point further along in the buffer
	m_lv.InsertText(nLineInsert - 1, nLen);
	CHAR chPrev = m_Substance.ValueAt(nPos - 1);
	CHAR chAfter = m_Substance.ValueAt(nPos + nLen);

	if (chPrev == '\r' && chAfter == '\n')
	{
		// Splitting up a crlf pair at position
		InsertLine(nLineInsert, nPos, FALSE);
		++nLineInsert;
	}

	CHAR chChar = ' ';
	for (int n=0; n < nLen; ++n) {
		chChar = s[n];
		if (chChar == '\r') {
			InsertLine(nLineInsert, (nPos + n) + 1, bAtLineStart);
			++nLineInsert;
		} else if (chChar == '\n') {
			if (chPrev == '\r'){
				// Patch up what was end of line
				m_lv.SetLineStart(nLineInsert - 1, (nPos + n) + 1);
			} else {
				InsertLine(nLineInsert, (nPos + n) + 1, bAtLineStart);
				++nLineInsert;
			}
		}

		chPrev = chChar;
	}

	// Joining two lines where last insertion is cr and following substance starts with lf
	if (chAfter == '\n') {
		if (chChar == '\r') {
			// End of line already in buffer so drop the newly created one
			RemoveLine(nLineInsert - 1);
		}
	}
}

void CellBuffer::BasicDeleteChars(int nPos, int nLen)
{
	if (nLen == 0)
		return;

	if ((nPos == 0) && (nLen == m_Substance.Length())) {
		// If whole buffer is being deleted, faster to reinitialise lines data
		// than to delete each line.
		m_lv.Init();
	} else {
		// Have to fix up line positions before doing deletion as looking at text in buffer
		// to work out which lines have been removed
		int nLineRemove = m_lv.GetLineFromPosition(nPos) + 1;
		m_lv.InsertText(nLineRemove - 1, -(nLen));
		CHAR chPrev = m_Substance.ValueAt(nPos - 1);
		CHAR chBefore = chPrev;
		CHAR chNext = m_Substance.ValueAt(nPos);
		BOOL bIgnoreNL = FALSE;
		if (chPrev == '\r' && chNext == '\n') { 
			// Move back one
			m_lv.SetLineStart(nLineRemove, nPos);
			++nLineRemove;
			bIgnoreNL = TRUE; // First \n is not real deletion
		}
		CHAR chChar = chNext;
		for (int n=0; n<nLen; ++n) {
			chNext = m_Substance.ValueAt(nPos + n + 1);
			if (chChar == '\r') {
				if (chNext != '\n') 
					RemoveLine(nLineRemove);
			} else if (chChar == '\n') {
				if (bIgnoreNL)
					bIgnoreNL = FALSE;	// Further \n are real deleteion
				else
					RemoveLine(nLineRemove);
			}

			chChar = chNext;
		}
		// May have to fix up end if last deletion causes cr to be next to lf
		// or removes one of a crlf pair

		CHAR chAfter = m_Substance.ValueAt(nPos + nLen);
		if (chBefore == '\r' && chAfter == '\n')
		{
			// Using lineRemove-1 as cr ended line before start of deletion
			RemoveLine(nLineRemove - 1);
			m_lv.SetLineStart(nLineRemove - 1, nPos + 1);
		}
	}

	m_Substance.DeleteRange(nPos, nLen);
	m_Style.DeleteRange(nPos, nLen);
}

BOOL CellBuffer::SetUndoCollection(BOOL bCollectUndo /*= TRUE*/)
{
	m_bCollectingUndo = bCollectUndo;
	m_uh.DropUndoSequence();
	return m_bCollectingUndo;
}

BOOL CellBuffer::IsCollectingUndo() const
{
	return m_bCollectingUndo;
}

void CellBuffer::BeginUndoAction()
{
	m_uh.BeginUndoAction();
}

void CellBuffer::EndUndoAction()
{
	m_uh.EndUndoAction();
}

void CellBuffer::AddUndoAction(int nToken, BOOL bMayCoalesce)
{
	BOOL bStartSequence;
	m_uh.AppendAction(containerAction, nToken, 0, 0, bStartSequence, bMayCoalesce);
}

void CellBuffer::DeleteUndoHistory()
{
	m_uh.DeleteUndoHistory();
}

BOOL CellBuffer::CanUndo() const
{
	return m_uh.CanUndo();
}

int CellBuffer::StartUndo()
{
	return m_uh.StartUndo();
}

const Action& CellBuffer::GetUndoStep() const
{
	return m_uh.GetUndoStep();
}

void CellBuffer::PerformUndoStep()
{
	const Action& ActionStep = m_uh.GetUndoStep();
	if (ActionStep.GetActionType() == insertAction)
		BasicDeleteChars(ActionStep.GetPos(), ActionStep.GetDataLength());
	else if (ActionStep.GetActionType() == removeAction)
		BasicInsertString(ActionStep.GetPos(), ActionStep.GetData(), ActionStep.GetDataLength());

	m_uh.CompletedUndoStep();
}

BOOL CellBuffer::CanRedo() const
{
	return m_uh.CanRedo();
}

int CellBuffer::StartRedo()
{
	return m_uh.StartRedo();
}

const Action& CellBuffer::GetRedoStep() const
{
	return m_uh.GetRedoStep();
}

void CellBuffer::PerformRedoStep()
{
	const Action& ActionStep = GetRedoStep();
	if (ActionStep.GetActionType() == insertAction)
		BasicInsertString(ActionStep.GetPos(), ActionStep.GetData(), ActionStep.GetDataLength());
	else if (ActionStep.GetActionType() == removeAction)
		BasicDeleteChars(ActionStep.GetPos(), ActionStep.GetDataLength());

	m_uh.CompletedRedoStep();
}