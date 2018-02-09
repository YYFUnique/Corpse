#pragma once

#include <atlstr.h>
typedef int (* SCINTILLA_FUNC) (void*, int, int, int);
typedef void * SCINTILLA_PTR;

class CScintillaEditor
{
public:
	CScintillaEditor();
	~CScintillaEditor();

public:
	void Notify(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK scintillaStatic_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	LRESULT scintillaNew_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	int GetLineNumberChars();
	int GetLineNumberWidth ();
	void SetDisplayLineNumbers(BOOL bFlag); //@parm flag if we shuld display line numbers

	void Cut();
	void Copy();
	void Paste();
	void Clear();
	void Undo();
	void Redo();
	void CanUndo();
	void CanRedo();
	void CanPaste();
	void SelectAll();

	//λ�����
	LONG GetCurrentLine();
	LONG GetCurrentColumn();
	LONG GetCurrentPosition();

	int GetCurrentStyle();
	int GetCurrentFoldinglevel();
	
	//����������ɫ���
	void SetFontName(int nStyle, LPCTSTR lpszFontName);
	void SetFontHeight(int nStyle, int nHeigth);
	void SetForeground(int nStyle, COLORREF crForeground);
	void SetBackground(int nStyle, COLORREF crBackground);
	void SetBold(int nStyle, BOOL bBold);
	void SetItalic(int nStyle, BOOL bItalic);
	void SetUnderLine(int nStyle, BOOL bUnderLine);

	//���á���ȡ����ģʽ
	BOOL GetOverstrike();
	//set set overstrike mode (TRUE) or insert mode (FALSE)
	void SetOverstrike(BOOL bOverstrike);

	//Toggle the display of the selection bookmark margin
	void SetDisplaySelection(BOOL bFlag);
	//Toggle the display of the folding margin
	void SetDisplayFolding(BOOL bFlag);
	//����Ĭ�Ϸ�ʽ
	void DoDefaultFolding(int nMargin, LONG lPosition);
	void Refresh();

	//��ǩ���
	void AddBookMark(DWORD dwLine);
	void DeleteBookMark(DWORD dwLine);
	BOOL HasBookMark(DWORD dwLine);
	void GotoNextBookMark(DWORD dwLine);
	DWORD GetNextBookMark(DWORD dwLine);
	void GotoPreviousBookMark(DWORD dwLine);
	DWORD GetPreviousBookMark(DWORD dwLine);

	void GotoPosition(DWORD dwPos); //@parm new character position
	void GotoLine(DWORD dwLine);

	//BOOL SearchForward(LPSTR szText); //@parm text to search
	//BOOL SearchBackward(LPSTR szText); //@parm text to search
	//void ReplaceSearchedText(LPCSTR szText); //@parm new text

	LONG GetSelectionStart();
	LONG GetSelectionEnd();
	CString GetSelectedText();

	//ˢ�±༭���ؼ�
	void UpdateUI();
	BOOL InitScintilla(HWND hWnd);
protected:
private:
	int						 m_nRef;								//���ô���
	BOOL					 m_bFolding;
	BOOL					 m_bSelection;
	BOOL					 m_bLinenumbers;				//�Ƿ���ʾ�к�
	HWND					m_hScintillaWnd;				//Scintilla���ھ��
	SCINTILLA_FUNC m_pScintillaFunc;				//Scintilla���ƺ���
	SCINTILLA_PTR	 m_pScintillaPtr;					//Scintillaʵ�����
	static	HINSTANCE m_hScintilla;					//Scintilla��̬����
	static WNDPROC _scintillaDefaultProc;
};