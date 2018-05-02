#include "stdafx.h"
#include "Resource.h"
#include "MainWnd.h"
#include "api.h"

extern extra_parameters* g_pluginParms;

namespace DuiLib
{
	TBCIAMainWnd::TBCIAMainWnd()
	{

	}

	TBCIAMainWnd::~TBCIAMainWnd()
	{

	}

	void TBCIAMainWnd::OnFinalMessage(HWND hWnd)
	{
		OutputDebugString(_T("OnFinalMessage"));
		WindowImplBase::OnFinalMessage(hWnd);
		delete this; 
	}

	LPCTSTR TBCIAMainWnd::GetWindowClassName() const
	{
		return _T("LsCIASkinEngine");
	}

	UILIB_RESOURCETYPE TBCIAMainWnd::GetResourceType() const
	{
		return UILIB_ZIP;
	}

	void TBCIAMainWnd::SetFolderFilePath(LPCTSTR lpszFolderFilePath)
	{
		m_strFolderFilePath = lpszFolderFilePath;
	}

	CDuiString TBCIAMainWnd::GetSkinFolder()
	{
		return m_strFolderFilePath;
	}

	void TBCIAMainWnd::SetZIPFileName(LPCTSTR lpszZIPFileName)
	{
		m_strZIPFileName = lpszZIPFileName;
	}

	CDuiString TBCIAMainWnd::GetZIPFileName() const
	{
		return m_strZIPFileName;
	}

	void TBCIAMainWnd::SetSkinFile(LPCTSTR lpszSkinFile)
	{
		m_strSkinFile = lpszSkinFile;
	}

	CDuiString TBCIAMainWnd::GetSkinFile()
	{
		return m_strSkinFile;
	}

	CPaintManagerUI* TBCIAMainWnd::GetPaintManager()
	{
		return &m_PaintManager;
	}

	void TBCIAMainWnd::Notify(TNotifyUI& msg)
	{
		CCallbackMap::iterator iter = m_ControlCallbackMap.find(msg.pSender->GetName());		
		if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0) {
			if (iter != m_ControlCallbackMap.end())
				g_pluginParms->ExecuteCodeSegment(iter->second - 1, 0);
		}
		else if (_tcsicmp(msg.sType, DUI_MSGTYPE_TEXTCHANGED) == 0) {
			if (iter != m_ControlCallbackMap.end())
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0);
		} else {
			WindowImplBase::Notify(msg);
		}
	}

	void TBCIAMainWnd::InitWindow()
	{
		// 通过安装程序获取图标，设置到安全过程显示
		TCHAR szModuleName[MAX_PATH];
		GetModuleFileName(NULL, szModuleName, _countof(szModuleName));

		SHFILEINFO shFileInfo;
		SHGetFileInfo(szModuleName, NULL, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_LARGEICON);

		// 设置大图标
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) shFileInfo.hIcon);
		//	设置小图标
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) shFileInfo.hIcon);
	}

	void TBCIAMainWnd::SaveToControlCallbackMap(CDuiString strName, int nCallback)
	{ 
		m_ControlCallbackMap[strName] = nCallback; 
	}

	////////////////////////////////////////////////////////////////////////////////
	//// 对话框
	////////////////////////////////////////////////////////////////////////////////
	TBCIAMessageBox::TBCIAMessageBox()
	{

	}

	TBCIAMessageBox::~TBCIAMessageBox()
	{

	}

	LPCTSTR TBCIAMessageBox::GetWindowClassName() const 
	{
		return _T("TBCIAMessageBox"); 
	}

	void TBCIAMessageBox::OnFinalMessage(HWND hWnd) 
	{
		m_PaintManager.RemoveNotifier(this);
		delete this;
	}

	/*CDuiString TBCIAMessageBox::GetZIPFileName() const
	{
		return _T("");
	}*/

	CDuiString TBCIAMessageBox::GetSkinFolder()
	{
		return _T("");
	}

	CDuiString TBCIAMessageBox::GetSkinFile()
	{
		return _T("Modal.xml");
	}

	LPCTSTR TBCIAMessageBox::GetResourceID() const
	{
		return MAKEINTRESOURCE(IDR_MODAL);
	}

	UILIB_RESOURCETYPE TBCIAMessageBox::GetResourceType() const
	{
		return UILIB_ZIPRESOURCE;
	}

	void TBCIAMessageBox::InitWindow()
	{
	
	}

	void TBCIAMessageBox::Notify(TNotifyUI& msg)
	{
		if (msg.sType == DUI_MSGTYPE_CLICK)
			OnClick(msg);
	}

	CPaintManagerUI* TBCIAMessageBox::GetPaintManager()
	{	
		return &m_PaintManager;
	}

	void TBCIAMessageBox::OnClick(TNotifyUI& msg)
	{
		if (msg.pSender == m_PaintManager.FindControl(_T("BtnConfirm")))
			Close(IDYES);
		else if (msg.pSender == m_PaintManager.FindControl(_T("BtnCancel")))
			Close(IDNO);
		else if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
			Close(IDCANCEL);
	}
}