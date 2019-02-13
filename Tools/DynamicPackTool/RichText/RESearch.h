#pragma once

#include "CharClassify.h"

#define MAXCHR	256
#define CHARBIT	8
#define BITBLK		MAXCHR/CHARBIT

class ICharacterIndexer
{
public:
	ICharacterIndexer();
	virtual ~ICharacterIndexer();
public:
	virtual CHAR CharAt(int nIndex) = 0;
};

class RESearch
{
public:
	RESearch(CharClassify* chClassTables);
	~RESearch();
public:
	BOOL GrabMatches(ICharacterIndexer& ci);
	const CHAR* Compile(const CHAR* chPattern, int nLen, BOOL bCaseSensitive, BOOL bPosix);
	int Execute(ICharacterIndexer& ci, int nStartPos, int nEndPos);
	int Substitute(ICharacterIndexer& ci, CHAR* chSrc, CHAR* chDst);

	enum { MAXTAG=10 };
	enum { MAXNFA=2048 };
	enum { NOTFOUND=-1 };

	int m_nBopat[MAXTAG];
	int m_nEopat[MAXTAG];
	CHAR *m_chPat[MAXTAG];
protected:
	void Init();
	void Clear();
	void CharSet(BYTE chByte);
	void CharSetWithCase(BYTE chByte, BOOL bCaseSensitive = TRUE);
	int GetBackslashExpression(const CHAR* chPattern, int& nIncr);

	int PMatch(ICharacterIndexer& ci, int nStartPos, int nEndPos, CHAR* chText);
	BOOL IsWordc(BYTE chByte) const;
	static int GetHexChar(BYTE chByte1, BYTE chByte2);
	static BYTE EscapeValue(BYTE chByte);
protected:
	int		m_nBol;
	int		m_nTagstk[MAXTAG];		/* subpat tag stack */
	CHAR	m_chNFA[MAXNFA];		/* automaton */
	int		m_nSTA;
	BYTE	m_chBittab[BITBLK];	/* bit table for CCL pre-set bits */
	int		m_nFailure;
	CharClassify* m_chCharClass;
};