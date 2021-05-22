#pragma once

typedef enum SYNTAX_TYPE
{
	SYNTAX_SKIP			=	0,
	SYNTAX_DQSTART	=	1,		// Double Quotes start
	SYNTAX_DQEND		=	2,		//	Double Quotes end
	SYNTAX_SQSTART	=	3,		//	Single Quotes start
	SYNTAX_SQEND		=	4,		// Single Quotes end
	SYNTAX_CMSTART	=	5,		// Comment Start(both single and multi line)
	SYNTAX_CMEND		=	6,		//	Comment End
	SYNTAX_SLEND		=	7,		// Single line comment end
	SYNTAX_MLEND		=	8,		// Multi line comment end
	SYNTAX_KEYWORD= 9,		// Keyword start
	SYNTAX_USER			=10,
};

typedef struct tagWORDINFO
{
	UINT				dwLen;
	LPTSTR			lpKeyword;
	int					nGroup;
	tagWORDINFO*	pNext;
	CHARFORMAT2 cf;
}WORDINFO, *PWORDINFO;

class CRichTextUI;

class CLexerMarker
{
public:
	CLexerMarker();
	virtual ~CLexerMarker();
public:
	void CreateTables();
	void DeleteTables();
	void ClearKeywordList();
	void Colorize(CHARRANGE cfPos, CRichTextUI* pRichText);
	void Colorize(LONG lStart, LONG lEnd, CRichTextUI* pRichText);
	void AddKeyword(LPCTSTR lpszKeyword, CHARFORMAT2 cf, int nGroup);
	void AddKeyword(LPCTSTR lpszKeyword, COLORREF cr, int nGroup);
public:
	void SetCommentColor(COLORREF cr);
	void SetCommentSyte(CHARFORMAT2& cfComment);
	void SetStringColor(COLORREF cr);
	void SetStringStyle(CHARFORMAT2& cfString);
	void SetGroupColor(int nGroup, COLORREF cr);
	void GetGroupStyle(int nGroup, CHARFORMAT2& cfGroup);
	void SetGroupStyle(int nGroup, CHARFORMAT2& cfGroup);
protected:
	void AddKey(LPCTSTR lpszKeyword, CHARFORMAT2 cf, int nGroup);
	void colorize(LPCTSTR lpszBuf, CRichTextUI *pCtrl, long iOffset =0);
protected:
	WORDINFO*	m_pWordInfo;
	CHARFORMAT2	m_cfComment;
	CHARFORMAT2	m_cfString;
	CHARFORMAT2	m_cfDefault;
	BYTE* m_pTableZero;
	BYTE* m_pTableOne;
	BYTE* m_pTableTwo;
	BYTE* m_pTableThree;
	BYTE* m_pTableFour;
	BYTE* m_pAllowable;
};