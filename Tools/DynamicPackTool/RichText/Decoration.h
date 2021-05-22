#pragma once
#include "RunStyles.h"

class Decoration
{
public:
	Decoration(int nIndicator);
	~Decoration();
public:
	BOOL IsEmpty() const;
	void SetNext(Decoration* pNext);
	Decoration* GetNext() const;
	RunStyles GetRunStyles() const;
	int GetIndicator() const;
public:
	int ValueAt(int nPos);
	int StartRun(int nPos);
	int EndRun(int nPos);
	BOOL FillRange(int nPos, int nVal, int& nLen);
	void DeleteRange(int nPos, int nLen);
	void InsertSpace(int nPos, int nLen);
protected:
	Decoration* m_pNext;
	RunStyles		 m_rs;
	int				 m_nIndicator;
};

class DecorationList
{
public:
	DecorationList();
	~DecorationList();
public:
	void SetCurrentIndicator(int nIndicator);
	int GetCurrentIndicator() const;

	void SetCurrentValue(int nVal);
	int GetCurrentValue() const;

	// Returns true if some values may have changed
	BOOL FillRange(int& nPos, int nVal, int& nFillLen);

	void InsertSpace(int nPos, int nLen);
	void DeleteRange(int nPos, int nLen);

	int AllOnFor(int nPos);
	int ValueAt(int nIndicator, int nPos);
	int Start(int nIndicator, int nPos);
	int End(int nIndicator, int nPos);
protected:
	Decoration* DecorationFromIndicator(int nIndicator) const;
	Decoration* Create(int nIndicator, int nLen);
	void Delete(int nIndicator);
	void DeleteAnyEmpty();
protected:
	int				m_nCurrentIndicator;
	int				m_nCurrentValue;
	int				m_DocumentLen;

	Decoration*	m_pRoot;
	BOOL			 m_bClickNotified;
	Decoration* m_pCurrent;
};