#include "stdafx.h"
#include "SkinEngine.h"
#include "DllCore/File/MiniDump.h"

extern CString strInstallPageName;
extern BOOL bCancleOrExit;
namespace DuiLib
{
	CSkinEngine::CSkinEngine()
	{

	}

	CSkinEngine::~CSkinEngine()
	{

	}

	void CSkinEngine::OnFinalMessage( HWND hWnd )
	{
		m_PaintManager.RemoveNotifier( this );	
		delete this; 
	}

	LPCTSTR CSkinEngine::GetWindowClassName() const
	{
		return _T("LsCIASkinEngine");
	}

	CDuiString CSkinEngine::GetZIPFileName() const
	{
		return m_strZIPFileName;
	}

	CDuiString CSkinEngine::GetSkinFile()
	{
		return m_strSkinFile;
	}

	CDuiString CSkinEngine::GetSkinFolder()
	{
		return _T("");
	}

	UILIB_RESOURCETYPE CSkinEngine::GetResourceType() const
	{
		return UILIB_FILE;
	}

	void CSkinEngine::SetZIPFileName(LPCTSTR lpszZIPFileName)
	{
		m_strZIPFileName = lpszZIPFileName;
	}

	void CSkinEngine::SetSkinFile(LPCTSTR lpszSkinFile)
	{
		m_strSkinFile = lpszSkinFile;
	}

	CPaintManagerUI& CSkinEngine::GetPaintManager()
	{
		return m_PaintManager;
	}

	void CSkinEngine::Notify(TNotifyUI& msg)
	{
		if (msg.sType == DUI_MSGTYPE_CLICK)
			OnClick(msg);
	}

	LRESULT CSkinEngine::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam , BOOL& bHandled)
	{
		bCancleOrExit = TRUE;
		return WindowImplBase::OnClose(uMsg,wParam,lParam,bHandled);
	}

	LRESULT CSkinEngine::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
	}

	LRESULT CSkinEngine::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
	}

	CControlUI* CSkinEngine::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	void CSkinEngine::InitWindow()
	{
		CMiniDump::InitDumpDebugInfo();
	}

	void CSkinEngine::OnClick(TNotifyUI& msg)
	{
		CDuiString strCtrlName = msg.pSender->GetName();
		if (strCtrlName == _T("BtnCancel") || strCtrlName == _T("BtnClose"))
		{
			//提示是否真的需要退出
			bCancleOrExit = TRUE;
			PostQuitMessage(0);
		}
		else if (strCtrlName == _T("BtnMin"))
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else if (strCtrlName == _T("BtnAccept") || strCtrlName == _T("BtnNext"))
			OnBtnAccept(msg);
		else if (strCtrlName == _T("BtnBack"))
			OnBtnBack(msg);
		else if (strCtrlName == _T("BtnInstall") || strCtrlName == _T("BtnUnInstall"))
			OnInstallOrUninstall(msg);
		else if (strCtrlName == _T("BtnComplete"))
			OnComplete(msg);
	}

	void CSkinEngine::OnComplete(TNotifyUI& msg)
	{
		PostQuitMessage(0);
	}

	void CSkinEngine::OnInstallOrUninstall(TNotifyUI& msg)
	{
		//开始安装或者卸载的时候需要退出Duilib自己的消息循环
		//将消息控制交给NSIS
		bCancleOrExit = FALSE;
		PostQuitMessage(0);
		OnBtnAccept(msg);
		//禁用安装页面的上一步和下一步
// 		CControlUI* pBtnBack = (CControlUI*)m_PaintManager.FindControl(_T("BtnBack"));
// 		pBtnBack->SetEnabled(false);
// 		pBtnBack->SetVisible(false);
		CControlUI* pBtnNext = (CControlUI*)m_PaintManager.FindControl(_T("BtnNext"));
		pBtnNext->SetEnabled(false);
	}

	void CSkinEngine::OnInstallOrUninstallOK()
	{
		CControlUI* pBtnNext = (CControlUI*)m_PaintManager.FindControl(_T("BtnNext"));
		pBtnNext->SetEnabled(true);
	}

	void CSkinEngine::OnBtnAccept(TNotifyUI& msg)
	{
		CTabLayoutUI* pWizardTab = (CTabLayoutUI*)m_PaintManager.FindControl(strInstallPageName);
		pWizardTab->SelectItem(pWizardTab->GetCurSel() + 1);
	}

	void CSkinEngine::OnBtnBack(TNotifyUI& msg)
	{
		CTabLayoutUI* pWizardTab = (CTabLayoutUI*)m_PaintManager.FindControl(strInstallPageName);
		pWizardTab->SelectItem(pWizardTab->GetCurSel() - 1);
	}

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	CLsCIAMessageBox::CLsCIAMessageBox()
	{

	}

	CLsCIAMessageBox::~CLsCIAMessageBox()
	{

	}

	LPCTSTR CLsCIAMessageBox::GetWindowClassName() const 
	{
		return _T("LsCIASkinEngine"); 
	}

	void CLsCIAMessageBox::OnFinalMessage(HWND hWnd) 
	{
		m_PaintManager.RemoveNotifier(this);
		delete this;
	}

	CDuiString CLsCIAMessageBox::GetZIPFileName() const
	{
		return m_strZIPFileName;
	}

	CDuiString CLsCIAMessageBox::GetSkinFile()
	{
		return m_strSkinFile;
	}

	CDuiString CLsCIAMessageBox::GetSkinFolder()
	{
		return m_strSkinFolder;
	}

	void CLsCIAMessageBox::InitWindow()
	{

	}

	LRESULT CLsCIAMessageBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
	}

	void CLsCIAMessageBox::Notify(TNotifyUI& msg)
	{
		OutputDebugString(msg.pSender->GetName());
		if (msg.sType == DUI_MSGTYPE_CLICK)
			OnClick(msg);
	}

	CPaintManagerUI& CLsCIAMessageBox::GetPaintManager()
	{	
		return m_PaintManager;
	}

	void CLsCIAMessageBox::SetZIPFileName(LPCTSTR lpszZIPFileName)
	{
		m_strZIPFileName = lpszZIPFileName;
	}

	void CLsCIAMessageBox::SetSkinFile(LPCTSTR lpszSkinFile)
	{
		m_strSkinFile = lpszSkinFile;
	}

	void CLsCIAMessageBox::SetSkinFileFolder(LPCTSTR lpszSkinFileFolder)
	{
		m_strSkinFolder = lpszSkinFileFolder;
	}

	void CLsCIAMessageBox::OnClick(TNotifyUI& msg)
	{
		if (msg.pSender->GetName() == _T("BtnClose"))
			PostQuitMessage(0);
		else if (msg.pSender->GetName() == _T("BtnMin"))
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
}