#include "StdAfx.h"
#include "HighlightDelimiter.h"

HighlightDelimiter::HighlightDelimiter()
	: m_bEnabled(FALSE) 
{
	Clear();
}

void HighlightDelimiter::Clear() 
{
	m_nBeginFoldBlock = -1;
	m_nEndFoldBlock = -1;
	m_nFirstChangeableLineBefore = -1;
	m_nFirstChangeableLineAfter = -1;
}

BOOL HighlightDelimiter::IsNeedsDrawing(int nLine) 
{
	return m_bEnabled && (nLine <= m_nFirstChangeableLineBefore || nLine >= m_nFirstChangeableLineAfter);
}

BOOL HighlightDelimiter::IsFoldBlockHighlighted(int nLine) 
{
	return m_bEnabled && m_nBeginFoldBlock != -1 && (nLine <= m_nFirstChangeableLineBefore || nLine >= m_nFirstChangeableLineAfter);
}

BOOL HighlightDelimiter::IsHeadOfFoldBlock(int nLine) 
{
	return m_nBeginFoldBlock != -1 && m_nBeginFoldBlock < nLine && nLine < m_nEndFoldBlock;
}

BOOL HighlightDelimiter::IsTailOfFoldBlock(int nLine) 
{
	return m_nBeginFoldBlock != -1 && m_nBeginFoldBlock < nLine && nLine == m_nEndFoldBlock;
}

void HighlightDelimiter::SetBeginFoldBlock(int nBeginFoldBlock)
{
	m_nBeginFoldBlock = nBeginFoldBlock;
}

void HighlightDelimiter::SetEndFoldBlock(int nEndFoldBlock)
{
	m_nEndFoldBlock = nEndFoldBlock;
}

void HighlightDelimiter::SetFirstChangeableLineBefore(int nFirstChangeableLineBefore)
{
	m_nFirstChangeableLineBefore = nFirstChangeableLineBefore;
}

void HighlightDelimiter::SetFirstChangeableLineAfter(int nFirstChangeableLineAfter)
{
	m_nFirstChangeableLineAfter = nFirstChangeableLineAfter;
}