#pragma once
#include "afxwin.h"

// 大气象：自定义编辑结束消息
#define WM_USER_EDIT_END  (WM_USER + 1001)

class CEditEx :
	public CEdit
{
	//声明动态运行时CRuntimeClass
	DECLARE_DYNAMIC(CEditEx)
public:
	DECLARE_MESSAGE_MAP()
	// 控件中消息定义，需要以CWnd为参数
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	CEditEx(void);
	~CEditEx(void);
};
