#include "StdAfx.h"
#include "LexerMarker.h"

#define MAX_TABLE_LEN		256

#define CLR_STRING	RGB(55,0,200)
#define CLR_PLAIN	RGB(0,0,0)
#define CLR_COMMENT RGB(0,170,0)
#define CLR_KEYWORD RGB(0,0,255)
#define GRP_KEYWORD		0

CLexerMarker::CLexerMarker()
{
	m_cfDefault.dwMask = CFM_CHARSET | CFM_FACE | CFM_SIZE | CFM_OFFSET | CFM_COLOR;
	m_cfDefault.dwMask ^= CFM_ITALIC ^ CFM_BOLD ^ CFM_STRIKEOUT ^ CFM_UNDERLINE;
	m_cfDefault.dwEffects = 0;
	m_cfDefault.yHeight = 200; //10pts * 20 twips/point = 200 twips
	m_cfDefault.bCharSet = ANSI_CHARSET;
	m_cfDefault.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	m_cfDefault.yOffset = 0;
	m_cfDefault.crTextColor = CLR_PLAIN;
	_tcscpy(m_cfDefault.szFaceName, _T("Courier New"));
	m_cfDefault.cbSize = sizeof(m_cfDefault);

	m_cfComment = m_cfDefault;
	m_cfString = m_cfDefault;

	SetCommentColor(CLR_COMMENT);
	SetStringColor(CLR_STRING);

	CreateTables();
	m_pWordInfo = NULL;

	LPTSTR sKeywords = _T("__asm,else,main,struct,__assume,enum,")
										_T("__multiple_inheritance,switch,auto,__except,__single_inheritance,")
										_T("template,__based,explicit,__virtual_inheritance,this,bool,extern,")
										_T("mutable,thread,break,false,naked,throw,case,__fastcall,namespace,")
										_T("true,catch,__finally,new,try,__cdecl,float,noreturn,__try,char,for,")
										_T("operator,typedef,class,friend,private,typeid,const,goto,protected,")
										_T("typename,const_cast,if,public,union,continue,inline,register,")
										_T("unsigned,__declspec,__inline,reinterpret_cast,using,declaration,")
										_T("directive,default,int,return,uuid,delete,__int8,short,")
										_T("__uuidof,dllexport,__int16,signed,virtual,dllimport,__int32,sizeof,")
										_T("void,do,__int64,static,volatile,double,__leave,static_cast,wmain,")
										_T("dynamic_cast,long,__stdcall,while");
	LPTSTR sDirectives = _T("#define,#elif,#else,#endif,#error,#ifdef,")
										_T("#ifndef,#import,#include,#line,#pragma,#undef");
	LPTSTR sPragmas = _T("alloc_text,comment,init_seg1,optimize,auto_inline,")
										_T("component,inline_depth,pack,bss_seg,data_seg,")
										_T("inline_recursion,pointers_to_members1,check_stack,")
										_T("function,intrinsic,setlocale,code_seg,hdrstop,message,")
										_T("vtordisp1,const_seg,include_alias,once,warning"); 

	AddKeyword(sKeywords,CLR_KEYWORD,GRP_KEYWORD);
	AddKeyword(sDirectives,CLR_KEYWORD,GRP_KEYWORD);
	AddKeyword(sPragmas,CLR_KEYWORD,GRP_KEYWORD);
}

CLexerMarker::~CLexerMarker()
{
	DeleteTables();
	ClearKeywordList();
}

void CLexerMarker::AddKeyword(LPCTSTR lpszKeyword, COLORREF cr, int nGroup)
{
	LPTSTR lpToken = NULL;
	DWORD dwLen = _tcslen(lpszKeyword);
	LPTSTR lpKeyword = new TCHAR[dwLen *2];

	_tcscpy_s(lpKeyword, dwLen*2, lpszKeyword);

	CHARFORMAT2 cf = m_cfDefault;
	cf.dwMask |= CFM_COLOR;
	cf.crTextColor = cr;

	lpToken = _tcstok(lpKeyword, _T(","));
	while(lpToken != NULL)
	{
		if (_tcsicmp(lpToken, _T("rem")) == 0)
			*(m_pTableTwo + '\n') = SYNTAX_SLEND;
		AddKey(lpToken, cf, nGroup);
		lpToken = _tcstok(NULL, _T(","));
	}

	delete lpKeyword;
}

void CLexerMarker::AddKeyword(LPCTSTR lpszKeyword, CHARFORMAT2 cf, int nGroup)
{
	LPTSTR lpToken = NULL;
	DWORD dwLen = _tcslen(lpszKeyword);
	LPTSTR lpKeyword = new TCHAR[dwLen + 1];

	_tcscpy_s(lpKeyword, dwLen+1, lpszKeyword);

	lpToken = _tcstok(lpKeyword, _T(","));
	while(lpToken != NULL)
	{
		if (_tcsicmp(lpToken, _T("rem")) == 0)
			*(m_pTableTwo + '\n') = SYNTAX_SLEND;
		AddKey(lpToken, cf, nGroup);
		lpToken = _tcstok(NULL, _T(","));
	}

	delete lpKeyword;
}


void CLexerMarker::AddKey(LPCTSTR lpszKeyword, CHARFORMAT2 cf, int nGroup)
{
	WORDINFO* pWordInfo = new WORDINFO;
	ZeroMemory(pWordInfo, sizeof(WORDINFO));

	pWordInfo->lpKeyword = new TCHAR[_tcslen(lpszKeyword)+1];
	_tcscpy(pWordInfo->lpKeyword, lpszKeyword);

	pWordInfo->dwLen = _tcslen(lpszKeyword);
	pWordInfo->cf = cf;
	pWordInfo->nGroup = nGroup;
	pWordInfo->pNext = NULL;

	*(m_pTableZero + pWordInfo->lpKeyword[0]) = SYNTAX_KEYWORD;

	if (m_pWordInfo == NULL)
		m_pWordInfo = pWordInfo;
	else
	{
		if (_tcscmp(lpszKeyword, m_pWordInfo->lpKeyword) < 0) {
			pWordInfo->pNext = m_pWordInfo;
			m_pWordInfo = pWordInfo;
		} else if (_tcscmp(lpszKeyword, m_pWordInfo->lpKeyword) == 0) {
			pWordInfo->pNext = m_pWordInfo->pNext;
			delete[] m_pWordInfo->lpKeyword;
			m_pWordInfo = pWordInfo;
		} else {
			WORDINFO* pPrev = m_pWordInfo;
			WORDINFO* pNow = m_pWordInfo->pNext;

			while(pNow != NULL && _tcscmp(pNow->lpKeyword, lpszKeyword) < 0)
			{
				pPrev = pNow;
				pNow = pNow->pNext;
			}
			if (pNow != NULL && _tcscmp(pNow->lpKeyword, lpszKeyword) == 0)
			{
				pPrev->pNext = pWordInfo;
				pWordInfo->pNext = pNow->pNext;
				delete pNow->lpKeyword;
				delete pNow;
			}
			else
			{
				pWordInfo->pNext = pNow;
				pPrev->pNext = pWordInfo;
			}
		}
	}
}

void CLexerMarker::CreateTables()
{
	m_pTableZero = new BYTE[MAX_TABLE_LEN];			m_pTableOne = new BYTE[MAX_TABLE_LEN];
	m_pTableTwo = new BYTE[MAX_TABLE_LEN];			m_pTableThree = new BYTE[MAX_TABLE_LEN];
	m_pTableFour = new BYTE[MAX_TABLE_LEN];			m_pAllowable = new BYTE[MAX_TABLE_LEN];

	ZeroMemory(m_pTableZero, MAX_TABLE_LEN);		ZeroMemory(m_pTableOne, MAX_TABLE_LEN);
	ZeroMemory(m_pTableTwo, MAX_TABLE_LEN);		ZeroMemory(m_pTableThree, MAX_TABLE_LEN);
	ZeroMemory(m_pTableFour, MAX_TABLE_LEN);		ZeroMemory(m_pAllowable, MAX_TABLE_LEN);

	*(m_pTableZero + '"') = SYNTAX_DQSTART;		*(m_pTableZero + '\'') = SYNTAX_SQSTART;
	*(m_pTableZero + '/') = SYNTAX_CMSTART;		*(m_pTableOne + '"') = SYNTAX_DQEND;
	*(m_pTableTwo + '\'') = SYNTAX_SQEND;			*(m_pTableThree + '\n') = SYNTAX_SLEND;
	*(m_pTableFour + '*') = SYNTAX_MLEND;

	*(m_pAllowable + '\n') = true;	*(m_pAllowable + '\r') = true; 
	*(m_pAllowable + '\t') = true;		*(m_pAllowable + '\0') = true;
	*(m_pAllowable + ' ') = true;		*(m_pAllowable + ';') = true;
	*(m_pAllowable + '(') = true;		*(m_pAllowable + ')') = true;
	*(m_pAllowable + '{') = true;		*(m_pAllowable + '}') = true;
	*(m_pAllowable + '[') = true;		*(m_pAllowable + ']') = true;
	*(m_pAllowable + '*') = true;
}

void CLexerMarker::DeleteTables()
{
	delete[] m_pTableZero;
	delete[] m_pTableOne;
	delete[] m_pTableTwo;
	delete[] m_pTableThree;
	delete[] m_pTableFour;
	delete[] m_pAllowable;
}

void CLexerMarker::ClearKeywordList()
{
	WORDINFO* pWordInfo = m_pWordInfo;
	while(pWordInfo != NULL)
	{
		*(m_pTableZero + pWordInfo->lpKeyword[0]) = SYNTAX_SKIP;
		if (_tcsicmp(pWordInfo->lpKeyword, _T("rem")) == 0)
			*(m_pTableTwo + '\n') = SYNTAX_SKIP;

		WORDINFO* pNextWord = pWordInfo->pNext;
		delete pWordInfo->lpKeyword;
		delete pWordInfo;
		pWordInfo = pNextWord;
	}
	m_pWordInfo = NULL;
}

void CLexerMarker::Colorize(CHARRANGE cfPos, CRichTextUI* pRichText)
{

}

void CLexerMarker::Colorize(LONG lStart, LONG lEnd, CRichTextUI* pRichText)
{
	LONG nTextLength = 0;

	if(lStart == 0 && lEnd == -1) //send entire text of rich edit box
	{
		nTextLength = pRichText->GetTextLength();

		//if there is alot of text in the Rich Edit (>64K) then GetWindowText doesn't
		//work. We have to select all of the text, and then use GetSelText
		pRichText->SetSel(0,-1);
	}
	else
	{
		//set up the text buffer; add 1 because zero-based array
		nTextLength = lEnd - lStart + 1;

		pRichText->SetSel(lStart, lEnd);
	}

	//LPTSTR lpszBuf = new TCHAR[nTextLength+1];
	CDuiString strTipInfo = pRichText->GetSelText();
	pRichText->SetSelectionCharFormat(m_cfDefault);

	colorize(strTipInfo, pRichText, lStart);

	//delete lpszBuf;
}

void CLexerMarker::SetCommentColor(COLORREF cr)
{
	CHARFORMAT2 cfComment = m_cfComment;
	cfComment.dwMask |= CFM_COLOR;
	cfComment.crTextColor = cr;
	SetCommentSyte(cfComment);
}

void CLexerMarker::SetCommentSyte(CHARFORMAT2& cfComment)
{
	m_cfComment = cfComment;
}

void CLexerMarker::SetStringColor(COLORREF cr)
{
	CHARFORMAT2 cfString = m_cfString;
	cfString.dwMask |= CFM_COLOR;
	cfString.crTextColor = cr;
	SetCommentSyte(cfString);
}

void CLexerMarker::SetStringStyle(CHARFORMAT2& cfString)
{
	m_cfString = cfString;
}

void CLexerMarker::SetGroupColor(int nGroup, COLORREF cr)
{
	CHARFORMAT2 cfGroup;

	cfGroup.dwMask |= CFM_COLOR;
	cfGroup.crTextColor = cr;
	SetGroupStyle(nGroup, cfGroup);
}

void CLexerMarker::GetGroupStyle(int nGroup, CHARFORMAT2& cfGroup)
{
	cfGroup = m_cfDefault;

	WORDINFO* pWordInfo = m_pWordInfo;
	while(pWordInfo)
	{
		if (pWordInfo->nGroup == nGroup)
		{
			cfGroup = pWordInfo->cf;
			break;
		}
		pWordInfo = pWordInfo->pNext;
	}
}

void CLexerMarker::SetGroupStyle(int nGroup, CHARFORMAT2& cfGroup)
{
	WORDINFO* pWordInfo = m_pWordInfo;
	while(pWordInfo != NULL)
	{
		if (pWordInfo->nGroup == nGroup)
			pWordInfo->cf = cfGroup;
		pWordInfo = pWordInfo->pNext;
	}
}

void CLexerMarker::colorize(LPCTSTR lpszBuf, CRichTextUI *pCtrl, long iOffset /*=0*/)
{
	//setup some vars
	CHARFORMAT2 cf;
	LPCTSTR lpszTemp;
	long iStart;
	long x = 0;
	WORDINFO* pskTemp = m_pWordInfo;
	unsigned char* pTable = m_pTableZero;

	//do the work
	while(lpszBuf[x])
	{
		switch(pTable[lpszBuf[x]])
		{
		case SYNTAX_DQSTART:
			pTable = m_pTableOne;
			iStart = iOffset + x;
			break;
		case SYNTAX_SQSTART:
			pTable = m_pTableTwo;
			iStart = iOffset + x;
			break;
		case SYNTAX_CMSTART:
			if(lpszBuf[x+1] == '/')
			{
				pTable = m_pTableThree;
				iStart = iOffset + x;
				x++;
			}
			else if(lpszBuf[x+1] == '*')
			{
				pTable = m_pTableFour;
				iStart = iOffset + x;
				x++;
			}
			else if(lpszBuf[x] == '\'')
			{
				pTable = m_pTableThree;
				iStart = iOffset + x;
				x++;
			}

			break;
		case SYNTAX_MLEND:
			if(lpszBuf[x+1] == '/')
			{
				x++;
				pTable = m_pTableZero;
				pCtrl->SetSel(iStart,iOffset + x+1);
				pCtrl->SetSelectionCharFormat(m_cfComment);
			}
			break;
		case SYNTAX_SLEND:
			if(lpszBuf[x-2] != '\\') // line continuation character
			{
				pTable = m_pTableZero;
				pCtrl->SetSel(iStart,iOffset + x+1);
				pCtrl->SetSelectionCharFormat(m_cfComment);
			}
			break;
		case SYNTAX_DQEND:
			pTable = m_pTableZero;
			pCtrl->SetSel(iStart,iOffset + x+1);
			pCtrl->SetSelectionCharFormat(m_cfString);
			break;
		case SYNTAX_SQEND:
			if(lpszBuf[x-1] == '\\' && lpszBuf[x+1] == '\'')
				break;
			pTable = m_pTableZero;
			pCtrl->SetSel(iStart,iOffset + x+1);
			pCtrl->SetSelectionCharFormat(m_cfString);
			break;
		case SYNTAX_KEYWORD:
			lpszTemp = lpszBuf+x;
			while(pskTemp != NULL)
			{
				if(pskTemp->lpKeyword[0] == lpszTemp[0])
				{
					int x1=0,y1=0;iStart = iOffset + x;
					while(pskTemp->lpKeyword[x1])
					{
						y1 += lpszTemp[x1] ^ pskTemp->lpKeyword[x1];
						x1++;
					}
					if(y1 == 0 && (*(m_pAllowable + (lpszBuf[x-1])) && 
						*(m_pAllowable + (lpszBuf[x+pskTemp->dwLen]))))
					{
						if(_tcsicmp(pskTemp->lpKeyword, _T("rem")) == 0)
						{
							pTable = m_pTableThree;
						}
						else 
						{
							x += pskTemp->dwLen;
							pCtrl->SetSel(iStart,iOffset + x);
							pCtrl->SetSelectionCharFormat(pskTemp->cf);
						}
					}
				}
				pskTemp = pskTemp->pNext;
			}
			pskTemp = m_pWordInfo;
			break;
		case SYNTAX_SKIP:;
		}
		if (lpszBuf[x] == NULL)
			break;
		x++;
	}
	//sometimes we get to the end of the file before the end of the string
	//or comment, so we deal with that situation here
	if(pTable == m_pTableOne)
	{
		cf = m_cfString;
		pCtrl->SetSel(iStart,iOffset + x+1);
		pCtrl->SetSelectionCharFormat(cf);
	}
	else if(pTable == m_pTableTwo)
	{
		cf = m_cfString;
		pCtrl->SetSel(iStart,iOffset + x+1);
		pCtrl->SetSelectionCharFormat(cf);
	}
	else if(pTable == m_pTableThree)
	{
		cf = m_cfComment;
		pCtrl->SetSel(iStart,iOffset + x+1);
		pCtrl->SetSelectionCharFormat(cf);
	}
	else if(pTable == m_pTableFour)
	{
		cf = m_cfComment;
		pCtrl->SetSel(iStart,iOffset + x+1);
		pCtrl->SetSelectionCharFormat(cf);
	}
}