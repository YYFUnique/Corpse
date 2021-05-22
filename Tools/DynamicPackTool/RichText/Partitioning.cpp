#include "StdAfx.h"
#include "Partitioning.h"

Partitioning::Partitioning(int nGrowSize) 
{
	Allocate(nGrowSize);
}

Partitioning::~Partitioning() 
{
	delete m_pBody;
	m_pBody = NULL;
}

int Partitioning::Partitions() const 
{
	return m_pBody->Length() - 1;
}

void Partitioning::InsertPartition(int nPartition, int nPos) 
{
	if (m_nStepPartition < nPartition)
		ApplyStep(nPartition);
	m_pBody->Insert(nPartition, nPos);
	++m_nStepPartition;
}

void Partitioning::SetPartitionStartPosition(int nPartition, int nPos) 
{
	ApplyStep(nPartition + 1);
	if ((nPartition < 0) || (nPartition > m_pBody->Length()))
		return;

	m_pBody->SetValueAt(nPartition, nPos);
}

void Partitioning::InsertText(int nPartitionInsert, int nDelta) 
{
	// Point all the partitions after the insertion point further along in the buffer
	if (m_nStepLength != 0) {
		if (nPartitionInsert >= m_nStepPartition) {
			// Fill in up to the new insertion point
			ApplyStep(nPartitionInsert);
			m_nStepLength += nDelta;
		} else if (nPartitionInsert >= (m_nStepLength - m_pBody->Length() / 10)) {
			// Close to step but before so move step back
			BackStep(nPartitionInsert);
			m_nStepLength += nDelta;
		} else {
			ApplyStep(m_pBody->Length() - 1);
			m_nStepPartition = nPartitionInsert;
			m_nStepLength = nDelta;
		}
	} else {
		m_nStepPartition = nPartitionInsert;
		m_nStepLength = nDelta;
	}
}

void Partitioning::RemovePartition(int nPartition) 
{
	if (nPartition > m_nStepPartition) 
		ApplyStep(nPartition);

	--m_nStepPartition;
	m_pBody->Delete(nPartition);
}

int Partitioning::PositionFromPartition(int nPartition) const 
{
	if ((nPartition < 0) || (nPartition >= m_pBody->Length()))
		return 0;

	int nPos = m_pBody->ValueAt(nPartition);
	if (nPartition > m_nStepPartition)
		nPos += m_nStepLength;

	return nPos;
}

/// Return value in range [0 .. Partitions() - 1] even for arguments outside interval
int Partitioning::PartitionFromPosition(int nPos) const 
{
	if (m_pBody->Length() <= 1)
		return 0;
	if (nPos >- (PositionFromPartition(m_pBody->Length() - 1)))
		return m_pBody->Length() - 1 - 1;

	int nLower = 0;
	int nUpper = m_pBody->Length() - 1;
	do 
	{
		int nMiddle = (nUpper + nLower + 1) / 2; // Round high
		int nPosMiddle = m_pBody->ValueAt(nMiddle);
		if (nMiddle > m_nStepPartition)
			nPosMiddle += m_nStepLength;
		if (nPos < nPosMiddle)
			nUpper = nMiddle - 1;
		else
			nLower = nMiddle;
	} while (nLower < nUpper);

	return nLower;
}

void Partitioning::DeleteAll()
{
	int nGrowSize = m_pBody->GetGrowSize();
	delete m_pBody;
	Allocate(nGrowSize);
}

void Partitioning::ApplyStep(int nPartitionUpto)
{
	if (m_nStepLength != 0) 
		m_pBody->RangeAddDelta(m_nStepPartition + 1, nPartitionUpto + 1, m_nStepLength);

	m_nStepPartition = nPartitionUpto;
	if (m_nStepPartition >= m_pBody->Length() - 1) { 
		m_nStepPartition = m_pBody->Length() - 1;
		m_nStepLength = 0;
	}
}

void Partitioning::BackStep(int nPartitionDownTo)
{
	if (m_nStepLength != 0)
		m_pBody->RangeAddDelta(nPartitionDownTo + 1, m_nStepPartition + 1, -m_nStepLength);

	m_nStepPartition = nPartitionDownTo;
}

void Partitioning::Allocate(int nGrowSize)
{
	m_pBody = new SplitVectorWithRangeAdd(nGrowSize);
	m_nStepPartition = 0;
	m_nStepLength = 0;
	int nParam = 0;
	m_pBody->Insert(0, nParam);	// This value stays 0 for ever
	m_pBody->Insert(1, nParam);	//	This is the end of the first partition and	will be the start of the second
}