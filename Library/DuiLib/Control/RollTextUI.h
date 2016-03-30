#pragma once

namespace DuiLib
{
	typedef enum _ROLLTEXT_DIRECTION
	{
		ROLLTEXT_DIRECTION_LEFT		=0,		//���������ƶ�
		ROLLTEXT_DIRECTION_RIGHT	=1,		//���������ƶ�
		ROLLTEXT_DIRECTION_UP 			=2,		//���������ƶ�
		ROLLTEXT_DIRECTION_DOWN	=3,		//���������ƶ�
	}ROLLTEXT_DIRECTION;

	#define ROLLTEXT_TIMERID						20		//�����ƶ���ʱ��ID
	#define ROLLTEXT_TIMERID_SPAN			100U		//�����ƶ���ʱ��ʱ����

	class UILIB_API CRollTextUI : public CLabelUI
	{
	public:
		CRollTextUI(void);
		~CRollTextUI(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		virtual void DoInit();

	public:	
		virtual void SetText(LPCTSTR pstrText);
		virtual void PaintText(HDC hDC);
		virtual void DoEvent(TEventUI& event);
		virtual void SetPos(RECT rc);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		
	public:
		void BeginRoll();
		void EndRoll();
		
		void SetRollText(BOOL bRollText);
		void SetRollTextStep(int nStep);
		void SetOverStop(BOOL bOverStop);
		void SetRollType(LPCTSTR lpszRollType);
	protected:

	private:    
		//ÿ���ƶ����ֵĲ���
		int m_nStep;      
		int m_nScrollYPos;      
		int m_nScrollXPos;      
		//�Ƿ�ʹ��RollText������ʾ����
		BOOL m_bRollText;  
		//����������ڹ���������ʱ���Ƿ���Ҫֹͣ
		BOOL m_bOverStop;
		//���ֵ��ۼƸ߶�(��ֱ�ƶ�ʱ)��
		int m_nTextHeight;      
		//���������(��ֱ�ƶ�ʱ)
		int m_nTextWidth;       
		//�����Ƿ��Ѿ��ı䣬����ı���Ҫ���¼�����ʾ����͸߶�
		BOOL m_bTextChanged;
		BOOL m_bTextPosed;
		//�����ƶ�����
		ROLLTEXT_DIRECTION m_nRollDirection;   
	};
}	// namespace DuiLib