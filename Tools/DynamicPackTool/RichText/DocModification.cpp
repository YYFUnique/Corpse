#include "StdAfx.h"
#include "DocModification.h"

DocModification::DocModification(int nModificationType, int nPos /*= 0*/, int nLen /*= 0*/, int nLineAdd /*= 0*/, const CHAR* chData /*= NULL*/, int nLine /*= 0*/)
	: m_nModificationType(nModificationType)
	, m_nPos(nPos)
	, m_nLen(nLine)
	, m_nLineAdd(nLineAdd)
	, m_chData(chData)
	, m_nLine(nLine)
	, m_nFoldLevelNow(0)
	, m_nFoldLevelPrev(0)
	, m_nAnnotationLinesAdd(0)
	, m_nToken(0)
{

}

DocModification::DocModification(int nModificationType, const Action& act, int nLineAdd /*= 0*/)
	: m_nModificationType(nModificationType)
	, m_nPos(act.GetPos())
	, m_nLen(act.GetDataLength())
	, m_nLineAdd(nLineAdd)
	, m_chData(act.GetData())
	, m_nLine(0)
	, m_nFoldLevelNow(0)
	, m_nFoldLevelPrev(0)
	, m_nAnnotationLinesAdd(0)
	, m_nToken(0)
{

}

int DocModification::GetModificationType() const
{
	return m_nModificationType;
}

int DocModification::GetPos() const
{
	return m_nPos;
}	

int DocModification::GetLength() const
{
	return m_nLen;
}

int DocModification::GetLine() const
{
	return m_nLine;
}

int DocModification::GetAddLines() const
{
	return m_nLineAdd;
}

const CHAR* DocModification::GetData() const
{
	return m_chData;
}

void DocModification::SetLine(int nLine)
{
	m_nLine = nLine;
}

void DocModification::SetToken(int nToken)
{
	m_nToken = nToken;
}

void DocModification::SetFoldLevelNow(int nFoldLevelNow)
{
	m_nFoldLevelNow = nFoldLevelNow;
}

void DocModification::SetFoldLevelPrev(int nFoldLevelPrev)
{
	m_nFoldLevelPrev = nFoldLevelPrev;
}

void DocModification::SetAnnotationLinesAdd(int nAnnotationLinesAdd)
{
	m_nAnnotationLinesAdd = nAnnotationLinesAdd;
}