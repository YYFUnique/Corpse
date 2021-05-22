#pragma once

class HighlightDelimiter
{
public:
	HighlightDelimiter();

public:
	void Clear() ;
	BOOL IsNeedsDrawing(int nLine) ;
	BOOL IsFoldBlockHighlighted(int nLine) ;
	BOOL IsHeadOfFoldBlock(int nLine) ;
	BOOL IsTailOfFoldBlock(int nLine) ;
	
	void SetBeginFoldBlock(int nBeginFoldBlock);
	void SetEndFoldBlock(int nEndFoldBlock);
	void SetFirstChangeableLineBefore(int nFirstChangeableLineBefore);
	void SetFirstChangeableLineAfter(int nFirstChangeableLineAfter);
protected:
	int m_nBeginFoldBlock;							// Begin of current fold block
	int m_nEndFoldBlock;								// End of current fold block
	int m_nFirstChangeableLineBefore;		// First line that triggers repaint before starting line that determined current fold block
	int m_nFirstChangeableLineAfter;			// First line that triggers repaint after starting line that determined current fold block
	BOOL m_bEnabled;
};