#pragma once

#include "IRegexSearch.h"
#include "RESearch.h"
/**
 * Implementation of RegexSearchBase for the default built-in regular expression engine
 */
class BuiltInRegex : public IRegexSearch
{
public:
	BuiltInRegex(CharClassify* chCharClassify);
	virtual ~BuiltInRegex();
public:
	virtual LONG FindText(Document* pDoc, int nMinPos, int nMaxPos, const CHAR* chText,
								BOOL bCaseSensitive, BOOL bWord, BOOL bWordStart, int nFlags, int* pLen);
	virtual const CHAR* SubstituteByPosition(Document* pDoc, const CHAR* chText, int* pLen);
private:
	RESearch m_Research;
	CHAR*		m_chSubstituted;
};