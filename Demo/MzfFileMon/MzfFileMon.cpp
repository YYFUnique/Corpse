#include "StdAfx.h"
#include "MzfFileMon.h"
#include "Resource.h"
#include "Utils/ControlBuilder.h"
#include "DllCore/Window/DwmHelper.h"
#include "DllCore/Window/FloatWindow.h"
#include "DllCore/Log/LogHelper.h"

#include "IAPIHook/IATHook.h"

const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));

CMzfFileMon::CMzfFileMon()
{
	m_FileProcMgr.SetVirtualWnd(this, &m_PaintManager);
}

CMzfFileMon::~CMzfFileMon()
{
	m_FileProcMgr.ClearVirtualWnd(this);
	UninstallHook();
}

void CMzfFileMon::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CMzfFileMon::GetWindowClassName() const
{
	return _T("FileMgr");
}

UILIB_RESOURCETYPE CMzfFileMon::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CMzfFileMon::GetZIPFileName() const
{
	return _T("MzfFileMon.zip");
}

CDuiString CMzfFileMon::GetSkinFile()
{
	return _T("MzfFileMon.xml");
}

CDuiString CMzfFileMon::GetSkinFolder()
{
	return _T("MzfFileMon");
}

DUI_BEGIN_MESSAGE_MAP(CMzfFileMon, CNotifyPump)  
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()  

LRESULT CMzfFileMon::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT bRes = FALSE;
	bHandled = TRUE;

	if (uMsg == WM_TASKBARCREATED)
	{
		PinToDesktop();
		return FALSE;
	}

	switch(uMsg)
	{
		case WM_MENUCLICK:
				bRes = OnMenuClick(wParam,lParam);
			break;
		default:
			bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

void CMzfFileMon::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	InstallHook(2860);

	// 桌面进程崩溃后，需要重新设置拥有者
	PinToDesktop();

	CHorizontalLayoutUI* pControl = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("TabSwitch"));
	if (pControl)
		m_PaintManager.SendNotify(pControl->GetItemAt(0), DUI_MSGTYPE_SELECTCHANGED);

	HMODULE hModule = LoadLibrary(_T("dwmapi.dll"));
	if (hModule != NULL)
	{
		typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD, LPCVOID, DWORD);
		DwmSetWindowAttributeFn pFnDwmSetWindowAttribute = (DwmSetWindowAttributeFn)GetProcAddress(hModule, "DwmSetWindowAttribute");
		if (pFnDwmSetWindowAttribute != NULL)
		{
			DWMNCRENDERINGPOLICY RenderPolicy = DWMNCRP_ENABLED;

			pFnDwmSetWindowAttribute(m_hWnd, DWMWA_EXCLUDED_FROM_PEEK, &RenderPolicy, sizeof(RenderPolicy));

			DWMFLIP3DWINDOWPOLICY Flip3D = DWMFLIP3D_EXCLUDEABOVE;
			pFnDwmSetWindowAttribute(m_hWnd, DWMWA_FLIP3D_POLICY, &Flip3D, sizeof(Flip3D));
		}
		FreeLibrary(hModule);
	}
}

CControlUI* CMzfFileMon::CreateControl(LPCTSTR pstrClass)
{
	CControlBuilder RootBuilder;
	return RootBuilder.CreateControl(pstrClass);
}

void CMzfFileMon::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
	else if (msg.pSender->GetName() == _T("BtnMin"))
	{
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender->GetName() == _T("BtnSkin"))
	{
		HWND hProgman = FindWindow(_T("Progman"), _T("Program Manager"));
		::PostMessage(hProgman, WM_USER+0x1000,NULL,NULL);
	}
}

CControlUI* CMzfFileMon::GetViewObject(CTabLayoutUI* pTabLayout,int nIndex)
{
	CVerticalLayoutUI* pSubObject = (CVerticalLayoutUI*)pTabLayout->GetItemAt(nIndex);
	CTabLayoutUI* pSubTab = (CTabLayoutUI*)pSubObject->FindSubControlByClassName(_T("TabLayoutUI"));
	if (pSubTab == NULL )
		return NULL;

	int nSelectId = pSubTab->GetCurSel();
	return pSubTab->GetItemAt(nSelectId);
}

void CMzfFileMon::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent() == (CControlUI*)m_PaintManager.FindControl(_T("TabSwitch")))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			return;

		UINT nIndex = _ttoi(msg.pSender->GetUserData());
		pTabLayout->SelectItem(nIndex);

		CControlUI* pMsgNotify = GetViewObject(pTabLayout,nIndex);
		if (pMsgNotify)
			m_PaintManager.SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
	}
}

LRESULT CMzfFileMon::OnMenuClick(WPARAM wParam, LPARAM lParam)
{
	CControlUI* pControl = (CControlUI*)lParam;
	ASSERT(pControl);
	if (pControl == NULL)
		return FALSE;

	//获取菜单对应根节点的控件名称
	CDuiString strMenuName = pControl->GetManager()->GetRoot()->GetName();
	if (strMenuName == _T("HideMenu"))
		m_FileProcMgr.OnAppMenu(pControl);
	
	return TRUE;
}

void CMzfFileMon::PinToDesktop()
{
	if (IsWindow(m_hWnd))
	{
		HWND hWndProgram = FindWindow(_T("Progman"), _T("Program Manager")); 
		HWND hOwner = GetWindow(hWndProgram, GW_HWNDPREV);

		if (hOwner == NULL)
			hOwner = FindWindowEx(hWndProgram, NULL, _T("SHELLDLL_DefView"), NULL); 

		if (IsWindow(hOwner))
			SetWindowLong(m_hWnd, GWL_HWNDPARENT, (LONG)hOwner); 
	}
}