#pragma once

class CFontInfo				// 字体信息
{
public:
	CFontInfo(void);
	~CFontInfo(void);
public:
	BOOL IsEqualFont(const CFontInfo* pFontInfo);
	void Copy(const CFontInfo* pFontInfo);
	//BOOL operator == (const CFontInfo& FontInfo);
public:
	int m_nSize;			// 字体大小
	COLORREF m_clrText;		// 字体颜色
	CDuiString m_strName;		// 字体名称
	BOOL m_bBold;			// 是否加粗
	BOOL m_bItalic;			// 是否倾斜
	BOOL m_bUnderLine;		// 是否带下划线
};