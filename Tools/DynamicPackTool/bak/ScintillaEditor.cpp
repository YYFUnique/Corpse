#include "StdAfx.h"
#include "ScintillaEditor.h"
#include "minus.xpm" 
#include "plus.xpm" 

#include <Scintilla/include/Scintilla.h>
#include <Scintilla/include/SciLexer.h>
HINSTANCE CScintillaEditor::m_hScintilla = LoadLibrary(_T("Scintilla.dll"));
WNDPROC CScintillaEditor::_scintillaDefaultProc = NULL;

#define MARGIN_FOLD_INDEX 2	 

typedef enum tagSCINTILLA_EDITOR
{
	SCINTILLA_EDITOR_SC_MARGE_LINENUMBER = 0,
	SCINTILLA_EDITOR_SC_MARGE_SYBOLE			= 1,
	SCINTILLA_EDITOR_SC_MARGE_FOLDER			= 2,
}SCINTILLA_EDITOR;

CScintillaEditor::CScintillaEditor()
	:m_bFolding(FALSE)
	,m_bSelection(TRUE)
	,m_bLinenumbers(FALSE)
{
	++m_nRef;
}

CScintillaEditor::~CScintillaEditor()
{
	if (--m_nRef == 0 && m_hScintilla != NULL)
	{
		FreeLibrary(m_hScintilla);
		m_hScintilla = NULL;
	}
}

BOOL CScintillaEditor::InitScintilla(HWND hWnd)
{
	BOOL bSuccess = FALSE;
	do 
	{
		m_hScintillaWnd = hWnd;

		//显示窗口
		//ShowWindow(m_hScintillaWnd,SW_SHOW);

		m_pScintillaFunc = (SCINTILLA_FUNC)::SendMessage(m_hScintillaWnd, SCI_GETDIRECTFUNCTION, 0, 0);
		m_pScintillaPtr = (SCINTILLA_PTR)::SendMessage(m_hScintillaWnd, SCI_GETDIRECTPOINTER, 0, 0);

		//::SetWindowLong(_hSelf, GWL_USERDATA, reinterpret_cast<LONG>(this));
		//_scintillaDefaultProc = reinterpret_cast<WNDPROC>(::SetWindowLong(_hSelf, GWL_WNDPROC, reinterpret_cast<LONG>(scintillaStatic_Proc)));

		//清除所有文本风格
		m_pScintillaFunc(m_pScintillaPtr, SCI_CLEARDOCUMENTSTYLE, 0, 0);
		
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETLEXER, SCLEX_CPP, 0);

		m_pScintillaFunc(m_pScintillaPtr, SCI_SETSTYLEBITS, 7, 0);
		// set the display for indetation guides to on - this displays virtical dotted lines from the beginning of 
		// a code block to the end of the block
		//m_pScintillaFunc(m_pScintillaPtr, SCI_SETINDENTATIONGUIDES, TRUE, 0);
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETTABWIDTH, 4, 0);
		// set the caret blinking time 
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETCARETPERIOD, GetCaretBlinkTime(), 0);


		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 0, RGB(0,0,0));
		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 2, RGB(0,64,0));
		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 5, RGB(0,0,255));
		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 6, RGB(200,20,0));
		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 9, RGB(0,0,255));
		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 10, RGB(255,0,64));
		m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, 11, RGB(0,0,0));


		// set the backgroundcolor of brace highlights
		//m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETBACK, STYLE_BRACELIGHT, RGB(0,255,0));
		// set end of line mode to CRLF
		m_pScintillaFunc(m_pScintillaPtr, SCI_CONVERTEOLS, 2, 0);
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETEOLMODE, 2, 0);
		//   SendMessage(SCI_SETVIEWEOL, TRUE, 0);
		// set markersymbol for marker type 0 - bookmark
		//m_pScintillaFunc(m_pScintillaPtr, SCI_MARKERDEFINE, 0, SC_MARK_CIRCLE);
		// display all margins
		SetDisplayLineNumbers(TRUE);
		//
		SetDisplayFolding(TRUE);
		m_pScintillaFunc(m_pScintillaPtr,SCI_SETPROPERTY,(sptr_t)"fold",(sptr_t)"1"); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_SETMARGINTYPEN, MARGIN_FOLD_INDEX, SC_MARGIN_SYMBOL);//页边类型 
		m_pScintillaFunc(m_pScintillaPtr,SCI_SETMARGINMASKN, MARGIN_FOLD_INDEX, SC_MASK_FOLDERS); //页边掩码 
		m_pScintillaFunc(m_pScintillaPtr,SCI_SETMARGINWIDTHN, MARGIN_FOLD_INDEX, 11); //页边宽度 
		m_pScintillaFunc(m_pScintillaPtr,SCI_SETMARGINSENSITIVEN, MARGIN_FOLD_INDEX, TRUE); //响应鼠标消息 

		// 折叠标签样式 
		/*m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS);  
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS);  
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND,  SC_MARK_CIRCLEPLUSCONNECTED); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);  
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE); */

		/*m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PIXMAP);
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_PIXMAP); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND,  SC_MARK_PIXMAP); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_PIXMAP); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE); */
		// 折叠标签颜色 

		/*m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, 0xa0a0a0); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, 0xa0a0a0); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, 0xa0a0a0); */

		/*m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDER, (sptr_t)plus_xpm); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEROPEN, (sptr_t)minus_xpm); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEREND, (sptr_t)plus_xpm); 
		m_pScintillaFunc(m_pScintillaPtr,SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEROPENMID, (sptr_t)minus_xpm); */
		//m_pScintillaFunc(m_pScintillaPtr,SCI_SETFOLDFLAGS, 16|4, 0); //如果折叠就在折叠行的上下各画一条横线 

		SetDisplaySelection(FALSE);
 
		::SetWindowLong(m_hScintillaWnd, GWL_USERDATA, reinterpret_cast<LONG>(this));
		_scintillaDefaultProc = reinterpret_cast<WNDPROC>(::SetWindowLong(m_hScintillaWnd, GWL_WNDPROC, reinterpret_cast<LONG>(scintillaStatic_Proc)));

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

 LRESULT CALLBACK CScintillaEditor::scintillaStatic_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CScintillaEditor *pScint = (CScintillaEditor *)(::GetWindowLong(hwnd, GWL_USERDATA));
	//
	if (pScint)
		return (pScint->scintillaNew_Proc(hwnd, Message, wParam, lParam));
	else
		return ::DefWindowProc(hwnd, Message, wParam, lParam);
	//
}

LRESULT CScintillaEditor::scintillaNew_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
	return ::CallWindowProc(_scintillaDefaultProc, hwnd, Message, wParam, lParam);
}

void CScintillaEditor::Notify(WPARAM wParam, LPARAM lParam)
{
	NMHDR *phDR = (NMHDR*)lParam;

	if (phDR != NULL && phDR->hwndFrom == m_hScintillaWnd)
	{
		SCNotification *pMsg = (SCNotification*)lParam;
		switch(phDR->code)
		{
			case SCN_UPDATEUI:
				UpdateUI();
			case SCN_MARGINCLICK:
				{
					//if (pMsg->nmhdr.idFrom == SCINT_ID){ 
						// 确定是页边点击事件 
						DWORD dwLine = m_pScintillaFunc(m_pScintillaPtr, SCI_LINEFROMPOSITION, pMsg->position, 0); 
						m_pScintillaFunc(m_pScintillaPtr, SCI_TOGGLEFOLD, dwLine, 0); 
					//
				}
				break;
			default:
				{
					CString strTipInfo;
					strTipInfo.Format(_T("CODE:%d.\r\n"),phDR->code);
					OutputDebugString(strTipInfo);
				}
		}
	}
}

int CScintillaEditor::GetLineNumberChars()
{
	// try to get number of lines in control
	LRESULT lLines = m_pScintillaFunc(m_pScintillaPtr, SCI_GETLINECOUNT, 0, 0);

	int nChars = 1;
	while (lLines > 0)
	{
		lLines = lLines / 10;
		nChars++;
	}
	return nChars; // allow an extra char for safety
}
/////////////////////////////////////
// @mfunc Calculate a reasonable width for Linenumberdisplay
// @rvalue int | number of pixels for the margin width of margin (0) 
//
int CScintillaEditor::GetLineNumberWidth()
{
	// get number of chars needed to display highest linenumber
	int nChars = GetLineNumberChars ()+1;
	// get width of character '9' in pixels
	LRESULT lWidth = m_pScintillaFunc(m_pScintillaPtr, SCI_TEXTWIDTH, STYLE_LINENUMBER, (long)_T("9"));
	return nChars * lWidth;
}

void CScintillaEditor::Cut()
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_CUT, 0, 0);
}

void CScintillaEditor::Copy()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_COPY, 0, 0);
}

void CScintillaEditor::Paste()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_PASTE, 0, 0);
}

void CScintillaEditor::Clear()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_CLEAR, 0, 0);
}

void CScintillaEditor::Undo()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_UNDO, 0, 0);
}

void CScintillaEditor::Redo()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_REDO, 0, 0);
}

void CScintillaEditor::CanUndo()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_CANUNDO, 0, 0);
}

void CScintillaEditor::CanRedo()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_CANREDO, 0, 0);
}

void CScintillaEditor::CanPaste()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_CANPASTE, 0, 0);
}

void CScintillaEditor::SelectAll()
{
	m_pScintillaFunc(m_pScintillaPtr,SCI_SELECTALL, 0, 0);
}

LONG CScintillaEditor::GetCurrentLine()
{
	LONG lPos = m_pScintillaFunc(m_pScintillaPtr, SCI_GETCURRENTPOS, 0, 0);
	return m_pScintillaFunc(m_pScintillaPtr, SCI_LINEFROMPOSITION, lPos, 0) + 1;
}

LONG CScintillaEditor::GetCurrentColumn()
{
	LONG lPos = m_pScintillaFunc(m_pScintillaPtr, SCI_GETCURRENTPOS, 0, 0);
	return m_pScintillaFunc(m_pScintillaPtr, SCI_GETCOLUMN, lPos, 0) +1;
}

LONG CScintillaEditor::GetCurrentPosition()
{
	return m_pScintillaFunc(m_pScintillaPtr, SCI_GETCURRENTPOS, 0, 0);
}

int CScintillaEditor::GetCurrentStyle()
{
	LONG lPos = m_pScintillaFunc(m_pScintillaPtr, SCI_GETCURRENTPOS, 0, 0);
	return m_pScintillaFunc(m_pScintillaPtr, SCI_GETSTYLEAT, lPos, 0);
}

// @mfunc Return the current folding level at the caret line
// @rvalue int | the current folding level (0...n)
//
int CScintillaEditor::GetCurrentFoldinglevel()
{
	LONG lLine = GetCurrentLine();
	int nLevel = (m_pScintillaFunc(m_pScintillaPtr, SCI_GETFOLDLEVEL, lLine, 0) & SC_FOLDLEVELNUMBERMASK);
	return nLevel - 1024;
}

// @mfunc set the fontname e.g. "Arial"
// @rvalue void | not used
//
void CScintillaEditor::SetFontName(int nStyle, LPCTSTR lpszFontName)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFONT, nStyle, (LONG)lpszFontName);
}

void CScintillaEditor::SetFontHeight(int nStyle, int nHeigth)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETSIZE, nStyle, (LONG)nHeigth);
}

void CScintillaEditor::SetForeground(int nStyle, COLORREF crForeground)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETFORE, nStyle, (LONG)crForeground);
}

void CScintillaEditor::SetBackground(int nStyle, COLORREF crBackground)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETBACK, nStyle, (LONG)crBackground);
}

void CScintillaEditor::SetBold(int nStyle, BOOL bBold)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETBOLD, nStyle, (LONG)bBold);
}

void CScintillaEditor::SetItalic(int nStyle, BOOL bItalic)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETITALIC, nStyle, (LONG)bItalic);
}

void CScintillaEditor::SetUnderLine(int nStyle, BOOL bUnderLine)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_STYLESETUNDERLINE, nStyle, (LONG)bUnderLine);
}

BOOL CScintillaEditor::GetOverstrike()
{
	return m_pScintillaFunc(m_pScintillaPtr, SCI_GETOVERTYPE, 0, 0);
}

void CScintillaEditor::SetOverstrike(BOOL bOverstrike)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_SETOVERTYPE, bOverstrike, 0);
}

void CScintillaEditor::DoDefaultFolding(int nMargin, LONG lPosition)
{
	if (nMargin == SCINTILLA_EDITOR_SC_MARGE_FOLDER)
	{
		LONG lLine = m_pScintillaFunc(m_pScintillaPtr, SCI_LINEFROMPOSITION, lPosition, 0);
		m_pScintillaFunc(m_pScintillaPtr, SCI_TOGGLEFOLD, lLine, 0);
	}
}

void CScintillaEditor::Refresh()
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_COLOURISE, 0, -1);
}

//是否显示行号开关
void CScintillaEditor::SetDisplayLineNumbers(BOOL bFlag) //@parm flag if we shuld display line numbers
{
	// if nothing changes just return
	if (m_bLinenumbers == bFlag)
		return;
	// if display is turned off we set margin 0 to 0
	if (!bFlag)
	{
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETMARGINWIDTHN, 0, 0);
	}
	// if display is turned o we set margin 0 to the calculated width
	else
	{
		int nWidth = GetLineNumberWidth();
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETMARGINWIDTHN, 0, nWidth);
	}
	m_bLinenumbers = bFlag;
}

//折叠开关
void CScintillaEditor::SetDisplayFolding(BOOL bFlag)
{
	m_bFolding = bFlag;
	if (bFlag)
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETMARGINWIDTHN, 2, 16);
	else
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETMARGINWIDTHN, 2, 0);
}

//Toggle the display of the selection bookmark margin
void CScintillaEditor::SetDisplaySelection(BOOL bFlag)
{
	m_bSelection = bFlag;
	if (bFlag)
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETMARGINWIDTHN, 1, 16);
	else
		m_pScintillaFunc(m_pScintillaPtr, SCI_SETMARGINWIDTHN, 1, 0);
}

//line where to add bookmark - lines start at 1
void CScintillaEditor::AddBookMark(DWORD dwLine)
{
	if (dwLine == 0)
		dwLine = GetCurrentLine();
	m_pScintillaFunc(m_pScintillaPtr, SCI_MARKERADD, dwLine - 1, 0);
}

void CScintillaEditor::DeleteBookMark(DWORD dwLine)
{
	if (HasBookMark(dwLine))
		m_pScintillaFunc(m_pScintillaPtr, SCI_MARKERDELETE, dwLine - 1, 0);
}

BOOL CScintillaEditor::HasBookMark(DWORD dwLine)
{
	if (dwLine == 0)
		dwLine = GetCurrentLine();

	int nBookMark = m_pScintillaFunc(m_pScintillaPtr, SCI_MARKERGET, dwLine - 1, 0);
	// check mask for markerbit 0
	return nBookMark & 0x1;
}

DWORD CScintillaEditor::GetNextBookMark(DWORD dwLine)
{
	if (dwLine == 0)
		dwLine = GetCurrentLine();

	return m_pScintillaFunc(m_pScintillaPtr, SCI_MARKERNEXT, dwLine, 1);
}

void CScintillaEditor::GotoNextBookMark(DWORD dwLine)
{
	DWORD dwNextBookMark = GetNextBookMark(dwLine);
	if (dwNextBookMark >= 0)
		m_pScintillaFunc(m_pScintillaPtr, SCI_GOTOLINE, dwNextBookMark, 0);
}

DWORD CScintillaEditor::GetPreviousBookMark(DWORD dwLine)
{
	if (dwLine == 0)
		dwLine = GetCurrentLine();

	return m_pScintillaFunc(m_pScintillaPtr, SCI_MARKERPREVIOUS, dwLine - 2, 1);
}

void CScintillaEditor::GotoPreviousBookMark(DWORD dwLine)
{
	DWORD dwPreviousBookMark = GetPreviousBookMark(dwLine);
	if (dwPreviousBookMark >= 0)
		m_pScintillaFunc(m_pScintillaPtr, SCI_GOTOLINE, dwPreviousBookMark, 0);
}

void CScintillaEditor::GotoPosition(DWORD dwPos)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_GOTOPOS, dwPos, 0);
}

void CScintillaEditor::GotoLine(DWORD dwLine)
{
	m_pScintillaFunc(m_pScintillaPtr, SCI_GOTOLINE, dwLine, 0);
}

LONG CScintillaEditor::GetSelectionStart()
{
	return m_pScintillaFunc(m_pScintillaPtr, SCI_GETSELECTIONSTART, 0, 0);
}

LONG CScintillaEditor::GetSelectionEnd()
{
	return m_pScintillaFunc(m_pScintillaPtr, SCI_GETSELECTIONEND, 0, 0);
}

CString CScintillaEditor::GetSelectedText()
{
	LONG iLen = (GetSelectionEnd() - GetSelectionStart()) + 1;
	if (iLen > 0)
	{
		LPTSTR pSelected = new TCHAR[iLen+1];
		if (pSelected != NULL)
		{
			*pSelected = _T('\0');
			m_pScintillaFunc(m_pScintillaPtr, SCI_GETSELTEXT, 0, (LONG)pSelected);
			CString strReturn = pSelected;
			delete [] pSelected;
			return strReturn;
		}
	}
	return _T("");
}

void CScintillaEditor::UpdateUI()
{
	LONG lStart = m_pScintillaFunc(m_pScintillaPtr, SCI_GETCURRENTPOS, 0, 0);
	LONG lEnd = m_pScintillaFunc(m_pScintillaPtr, SCI_BRACEMATCH, lStart - 1, 0);

	if (lEnd >= 0)
		m_pScintillaFunc(m_pScintillaPtr, SCI_BRACEHIGHLIGHT, lStart - 1, lEnd);
	else
		m_pScintillaFunc(m_pScintillaPtr, SCI_BRACEHIGHLIGHT, -1, -1);
}