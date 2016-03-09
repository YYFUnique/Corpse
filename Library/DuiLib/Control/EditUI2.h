#ifndef __UIEDIT2_H__
#define __UIEDIT2_H__

#pragma once

namespace DuiLib
{
	typedef enum tagCALC_CARET_TYPE
	{
		CAC_CARET_TYPE_NONE = 0,
		CALC_CARET_TYPE_POS = 1,						//计算点击左键后光标的位置
		CALC_CARET_TYPE_SELECT_MODE = 2,		//计算选择文字的范围
	}CALC_CARET_TYPE;

	class UILIB_API CEditUI2 : public CLabelUI
	{
	public:
		CEditUI2();
		~CEditUI2();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);
		void PaintBorder(HDC hDC);

		bool IsReadOnly();
		void SetReadOnly(bool breadonly);
		void SetPasswordMode(bool bpassword);
		void SetPasswordChar(LPCTSTR lpszPasswordChar);
		bool IsDigitalMode();
		void SetDigitalMode(bool bdigital);
		void SetLimitText(UINT nLimitText);
		UINT GetLimitText();
		void SetNormalImage(LPCTSTR lpszNormalImage);
		void SetHotImage(LPCTSTR lpszHotImage);
		void SetFocusedImage(LPCTSTR lpszFocusedImage);
		void SetDisabledImage(LPCTSTR lpszDisabledImage);

		void SetCaretWidth(int nCaretWidth);
		void SetSelectTextColor(DWORD dwSelectTextColor);
		void SetSelectBkColor(DWORD dwSelectbkColor);
		void SetText(LPCTSTR pstrText);
		void SetWaterText(LPCTSTR lpszWaterText);
		void SetWaterTextColor(DWORD dwWaterColor);
	protected:
		void OnCut(int nPos,int nLen);
		int OnPaste(int nMin,int nMax);
		void OnCopy(int nPos,int nLen);
		void OnCheckAll();

		bool CanPaste();

		bool IsWaterMode();
		void SetWaterMode(bool bWaterMode);
		bool GetEditText(CDuiString& strEditText);
		void SetPos(RECT rc);
		void FixedCaretPos(int nFixed);

		void SaveEditText(LPCTSTR lpszEditText);
		bool OnKeyEvent(TEventUI& event);
		void PaintCaret(HDC hDC);
		void CalcCaretRect(HDC hDC,const CDuiString& sText);
		void CalcStartSelRect(HDC hDC,const CDuiString& sText,int nPos,RECT& rcRange);
		int GetCaretPos(HDC hDC,const CDuiString& sText);
		int GetTextLen(HDC hDC,int nPos);
		bool AdjustImeWindowPos();
		void RestartCaretBlinkTimer();
		void DrawText(HDC hDC,const RECT& rc,const CDuiString& sText);
		void DrawSelectionText(HDC hDC,const RECT& rc,const CDuiString& sText);
		void CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen);

	protected:
		int	 m_nCaretWidth;							//输入标记宽度
		UINT m_uMaxChar;								//最大字符数
		int m_nSelStart;									//选择文字的开始位置
		int	m_nCaretPos;								//输入标记在文字的序号
		DWORD m_dwSelectTextColor;			//选择文本的文字颜色
		DWORD m_dwSelectbkColor;				//选择文本的文字背景色
		bool m_bReadOnly;							//只读模式
		bool m_bPasswordMode;					//密码模式
		bool m_bDigitalMode;						//数字模式
		bool m_bCaretOn;								//显示输入标记
		bool m_bDeleteKeyDown;					//删除键已经按下
		CALC_CARET_TYPE CalcCaretType;		//计算输入标记的方式
		UINT	m_nLimitText;								//文字限制个数
		int	m_nCaretOffset;							//文字相对于编辑框的偏移量
		POINT  m_szCaretPt;							//鼠标在编辑框点击时的位置
		RECT     m_rcCaret;								//输入标记所在位置
		TCHAR m_cPasswordChar;					//密码模式显示的字符
		UINT	m_nEditState;							//编辑框状态
		bool			  m_bWaterMode;				//水印模式
		DWORD    m_dwWaterColor;				//水印文字颜色
		CDuiString m_sWaterText;					//水印文字
		CDuiString m_strTooltip;						//提示文字
		CDuiString m_sNormalImage;			//编辑框普通状态图片
		CDuiString m_sFocusedImage;			//编辑框获取焦点状态图片
		CDuiString m_sHotImage;					
		CDuiString m_sDisabledImage;

		CDuiString m_sTextBak;
	};
}
#endif // __UIEDIT2_H__