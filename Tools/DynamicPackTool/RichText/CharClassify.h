#pragma once

typedef enum tagCHAR_TYPE
{
	CHAR_TYPE_SPACE			= 0,
	CHAR_TYPE_NEW_LINE	=	1,
	CHAR_TYPE_WORD			=	2,
	CHAR_TYPE_PUNCTUATION = 3,
}CHAR_TYPE;

class CharClassify
{
public:
	CharClassify();
public:
	void SetDefaultCharClasses(BOOL bIncludeWordClass = TRUE);
	void SetCharClasses(const BYTE* bChars, CHAR_TYPE NewCharClass);
	int GetCharsOfClass(CHAR_TYPE CharacterClass, BYTE* pBuffer) const;
	CHAR_TYPE GetClass(BYTE bChar) const {return static_cast<CHAR_TYPE>(m_bCharClass[bChar]);}
	BOOL IsWord(BYTE bChar) const {return static_cast<CHAR_TYPE>(m_bCharClass[bChar] )== CHAR_TYPE_WORD ;}

private:
	enum {maxChar = 256};
	BYTE m_bCharClass[maxChar];	// not type cc to save space
};