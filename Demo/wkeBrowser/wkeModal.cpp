#include "stdafx.h"
#include "wkeModal.h"

CWkeModal::CWkeModal()
	: m_pWebView(NULL)
{

}

LPCTSTR CWkeModal::GetWindowClassName() const
{
	return _T("MainWnd");
}

CDuiString CWkeModal::GetSkinFile()
{
	return _T("wkeModal.xml");
}

CDuiString CWkeModal::GetSkinFolder()
{
	return _T("wkeBrowser");
}

UILIB_RESOURCETYPE CWkeModal::GetResourceType() const
{
	return UILIB_FILE;
}

void CWkeModal::InitWindow()
{
	MessageBox(NULL,_T("OK"),_T("ÌבÊ¾"),MB_OK);

	m_pWebView = wkeCreateWebView();
}

wkeWebView CWkeModal::GetWebView()
{
	return m_pWebView;
}

void CWkeModal::Notify(TNotifyUI& msg)
{

}

CControlUI* CWkeModal::CreateControl(LPCTSTR pstrClassName)
{
	return NULL;
}