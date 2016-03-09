#pragma once

#include "Utils/wke.h"
namespace DuiLib
{
	class UILIB_API CWkeBrowserUI : public CControlUI ,public _wkeBufHandler 
	{
	public:
		CWkeBrowserUI(void);
		~CWkeBrowserUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetPos(RECT rc);
		void DoPaint(HDC hDC, const RECT& rcPaint);
	public:
		void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);
		void InitBrowser(wkeWebView pWebView,UINT nTimerID);
		void Navigate(LPCTSTR szUrl); 
		void SetUrl(LPCTSTR lpszUrl);
		void Refresh();

		bool CanGoBack() const;
		bool GoBack();
		bool CanGoForward() const;
		bool GoForward();
		void SetZoomFactor(float factor);
		float GetZoomFactor();
		void SetEditable(bool bEditable = true);

		void SetClientHandler(PwkeClientHandler pwkeClientHandler);
	protected:
		void MouseEvent(UINT uMsg,TEventUI& event);

		/*static UINT wkeThread(LPVOID lParam);*/
	protected:
		wkeWebView m_pWebView;
		CDuiString m_strUrl;
		UINT	m_nButtonState;
		float		m_fZoom;
		bool		m_bEdit;
	};
}