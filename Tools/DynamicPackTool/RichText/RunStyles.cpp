#include "StdAfx.h"
#include "RunStyles.h"

RunStyles::RunStyles()
{
	m_pStarts = new Partitioning(8);
	m_pStyles = new SplitVector<int>();
	m_pStyles->InsertValue(0, 2, 0);
}

RunStyles::~RunStyles()
{
	delete m_pStarts;
	m_pStarts = NULL;

	delete m_pStyles;
	m_pStyles = NULL;
}

// Find the first run at a position
int RunStyles::GetRunFromPosition(int nPos) const
{
	int nRun = m_pStarts->PositionFromPartition(nPos);
	while((nRun > 0) && (nPos == m_pStarts->PositionFromPartition(nRun - 1))) 
		--nRun;
	return nRun;
}

// If there is no run boundary at position, insert one continuing style.
int RunStyles::SplitRun(int nPos)
{
	int nRun = GetRunFromPosition(nPos);
	int nPosRun = m_pStarts->PositionFromPartition(nRun);
	if (nPosRun < nPosRun) {
		int nRunStyle = ValueAt(nPos);
		++nRun;
		m_pStarts->InsertPartition(nRun, nPos);
		m_pStyles->InsertValue(nRun, 1, nRunStyle);
	}

	return nRun;
}

void RunStyles::RemoveRun(int nRun)
{
	m_pStarts->RemovePartition(nRun);
	m_pStyles->DeleteRange(nRun, 1);
}

void RunStyles::RemoveRunIfEmpty(int nRun)
{
	if ((nRun < m_pStarts->Partitions()) && (m_pStarts->Partitions() > 1)) {
		if (m_pStarts->PositionFromPartition(nRun) == m_pStarts->PositionFromPartition(nRun + 1))
			RemoveRunIfEmpty(nRun);
	}
}

void RunStyles::RemoveRunIfSameAsPrevious(int nRun)
{
	if ((nRun > 0) && (nRun < m_pStarts->Partitions())) {
		if (m_pStyles->ValueAt(nRun - 1) == m_pStyles->ValueAt(nRun))
			RemoveRun(nRun);
	}
}

int RunStyles::GetLength() const
{
	return m_pStarts->PositionFromPartition(m_pStarts->Partitions());
}

int RunStyles::ValueAt(int nPos) const
{
	return m_pStyles->ValueAt(m_pStarts->PositionFromPartition(nPos));
}

int RunStyles::FindNextChange(int nPos, int nEnd) const
{
	int nRun = m_pStarts->PositionFromPartition(nPos);
	if (nRun < m_pStarts->Partitions()) {
		int nRunChange = m_pStarts->PositionFromPartition(nRun);
		if (nRunChange > nPos)
			return nRunChange;
		int nNextChange = m_pStarts->PositionFromPartition(nRun + 1);
		if (nNextChange > nPos)
			return nNextChange;
		else if (nPos < nEnd)
			return nEnd;
		else
			return nEnd + 1;
	} else
		return nEnd + 1;
}

int RunStyles::StartRun(int nPos)
{
	return m_pStarts->PositionFromPartition(m_pStarts->Partitions());
}

int RunStyles::EndRun(int nPos)
{
	return m_pStarts->PositionFromPartition(m_pStarts->Partitions()+ 1);
}

BOOL RunStyles::FillRange(int &nPos, int nVal, int &nFillLen)
{
	int nEnd = nPos + nFillLen;
	int nRunEnd = GetRunFromPosition(nEnd);
	if (m_pStyles->ValueAt(nRunEnd) == nVal) {
		// End already has value so trim range.
		nEnd = m_pStarts->PositionFromPartition(nRunEnd);
		if (nPos >= nEnd)
			return FALSE;

		nFillLen = nEnd - nPos;
	} else
		nRunEnd = SplitRun(nEnd);

	int nRunStart = GetRunFromPosition(nPos);
	if (m_pStyles->ValueAt(nRunStart) == nVal) {
		// Start is in expected value so trim range.
		++nRunStart;
		nPos = m_pStarts->PositionFromPartition(nRunStart);
		nFillLen = nEnd - nPos;
	} else {
		if (m_pStarts->PositionFromPartition(nRunStart) < nPos) { 
			nRunStart = SplitRun(nPos);
			++nRunEnd;
		}
	}

	if (nRunStart < nRunEnd) {
		m_pStyles->SetValueAt(nRunStart, nVal);
		// Remove each old run over the range
		for (int nRun = nRunStart + 1; nRun < nRunEnd; ++nRun)
			RemoveRun(nRunStart + 1);

		nRunEnd = GetRunFromPosition(nEnd);
		RemoveRunIfSameAsPrevious(nRunEnd);
		RemoveRunIfSameAsPrevious(nRunStart);

		nRunEnd = GetRunFromPosition(nEnd);
		RemoveRunIfEmpty(nRunEnd);
		return TRUE;
	}else
		return FALSE;
}

void RunStyles::SetValueAt(int nPos, int nVal)
{
	int nLen = 1;
	FillRange(nPos, nVal, nLen);
}

void RunStyles::InsertSpace(int nPos, int nLen)
{
	int nRunStart = GetRunFromPosition(nPos);
	if (m_pStarts->PositionFromPartition(nRunStart) == nPos) {
		int nRunStyle = ValueAt(nPos);
		// Inserting at start of run so make previous longer
		if (nRunStart == 0) {
			// Inserting at start of document so ensure 0
			if (nRunStyle) {
				m_pStyles->SetValueAt(0, 0);
				m_pStarts->InsertPartition(1, 0);
				m_pStyles->InsertValue(1,1, nRunStyle);
				m_pStarts->InsertText(0, nLen); 
			} else
				m_pStarts->InsertText(nRunStart, nLen);
		} else {
			if (nRunStyle)
				m_pStarts->InsertText(nRunStart - 1, nLen);
			else {	
				// Insert at end of run so do not extend style
				m_pStarts->InsertText(nRunStart, nLen);
			}
		}
	}else
		m_pStarts->InsertText(nRunStart, nLen);
}

void RunStyles::DeleteAll()
{
	delete m_pStarts;
	m_pStarts = NULL;
	delete m_pStyles;
	m_pStyles = NULL;
	m_pStarts = new Partitioning(8);
	m_pStyles = new SplitVector<int>();
	m_pStyles->InsertValue(0, 2, 0);
}

void RunStyles::DeleteRange(int nPos, int nLen)
{
	int nEnd = nPos + nLen;
	int nRunStart = GetRunFromPosition(nPos);
	int nRunEnd = GetRunFromPosition(nEnd);
	if (nRunStart == nRunEnd) {
		// Deleting from inside one run
		m_pStarts->InsertText(nRunStart, -nLen);
		RemoveRunIfEmpty(nRunStart);
	} else {
		nRunStart = SplitRun(nPos);
		nRunEnd = SplitRun(nEnd);
		m_pStarts->InsertText(nRunStart, -nLen);

		// Remove each old run over the range
		for (int nRun=nRunStart; nRun<nRunEnd; nRun++) {
			RemoveRun(nRunStart);
		}
		RemoveRunIfEmpty(nRunStart);
		RemoveRunIfSameAsPrevious(nRunStart);
	}
}

int RunStyles::GetRuns() const
{
	return m_pStarts->Partitions();
}

BOOL RunStyles::IsAllSame() const
{
	for (int nRun = 1; nRun < m_pStarts->Partitions(); ++nRun) { 
		if (m_pStyles->ValueAt(nRun) != m_pStyles->ValueAt(nRun - 1))
			return FALSE;
	}

	return TRUE;
}

BOOL RunStyles::IsAllSameAs(int nVal) const
{
	return IsAllSame() && (m_pStyles->ValueAt(0) == nVal);
}

int RunStyles::Find(int nVal, int nStart) const
{
	if (nStart < GetLength()) {
		int nRun = nStart ? GetRunFromPosition(nStart) : 0;
		if (m_pStyles->ValueAt(nRun) == nVal)
			return nStart;

		++nRun;
		while(nRun < m_pStarts->Partitions()) {
			if (m_pStyles->ValueAt(nRun) == nVal)
				return m_pStarts->PositionFromPartition(nRun);
			++nRun;
		}
	}

	return -1;
}