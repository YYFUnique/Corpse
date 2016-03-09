#pragma once
#include "afxwin.h"


class CHandlePic :
	public CStatic
{
public:
	CHandlePic(void);
	~CHandlePic(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	HWND m_hWndNew , m_hWndOld;
};
