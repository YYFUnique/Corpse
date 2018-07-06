#pragma once

#define DUI_CTR_BUBBLELAYOUT  _T("BubbleLayout")

class CBubbleLayoutUI : public CHorizontalLayoutUI
{
public:
	CBubbleLayoutUI();
	~CBubbleLayoutUI();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	//UINT GetControlFlags() const;

	//void DoInit();
	//bool Add(CControlUI* pControl);
	void SetPos(RECT rc);
	SIZE EstimateSize(SIZE szAvailable);
public:
	void RejustBubble();
	void SetParaSpace(DWORD dwParaSpace);
	void AddBubbleMsg(LPCTSTR lpszNickName, LPCTSTR lpszBubbleMsg);
	void SetBubbleSize(SIZE size);
	void SetBubbleImage(LPCTSTR lpszBubbleImage);

	void SetRichTextRange(DWORD dwStart, DWORD dwEnd);
	void SetRichTextRange(const CHARRANGE& CharRange);
	
	void SetNickNameLen(DWORD dwNickLen);
protected:
	void CalAsLeft();
	void CalAsRight();
	
	bool OnLabelSize(LPVOID lParam);
protected:
	//CHARRANGE	m_CharRange;		// 标记内容开头和结束位置
	int			m_dwNickNameLen;		// 昵称文字开头索引,包含回车换行长度
	DWORD			m_dwParaSpace;				// 昵称文字结束索引

	int					m_dwTextStart;				// 消息文字开头索引
	int					m_dwTextEnd;					//	消息文字结束索引
	BOOL				m_bLeftAlign;					// 是否左对齐
	DWORD			m_dwLIndent;					//	消息文字左缩进
	DWORD			m_dwRIndent;					//	消息文字右缩进
	//BOOL				m_bEstimateSized;
	SIZE					m_szAvailable;
	CDuiString		m_strBubbleImage;
};
