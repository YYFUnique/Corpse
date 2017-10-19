#ifndef __UIEDIT2_H__
#define __UIEDIT2_H__

#pragma once

namespace DuiLib
{
	typedef enum tagCALC_CARET_TYPE
	{
		CALC_CARET_TYPE_NONE = 0,
		CALC_CARET_TYPE_POS = 1,						//��������������λ��
		CALC_CARET_TYPE_SELECT_MODE = 2,		//����ѡ�����ֵķ�Χ
		CALC_CARET_TYPE_SELECT_ALL = 4,			//ȫѡģʽ
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
		void OnSelectAll();

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
		/************************************************************************/
		/* �������ƣ�CalcTextRect                                                                         */
		/* �������ܣ����ַ���strText�д�nStartλ�ÿ�ʼȡnLen���ַ���			   */
		/*                  ������ռ��λ��																	   */
		/************************************************************************/
		void CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen);

	protected:
		int	 m_nCaretWidth;							//�����ǿ��
		UINT m_uMaxChar;								//����ַ���
		int m_nSelStart;									//ѡ�����ֵĿ�ʼλ��
		int	m_nCaretPos;								//�����������ֵ����
		DWORD m_dwSelectTextColor;			//ѡ���ı���������ɫ
		DWORD m_dwSelectbkColor;				//ѡ���ı������ֱ���ɫ
		bool m_bReadOnly;							//ֻ��ģʽ
		bool m_bPasswordMode;					//����ģʽ
		bool m_bDigitalMode;						//����ģʽ
		bool m_bCaretOn;								//��ʾ������
		bool m_bDeleteKeyDown;					//ɾ�����Ѿ�����
		CALC_CARET_TYPE CalcCaretType;		//���������ǵķ�ʽ
		UINT	m_nLimitText;							//�������Ƹ���
		int	m_nCaretOffset;							//��������ڱ༭���ƫ����
		POINT  m_szCaretPt;							//����ڱ༭����ʱ��λ��
		RECT     m_rcCaret;								//����������λ��
		TCHAR m_cPasswordChar;					//����ģʽ��ʾ���ַ�
		UINT	m_nEditState;							//�༭��״̬
		bool			  m_bWaterMode;				//ˮӡģʽ
		DWORD    m_dwWaterColor;				//ˮӡ������ɫ
		CDuiString m_sWaterText;					//ˮӡ����
		//CDuiString m_strTooltip;						//��ʾ����
		CDuiString m_sNormalImage;			//�༭����ͨ״̬ͼƬ
		CDuiString m_sFocusedImage;			//�༭���ȡ����״̬ͼƬ
		CDuiString m_sHotImage;					
		CDuiString m_sDisabledImage;

		CDuiString m_sTextBak;
	};
}
#endif // __UIEDIT2_H__