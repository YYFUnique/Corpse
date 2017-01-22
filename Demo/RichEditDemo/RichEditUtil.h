#pragma once
#include <string>
#include "Utils.h"

DWORD RichEdit_GetDefaultCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);

BOOL RichEdit_SetDefaultCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);

DWORD RichEdit_GetSelectionCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);

BOOL RichEdit_SetSelectionCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);

// 设置字体
void RichEdit_SetFont(ITextServices * pTextServices, LPCTSTR lpFontName, int nFontSize,
					  COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);

// 设置默认字体
void RichEdit_SetDefFont(ITextServices * pTextServices, LPCTSTR lpFontName,	
						 int nFontSize,	COLORREF clrText, BOOL bBold, 
						 BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);


IRichEditOle* RichEdit_GetOleInterface(ITextServices* pTextServices);

int RichEdit_GetWindowTextLength(ITextServices* pTextServices);

int RichEdit_GetTextRange(ITextServices * pTextServices, CHARRANGE * lpchrg, CDuiString& strText);

// 获取文本
void RichEdit_GetText(ITextServices * pTextServices, CDuiString& strText);

void RichEdit_GetSel(ITextServices * pTextServices, LONG& nStartChar, LONG& nEndChar);

int RichEdit_SetSel(ITextServices * pTextServices, LONG nStartChar, LONG nEndChar);

// 替换选中文本
// 其中将左缩进设置为-1 表示居中
void RichEdit_ReplaceSel(ITextServices * pTextServices, LPCTSTR lpszNewText,	
						 LPCTSTR lpFontName, int nFontSize,	COLORREF clrText, 
						 BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink, 
						 int nStartIndent, BOOL bCanUndo = FALSE);

void RichEdit_ReplaceSel(ITextServices * pTextServices, LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);

BOOL RichEdit_SetStartIndent(ITextServices * pTextServices, int nSize,BOOL bNickName);

// 插入表情图片
BOOL RichEdit_InsertFace(ITextServices *pTextServices, ITextHost *pTextHost,	
						 LPCTSTR lpszFileName, int nFaceId,	int nFaceIndex, 
						 COLORREF clrBg, BOOL bAutoScale, int nReservedWidth);

void InsertTextToEdit(CRichEditUI* edit, LPCTSTR str);

BOOL GetSelectionRect(CRichEditUI* pRichEdit, LONG cpStart, LONG cpEnd, RECT &rcCursor);