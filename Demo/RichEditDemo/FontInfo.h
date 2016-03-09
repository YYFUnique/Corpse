#pragma once

class CFontInfo				// ������Ϣ
{
public:
	CFontInfo(void);
	~CFontInfo(void);
public:
	BOOL IsEqualFont(const CFontInfo* pFontInfo);
	void Copy(const CFontInfo* pFontInfo);
	//BOOL operator == (const CFontInfo& FontInfo);
public:
	int m_nSize;			// �����С
	COLORREF m_clrText;		// ������ɫ
	CDuiString m_strName;		// ��������
	BOOL m_bBold;			// �Ƿ�Ӵ�
	BOOL m_bItalic;			// �Ƿ���б
	BOOL m_bUnderLine;		// �Ƿ���»���
};