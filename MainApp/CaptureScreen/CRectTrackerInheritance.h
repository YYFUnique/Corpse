
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
��Ƥ�����������
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#ifndef  _CRECTTRACKERINHERITANCE_H_
#define  _CRECTTRACKERINHERITANCE_H_

#define		DIRECTE_WNES			0	//����-����
#define		DIRECTE_ENWS			1	//����-����
#define		DIRECTE_ESWN			2	//����-����
#define		DIRECTE_WSEN			3	//����-����
#define		DIRECTE_NS				4	//����-����
#define		DIRECTE_EW				5	//����-����
#define		DIRECTE_SN				6	//����-����
#define		DIRECTE_WE				7	//����-����
#define		DIRECTE_ALL				8	//����˷�

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

