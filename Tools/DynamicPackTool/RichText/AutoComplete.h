#pragma once

// Scintilla source code edit control
/** @file AutoComplete.h
 ** Defines the auto completion list box.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

class AutoComplete
{
public:
	AutoComplete();
	~AutoComplete();

public:
	/// Is the auto completion list displayed?
	BOOL IsActive() const;

	/// Display the auto completion list positioned to be near a character position
	void Start(Window& parent, int nCtrlID, int nPos, POINT pt, int nStartLen, int nLineHeight, BOOL bUnicodeMode, int nTechnology);

	/// The stop chars are characters which, when typed, cause the auto completion list to disappear
	void SetStopChars(const CHAR* chStopChars);
	BOOL IsStopChar(CHAR chChar);

	/// The fillup chars are characters which, when typed, fill up the selected word
	void SetFillupChars(const CHAR* chFillupChars);
	BOOL IsFillUpChars(CHAR chChar);

	/// The separator character is used when interpreting the list in SetList
	void SetSeparator(CHAR chSeparator);
	CHAR GetSeparator() const;

	/// The typesep character is used for seperating the word from the type
	void SetTypesep(CHAR chSeparator);
	CHAR GetTypesep() const;

	/// The list string contains a sequence of words separated by the separator character
	void SetList(const CHAR* chList);

	/// Return the position of the currently selected list item
	int GetSelection() const;

	/// Return the value of an item in the list
	std::string GetValue(int nItem) const;

	void Show(BOOL bShow = TRUE);
	void Cancel();

	/// Move the current list element by delta, scrolling appropriately
	void Move(int nDelta);

	/// Select a list element that starts with word as the current element
	void Select(const CHAR* chWord);
protected:
	BOOL		m_bActive;
	CHAR		m_chStopChars[256];
	CHAR		m_chFillUpChars[256];
	CHAR		m_chSeparator;
	CHAR		m_chTypesep;			//Type seperator

	enum {maxItemLen = 1000};

	BOOL		m_bIgnoreCase;
	BOOL		m_bChooseSingle;
	int			m_nPosStart;
	int			m_nStartLen;
	ListBox*	m_lb;
	/// Should autocompletion be canceled if editor's currentPos <= startPos?
	BOOL		m_bCancelAtStartPos;
	BOOL		m_bAutoHide;
	BOOL		m_bDropRestOfWord;
	UINT		m_nIgnoreCaseBehaviour;
	int			m_nWidthLBDefault;
	int			m_nHeightLBDefault;
};