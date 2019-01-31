#pragma once

typedef enum tagRICHTEXT_COLOR_STATE
{
	RICHTEXT_UNKNOW			=	0x00,
	RICHTEXT_INTAG				=	0x01,
	RICHTEXT_INQUOTES			=	0x02,
	RICHTEXT_INCOMMENT		=	0x03,
	RICHTEXT_INTEXT				=	0x04,
	RICHTEXT_ERROR				=	0x05,
}RICHTEXT_COLOR_STATE;

#define LINE_COLORED	0x80	

#define DUI_CTR_RICHTEXT	_T("RichText")

class CRichTextUI : public CRichEditUI
{
public:
	CRichTextUI();
	~CRichTextUI();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	/*UINT GetControlFlags() const;*/

	void DoInit();
	void SetPos(RECT rc);
	void DoEvent(TEventUI& event);
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
public:
	void ParseAllLines();
public:
	void OnTxNotify(DWORD iNotify, void *pv);
	void OnChange();
	BOOL IsShowLineNum() const;
	void SetShowLineNum(BOOL bShowLineNumber);
public:
	void InitlizeTagCharFormat(CHARFORMAT2& cfTag);
	void InitlizeTextCharFormat(CHARFORMAT2& cfText);
	void InitlizeQuoteCharFormat(CHARFORMAT2& cfQuote);
	void InitlizeCommentCharFormat(CHARFORMAT2& cfComment);
	
	void SetTagCharFormat(CHARFORMAT2& cfTags, BOOL bParse);
	void SetQuoteCharFormat(CHARFORMAT2& cfQuoted, BOOL bParse);
	void SetTextCharFormat(CHARFORMAT2& cfText, BOOL bParse);
	void SetCommentCharFormat(CHARFORMAT2& cfComments, BOOL bParse);
	void SetTagCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse);
	void SetQuoteCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse);
	void SetTextCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse);
	void SetCommentCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse);
protected:
	void OnChar(UINT nChar/*, UINT nRepCnt, UINT nFlags*/);
	void OnKeyDown(UINT nChar/*, UINT nRepCnt, UINT nFlags*/);

	void StartColoringTimer();
	void StopColoringTimer();

	void UpdateLinesArraySize();
	void SetFirstLineCharColor(int nLineIndex);
	int FindCommentStartHelper(int nCharPosition);
	int FindCommentEndHelper(int nCharPosition);

	void ColorVisibleLines(int nCharPosition = -1);
	void InvalidateColorStates(int nCharPosition);
	BOOL IsWindowCompletelyObscured();
	int GetLineHelper(int nLineIndex, CDuiString& strLine, int nLineLength  = -1 );
	//void TrimRightCrLfHelper(CDuiString& strText, int nLength  = -1 );
	RICHTEXT_COLOR_STATE ParseLines(LPCTSTR lpszLines, int nCharPos, BOOL bColor, int nCurrentLine = -1);
	int ColorRangeHelper(int nColorStart, int nColorEnd, CHARFORMAT2 cfText, int nColorFromChar = -1 );
protected:
	LONG		m_nLineCount;
	BYTE*	   m_pLineEndState;

	BOOL	  m_bShowLineNum;		// «∑Òœ‘ æ––∫≈

	UINT	m_uiBckgdTimerInterval;		// How many millis between each iteration
	int		m_nBckgdTimerNumOfLines;	// How many lines to color in each iteration
	bool	m_bBckgdTimerActivated;

	BOOL	m_bOnEnVscrollDisabled;
	int		m_nOnChangeCharPosition;	// OnKeyDown signals OnChange to InvalidateColorStates

	CHARFORMAT2	m_cfTags;
	CHARFORMAT2	m_cfText;
	CHARFORMAT2	m_cfQuoted;
	CHARFORMAT2	m_cfComment;
};