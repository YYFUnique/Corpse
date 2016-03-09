#include "StdAfx.h"
#include "EditEx.h"

IMPLEMENT_DYNAMIC(CEditEx,CEdit)

// 大气象 2012-11-16 ↓
BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_KILLFOCUS()// 大气象：与对话框中控件消息映射定义不同。
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CEditEx::CEditEx(void)
{
}

CEditEx::~CEditEx(void)
{

}
void CEditEx::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	// 向父窗口发消息
	GetParent()->PostMessage(WM_USER_EDIT_END , 0 , 0);
}
void CEditEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TRACE(_T("Key Num %d!\n"), nChar);
	if (nChar == VK_RETURN)
		SendMessage(WM_KILLFOCUS,0,0);
	else
		CEdit::OnKeyDown(nChar,nRepCnt,nFlags);
}