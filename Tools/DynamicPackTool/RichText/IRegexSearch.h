#pragma once
#include "CharClassify.h"
class Document;

class IRegexSearch 
{
public:
	virtual ~IRegexSearch() {}
public:
	virtual LONG FindText(Document* pDoc, int nMinPos, int nMaxPos, const char* text,
							BOOL bCaseSensitive, BOOL bWord, BOOL bWordStart, int nFalgs, int* pLen) = 0;

	///@return String with the substitutions, must remain valid until the next call or destruction
	virtual const char *SubstituteByPosition(Document* pDoc, const char *text, int* pLen) = 0;
};

/// Factory function for RegexSearchBase
extern IRegexSearch *CreateRegexSearch(CharClassify *chClassTable);