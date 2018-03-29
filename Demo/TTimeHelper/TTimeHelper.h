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
		SYSTEMTIME		m_SysTime;					//保存上一次获取到的系统时间
		CFrameUI* 			m_pFrame[4];				//用于保存界面上显示时间的4个控件
		CControlUI*			m_pHLayoutHead;
		BOOL					m_bShowChild;			//
		CFloatWindow*	m_pFloatWindow;		//用户支持悬浮窗口
		CTxMiniSkin*		m_pTxMiniSkin;			//用户确定是否退出系统
		CCityHelper*		m_pCityInfo;				//保存城市位置及天气信息
		CWeatherHelper* m_pWeatherInfo;		//用于获取城市位置及天气信息
		CString					m_strCityLocation;		//当前城市位置
		CMailHelper*		m_pMailHelper;			//	邮件助手
		CSendMailThread* m_pSendMailThread;			//	发送邮件线程
		DWORD				m_dwOutOfTime;		//	功能过期时间
};