#pragma once
#include "SplitVector.h"
#include "Partitioning.h"
class RunStyles
{
public:
	RunStyles();
	~RunStyles();
public:
	int GetLength() const;
	int ValueAt(int nPos) const;
	int FindNextChange(int nPos, int nEnd) const;
	int StartRun(int nPos);
	int EndRun(int nPos);
	// Returns true if some values may have changed
	BOOL FillRange(int& nPos, int nVal, int& nFillLen);
	void SetValueAt(int nPos, int nVal);
	void InsertSpace(int nPos, int nLen);
	void DeleteAll();
	void DeleteRange(int nPos, int nLen);
	int GetRuns() const;
	BOOL IsAllSame() const;
	BOOL IsAllSameAs(int nVal) const;
	int Find(int nVal, int nStart) const;
protected:
	int GetRunFromPosition(int nPos) const;
	int SplitRun(int nPos);
	void RemoveRun(int nRun);
	void RemoveRunIfEmpty(int nRun);
	void RemoveRunIfSameAsPrevious(int nRun);
private:
	Partitioning* m_pStarts;
	SplitVector<int>* m_pStyles;
};