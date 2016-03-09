
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
橡皮筋类的派生类
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#ifndef  _CRECTTRACKERINHERITANCE_H_
#define  _CRECTTRACKERINHERITANCE_H_

#define		DIRECTE_WNES			0	//西北-东南
#define		DIRECTE_ENWS			1	//东北-西南
#define		DIRECTE_ESWN			2	//东南-西北
#define		DIRECTE_WSEN			3	//西南-东北
#define		DIRECTE_NS				4	//正北-正南
#define		DIRECTE_EW				5	//正东-正西
#define		DIRECTE_SN				6	//正南-正北
#define		DIRECTE_WE				7	//正西-正东
#define		DIRECTE_ALL				8	//四面八方

class CRectTrackerInheritance : public CRectTracker
{
public:
	CRectTrackerInheritance();
	~CRectTrackerInheritance();
	void SetRect(LPRECT lpRect);
	void SetCursor(UINT nHitTest);
	void SetCursor(CWnd* pWnd, UINT nHitTest);
	int GetHitTestHandle(CPoint point);
protected:
	void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,CDC* pDC, CWnd* pWnd );
private:
	HCURSOR m_hCursors[10];
};

#endif

