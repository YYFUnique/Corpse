
// ScreenSpyDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "CRectTrackerInheritance.h"

#include "PngImage.h"
// CScreenSpyDlg �Ի���
class CScreenSpyDlg : public CDialogEx
{
// ����
public:
	CScreenSpyDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SCREENSPY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	/////////////////////////////////////////////////////////////////////////////////
	CPngImage2	m_pngMask;
	CPngImage2	m_pngDot;
	CPngImage2	m_pngAction;
	CBitmap		*m_backGround;
	int			m_iCapture;
	bool		m_bLeftDown;
	CPoint		m_ptLButtonDown;
	CRect		m_rcSel[9];
	UINT		m_curSel[9];

	int			m_iActionMask;
	CRect		m_rcAction[3];
	bool		m_bSelect;
	bool		m_bSelected;

	int			m_width;
	int			m_height;
	void DrawFrame(void);

	void saveToFile(CString strSaveFile="");
	/////////////////////////////////////////////////////////////////////////////
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void	ShowInfo(CRect &inRect);
	HBITMAP CopyScreenToBitmap(LPRECT lpRect,BOOL bSave =FALSE );
	void    OnShowToolBar();
		
		
	DECLARE_MESSAGE_MAP()
public:
	
	CRectTrackerInheritance m_RectTracker; //��Ƥ�����������
	int		m_xScreen;
	int		m_yScreen;  //��Ļ�ķֱ���
	CBitmap* m_pBitmap;	//����λͼ
	HCURSOR m_hCursor;//���
	CRect   m_RtClient;//�ͻ���
	CRgn	m_rgn;//Ҫ���µ�����
	CPoint  m_startPt;//��ȡ�������Ͻ�
	BOOL    m_bDraw;//�Ƿ�Ϊ��ȡ״̬
	BOOL    m_bStartDraw;//�Ƿ���Կ�ʼ������Ƥ��
	BOOL	m_bShowInfo;//�Ƿ���ʾ�����Ϣ
	CToolBar m_ToolBar;//������

	void	PaintWindow();//�ػ洰��

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnOK();
	afx_msg void OnCancel();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
