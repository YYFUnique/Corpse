#pragma once

namespace DuiLib
{
	typedef enum tagCALC_CARET_TYPE
	{
		CALC_CARET_TYPE_NONE				= 0,
		CALC_CARET_TYPE_POS					= 1,		// 计算点击左键后光标的位置
		CALC_CARET_TYPE_SELECT_MODE = 2,		// 计算选择文字的范围
		CALC_CARET_TYPE_SELECT_ALL		 = 4,		// 全选模式
	}CALC_CARET_TYPE;

	class UILIB_API CEditTextHost
	{
	public:
		CEditTextHost();
		~CEditTextHost();

	public:
		void SetOwner(CControlUI* pOwner);
		BOOL DoEvent(TEventUI& event);
		void PaintCaret(HDC hDC);
		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		void CalcStartSelRect(HDC hDC,const CDuiString& sText,int nPos,RECT& rcRange);
		void CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen);
	public:
		void SetText(LPCTSTR pstrText);
		CDuiString GetText() const;
		CDuiString GetEditText() const;
		CDuiString GetSelectionText() const;

		BOOL OnKeyEvent(TEventUI& event);
		void RestartCaretBlinkTimer();
		BOOL AdjustImeWindowPos();
	public:
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		UINT GetTextState() const;

		BOOL IsReadOnly() const;
		void SetReadOnly(BOOL bReadOnly = TRUE);

		BOOL IsPasswordMode() const;
		void SetPasswordMode(BOOL bPassword = TRUE);
		void SetPasswordChar(LPCTSTR lpszPasswordChar);

		BOOL IsDigitalMode() const;
		void SetDigitalMode(BOOL bDigital = TRUE);

		BOOL IsWaterMode() const;
		void SetWaterMode(BOOL bWaterMode = TRUE);

		UINT GetLimitLength() const;
		void SetLimitLength(UINT nLimitLength);

		BOOL IsEnabled() const;
		BOOL IsFocused() const;
		CPaintManagerUI* GetManager();
		void Invalidate();

		void SetFont(int iFont);
		void SetTextStyle(int uTextStyle);

		void SetCaretColor(DWORD dwCaretColor);
		void SetCaretWidth(int nCaretWidth);

		void SetWaterText(LPCTSTR lpszWaterText);
		void SetWaterTextColor(DWORD dwWaterColor);
		void SetSelectBkColor(DWORD dwSelectBkColor);
		void SetSelectTextColor(DWORD dwSelectTextColor);
	public:
		void SelectAll();
		BOOL CanPaste();
		void OnCopy(int nPos, int nLen);
		int OnPaste(int nMin, int nMax);
		void OnCut(int nPos, int nLen);
	protected:
		void DrawText(HDC hDC,const RECT& rc,const CDuiString& sText);
		void DrawSelectionText(HDC hDC,const RECT& rc,const CDuiString& sText);

		void FixedCaretPos(int nFixed);
		int GetCaretPos(HDC hDC, const CDuiString& sText);
		void CalcCaretRect(HDC hDC,const CDuiString& sText);
	protected:
		CControlUI*		m_pOwner;									// 对应界面控件
		CDuiString		m_sText;										// 界面绘制文字
		CDuiString		m_strWaterText;							// 水印文字
		CDuiString		m_strTextBak;								// 用于保存原始文字

		CALC_CARET_TYPE CalcCaretType;						// 计算输入标记的方式

		int					m_nCaretWidth;							// 输入标记宽度
		DWORD			m_dwCaretColor;							// 输入标记颜色
		BOOL				m_bCaretOn;								//	显示输入标记
		BOOL				m_bKeyDown;								// 退格按钮已按下
		BOOL				m_bFocused;								// 否是获取了焦点
		UINT				m_nTextHostState;						// 文字状态
		RECT				m_rcCaret;									//	输入标记所在位置
		POINT				m_szCaretPt;								// 鼠标在编辑框点击时的位置
		RECT				m_rcTextPadding;						// 文字显示位置
		int					m_iFont;										//	文字显示字体
		int					m_uTextStyle;								//	文字字体样式
		int					m_nSelStart;								// 选择文字的开始位置
		int					m_nCaretPos;								// 输入标记在文字的序号
		int					m_nCaretOffset;							// 文字相对于编辑框的偏移量

		BOOL				m_bReadOnly;								//	只读模式
		BOOL				m_bPasswordMode;					//	密码模式
		BOOL				m_bDigitalMode;						//	数字模式
		TCHAR				m_cPasswordChar;						//	密码模式显示的字符
		BOOL				m_bWaterMode;							// 水印模式
		DWORD			m_nLimitLength;							//	限制字符长度

		DWORD			m_dwTextColor;							// 文件正常显示的颜色
		DWORD			m_dwDisabledTextColor;			// 文字禁用时的颜色
		DWORD			m_dwWaterColor;						//	水印文字颜色
		DWORD			m_dwSelectTextColor;				//	选择文本的文字颜色
		DWORD			m_dwSelectbkColor;					//	选择文本的文字背景色
	};
}