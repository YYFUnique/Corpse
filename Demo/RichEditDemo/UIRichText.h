#pragma once

#include "UIBubbleLayout.h"

namespace DuiLib
{
	#define DUI_CTR_RICHTEXT		_T("RichText")
	class CRichTextUI : public CRichEditUI
	{
	public:
		CRichTextUI();
		~CRichTextUI();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	public:
		void SetPos(RECT rc);
		void DoPaint(HDC hDC, const RECT& rcPaint);
	public:
		void LineUp();
		void LineDown();
		void PageUp();
		void PageDown();
		void HomeUp();
		void EndDown();
		void LineLeft();
		void LineRight();
		void PageLeft();
		void PageRight();
		void HomeLeft();
		void EndRight();
		void SetScrollPos(SIZE szPos);
	public:

		//void AddBubble(CBubbleInfo* sbi);
		//void RejustBubble(CBubbleInfo& sbi);
		DWORD GetParaSpace() const;
		void SetParaSpace(DWORD dwParaSpace);
		int GetWindowTextLength() const;
		BOOL GetTextRange(CHARRANGE* pChrg, CString& strRangeText) const;
		BOOL GetTextRange(DWORD dwStart, DWORD dwEnd, CString& strRangeText) const;

		BOOL SetParaSpace(DWORD dwBefore, DWORD dwAfter);
		// 设置左缩进(单位:缇)
		BOOL SetStartIndent(int nLeftSize, int nRightSize, DWORD dwAlignment = PFA_LEFT);

		DWORD GetDefaultCharFormat(CHARFORMAT &cf) const;
		BOOL SetDefaultCharFormat(CHARFORMAT &cf);
		
		//DWORD GetSelectionCharFormat(CHARFORMAT& cf) const;
		//BOOL SetSelectionCharFormat(CHARFORMAT &cf);

		void SetDefaultFont(LPCTSTR lpFontName,int nFontSize,	COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);
		DWORD SetFont(LPCTSTR lpFontName, int nFontSize, COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);

		void ReplaceSel(LPCTSTR lpszNewText, LPCTSTR lpFontName, int nFontSize,	COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink, int nStartIndent, BOOL bCanUndo = FALSE);

		// 获取光标的x坐标
		LONG GetCurTargetX() const;
		void GetControlRect(LPRECT lpControlRect) const;
	protected:
		DWORD	m_dwParaSpace;
	};
}
