#include "StdAfx.h"
#include "CharClassify.h"

CharClassify::CharClassify()
{
	SetDefaultCharClasses(TRUE);
}

void CharClassify::SetDefaultCharClasses(BOOL bIncludeWordClass /*= TRUE*/)
{
	// Initialize all char classes to default values
	for (int nIndex = 0; nIndex < maxChar ; ++nIndex) {
		if (nIndex == '\n' || nIndex == '\r')
			m_bCharClass[nIndex] = CHAR_TYPE_NEW_LINE;
		else if (nIndex < 0x20 || nIndex == ' ')
			m_bCharClass[nIndex] = CHAR_TYPE_SPACE;
		else if (bIncludeWordClass && (nIndex >= 0x80 || isalnum(nIndex) || nIndex == '_'))
			m_bCharClass[nIndex] = CHAR_TYPE_WORD;
		else
			m_bCharClass[nIndex] = CHAR_TYPE_PUNCTUATION;
	}
}

void CharClassify::SetCharClasses(const BYTE *bChars, CHAR_TYPE NewCharClass)
{
	// Apply the newCharClass to the specifed chars
	if (bChars) {
		while(*bChars) {
			m_bCharClass[*bChars] = static_cast<BYTE>(NewCharClass);
			++bChars;
		}
	}
}

int CharClassify::GetCharsOfClass(CHAR_TYPE CharacterClass, BYTE *pBuffer) const
{
	// Get characters belonging to the given char class; return the number
	// of characters (if the buffer is NULL, don't write to it).

	int nCount = 0;
	for (int nIndex = maxChar - 1; nIndex >= 0; --nIndex) {
		if (m_bCharClass[nIndex] == CharacterClass) {
			++nCount;
			if (pBuffer) {
				*pBuffer = static_cast<BYTE>(nIndex);
				++pBuffer;
			}
		}
	}

	return nCount;
}