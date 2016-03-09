#include "StdAfx.h"
#include "resource.h"
#include "ExpressBus.h"

CExpressBus::CExpressBus()
{

}

CExpressBus::~CExpressBus()
{

}

DUI_BEGIN_MESSAGE_MAP(CExpressBus, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

// CControlUI* CExpressBus::CreateControl(LPCTSTR pstrClass)
// {
// 	return NULL;
// }

LPCTSTR CExpressBus::GetWindowClassName() const
{
	return _T("DuilibExpressbus");
}

UILIB_RESOURCETYPE CExpressBus::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_ZIP;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

// LPCTSTR CExpressBus::GetResourceID() const  
// {  
// 	return MAKEINTRESOURCE(IDR_ZIP_SKIN);  
// };

CDuiString CExpressBus::GetZIPFileName() const
{
	return _T("QQLogin.zip");
}

CDuiString CExpressBus::GetSkinFile()
{
	return _T("QQLogin.xml");
}

CDuiString CExpressBus::GetSkinFolder()
{
	return _T("QQLogin");
}

void CExpressBus::InitWindow()
{
//  	IShockwaveFlash* pFlash = NULL;
//  	CActiveXUI* pActiveX =  static_cast<CActiveXUI*>(m_PaintManager.FindControl(_T("Flash")));
//  	pActiveX->GetControl(IID_IUnknown, (void**)&pFlash);
//  	if( pFlash != NULL ) {
//  		pFlash->put_WMode( _bstr_t(_T("Transparent") ) );
//  		pFlash->put_Movie( _bstr_t(_T("morning.swf")) );
//  		pFlash->DisableLocalSecurity();
//  		pFlash->put_AllowScriptAccess(L"always");
//  		BSTR response;
//  		pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
//  		pFlash->Release();
//  	}  
}

void CExpressBus::Notify(TNotifyUI& msg)
{

}

LRESULT CExpressBus::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return TRUE;
}

void CExpressBus::OnClick(TNotifyUI& msg)
{

}

void CExpressBus::OnSelectChanged( TNotifyUI &msg )
{

}

void CExpressBus::OnItemClick( TNotifyUI &msg )
{

}