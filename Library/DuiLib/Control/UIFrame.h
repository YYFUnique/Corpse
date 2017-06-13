#pragma once

namespace DuiLib
{		
	class UILIB_API CFrameUI : public CLabelUI
	{
	public:
		CFrameUI();
		~CFrameUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void DoEvent(TEventUI& event);
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	
		//ִ��֡���ж���
		void Run();
		//���ö�������
		void SetPeriod(DWORD dwPeroid);
		//���ö����ܹ�֡��
		void SetFrameCount(DWORD dwFrame);
		//����֡���ж���ͼƬ
		void SetFrameImage(LPCTSTR lpszFrameImg);
	protected:
		DWORD				m_dwPeroid;					//һ������ʱ�䣬��λ����
		DWORD				m_dwTotal;						//ÿ��������Ҫ����֡
		DWORD				m_dwFrame;					//��ǰ������һ֡
		POINT					m_ptTopLeft;					//���Ͻ���֡��ͼƬ������
		CDuiString			m_strFrameImg;
	};
}