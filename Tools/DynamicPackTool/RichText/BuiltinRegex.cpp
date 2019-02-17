#include "StdAfx.h"
#include "BuiltinRegex.h"
#include "Document.h"
#include "Define.h"
#include "DocumentIndexer.h"

BuiltInRegex::BuiltInRegex(CharClassify* chCharClassify)
	: m_Research(chCharClassify)
	, m_chSubstituted(NULL)
{

}

BuiltInRegex::~BuiltInRegex()
{
	delete m_chSubstituted;
}

LONG BuiltInRegex::FindText(Document *pDoc, int nMinPos, int nMaxPos, const CHAR *chText, BOOL bCaseSensitive, BOOL bWord, BOOL bWordStart, int nFlags, int *pLen)
{
	BOOL bPosix = (nFlags & SCFIND_POSIX) != 0;
	int nIncrement = (nMinPos <= nMaxPos) ? 1 : -1;

	int nStartPos = nMinPos;
	int nEndPos = nMaxPos;

	// Range endpoints should not be inside DBCS characters, but just in case, move them.
	nStartPos =  pDoc->MovePositionOutsideChar(nStartPos, 1, FALSE);
	nEndPos = pDoc->MovePositionOutsideChar(nEndPos, 1, FALSE);

	const CHAR* chErrMsg = m_Research.Compile(chText, *pLen, bCaseSensitive, bPosix);
	if (chErrMsg)
		return -1;

	// Find a variable in a property file: \$(\([A-Za-z0-9_.]+\))
	// Replace first '.' with '-' in each property file variable reference:
	//     Search: \$(\([A-Za-z0-9_-]+\)\.\([A-Za-z0-9_.]+\))
	//     Replace: $(\1-\2)
	int nLineRangeStart = pDoc->LineFromPosition(nStartPos);
	int nLineRangeEnd = pDoc->LineFromPosition(nEndPos);
	if ((nIncrement == 1) && (nStartPos >= pDoc->LineEnd(nLineRangeStart)) && (nLineRangeStart < nLineRangeEnd)) {
		// the start position is at end of line or between line end characters.
		++nLineRangeStart;
		nStartPos = pDoc->LineStart(nLineRangeStart);
	} else if ((nIncrement == -1) && (nStartPos <= pDoc->LineStart(nLineRangeStart)) && (nLineRangeStart < nLineRangeEnd)) {
		// the start position is at beginning of line.
		--nLineRangeStart;
		nStartPos = pDoc->LineEnd(nLineRangeStart);
	}

	int nPos = -1;
	int nLenRet = 0;
	CHAR chSearchEnd = chText[*pLen - 1];
	CHAR chSearchEndPrev = (*pLen > 1) ? chText[*pLen - 2] : '\0';
	int nLineRangeBreak = nLineRangeEnd + nIncrement;
	for (int nLine = nLineRangeStart; nLine != nLineRangeBreak; nLine += nIncrement){
		int nStartOfLine = pDoc->LineStart(nLine);
		int nEndOfLine = pDoc->LineEnd(nLine);
		if (nIncrement == 1) {
			if (nLine == nLineRangeStart) {
				if ((nStartPos != nStartOfLine) && (chText[0] == '^'))
					continue;	// Can't match start of line if start position after start of line
				nStartOfLine = nStartPos;
			} 
			if (nLine == nLineRangeEnd) {
				if ((nEndPos != nEndOfLine) && (chSearchEnd == '$') && (chSearchEndPrev != '\\'))
					continue;	// Can't match end of line if end position before end of line
				nEndOfLine = nEndPos;
			}
		} else {
			if (nLine == nLineRangeEnd) {
				if ((nEndPos != nStartOfLine) && (chText[0] == '^'))
					continue; // Can't match start of line if end position after start of line
				nStartOfLine = nEndPos;
			}
			if (nLine == nLineRangeStart) {
				if ((nStartPos != nEndOfLine) && (chSearchEnd == '$') && (chSearchEndPrev != '\\'))
					continue;	// Can't match end of line if start position before end of line
				nEndOfLine = nStartPos;
			}
		}

		DocumentIndexer DocDi(pDoc, nEndOfLine);
		int nSuccess = m_Research.Execute(DocDi, nStartOfLine, nEndOfLine);
		if (nSuccess) {
			nPos = m_Research.m_nBopat[0];
			nLenRet = m_Research.m_nEopat[0] - m_Research.m_nBopat[0];
			// There can be only one start of a line, so no need to look for last match in line
			if ((nIncrement == -1) && (chText[0] != '^')) {
				// Check for the last match on this line.
				int nRepetitions = 1000;
				while (nSuccess && (m_Research.m_nEopat[0] <= nEndOfLine) && (--nRepetitions)) {
					nSuccess = m_Research.Execute(DocDi, nPos + 1, nEndOfLine);
					if (nSuccess) {
						if (m_Research.m_nEopat[0] <= nMinPos) {
							nPos = m_Research.m_nBopat[0];
							nLenRet = m_Research.m_nEopat[0] - m_Research.m_nBopat[0];
						} else {
							nSuccess = 0;
						}
					}
				}
			}

			break;
		}
	}

	*pLen = nLenRet;
	return nPos;
}

const CHAR* BuiltInRegex::SubstituteByPosition(Document *pDoc, const CHAR *chText, int *pLen)
{
	delete[] m_chSubstituted;
	m_chSubstituted = NULL;

	DocumentIndexer DocDi(pDoc, pDoc->GetLength());
	if (m_Research.GrabMatches(DocDi) == FALSE)
		return 0;

	UINT nLenRet = 0;
	for (int n=0; n<*pLen; ++n) {
		if (chText[n] == '\\') {
			if (chText[n + 1] >= '0' && chText[n + 1] <= '9') {
				UINT nPatNum = chText[n + 1] - '0';
				nLenRet += m_Research.m_nEopat[nPatNum] - m_Research.m_nBopat[nPatNum];
				++n;
			} else {
				switch (chText[n + 1]) {
					case 'a':
					case 'b':
					case 'f':
					case 'n':
					case 'r':
					case 't':
					case 'v':
					case '\\':
						++n;
				}
				++nLenRet;
			}
		} else {
			++nLenRet;
		}
	}

	m_chSubstituted = new CHAR[nLenRet + 1];
	CHAR* pSubstituted = m_chSubstituted;

	for (int m=0; m<*pLen; ++m) {
		if (chText[m] == '\\') {
			if (chText[m + 1] >= '0' && chText[m + 1] <= '9') {
				UINT nPatNum = chText[m + 1] - '0';
				UINT nLen = m_Research.m_nEopat[nPatNum] - m_Research.m_nBopat[nPatNum];
				// Will be null if try for a match that did not occur
				if (m_Research.m_chPat[nPatNum])
					memcpy(pSubstituted, m_Research.m_chPat[nPatNum], nLen);
				pSubstituted += nLen;
				++m;
			} else {
				++m;
				switch (chText[m]) {
					case 'a':
							*pSubstituted++ ='\a';
						break;
					case 'b':
							*pSubstituted++ = '\b';
						break;
					case 'f':
							*pSubstituted++ = '\f';
						break;
					case 'n':
							*pSubstituted++ = '\n';
						break;
					case 'r':
							*pSubstituted++ = '\r';
						break;
					case 't':
							*pSubstituted++ = '\t';
						break;
					case 'v':
							*pSubstituted++ = '\v';
						break;
					case '\\':
							*pSubstituted++ = '\\';
						break;
					default:
						*pSubstituted++ = '\\';
						--m;
				}
			}
		} else {
			*pSubstituted++ = chText[m];
		}
	}

	*pSubstituted = '\0';
	*pLen = nLenRet;
	return m_chSubstituted;
}

IRegexSearch *CreateRegexSearch(CharClassify *charClassTable) 
{
	return new BuiltInRegex(charClassTable);
}