#include "StdAfx.h"
#include "AddPathFileItem.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Log/LogHelper.h"
#include "DrvControl/FileSystemFilter.h"

CAddPathFileItem::CAddPathFileItem(HWND hWnd)
	: m_bEnabled(FALSE)
	, m_dwMatchMode(0)
{
	Create(hWnd, _T("添加文件规则"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CAddPathFileItem::~CAddPathFileItem()
{

}

// void CAddPathFileItem::OnFinalMessage(HWND hWnd)
// {
// 	WindowImplBase::OnFinalMessage(hWnd);
// 	delete this;
// }

LPCTSTR CAddPathFileItem::GetWindowClassName() const
{
	return _T("AddPathFileItem");
}

CDuiString CAddPathFileItem::GetSkinFile()
{
	return _T("AddPathFileItem.xml");
}

CDuiString CAddPathFileItem::GetSkinFolder()
{
	return _T("MzfFileMon");
}

void CAddPathFileItem::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_RETURN)
		OnReturn(msg);
}

void CAddPathFileItem::InitWindow()
{
	CEditUI2* pFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("FilePath"));
	pFilePath->SetFocus();
	CenterWindow();
}

void CAddPathFileItem::OnClick(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	if (strSender == _T("BtnClose"))
		Close(IDCANCEL);
	else if (strSender == _T("BtnCancel"))
		Close(IDCANCEL);
	else if (strSender == _T("BtnFinish"))
		OnFinish();
}

void CAddPathFileItem::OnReturn(TNotifyUI& msg)
{
	CDuiString strName = msg.pSender->GetName();
	if (strName == _T("FilePath"))
		OnFinish();
}

LRESULT CAddPathFileItem::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CAddPathFileItem::OnFinish()
{
	CEditUI2* pFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("FilePath"));
	if (pFilePath->GetText().IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入需要匹配的文件夹(名)或路径！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	LPCTSTR lpszMatchMode[] = {_T("PreName"), _T("SubName"), _T("OnlyName"), _T("All")};
	COptionUI* pMatchMode = NULL;
	for (int n=0; n<_countof(lpszMatchMode); ++n)
	{
		pMatchMode = (COptionUI*)m_PaintManager.FindControl(lpszMatchMode[n]);
		if (pMatchMode->IsSelected() != FALSE)
			break;
	}

	COptionUI* pMatchEnable = (COptionUI*)m_PaintManager.FindControl(_T("Enable"));

	m_strFilePath = pFilePath->GetText();
	m_dwMatchMode = _ttoi(pMatchMode->GetUserData());
	m_bEnabled = pMatchMode->IsSelected();

	Close(IDOK);
}

BOOL CAddPathFileItem::IsMatchRuleEnabled() const
{
	return m_bEnabled;
}

DWORD CAddPathFileItem::GetMatchMode() const
{
	return m_dwMatchMode;
}

CDuiString CAddPathFileItem::GetMatchFilePath() const
{
	return m_strFilePath;
}