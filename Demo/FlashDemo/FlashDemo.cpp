#include "stdafx.h"
#include "Resource.h"
#include "FlashDemo.h"
#include <atltime.h>
#include <atlstr.h>

CFlashDemo::CFlashDemo()
{
}

CFlashDemo::~CFlashDemo()
{

}

LPCTSTR CFlashDemo::GetWindowClassName() const
{
	return _T("DuilibFlashDemo");
}

UILIB_RESOURCETYPE CFlashDemo::GetResourceType() const
{
	return UILIB_ZIP;
}

LPCTSTR CFlashDemo::GetResourceID() const
{
	return _T("");
}

CDuiString CFlashDemo::GetZIPFileName() const
{
	return _T("QQLogin.zip");
}

CDuiString CFlashDemo::GetSkinFile()
{
	return _T("QQLogin.xml");
}

CDuiString CFlashDemo::GetSkinFolder()
{
	return _T("QQLogin");
}

void CFlashDemo::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SHOWACTIVEX)
		OnShowActiveX(msg);
}

void CFlashDemo::InitWindow()
{
	SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	SetIcon(IDI_MAINFRAME);

	SetDemoBkImage();
	CControlUI* pControl = (CControlUI*)m_PaintManager.FindControl(_T("EditUI2"));
	if (pControl)
		pControl->SetFocus();
}

void CFlashDemo::SetDemoBkImage()
{
	CControlUI* pControl = (CControlUI*)m_PaintManager.FindControl(_T("VLayoutTotal"));
	if (pControl == NULL)
		return;

	LPCTSTR lpszBkImage = _T("noon_.jpg");
	LPCTSTR lpszFlashPath = _T("noon.swf");
	CTime CurrentTime = CTime::GetCurrentTime();
	int nHour = CurrentTime.GetHour();
	if (nHour>=6 && nHour<11)
	{
		m_TimeType = TIME_TYPE_MORNING;
		lpszBkImage = _T("morning_.jpg");
		lpszFlashPath = _T("morning.swf");
	}
	else if (nHour>=11 && nHour <13)
	{
		m_TimeType = TIME_TYPE_NOON;
		lpszBkImage = _T("noon_.jpg");
		lpszFlashPath = _T("noon.swf");
	}
	else if (nHour >=13 && nHour <18)
	{
		m_TimeType = TIME_TYPE_AFTERNOON;
		lpszBkImage = _T("afternoon_.jpg");
		lpszFlashPath = _T("afternoon.swf");
	}
	else
	{
		m_TimeType = TIME_TYPE_NIGHT; 
		lpszBkImage = _T("night_.jpg");
		lpszFlashPath = _T("night.swf");
	}

	TCHAR szFilePath[MAX_PATH];
	PathCombine(szFilePath,CPaintManagerUI::GetInstancePath(),GetSkinFolder());
	PathAppend(szFilePath,lpszFlashPath);
	m_strFlashPath = szFilePath;

	pControl->SetBkImage(lpszBkImage);
}

LRESULT CFlashDemo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CFlashDemo::OnClick(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (strControlName == _T("BtnClose"))
		Close();
	else if (strControlName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CFlashDemo::OnShowActiveX(TNotifyUI& msg)
{
	if (msg.pSender == (CActiveXUI*)m_PaintManager.FindControl(_T("Flash")))
	{
		IShockwaveFlash* pFlash = NULL;
		CActiveXUI* pActiveX = static_cast<CActiveXUI*>(msg.pSender);
		pActiveX->GetControl(IID_IUnknown, (void**)&pFlash);
		if( pFlash != NULL )
		{
			pFlash->put_WMode( _bstr_t(_T("Transparent") ) );
			pFlash->put_Movie( _bstr_t(m_strFlashPath));
			pFlash->DisableLocalSecurity();
			pFlash->put_AllowScriptAccess(L"always");
			BSTR response;
			pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
			pFlash->Release();
		}
	}
}