#pragma once

/// A split vector of integers with a method for adding a value to all elements
/// in a range.
/// Used by the Partitioning class.
#include "SplitVector.h"
class SplitVectorWithRangeAdd : public SplitVector<int>
{
public:
	SplitVectorWithRangeAdd(int nGrowSize) {
		SetGrowSize(nGrowSize);
		ReAllocate(nGrowSize);
	}
	~SplitVectorWithRangeAdd() {

	}
public:
	void RangeAddDelta(int nStart, int nEnd, int nDelta){
		// end is 1 past end, so end-start is number of elements to change
		int n = 0;
		int nRangelen = nEnd - nStart;
		int nRangeLen2 = nRangelen;
		int nPartLeft = m_nPartLen - nStart;
		if (nRangelen > m_nPartLen)
			nRangelen = m_nPartLen;
		while (n < nRangelen) {
			m_pBody[nStart++] += nDelta;
			++n;
		}

		nStart += m_nGapLen;
		while (n < nRangelen) {
			m_pBody[nStart++] += nDelta;
			++n;
		}
	}
};

/// Divide an interval into multiple partitions.
/// Useful for breaking a document down into sections such as lines.
/// A 0 length interval has a single 0 length partition, numbered 0
/// If interval not 0 length then each partition non-zero length
/// When needed, positions after the interval are considered part of the last partition
/// but the end of the last partition can be found with PositionFromPartition(last+1).

class Partitioning
{
public:
	Partitioning(int nGrowSize) ;
	~Partitioning();
public:
	int Partitions() const;
	void InsertPartition(int nPartition, int nPos) ;
	void SetPartitionStartPosition(int nPartition, int nPos) ;
	void InsertText(int nPartitionInsert, int nDelta);
	void RemovePartition(int nPartition);

	int PositionFromPartition(int nPartition) const ;

	/// Return value in range [0 .. Partitions() - 1] even for arguments outside interval
	int PartitionFromPosition(int nPos) const ;
	void DeleteAll() ;
protected:
	// Move Step forward
	void ApplyStep(int nPartitionUpto) ;
	// Move step backward
	void BackStep(int nPartitionDownTo) ;
	void Allocate(int nGrowSize) ;
private:
	// To avoid calculating all the partition positions whenever any text is inserted
	// there may be a step somewhere in the list.

	int m_nStepPartition;
	int m_nStepLength;
	SplitVectorWithRangeAdd* m_pBody;
};