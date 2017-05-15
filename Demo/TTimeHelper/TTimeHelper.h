#pragma once

#include "TxMiniSkin.h"
#include "WeatherHelper.h"
#include "DllCore/Window/FloatWindow.h"

class CTTimeHelper : public WindowImplBase, public ILibcurlCallback
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

		HRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	protected:
		BOOL PtInRect();
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
		
	protected:
		SYSTEMTIME		m_SysTime;
		CFrameSeqUI* 	m_pFrame[4];
		CControlUI*			m_pHLayoutHead;
		BOOL					m_bShowChild;
		WORD					m_wDayOfWeek;
		CFloatWindow*	m_pFloatWindow;
		CTxMiniSkin*		m_pTxMiniSkin;
		CWeatherHelper* m_pWeatherInfo;
};