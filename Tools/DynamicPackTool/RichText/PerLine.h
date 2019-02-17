#pragma once

#include "CellBuffer.h"
/**
 * This holds the marker identifier and the marker type to display.
 * MarkerHandleNumbers are members of lists.
 */
typedef struct tagMarkerHandleNumber 
{
	int handle;
	int number;
	tagMarkerHandleNumber *pNext;
}MarkerHandleNumber;

/**
 * A marker handle set contains any number of MarkerHandleNumbers.
 */
class MarkerHandleSet 
{
public:
	MarkerHandleSet();
	~MarkerHandleSet();
public:
	int GetLength() const;
	int NumberFromHandle(int handle) const;
	int MarkValue() const;	///< Bit set of marker numbers.
	BOOL IsContains(int handle) const;
	BOOL InsertHandle(int handle, int markerNum);
	void RemoveHandle(int handle);
	BOOL RemoveNumber(int markerNum, BOOL bAll);
	void CombineWith(MarkerHandleSet *pOther);
protected:
	MarkerHandleNumber* m_pRoot;
};

class LineMarkers : public IPerLine
{
public:
	LineMarkers();
	virtual ~LineMarkers();
public:
	virtual void Init();
	virtual void InsertLine(int nLine);
	virtual void RemoveLine(int nLine);

	int GetMarkValue(int nLine) const;
	int GetMarkerNext(int nLineStart, int nMask) const;
	int AddMark(int nLine, int nMarker, int nLines);
	void MergeMarkers(int nPos);
	BOOL DeleteMark(int nLine, int nMarkNum, BOOL bAll);
	void DeleteMardFromHandle(int nMarkHandle);
	int LineFromHandle(int nMarkerHandle);
protected:
	SplitVector<MarkerHandleSet*> m_pMarkers;
	/// Handles are allocated sequentially and should never have to be reused as 32 bit ints are very big.
	int m_nHandleCurrent;
};

class LineLevels : public IPerLine
{
public:
	LineLevels();
	virtual ~LineLevels();
public:
	virtual void Init();
	virtual void InsertLine(int nLine);
	virtual void RemoveLine(int nLine);

	void ExpandLevels(int nSizeNew = -1);
	void ClearLevels();
	int SetLevel(int nLine, int nLevel, int nLines);
	int GetLevel(int nLine) const;
protected:
	SplitVector<int> m_nLevels;
};


class LineState : public IPerLine
{
public:
	LineState();
	virtual ~LineState();
public:
	virtual void Init();
	virtual void InsertLine(int nLine);
	virtual void RemoveLine(int nLine);

	int SetLineState(int nLine, int nState);
	int GetLineState(int nLine) ;
	int GetMaxLineState() const;
protected:
	SplitVector<int> m_nLineStates;
};

class LineAnnotation : public IPerLine
{
public:
	LineAnnotation();
	virtual ~LineAnnotation();
public:
	virtual void Init();
	virtual void InsertLine(int nLine);
	virtual void RemoveLine(int nLine);

	BOOL AnySet() const;
	BOOL MultipleStyles(int nLine) const;
	int GetStyle(int nLine) const;
	const CHAR* GetText(int nLine) const;
	const BYTE* GetStyles(int nLine) const;
	void SetText(int nLine, const CHAR* chText);
	void ClearAll();
	void SetStyle(int nLine, int nStyle);
	void SetStyles(int nLine, const BYTE* chStyles);
	int GetLength(int nLen) const;
	int GetLines(int nLine) const;
protected:
	static int NumberLines(const CHAR* chData) ;
	static CHAR* AllocateAnnotation(int nLen, int nStyle);
protected:
	SplitVector<CHAR*> m_chAnnotations;
};