#pragma once

#include "TxMiniSkin.h"
#include "WeatherHelper.h"
#include "CityHelper.h"
#include "MailHelper.h"
#include "Thread/SendMailThread.h"
#include "DllCore/Window/FloatWindow.h"

class CTTimeHelper : public WindowImplBase, public CityInfoReceiver
{
	public:
		CTTimeHelper();
		~CTTimeHelper();

	public:
		void OnFinalMessage( HWND hWnd );
		virtual LPCTSTR GetWindowClassName() const;
		virtual CDuiString GetSkinFile();
		virtual CDuiString GetZIPFileName() const;
		virtual CDuiString GetSkinFolder();
		UILIB_RESOURCETYPE GetResourceType() const;
		virtual void Notify(TNotifyUI& msg);
		virtual BOOL Receive(CityInfoChangedParam CityInfoParam);

		HRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	public:
		void SetCityLocation(LPCTSTR lpszCityLocation);

	protected:
		BOOL PtInRect(LPCTSTR lpszName);
		void InitWindow();
		void OnInitDialog();
		void SetShowTimer();
		void OnClick(TNotifyUI& msg);
		void OnTimer(TNotifyUI& msg);
		
		CControlUI* CreateControl(LPCTSTR pstrClass);
		//void FormatLunarDay(WORD wDay, LPTSTR pLunarDay);
		BOOL SetChildVisible(CControlUI* pControl, bool bVisible);
		LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	private:
		void PraseMailInfo();
	protected:
		SYSTEMTIME		m_SysTime;					//������һ�λ�ȡ����ϵͳʱ��
		CFrameUI* 			m_pFrame[4];				//���ڱ����������ʾʱ���4���ؼ�
		CControlUI*			m_pHLayoutHead;
		BOOL					m_bShowChild;			//
		CFloatWindow*	m_pFloatWindow;		//�û�֧����������
		CTxMiniSkin*		m_pTxMiniSkin;			//�û�ȷ���Ƿ��˳�ϵͳ
		CCityHelper*		m_pCityInfo;				//�������λ�ü�������Ϣ
		CWeatherHelper* m_pWeatherInfo;		//���ڻ�ȡ����λ�ü�������Ϣ
		CString					m_strCityLocation;		//��ǰ����λ��
		CMailHelper*		m_pMailHelper;			//	�ʼ�����
		CSendMailThread* m_pSendMailThread;			//	�����ʼ��߳�
		DWORD				m_dwOutOfTime;		//	���ܹ���ʱ��
};