#pragma once

#define		DIRECTE_WNES			0	//西北-东南
#define		DIRECTE_ENWS			1	//东北-西南
#define		DIRECTE_ESWN			2	//东南-西北
#define		DIRECTE_WSEN			3	//西南-东北
#define		DIRECTE_NS				4	//正北-正南
#define		DIRECTE_EW				5	//正东-正西
#define		DIRECTE_SN				6	//正南-正北
#define		DIRECTE_WE				7	//正西-正东
#define		DIRECTE_ALL				8	//四面八方
#define		DIRECTE_NOTHINE		9	//未定义
typedef enum StyleFlags
{
	solidLine = 1, dottedLine = 2, hatchedBorder = 4,
	resizeInside = 8, resizeOutside = 16, hatchInside = 32,
}StyleFlags;

typedef enum TrackerHit
{
	hitNothing = -1,
	hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
	hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
}TrackerHit;

class CRectTracker
{
public:
	// Constructors
	CRectTracker();
	//CRectTracker(LPCRECT lpSrcRect, UINT nStyle);
	virtual ~CRectTracker();

public:
	int HitTest(POINT ptCursor) const;
	void GetTrueRect(LPRECT lpTrueRect) const;
	int HitTestHandles(POINT pt) const;
	UINT GetHandleMask() const;
	BOOL SetCursor(HWND hWnd, UINT nHitTest) ;
	void SetRect(LPRECT lpRect);
	int GetHandleSize(LPCRECT lpRect = NULL) const;
	void GetHandleRect(int nHandle, LPRECT pHandleRect) const;
	int NormalizeHit(int nHandle) const;
protected:
	BOOL m_bAllowInvert;    // flag passed to Track or TrackRubberBand
	CDuiRect m_rectLast;
	BOOL m_bErase;          // TRUE if DrawTrackerRect is called for erasing
	BOOL m_bFinalErase;     // TRUE if DragTrackerRect called for final erase

	// Attributes
	UINT m_nStyle;      // current state
	CDuiRect m_rect;       // current position (always in pixels)
	int m_nHandleSize;  // size of resize handles (default from WIN.INI)
	HCURSOR m_hCursors[10];
};

