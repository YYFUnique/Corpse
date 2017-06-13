#include "StdAfx.h"
#include "TxMiniSkin.h"
#include "DllCore/Utils/Registry.h"

#define HKLM_SOFTWARE_AUTORUN	_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")

CTxMiniSkin::CTxMiniSkin(HWND hParent)
{
	Create(hParent, _T("TxMiniSkinDialog"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CTxMiniSkin::~CTxMiniSkin()
{

}

void CTxMiniSkin::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CTxMiniSkin::GetWindowClassName() const
{
	return _T("TxMiniSkin");
}

CDuiString CTxMiniSkin::GetSkinFile()
{
	return _T("TxMiniSkin.xml");
}

CDuiString CTxMiniSkin::GetSkinFolder()
{
	return _T("TTimeHelper");
}

UILIB_RESOURCETYPE CTxMiniSkin::GetResourceType() const
{
	return UILIB_FILE;
}

void CTxMiniSkin::InitWindow()
{
	CenterWindow();
	RECT rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	OffsetRect(&rcWnd, 0, 90);
	MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, FALSE);
}

void CTxMiniSkin::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnChanged(msg);
}

void CTxMiniSkin::OnClick(TNotifyUI& msg)
{
	CDuiString strSendName = msg.pSender->GetName();
	if (strSendName == _T("BtnClose"))
		Close(IDCANCEL);
	else if (strSendName == _T("BtnSure"))
	{
		//触发OnFinalMessage析构对象
		Close(IDOK);
	}
	else if(strSendName == _T("BtnCannel"))
		Close(IDCANCEL);
}

void CTxMiniSkin::OnChanged(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnAutoStart")))
	{
		COptionUI* pAutoStart = (COptionUI*)msg.pSender->GetInterface(DUI_CTR_OPTION);
		BOOL bSelect = pAutoStart->IsSelected();
		SetAutoStart(bSelect);
	}
}

BOOL CTxMiniSkin::SetAutoStart(BOOL bAutoStart)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (bAutoStart != FALSE)
		{
			TCHAR szModuleFile[MAX_PATH];
			GetModuleFileName(NULL, szModuleFile, _countof(szModuleFile));

			CString strTimeHelperPath;
			BOOL bRet = LsRegQueryValue(HKEY_LOCAL_MACHINE , HKLM_SOFTWARE_AUTORUN, _T("TTimeHelper"), strTimeHelperPath);
			if (bRet != FALSE && strTimeHelperPath.CompareNoCase(szModuleFile))
			{
				bSuccess = TRUE;
				break;
			}

			if (LsRegSetValue(HKEY_LOCAL_MACHINE, HKLM_SOFTWARE_AUTORUN, _T("TTimeHelper"), szModuleFile) == FALSE)
				break;
		}
		else
		{
			if (SHDeleteValue(HKEY_LOCAL_MACHINE, HKLM_SOFTWARE_AUTORUN, _T("TTimeHelper")) != ERROR_SUCCESS)
				break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}