#pragma once
#include "afxwin.h"

// �������Զ���༭������Ϣ
#define WM_USER_EDIT_END  (WM_USER + 1001)

class CEditEx :
	public CEdit
{
	//������̬����ʱCRuntimeClass
	DECLARE_DYNAMIC(CEditEx)
public:
	DECLARE_MESSAGE_MAP()
	// �ؼ�����Ϣ���壬��Ҫ��CWndΪ����
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	CEditEx(void);
	~CEditEx(void);
};
