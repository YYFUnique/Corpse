#pragma once

#include "afxwin.h"
#include "DeclareCode.h"
class CHyperLink :
	public CStatic
{
public:
	CHyperLink(void);
	~CHyperLink(void);

	void SetLinkColor(UINT nLinkState, COLORREF ColorRGB);
	void SetLinkColor(COLORREF* pColorRGB,int nCount);
	inline void SetLinkState(UINT nLinkState);
	UINT GetLinkState();

	void SetTootTipText(LPCTSTR szToolTip);
	void SetToolTipTextColor(COLORREF sToolTipText);
	void SetToolTipBgColor(COLORREF sToolTipBgColor);
	void SetLinkUrl(LPCTSTR strLinkUrl,UINT nLinkType=LINK_URL);
	void ActiveToolTip(BOOL bFlag,UINT nDelayTime=5,LPCTSTR pStrTip=_T(""));
	void SetDlgItemText(int nID, LPCTSTR lpszString);
protected:
	virtual void PreSubclassWindow();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnMouseHover(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	void CreateDoubleFont();
	void SetTextUnderline(BOOL bHoverText=FALSE);
	inline void GotoDefineUrl();
private:
	UINT m_LinkState;	//当前控件所处状态
	UINT m_LinkType;		//控件类型
	COLORREF m_ColorLink[4];	//对应控件四种状态的颜色
	BOOL m_bVisited;
	BOOL m_bEnableToolTip;
	CToolTipCtrl m_ToolTip;
	CString m_strLinkUrl;
	BOOL m_bUnderlineText;
	CFont m_UnderlineFont,m_StdFont;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnStnClicked();
};

