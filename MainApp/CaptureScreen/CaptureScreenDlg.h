
// CaptureScreenDlg.h : ͷ�ļ�
//

#pragma once
#include "CRectTrackerInheritance.h"

typedef struct tgaDRAW_TOOLBAR
{
	CDC*			pTargetDC;
	CDC*			pSrcDc;
	HBITMAP		hBmp;
	DWORD		dwBmpBit;
	DWORD		dwLeftPosition;
	DWORD		dwTopPosition;
	DWORD		dwBmpWidth;
	DWORD		dwBmpHeight;
}DRAW_TOOLBAR;

typedef enum	TOOLBAR_BUTTON_TYPE
{
	TOOLBAR_BUTTON_TYPE_HOVER=0,
	TOOLBAR_BUTTON_TYPE_NORMAL,
	TOOLBAR_BUTTON_TYPE_DISABLE,
	TOOLBAR_BUTTON_TYPE_PRESS,
}TOOLBAR_BUTTON_TYPE;

// CCaptureScreenDlg �Ի���
class CCaptureScreenDlg : public CDialog
{
// ����
public:
	CCaptureScreenDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CCaptureScreenDlg();
// �Ի�������
	enum { IDD = IDD_CAPTURESCREEN_DIALOG };
	LPRECT GetSelectRect();
	void PaintScreenCapture();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual void OnCancel();
protected:
	void DrawTrackRect();
	void ChangeRect(int nHandle,CPoint point);
	void EnsureRectInScreen(LPRECT lpRect);
	void DrawToolBarBmp(DRAW_TOOLBAR DrawToolBar);
	void ChangeToolBarState(CPoint& point,TOOLBAR_BUTTON_TYPE ToolBarButtonType);
// ʵ��
protected:
	HICON			m_hIcon;
	HCURSOR	m_hCursor;
	HBITMAP		m_DotBmp;
	HBITMAP		m_ToolBarBackGround;
	HBITMAP		m_ToolBarSave;
	HBITMAP		m_ToolBarShare;
	HBITMAP		m_ToolBarCancel;
	HBITMAP		m_ToolBarBack;
	HBITMAP		m_ToolBarComplete;
	CPen			m_LinePen;
	CRectTrackerInheritance m_RectTrack;
	CBitmap*		m_pDeskTopBmp;
	CBitmap*		m_TrueColorBmp;
	CToolBar*	m_pToolBar;
	BOOL			m_bLBtnDown;
	BOOL			m_bHasDraw;
	BOOL			m_bShowInfo;
	BOOL			m_bShowToolBar;
	DWORD		m_dwToolBarID;
	//�����������λ��
	CPoint			m_ptLButtonDown;
	CRect			m_RectSelected;
	UINT			m_TrackPos;
	CRect			m_RectTool;

	TOOLBAR_BUTTON_TYPE m_ToolBarType;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnToolbarCommand(UINT nID);
	DECLARE_MESSAGE_MAP()
};
