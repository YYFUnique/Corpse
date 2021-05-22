#pragma once
/**
 * Class to hide the details of window manipulation.
 * Does not own the window which will normally have a longer life than this object.
 */
#include "FontHelper.h"

#include "UserDefine.h"

typedef enum tagCursor
{
	cursorInvalid		= 0,
	cursorText			= 1,
	cursorArrow			=	2,
	cursorUp				=	3,
	cursorWait			=	4,
	cursorHoriz			=	5,
	cursorVert			=	6,
	cursorReverseArrow	=	7,
	cursorHand			=	8,
}Cursor;

/**
 * A simple callback action passing one piece of untyped user data.
 */
typedef void (*CallBackAction)(void*);

class Window
{
public:
	Window();
	Window (const Window& winSource);
	Window& operator=(WindowID wid);
	virtual ~Window();
public:
	WindowID GetID() const;
	BOOL IsCreated() const;
	void Destroy();
	BOOL HasFocus() const;
	/*CDuiRect GetPos() const;*/
	CDuiRect GetPosition() const;
	void SetPosition(const CDuiRect* prcPos);
	void SetPositionRelative(const CDuiRect* prcPos, Window relativeTo);
	void Show(BOOL bSHow = TRUE);
	void InvalidateAll();
	void InvalidateRectangle(const CDuiRect& rcPos);
	virtual void SetFont(Font& font);
	void SetCursor(Cursor& curs);
	void SetTitle(const char* chTitle);
protected:
	WindowID m_wid;
private:
	Cursor m_cursorLast;
};

/**
 * Listbox management.
 */

class ListBox : public Window
{
public:
	ListBox();
	virtual ~ListBox();
public:
	static ListBox* Allocate();
public:
	virtual void SetFont(FontHelper& font) = 0;
	virtual void Create(Window& parent, int nCtrlID, POINT pt, int nLineHeight, BOOL bUnicodeMode, int nTechnology) = 0;
	virtual void SetAverageCharWidth(int nWidth) = 0;
	virtual void SetVisibleRows(int nRows) = 0;
	virtual int   GetVisibleRows() const = 0;
	virtual RECT GetDesiredRect() const = 0;
	virtual int CaretFromEdge() = 0;
	virtual void Clear() const;
	virtual void Append(char* sText, int nType = -1) = 0;
	virtual int GetLength() const;
	virtual void Select(int nItem) = 0;
	virtual int GetSelection() = 0;
	virtual int Find(const CHAR* chPreFix) = 0;
	virtual void GetValue(int nIndex, CHAR* chVal, int nLen) = 0;
	virtual void RegisterImage(int nType, const char* xpmData) = 0;
	virtual void RegisterRGBAImage(int nType, int nWidth, int nHeight, const BYTE* pixelsImage) = 0;
	virtual void ClearRegisteredImages() = 0;
	virtual void SetDoubleClickAction(CallBackAction, void*) = 0;
	virtual void SetList(const CHAR* chList, CHAR chSeparator, CHAR chTypesep) = 0;
};

/**
 * Menu management.
 */

class Menu
{
public:
	Menu();
public:
	MenuID GetID() const;
	void CreatePopup();
	void Destroy();
	void Show(POINT pt, Window& win);
};

class ElapsedTime
{
public:
	ElapsedTime();
public:
	double Duration(BOOL bReset = FALSE);
protected:
	LONG m_bigBit;
	LONG m_littleBit;
};
