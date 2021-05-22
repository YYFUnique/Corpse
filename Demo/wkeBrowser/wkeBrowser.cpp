#include "stdafx.h"
#include "wkeBrowser.h"
#include "Resource.h"

#define WM_CREATE_MODAL_WINDOW		(WM_USER+0x1000)
#define WM_DO_MODAL								(WM_USER+0x1001)



//
//jsValue JS_CALL js_msgBox(jsExecState es)
//{
//	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
//	const wchar_t* title = jsToStringW(es, jsArg(es, 1));
//
//	HWND hwnd = CWkeBrowser::MainWnd()->GetHWND();
//	if (::IsWindow(hwnd) )
//	{
//		::PostMessage(hwnd, WM_USER_JS_NOTIFY, (WPARAM)text, (LPARAM)title);
//	}
//
//	return jsUndefined();
//}
//
//void onURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString URL)
//{
//	HWND hwnd = CWkeBrowser::MainWnd()->GetHWND();
//	if (::IsWindow(hwnd) )
//	{
//		::PostMessage(hwnd, WM_USER_URL_CHANGE, (WPARAM)wkeToStringW(URL), (LPARAM)0);
//	}
//}
//
//void onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
//{
//	HWND hwnd = CWkeBrowser::MainWnd()->GetHWND();
//	if (::IsWindow(hwnd) )
//	{
//		::PostMessage(hwnd, WM_USER_TITLE_CHANGE, (WPARAM)wkeToStringW(title), (LPARAM)0);
//	}
//}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////
//////////////////

CWkeBrowser::CWkeBrowser()
	: m_pURLEdit(NULL)
	, m_pLblStatusBar(NULL)
{
}

LPCTSTR CWkeBrowser::GetWindowClassName() const
{
	return _T("MainWnd");
}

CDuiString CWkeBrowser::GetSkinFile()
{
	return _T("wkeBrowser.xml");
}

CDuiString CWkeBrowser::GetSkinFolder()
{
	return _T("wkeBrowser");
}

UILIB_RESOURCETYPE CWkeBrowser::GetResourceType() const
{
	return UILIB_FILE;
}
 
CControlUI* CWkeBrowser::CreateControl( LPCTSTR pstrClassName )
{
	//if (_tcsicmp(pstrClassName, _T("Wke")) == 0) return  new CWkeBrowserUI;
	return NULL;
}

void CWkeBrowser::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
	CenterWindow();
	m_pURLEdit = static_cast<CEditUI2*>(m_PaintManager.FindControl(_T("urlEdit")));
	m_pLblStatusBar = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("statusbar")));

	CWkeBrowserUI* pWke = static_cast<CWkeBrowserUI*>(m_PaintManager.FindControl(_T("ie")));
	if (pWke)
	{
		pWke->InitBrowser(wkeCreateWebView(),1000);
		
		//jsBindFunction("msgBox", js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数
		m_pWkeHandler.onTitleChanged = &CWkeBrowser::OnTitleChanged;
 		m_pWkeHandler.onURLChanged = &CWkeBrowser::OnURLChanged;
 		pWke->SetClientHandler(&m_pWkeHandler);
		//m_pWke->SetFile(_T("Html/index.html")msg.pSender->GetText().GetData());
		//m_pWke->SetURL(L"http://mudcu.be/sketchpad/"); //一个漂亮的网站，大家可以自己试试
		pWke->Navigate(_T("https://10.21.143.3:8443"));
	}
	/*CWebBrowserUI* pWeb = static_cast<CWebBrowserUI*>(m_PaintManager.FindControl(_T("ie2")));
	m_pWebBrowserEventHandler = new CWebBrowserEventHandler;
	pWeb->SetWebBrowserEventHandler(m_pWebBrowserEventHandler);
	if (pWeb)
		pWeb ->Navigate2(_T("http://www.duilib.com")); */
}

LRESULT CWkeBrowser::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
		case WM_CREATE_MODAL_WINDOW:
			{
				MessageBox(m_hWnd,_T("tishi "),_T("tishi"),MB_OK);
				m_pModal = new CWkeModal();
				m_pModal->InitWindow();
				return (LRESULT)m_pModal->GetWebView();
			}
	}
	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

/*
void CWkeBrowser::OnTitleChanged(const wkeClientHandler* pwkeHandle, LPCTSTR lpszTitleInfo)
{
}
*/
//void CWkeBrowser::OnURLChanged(const wkeClientHandler* pwkeHandle, LPCTSTR lpszUrlInfo)
//{
	/*CMainWnd *pWnd = (CMainWnd *)handler->pdata;
	DUIRichEdit *pRichEdit = pWnd->FindChildByNameT<DUIRichEdit>(L"web_url");
	DUIWebKit *pWebKit = pWnd->FindChildByNameT<DUIWebKit>(L"webobj");
	if (pRichEdit&&pWebKit)
	{
		pRichEdit->SetWindowText(pWebKit->GetWebView()->toStringW(url));
	}*/
//}

LRESULT CWkeBrowser::ResponseDefaultKeyEvent(WPARAM wParam)
{
	return FALSE;
}

void CWkeBrowser::Notify( TNotifyUI& msg )
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_RETURN)
		OnReturn(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
	else
		__super::Notify(msg);
}

void CWkeBrowser::OnClick(TNotifyUI& msg)
{
		if (msg.pSender->GetName() == _T("BtnClose")){
			Close();
		}
		else if (msg.pSender->GetName() == _T("BtnMin")){
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		else if (msg.pSender->GetName() == _T("BtnMax")){
			SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
		}
		else if (msg.pSender->GetName() == _T("BtnRestore")){
			SendMessage(WM_SYSCOMMAND,SC_RESTORE,0);
		}
}

void CWkeBrowser::OnReturn(TNotifyUI& msg)
{
	if( msg.pSender->GetName() == _T("urlEdit") ) 
	{
		if (m_pURLEdit)
		{

			CWebBrowserUI* pWke = static_cast<CWebBrowserUI*>(m_PaintManager.FindControl(_T("ie2")));
			pWke->Navigate2(m_pURLEdit->GetText());
		}
	}
}

void CWkeBrowser::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetParent() == (CControlUI*)m_PaintManager.FindControl(_T("IContainer")))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		UINT nIndex = _ttoi(msg.pSender->GetUserData());
		pTabLayout->SelectItem(nIndex);
	}
}

void CWkeBrowser::OnTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
{
	//m_Title = (LPCTSTR)title;
	CDuiString strText;
	//strText.Format(_T("网站标题：%s     网站网址:%s"),(LPCTSTR)m_Title,(LPCTSTR)m_URL);
	//m_pLblStatusBar->SetText(strText);
}

void CWkeBrowser::OnURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString url)
{
	//m_URL =(LPCTSTR) url;
	CDuiString strText;
	//strText.Format(_T("网站标题：%s     网站网址:%s"),(LPCTSTR)m_Title,(LPCTSTR)m_URL);
	//m_pLblStatusBar->SetText(strText);
}

LRESULT CWkeBrowser::OnJsNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	MessageBox(m_hWnd, (LPCTSTR)wParam, (LPCTSTR)lParam, 0);
	return 0;
}
