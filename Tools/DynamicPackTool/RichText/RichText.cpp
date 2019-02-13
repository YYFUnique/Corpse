#include "StdAfx.h"
#include "RichText.h"

#define KEY_TAG_START	'<'
#define KEY_TAG_END		'>'
#define KEY_DBL_QUOTE	'"'
#define COMMENT_START	_T("/*")
#define COMMENT_END		_T("*/")

#define TIMER_BACKGROUNDCOLORING	100

#define GRP_KEYWORD		0
#define GRP_PRAGMA		1
#define GRP_DIRECTIVE	2

CRichTextUI::CRichTextUI()
	: m_bShowLineNum(FALSE)
{
	m_pLexer = new CLexerMarker;

	UINT	uiSize				= sizeof(CHARFORMAT2);
	DWORD	dwMask		= CFM_COLOR | CFM_FACE | CFM_SIZE; /* | CFM_CHARSET;*/
	LONG	lHeight			= 160;	// 8 point => 160 * (1/20)

	// Initialise the Tags CHARFORMAT
	m_cfTags.cbSize			= uiSize;
	m_cfTags.dwMask		= dwMask;
	m_cfTags.dwEffects		= 0;
	m_cfTags.yHeight		= lHeight;
	m_cfTags.crTextColor	= RGB(128, 0, 0);
	m_cfTags.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfTags.szFaceName,	_T("Î¢ÈíÑÅºÚ"));

	// Initialise the Text CHARFORMAT
	m_cfText.cbSize			= uiSize;
	m_cfText.dwMask		= dwMask;
	m_cfText.dwEffects		= 0;
	m_cfText.yHeight		= lHeight;
	m_cfText.crTextColor	= RGB(0, 0, 0);
	m_cfText.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfText.szFaceName,	_T("Î¢ÈíÑÅºÚ"));

	// Initialise the Quoted Text CHARFORMAT
	m_cfQuoted.cbSize		= uiSize;
	m_cfQuoted.dwMask		= dwMask;
	m_cfQuoted.dwEffects	= 0;
	m_cfQuoted.yHeight		= lHeight;
	m_cfQuoted.crTextColor	= RGB(0, 128, 128);
	m_cfQuoted.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfQuoted.szFaceName, _T("Î¢ÈíÑÅºÚ"));

	// Initialise the Comment CHARFORMAT
	m_cfComment.cbSize		= uiSize;
	m_cfComment.dwMask		= dwMask;
	m_cfComment.dwEffects	= 0;
	m_cfComment.yHeight		= lHeight;
	m_cfComment.crTextColor	= RGB(0, 128, 0);
	m_cfComment.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfComment.szFaceName, _T("Î¢ÈíÑÅºÚ"));

	// For tracking the color state at the end of every line: 
	m_nLineCount = 0;
	m_pLineEndState = NULL;

	m_bOnEnVscrollDisabled = FALSE;	// Disable OnEnVscroll coloring during ColorVisibleLines and OnKeyDown
	m_nOnChangeCharPosition = -1;	// OnKeyDown defers coloring to OnChange when overriding selected text

	// Background coloring timer:
	m_uiBckgdTimerInterval = 500;
	m_nBckgdTimerNumOfLines = 10;
	m_bBckgdTimerActivated = FALSE;
}

CRichTextUI::~CRichTextUI()
{
	if (m_pLexer != NULL)
	{
		delete m_pLexer;
	}

	if (m_pLineEndState != NULL)
	{
		free(m_pLineEndState);	
		m_pLineEndState = NULL;
	}
}

LPCTSTR CRichTextUI::GetClass() const
{
	return _T("RichTextUI");
}

LPVOID CRichTextUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, DUI_CTR_RICHTEXT) == 0)
		return static_cast<CRichTextUI*>(this);
	else
		return CRichEditUI::GetInterface(pstrName);
}

void CRichTextUI::DoInit()
{
	CRichEditUI::DoInit();

	//TxSendMessage(EM_EXLIMITTEXT, 0, 0x7FFFFFFF); 
	//SetLimitText(0x7FFFFFFF);

	//CHARFORMAT2 cfText;
	//GetDefaultCharFormat(cfText);

	//PARAFORMAT2 pf;
	//GetParaFormat(pf);
	//pf.cTabCount = 4;
	//pf.bLineSpacingRule = 0;
	//pf.dwMask |= PFM_LINESPACING;
	//SetParaFormat(pf);
	//// Set the event mask to include ENM_CHANGE
	//LONG lMask = GetEventMask();
	//lMask |= ENM_CHANGE | ENM_SCROLL;
	//SetEventMask(lMask);

	//// Set the default character format to be m_cfText
	//InitlizeTagCharFormat(cfText);
	//InitlizeTextCharFormat(cfText);
	//InitlizeCommentCharFormat(cfText);
	//InitlizeQuoteCharFormat(cfText);

	//SetDefaultCharFormat(m_cfText);

	LPTSTR sKeywords = _T("for,for,else,main,struct,enum,switch,auto,")
										_T("template,explicit,this,bool,extern,thread,break,false,")
										_T("throw,case,namespace,true,catch,new,try,float,noreturn,")
										_T("char,operator,typedef,class,friend,private,const,goto,")
										_T("protected,typename,if,public,union,continue,inline,")
										_T("unsigned,using,directive,default,int,return,delete,short,")
										_T("signed,virtual,sizeof,void,do,static,double,long,while");
	LPTSTR sDirectives = _T("#define,#elif,#else,#endif,#error,#ifdef,")
										_T("#ifndef,#import,#include,#line,#pragma,#undef");
	LPTSTR sPragmas = _T("comment,optimize,auto_inline,once,warning,")
										_T("component,pack,function,intrinsic,setlocale,hdrstop,message");

	m_pLexer->ClearKeywordList();
	m_pLexer->AddKeyword(sKeywords,RGB(0,0,255),GRP_KEYWORD);
	m_pLexer->AddKeyword(sDirectives,RGB(0,0,255),GRP_DIRECTIVE);
	m_pLexer->AddKeyword(sPragmas,RGB(0,0,255),GRP_PRAGMA);
	m_pLexer->AddKeyword(_T("#"),RGB(255,0,255),4);

	/*PARAFORMAT2 MyFormat;
	ZeroMemory(&MyFormat, sizeof(MyFormat));
	MyFormat.cbSize=sizeof(MyFormat);
	GetParaFormat(MyFormat);
	MyFormat.dwMask |=PFM_LINESPACING;
	MyFormat.dyLineSpacing=300;
	MyFormat.bLineSpacingRule=0;
	SetParaFormat(MyFormat);*/

	SetTabStops(1,4);

	// The CRichEditCtrl always starts with LineCount == 1, so you can start typing text immediately
	// NOTE: When CRichEditCtrl used from resource, OnCreate isn't called and GetLineCount returns 1.
	// When used as a member and Create() is explicitly called, GetLineCount here return 0 and in OnCreate returns 1.
	m_nLineCount = 1;
	m_pLineEndState = (BYTE*)malloc(sizeof(BYTE));
	ASSERT(m_pLineEndState != NULL);
	m_pLineEndState[0] = RICHTEXT_INTEXT;	// Until changed by <, " and so, we're in state "text"
}

void CRichTextUI::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CControlUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_TIMER)
	{
		if (event.wParam == TIMER_BACKGROUNDCOLORING)
		{
			BOOL bColorEnabled = FALSE;
			CHARRANGE crPos;
			GetSel(crPos);
			int nCharPos = crPos.cpMin;
			// 1. Control is in focus, caret is at beginning of line and no selection is made
			if (IsFocused() && crPos.cpMin == crPos.cpMax && nCharPos == GetLineIndex(GetLineFromChar(nCharPos)))
				bColorEnabled = TRUE;
			// 2. Control is not in focus and its window completely obscured
			else if (IsWindowCompletelyObscured())
				bColorEnabled = TRUE;

			if (bColorEnabled == FALSE)
				return;	// To prevent flickering give up background coloring at this stage

			int nFirstVisibleLine = GetFirstVisibleLine();

			SetRedraw(FALSE);

			CDuiString strLineText;
			int nColored = 0;

			m_bOnEnVscrollDisabled = TRUE;	// Clicking the last visible line would cause OnEnVscroll when line only partially visible	
			for (int n = 0; n < m_nLineCount && nColored < m_nBckgdTimerNumOfLines; ++n)
			{
				if ((m_pLineEndState[n] & LINE_COLORED) == 0)
				{
					// Colour the all the text as Text initially, use nCharPosition for optimization:
					int nLineLength = GetLineHelper(n, strLineText);
					LONG lCharStart	= GetLineIndex(n);

					SetSel(lCharStart, lCharStart + nLineLength);
					SetSelectionCharFormat(m_cfText);

					ParseLines(strLineText, -1 , TRUE, n);
					m_pLineEndState[n] |= LINE_COLORED;

					nColored++;
				}
			}

			if (nColored < m_nBckgdTimerNumOfLines)
			{
				// All lines are colored? The background coloring timer can be turned off:
				m_pManager->KillTimer(this, TIMER_BACKGROUNDCOLORING);
				m_bBckgdTimerActivated = FALSE;
			}

			// Restore after painting:
			SetSel(crPos);
			//int nCurrentFirstVisibleLine = GetFirstVisibleLine();	
			//if (nCurrentFirstVisibleLine != nFirstVisibleLine)
			//{
			//	// Coloring might scroll the control, so restore original visible line.
			//	// OnEnVscroll is disabled at this stage because otherwise we get into endless recurssion
			//	LineScroll(nFirstVisibleLine - nCurrentFirstVisibleLine, 0);
			//}
			m_bOnEnVscrollDisabled = FALSE;

			SetRedraw(TRUE);
			Invalidate();
			return ;
		} 
	} 
	CRichEditUI::DoEvent(event);
}

void CRichTextUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CRichEditUI::DoPaint(hDC, rcPaint);
	// °´Ðè»æÖÆÐÐºÅ£¬ÕÛµþ£¬ÊéÇ©µÈ¹¦ÄÜ

	//UINT nLineCount = GetLineCount();

	//CHARFORMAT2     CharFmt;
	//RtlZeroMemory( &CharFmt, sizeof(CharFmt) );
	//CharFmt.cbSize = sizeof( CharFmt );
	///*SendMessage( hEdit, EM_GETCHARFORMAT, TRUE, (long)&CharFmt );*/
	//GetDefaultCharFormat(CharFmt);
	//int      CharHeight = CharFmt.yHeight / 20; 
	//int      chHeight = CharHeight;        //×Ö·ûµÄ¸ß¶È£¬³£Á¿ 
	//CharHeight = 1;

	//UINT nFirstLine = GetFirstVisibleLine();
	//UINT nLastLine = GetLastVisibleLine();
	//TCHAR szLine[10];
	//for (UINT nLine = nFirstLine; nLine<nLastLine; ++nLine)
	//{
	//	RECT rcLine;
	//	rcLine.left = 0;
	//	rcLine.top = CharHeight;
	//	rcLine.right = 30;
	//	rcLine.bottom = rcLine.top + chHeight;
	//	CDuiString strTipInfo;
	//	strTipInfo.Format(_T("left:%d,top:%d,right:%d,bottom:%d"),rcLine.left,rcLine.top,rcLine.right,rcLine.bottom);
	//	OutputDebugString(strTipInfo);
	//	_stprintf_s(szLine, _T("%4u"), nLine);
	//	/*TextOut(hDC, 1, CharHeight, _countof(szLine), szLine);*/
	//	CRenderEngine::DrawText(hDC, m_pManager, rcLine,szLine, m_dwTextColor, m_iFont, DT_SINGLELINE|DT_RIGHT|DT_BOTTOM);
	//	CharHeight += chHeight + 4;
	//}
}

void CRichTextUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("linenum")) == 0)	SetShowLineNum(_tcsicmp(pstrValue, _T("true")) == 0);
	else
		CRichEditUI::SetAttribute(pstrName, pstrValue);
}

LRESULT CRichTextUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//if (uMsg == WM_CHAR)
	//{
	//	HRESULT hRet = CRichEditUI::MessageHandler(uMsg, wParam, lParam, bHandled);
	//	if (bHandled)
	//		OnChar((UINT)wParam);
	//	return hRet;		
	//} /*else if (uMsg == WM_KEYDOWN) {
	//	LRESULT lRet = CRichEditUI::MessageHandler(uMsg, wParam, lParam, bHandled);
	//	if (bHandled == TRUE)
	//		OnKeyDown((UINT)wParam);

	//	return lRet;
	//} */else
		return CRichEditUI::MessageHandler(uMsg, wParam, lParam ,bHandled);
}

void CRichTextUI::OnChar(UINT nChar/*, UINT nRepCnt, UINT nFlags*/) 
{
	// Get current caret position. Not sure if GetSel the best choice???
	// NOTE: We call GetSel before base class to get the correct caret position
	//LONG lStart = 0, lEnd	= 0;
	//GetSel(lStart, lEnd);
	//int nCharPosition = lStart;

	//// After CRichEditCtrl has added the char, we can get the following:
	//LONG lCharStart	= GetLineIndex();	// Retrieves the character index of the current line (the line that contains the caret)
	//LONG lCharFromLineStart = nCharPosition - lCharStart;
	//int nLineIndex = GetLineFromChar(lCharStart);
	//
	//// Is it a special char? If not, basically the RichEditCtrl maintains himself the current WordCharFormat.
	//// However this may change when pressing a key breaks a comment combination or after the insertion point
	//// was moved just passed an ending tag or ending double quotes. This will be handled
	//// by the 'default' clause
	//switch(nChar)
	//{
	//case KEY_TAG_START :
	//	{// Invalide the colors of this and next lines as we might move to epsInTag
	//		InvalidateColorStates(nCharPosition);
	//		break;
	//	}
	//case KEY_TAG_END :
	//	{// Invalide the colors of this and next lines as we might move to InNormalText (or stay in DblQuotes\Comment)
	//	// Recalculate the color state of this line because after '>' we might move to InNormalText or stay in DblQuotes\Comment.
	//	 // However the color of this '>' stays the same as the previous char (Tag\DblQuotes\Comment\NormalText)	
	//			InvalidateColorStates(nCharPosition);
	//		break;
	//	}
	//case KEY_DBL_QUOTE:
	//	{// Recalculate the color state of this line because after '"' we might move to DblQuotes\Tag or stay in Comment\NormalText.
	//	 // The color of this '" changes if me move to DblQuotes and affects the following lines as well, so invalide the colors of this and next lines
	//		InvalidateColorStates(nCharPosition);
	//		if ((m_pLineEndState[nLineIndex] &~ LINE_COLORED) == RICHTEXT_INTAG)	// These were ending double quotes so now we're in Tag
	//			{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
	//		InsertText(lStart, _T("\""), true);
	//		SetSel(lStart,lStart);
	//		break;
	//	}
	//case '{':
	//		InsertText(lStart, _T("\r\t\r}"), true);
	//	SetSel(lStart+2,lStart+2);
	//	break;
	//case '(':
	//		InsertText(lStart, _T(")"), true);
	//		SetSel(lStart,lStart);
	//	break;
	//case '*' :
	//	{
	//		// Trap the Comment start ("<!--") and Comment end combinations, so we can color correctly in these case:
	//		// Check if we move to epsInComment:
	//		int nCommentStart = FindCommentStartHelper(nCharPosition);
	//		if (nCommentStart != -1)
	//			InvalidateColorStates(nCommentStart);
	//		else
	//		{// Check if we move to epsInNormalText
	//			CString strTmp = GetTextRange(nCharPosition - 1, nCharPosition + 2);	// Search for "-->" and this char is the middle '-'
	//			if (strTmp != _T("*/"))
	//				strTmp = GetTextRange(nCharPosition, nCharPosition + 3);	// Search for "-->" and this char is the leftmost '-'
	//			if (strTmp == _T("*/"))	// Search for "-->" and this char is the leftmost '-'
	//				InvalidateColorStates(nCharPosition);
	//		}
	//		break;
	//	}
	//case '/' :	// Trap the Comment start ("<!--"), so we can color correctly when state moves to epsInComment:
	//	{
	//		int nCommentStart = FindCommentStartHelper(nCharPosition+2);
	//		if (nCommentStart != -1)	InvalidateColorStates(nCommentStart);
	//		break;
	//	}
	//case VK_RETURN :
	//	{
	//		UpdateLinesArraySize();	// Since a new line has just been added

	//		// After VK_RETURN we must explicitly set the correct color:
	//		SetFirstLineCharColor(nLineIndex);
	//		break;
	//	}
	//default:
	//	{
	//		if (nChar != VK_BACK)	// VK_BACK was already handled by OnKeyDown
	//		{// Pressing a key might break comment combination.
	//		 // Pressing a key after the insertion point might result in a wrong color when we're
	//         // just passed an ending tag or ending double quotes.
	//			int nCommentStart = FindCommentStartHelper(nCharPosition);
	//			if (nCommentStart != -1)	// Just broke comment start combination
	//				InvalidateColorStates(nCommentStart);
	//			else
	//			{					
	//				int nCommentEnd = FindCommentEndHelper(nCharPosition);	// +1 to pass the currently added char 
	//				if (nCommentEnd != -1)	// Just broke comment end combination
	//					InvalidateColorStates(nCommentStart);
	//				else
	//				{
	//					CString prevChar = GetTextRange(nCharPosition - 1 , nCharPosition);
	//					if (prevChar == _T('>') || prevChar == _T('\"'))	// We just past ending tag/ending quotes)
	//						InvalidateColorStates(nCharPosition - 1);
	//				}
	//			}
	//		}
	//	}
	//}
	//ParseAllLines();
	
	return;
}

/**
 * OnChange is used to handle the cases of overriding selected text.
 * In these cases OnChange is called after OnKeyDown, thus OnKeyDown can't
 * see the updated text and defers the coloring to OnChange.
 */
void CRichTextUI::OnChange() 
{
	//if (m_nOnChangeCharPosition != -1)
	//{// OnKeyDown defers the call to OnChange when overriding selected text
	//	UpdateLinesArraySize();
	//	InvalidateColorStates(m_nOnChangeCharPosition);
	//	m_nOnChangeCharPosition = -1;
	//}
	SetRedraw(FALSE);
	if (m_pLexer != NULL)
	{
		CHARRANGE crPos;
		GetSel(crPos);
		m_pLexer->Colorize(0,-1,this);
		SetSel(crPos);
	}
	SetRedraw(TRUE);
}

void CRichTextUI::SetPos(RECT rc)
{
	CRichEditUI::SetPos(rc);

	m_nLineCount = GetLineCount();
	if (m_nLineCount > 1)
		ColorVisibleLines();
}

void CRichTextUI::OnTxNotify(DWORD iNotify, void *pv)
{
	switch(iNotify)
	{ 
	case EN_CHANGE:
		{
			GetManager()->SendNotify(this, DUI_MSGTYPE_TEXTCHANGED);
			OnChange();
		}
	default:
		CRichEditUI::OnTxNotify(iNotify, pv);
	}
}

void CRichTextUI::InitlizeTagCharFormat(CHARFORMAT2& cfTag)
{
	m_cfTags.yHeight = cfTag.yHeight;
	_tcscpy(m_cfTags.szFaceName, cfTag.szFaceName);
}

// Sets the character format to be used for Tags
void CRichTextUI::SetTagCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse)
{
	m_cfTags.yHeight		= 20 * nFontHeight;
	m_cfTags.crTextColor	= clrFontColour;
	_tcscpy(m_cfTags.szFaceName, lpszFontFace);

	if (bParse)
		ParseAllLines();
}


// Sets  the character format to be used for Tags
void CRichTextUI::SetTagCharFormat(CHARFORMAT2& cfTags, BOOL bParse)
{
	m_cfTags = cfTags;
	if (bParse)
		ParseAllLines();
}

void CRichTextUI::InitlizeQuoteCharFormat(CHARFORMAT2& cfQuote)
{
	m_cfQuoted.yHeight = cfQuote.yHeight;
	_tcscpy(m_cfQuoted.szFaceName, cfQuote.szFaceName);
}

// Sets the character format to be used for Quoted text
void CRichTextUI::SetQuoteCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse)
{
	m_cfQuoted.yHeight		= 20 * nFontHeight;
	m_cfQuoted.crTextColor	= clrFontColour;
	_tcscpy(m_cfQuoted.szFaceName, lpszFontFace);

	if (bParse)
		ParseAllLines();
}

// Sets  the character format to be used for Quoted text
void CRichTextUI::SetQuoteCharFormat(CHARFORMAT2& cfQuoted, BOOL bParse)
{
	m_cfQuoted = cfQuoted;
	if (bParse)	
		ParseAllLines();
}

void CRichTextUI::InitlizeCommentCharFormat(CHARFORMAT2& cfComment)
{
	m_cfComment.yHeight = cfComment.yHeight;
	_tcscpy(m_cfComment.szFaceName, cfComment.szFaceName);
}

// Sets the character format to be used for Comments
void CRichTextUI::SetCommentCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse)
{
	m_cfComment.yHeight		= 20 * nFontHeight;
	m_cfComment.crTextColor	= clrFontColour;
	_tcscpy(m_cfComment.szFaceName, lpszFontFace);

	if (bParse)	
		ParseAllLines();
}

// Sets  the character format to be used for Comments
void CRichTextUI::SetCommentCharFormat(CHARFORMAT2& cfComments, BOOL bParse)
{
	m_cfComment = cfComments;
	if(bParse)	
		ParseAllLines();
}

void CRichTextUI::InitlizeTextCharFormat(CHARFORMAT2& cfText)
{
	m_cfText.yHeight = cfText.yHeight;
	_tcscpy(m_cfText.szFaceName, cfText.szFaceName);
}

// Sets the character format to be used for Normal Text
void CRichTextUI::SetTextCharFormat(int nFontHeight, COLORREF clrFontColour, LPCTSTR lpszFontFace, BOOL bParse)
{
	m_cfText.yHeight		= 20 * nFontHeight;
	m_cfText.crTextColor	= clrFontColour;
	_tcscpy(m_cfText.szFaceName, lpszFontFace);

	if (bParse)	
		ParseAllLines();
}

// Sets  the character format to be used for Normal Text
void CRichTextUI::SetTextCharFormat(CHARFORMAT2& cfText, BOOL bParse)
{
	m_cfText = cfText;
	if (bParse)	
		ParseAllLines();
}

void CRichTextUI::ParseAllLines()
{
	m_nLineCount = GetLineCount();

	if (m_pLineEndState != NULL)
		free(m_pLineEndState);
	m_pLineEndState = (BYTE*)malloc(m_nLineCount);
	ZeroMemory(m_pLineEndState, m_nLineCount);

	// Go over HTML line by line and calculate the color state (Comment/Quoted/Tag/Normal)
	// of the last char.
	// NOTE: This calulation is quite fast as we do it in one pass over HTML without any coloring.
	//ParseLines(GetText(), -1, FALSE, 0);

	// Disable redraw to prevent flickering
	SetRedraw(FALSE);

	CHARRANGE crPos;
	GetSel(crPos);

	SetSel(0, -1);
	SetWordCharFormat(m_cfText);

	// Get the control's visible range
	int nFirstLine = GetFirstVisibleLine();	// Send the control EM_GETFIRSTVISIBLELINE message
	int nLastLine = GetLastVisibleLine();	// No such message as EM_GETLASTVISIBLELINE - use our own algorithm
	for (int n=nFirstLine; n<=nLastLine; ++n)
	{
		CDuiString strLine;
		int nLineLength = GetLineHelper(n, strLine);
		if (nLineLength > 0)
		{
			// Color the line (only one line)
			ParseLines(strLine, -1, TRUE, n);
			m_pLineEndState[n] |= LINE_COLORED;
		}
	}

	// Restore the original selection
	SetSel(crPos);
	// Restore the original view position
	//LineScroll(-nFirstLine, 0);

	SetRedraw(TRUE);
	Invalidate();

	// Activate the background coloring timer
	StartColoringTimer();
}

RICHTEXT_COLOR_STATE CRichTextUI::ParseLines(LPCTSTR lpszLines, int nCharPos, BOOL bColor, int nCurrentLine/* = -1*/)
{
	if (nCurrentLine == -1)
		nCurrentLine = GetLineFromChar(nCharPos);

	// Get color state of beginning char (same as previous line's ending char).
	// For first line we use InNormalText, as chars preceding  '<' are considered as normal text.
	RICHTEXT_COLOR_STATE ColorState = (nCurrentLine == 0) ? RICHTEXT_INTEXT : (RICHTEXT_COLOR_STATE)(m_pLineEndState[nCurrentLine - 1] & (~LINE_COLORED));

	// Take care of empty ("") strings
	// When caret is at the end position and user presses the enter, we get a new empty line (OnChar).
	// OnTimer also calls ParseLines with an empty string in the background
	if (*lpszLines == NULL)
	{
		m_pLineEndState[nCurrentLine] = ColorState;
		// Apply color state of previous line's ending char:
		if (bColor)
		{
			if (ColorState == RICHTEXT_INTAG)
				SetWordCharFormat(m_cfTags);
			else if (ColorState == RICHTEXT_INQUOTES)
				SetWordCharFormat(m_cfQuoted);
			else if (ColorState == RICHTEXT_INCOMMENT)
				SetWordCharFormat(m_cfComment);
			else
				SetWordCharFormat(m_cfText);
			m_pLineEndState[nCurrentLine] |= LINE_COLORED;
		}
		return ColorState;
	}

	const TCHAR* pCurChar = lpszLines;
	LONG lCharStart = GetLineIndex(nCurrentLine);
	int nColorStart = -1;

	// loop while not whole line has been coloured:
	while(*pCurChar != NULL)
	{
		if (*pCurChar == _T('\r') || *pCurChar == _T('\n'))
		{
			// EOL is reached? Set the ending-char color state:
			if (*pCurChar == _T('\r' && *(pCurChar+1) == _T('\n')))		// Take care of \r\n pattern
				++pCurChar;
			// If ending-char color state of this line hasn't changed - no point of recalculating next lines 
			if ((m_pLineEndState[nCurrentLine] & ~LINE_COLORED) == ColorState)
				break;
		}
		else if (ColorState == RICHTEXT_INCOMMENT)
		{
			// Inside Comment all chars are acceptable. The state is only changed by the "-->" combination:
			if ((*pCurChar == KEY_TAG_END) && (*(pCurChar - 1) == _T('-')) && (*(pCurChar - 2) == _T('-')))
			{
				// Colourise the Comment (If no Comment in this line - we found a Comment end of a previous line):
				if (bColor)
					nColorStart = ColorRangeHelper((nColorStart - 1) ? lCharStart : nColorStart, lCharStart + pCurChar - lpszLines + 1, m_cfComment, nCharPos);
				// After leaving InComment state, all chars till '<' are considered normal text chars
				ColorState = RICHTEXT_INTEXT;
			}
		}
		else if (*pCurChar == KEY_TAG_START)
		{
			// '<' can start a Tag or a Comment block:
			if (ColorState == RICHTEXT_INTAG || ColorState == RICHTEXT_INQUOTES)
			{
				// IE doesn't allow '<' to be inside quotes, thus "d<d" is illegal. However I'll handle it as moving to epsInNormalText state:
				ColorState = RICHTEXT_INTEXT;
			}
			else if (*(pCurChar + 1) == _T('!') && *(pCurChar + 2) == _T('-') && *(pCurChar + 3) == _T('-'))
			{
				// If we reach "<!--" we're staring a Comment
				ColorState = RICHTEXT_INCOMMENT;
			}
			else
				ColorState = RICHTEXT_INTAG;
			nColorStart = pCurChar - lpszLines + lCharStart;
		}
		else if (*pCurChar == _T(KEY_DBL_QUOTE))
		{
			// Double quotes ('"') can be starting or ending quotes.
			// However quotes are applicable in Tag only (in InNormalText we'll treat them as regular chars)
			if (ColorState == RICHTEXT_INQUOTES)
			{
				// These are ending quotes
				// Colourise the string (If no staring quotes found in this line, we're ending a Quoted Text of a previous line):
				if (bColor)
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - lpszLines + 1, m_cfQuoted, nCharPos);
				// Assumption: Before we entered the InDblQuotes state we were in a Tag:
				ColorState = RICHTEXT_INTAG;
			}
			else if (ColorState == RICHTEXT_INTAG)
			{
				// These are beginning quotes:
				if (bColor)
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - lpszLines, m_cfTags, nCharPos);
				ColorState = RICHTEXT_INQUOTES;
			}
		}
		else if (*pCurChar == _T(KEY_TAG_END))
		{
			// Ending tag ('>'):
			// '>' in normal text has no meaning, for example >"va>lue" as text is valid
			if (ColorState != RICHTEXT_INTEXT)
			{
				if (ColorState == RICHTEXT_INTAG)
				{
					if (bColor)
						nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - lpszLines + 1, m_cfTags, nCharPos);
					ColorState = RICHTEXT_INTEXT;
				}
				// If '>' is part of a string, for example ("d>d"), leave the InDblQuotes state
				//ASSERT(currentState == epsInDblQuotes);	//It can also be part of a string, for example ("d>d")
			}
		}
		++pCurChar;
	}

	// The = is because nColorStart position should be colored as well
	if (bColor && (pCurChar - 1 - lpszLines) >= (nColorStart - lCharStart))
	{
		// Then there are uncolored chars left till end of line. These are part of a multiline Tag/DblQuotes/Comment/NormalText
		// Haven't found any interesting keys - color whole line according to previous state
		if (nColorStart == -1)
			nColorStart = lCharStart;
		SetSel(nColorStart, lCharStart + pCurChar - lpszLines);
		if (ColorState == RICHTEXT_INTAG)
			SetWordCharFormat(m_cfTags);
		else if (ColorState == RICHTEXT_INQUOTES)
			SetWordCharFormat(m_cfQuoted);
		else if (ColorState == RICHTEXT_INCOMMENT)
			SetWordCharFormat(m_cfComment);
	}

	if (nCurrentLine < m_nLineCount)
		m_pLineEndState[nCurrentLine] = ColorState;

	return ColorState;
}

// Upon OnChar or OnChange the control's line count might have been changed
// (pressing enter, pasting text, deleting text)
void CRichTextUI::UpdateLinesArraySize()
{
	int nLineCount = GetLineCount();
	// If new lines have been added to the control - expand the color state array:
	if (m_nLineCount < nLineCount)
	{
		// Reallocate the m_pLinesEndState buffer:
		int nPrevLineCount = m_nLineCount;
		m_pLineEndState = (BYTE*)realloc(m_pLineEndState, nLineCount * sizeof(BYTE));	
		ASSERT(m_pLineEndState != NULL);

		ZeroMemory(m_pLineEndState + m_nLineCount * sizeof(BYTE), (nLineCount - m_nLineCount) * sizeof(BYTE));	// Clear color state of new lines
		m_nLineCount = nLineCount;	// Update line counter to reflect new array size

		LONG lCharStart	= GetLineIndex(nPrevLineCount);	// Get first char of new line (using previous m_nLineCount)

		// Get range of all new lines, starting with current line:
		CDuiString strCtrlText = GetTextRange(lCharStart, GetTextLength());

		// Parse color states of all new lines (without coloring)
		ParseLines(strCtrlText, -1, FALSE, nPrevLineCount);

		// If lines have been removed - just update the m_nLineCount member. realloc isn't necessary
		m_nLineCount = nLineCount;
	}
}

/**
 * NOTE: When I press enter on the first line add press chars on the second line, GetLine
 * always returns empty strings???
 */
int CRichTextUI::GetLineHelper(int nLineIndex, CDuiString& strLine, int nLineLength /* = -1 */)
{
	if (nLineLength == -1)	
		nLineLength = GetLineLength(GetLineIndex(nLineIndex));
	
	strLine = GetLine(nLineIndex, nLineLength);

	// The RichEditCtrl sometimes appends \r and sometimes \r\n so remove them:
	//TrimRightCrLfHelper(strLine, nLineLength);
	strLine.TrimRight(_T("\r\n"));

	return strLine.GetLength();
}

/**
 * This helper is used by ParseLines when called with bColor = "true".
 * When recoloring is required as a reaction to user-editing, nColorFromChar states the position
 * from which recoloring is required (caret position)
 * Return value is the updated nColorStart value (which is nColorEnd)
 */
int CRichTextUI::ColorRangeHelper(int nColorStart, int nColorEnd, CHARFORMAT2 cfText, int nColorFromChar /* = -1 */)
{
	if (nColorStart < nColorFromChar)	
		nColorStart = nColorFromChar;

	if (nColorStart < nColorEnd)
	{
		SetSel(nColorStart, nColorEnd);
		SetWordCharFormat(cfText);
	}

	return nColorEnd;
}


// Activate the background coloring timer if CRichEditCtrl has the focus.
// Otherwise we'll just wait till WM_SETFOCUS
void CRichTextUI::StartColoringTimer()
{
	if (m_uiBckgdTimerInterval == 0 || m_nBckgdTimerNumOfLines <= 0)
		return;	// Timer was disabled by a call to SetBckgdColorTimer

	m_bBckgdTimerActivated = true;
	m_pManager->SetTimer(this, TIMER_BACKGROUNDCOLORING, m_uiBckgdTimerInterval);
}

// CURRENTLY NOT USED
void CRichTextUI::StopColoringTimer()
{
	if (m_bBckgdTimerActivated)
	{// Disable current background coloring timer:
		BOOL bRes = m_pManager->KillTimer(this, TIMER_BACKGROUNDCOLORING);	ASSERT(bRes);
		m_bBckgdTimerActivated = false;
	}
}

/**
 * This helper was found somewhere on the net
 */
BOOL CRichTextUI::IsWindowCompletelyObscured()
{
	RECT rcClip, rcWnd; 

	HWND hMainWnd = m_pManager->GetPaintWindow();
	// Get Clipping box for window area (client not enough since I want to include the scrolls) 
	HDC hDC = ::GetWindowDC(hMainWnd);
	int nClipType = GetClipBox(hDC, &rcClip);	// In logical units
	::ReleaseDC(hMainWnd, hDC);

	// Check clipbox type
	if (nClipType == NULLREGION)
		return TRUE;	// Completely covered
	else if (nClipType == COMPLEXREGION)
		return FALSE;	// Partially covered
	else if (nClipType == SIMPLEREGION)
	{
		GetWindowRect(hMainWnd, &rcWnd);
		// Normalize coordinates:
		rcWnd.bottom -= rcWnd.top;
		rcWnd.top = 0;
		rcWnd.right -= rcWnd.left;
		rcWnd.left = 0;
        if (EqualRect(&rcClip,&rcWnd))
			return FALSE;	//completely exposed 
		else
			return TRUE;	//completely covered
	}
	return FALSE;
}

void CRichTextUI::ColorVisibleLines(int nCharPosition /*= -1*/)
{
	// Color visible lines between [nLineIndex..m_nLineCount) that are not colored already, one line at a time:	
	CDuiString strLine;
	int nFirstLine = GetFirstVisibleLine();	// Send the control EM_GETFIRSTVISIBLELINE message
	int nLastLine = GetLastVisibleLine();	// No such message as EM_GETLASTVISIBLELINE - use our own algorithm
	int nOrigFirstVisibleLine = nFirstLine;
	long nCharPositionLine = -1;
	if (nCharPosition != -1)
	{// Adjust nFirstLine to the line containing nCharPosition
		nCharPositionLine	= GetLineFromChar(nCharPosition);
		if (nFirstLine < nCharPositionLine)	nFirstLine = nCharPositionLine;
	}

	// Prepare for paint (disable redraw to prevent flickering):
	CHARRANGE crCurrent;
	SetRedraw(FALSE);
	GetSel(crCurrent);

	m_bOnEnVscrollDisabled = true;	// Clicking the last visible line would cause OnEnVscroll when line only partially visible
	for (int i = nFirstLine; i <= nLastLine; i++)
	{
		if (!(m_pLineEndState[i] & LINE_COLORED))
		{
			int nFirstLineDEBUG2 = GetFirstVisibleLine();

			// Colour the all the text as Text initially, use nCharPosition for optimization:
			int nLineLength = GetLineHelper(i, strLine);
			long lCharStart	= GetLineIndex(i);
			if (i == nCharPositionLine)
			{
				nLineLength -= (nCharPosition - lCharStart);
				lCharStart = nCharPosition;
			}
			SetSel(lCharStart, lCharStart + nLineLength);
			SetWordCharFormat(m_cfText);

			ParseLines((LPCTSTR)strLine, (i == nCharPositionLine) ? nCharPosition : -1 , true, i);
			m_pLineEndState[i] |= LINE_COLORED;
		}
	}

	// Restore after painting:
	SetSel(crCurrent);
	int nCurrentFirstVisibleLine = GetFirstVisibleLine();	
	if (nCurrentFirstVisibleLine != nOrigFirstVisibleLine)
	{// Coloring might scroll the control, so restore original visible line.
		// OnEnVscroll is disabled at this stage because otherwise we get into endless recurssion
		LineScroll(nOrigFirstVisibleLine - nCurrentFirstVisibleLine, 0);
	}
	m_bOnEnVscrollDisabled = false;
	SetRedraw(TRUE);
	Invalidate();
}

void CRichTextUI::InvalidateColorStates(int nCharPosition)
{
	// Invalidate all lines starting from nLineIndex:
	int nLineIndex = GetLineFromChar(nCharPosition);
	LONG lCharStart	= GetLineIndex(nLineIndex);

	CDuiString strCtrlText = GetTextRange(lCharStart, GetTextLength());
	ParseLines(strCtrlText, -1, false, nLineIndex);

	// Color visible lines between [nLineIndex..m_nLineCount) that are not colored already, one line at a time:	
	ColorVisibleLines(nCharPosition);

	// Activate the background coloring timer
	StartColoringTimer();
}

/**
 * Pressing '-' or '!' might bring us to a comment start.
 * Pressing a key inside "<!--" breaks a comment start.
 * Deleting/Pasting inside "<!--" breaks a comment start.
 * Return value: The invalidat position (up to 3 chars before nCharPosition) or -1 of none found
 */
int CRichTextUI::FindCommentStartHelper(int nCharPosition)
{
	CDuiString strComment = GetTextRange(nCharPosition - 2, nCharPosition);
	int nIndex = strComment.Find(_T("/*"));
	if (nIndex == -1)
		return nIndex;
	return nCharPosition - (2 - nIndex);
}

int CRichTextUI::FindCommentEndHelper(int nCharPosition)
{
 //  CString str1forward = GetTextRange(nCharPosition + 1, nCharPosition + 2);	// Search for "-->" and this char is the middle '-'
	//CString str2forward = GetTextRange(nCharPosition + 1, nCharPosition + 3);
	//if (str1forward == _T('>'))				return  nCharPosition + 2;
	//else if (str2forward == _T("->"))		return  nCharPosition + 3;
	//else									return -1;
	CDuiString strComment = GetTextRange(nCharPosition - 1, nCharPosition+1);
	int nIndex = strComment.Find(_T("*/"));
	if (nIndex == -1)
		return nIndex;
	return nCharPosition - (2 - nIndex);
}

/**
 * When user presses VK_RETURN or sets caret at begining of line, we must explicitly
 * set the correct color. For example if user writes <a>, then moves caret to line start
 * and presses a char, this char gets the color of the Tag instead of the InNormalText
 */
void CRichTextUI::SetFirstLineCharColor(int nLineIndex)
{
	// First char of this new line gets the color state of previous line's ending char:
	RICHTEXT_COLOR_STATE prevState = (RICHTEXT_COLOR_STATE)(m_pLineEndState[nLineIndex - 1] &~ LINE_COLORED);
	if (prevState == RICHTEXT_INTAG)
		{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
	else if (prevState == RICHTEXT_INQUOTES)
		{BOOL bRes = SetWordCharFormat(m_cfQuoted);	ASSERT(bRes);}
	else if (prevState == RICHTEXT_INCOMMENT)
		{BOOL bRes = SetWordCharFormat(m_cfComment);	ASSERT(bRes);}
	else
		{BOOL bRes = SetWordCharFormat(m_cfText);	ASSERT(bRes);}
}

void CRichTextUI::OnKeyDown(UINT nChar/*, UINT nRepCnt, UINT nFlags*/)
{
	// TODO: Add your message handler code here and/or call default
	LONG lStart = 0, lEnd = 0;
	GetSel(lStart, lEnd);
	int nCharPosition = lStart;

	// If caret at begining of line, we must explicitly set the correct color:
	LONG lCharStart	= GetLineIndex();	// Retrieves the character index of the current line (the line that contains the caret)
	if (lCharStart == lStart)
	{
		int nLineIndex = GetLineFromChar(lCharStart);
		SetFirstLineCharColor(nLineIndex);
	}

	BOOL bDeferedToOnChange = FALSE;	// When selection is made, we have to defer the action to OnChange

	CDuiString strDeleted;	// For trapping the deleted char/chars (GetTextRange must be called before base class's OnKeyDown)

	// Pressing ctrl+v invalidates all chars from nCharPosition and might also scroll the control.
	// OnKeyDown's base class calls OnEnVScroll and finally OnChar. Therefore we can query for updated
	// line count only after call to base class's OnKeyDown is made (GetWindowText after OnKeyDown returns updated text)
	BOOL bIsPasting = FALSE;
	if (nChar == _T('V') && GetKeyState(VK_CONTROL) & 0x8000)
	{//check state of left and right CTRL keys. If high order bit is 1 - indicates the key is down
		bIsPasting = TRUE;
		m_bOnEnVscrollDisabled = TRUE;	// Disable OnEnVScroll because we don't have updated line count yet
	}
	else
	{// Not pasted text - check for deleted chars
		if (lStart != lEnd)
		{// A selection is made - pressing any "regular" char would delete the selection.
			// However we cannot detect the changes here, so we must defer the action to OnChange
			strDeleted = GetTextRange(lStart, (lEnd == lStart) ? (lEnd + 1) : lEnd);
			bDeferedToOnChange = TRUE;
		}
		else
		{// The RichEditCtrl seems to add a "regular" char in his OnChar handler.
			// However VK_BACK and VK_DELETE seem to be handled here (GetWindowText after OnKeyDown returns updated text)
			if (nChar == VK_DELETE)
			{// Trap the about-to-be-deleted char:
				strDeleted = GetTextRange(lStart, lEnd + 1);	// Delete without selection is the same as delete with selecting the next char
			}
			else if (nChar == VK_BACK)
			{// The char just-before-caret-possition will be deleted:
				strDeleted = GetTextRange(lStart - 1 , lStart);
				nCharPosition--;	// Because after pressing "back" the caret moves to the left
			}
			else if (nChar == VK_RETURN)
			{
				ParseAllLines();
			}
		}
	}

	if (bIsPasting)
	{// At this point the chars have already been pasted:
		m_bOnEnVscrollDisabled = FALSE;
		UpdateLinesArraySize();

		// Take care of breaking/completing "<!--" combination:
		int nCommentStart = FindCommentStartHelper(nCharPosition);
		if (nCommentStart != -1)
			InvalidateColorStates(nCommentStart);
	}
	else if (strDeleted.GetLength())
	{// At this point the chars have already been deleted:
		UpdateLinesArraySize();
		if (strDeleted.FindOneOf(_T("<>\"")) != -1)
		{
			bDeferedToOnChange ? (m_nOnChangeCharPosition = nCharPosition) : InvalidateColorStates(nCharPosition);
		}
		else
		{// Take care of breaking/completing "<!--" "-->" combinations:
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if (nCommentStart != -1)
				InvalidateColorStates(nCommentStart);
			else
			{
				int nCommentEnd = FindCommentEndHelper(nCharPosition - 1);	// For VK_DELETE and VK_BACK nCharPosition - 1 should be used
				if (nCommentEnd != -1)	// Just broke/completed comment end combination
					InvalidateColorStates(nCommentStart);
			}
		}
	}
}

void CRichTextUI::SetShowLineNum(BOOL bShowLineNumber)
{
	if (m_bShowLineNum == bShowLineNumber)
		return;

	m_bShowLineNum = bShowLineNumber;
	Invalidate();
}

BOOL CRichTextUI::IsShowLineNum() const
{
	return m_bShowLineNum;
}