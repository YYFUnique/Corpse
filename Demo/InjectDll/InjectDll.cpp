#include "StdAfx.h"
#include "InjectDll.h"
#include "Resource.h"
#include "Wow64Ext/Wow64InjectDll.h"

CInjectDll::CInjectDll()
{

}

CInjectDll::~CInjectDll()
{

}

void CInjectDll::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CInjectDll::GetWindowClassName() const
{
	return _T("InjectDll");
}

UILIB_RESOURCETYPE CInjectDll::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CInjectDll::GetZIPFileName() const
{
	return _T("InjectDll.zip");
}

CDuiString CInjectDll::GetSkinFile()
{
	return _T("InjectDll.xml");
}

CDuiString CInjectDll::GetSkinFolder()
{
	return _T("InjectDll");
}

void CInjectDll::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CInjectDll::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	//以32位编译的应用程序，只能注入到32位应用程序中
	//m_RemoteInject.Attach(_T("explorer.exe"), _T("C:\\test\\ITrayClock.dll"), 2000);
	//Wow64Injectx64(6016,_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\x64_Release\\ITrayClock.dll"));
}

void CInjectDll::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnInject")))
	{
		CEditUI2* pProcessID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
		Wow64Injectx64(_ttoi(pProcessID->GetText()), _T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\x64_Release\\ITrayClock.dll"));
	}
}

BOOL CInjectDll::InjectDll(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId)
{
	return TRUE;
}