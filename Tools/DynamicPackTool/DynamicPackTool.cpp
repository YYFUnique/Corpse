#include "StdAfx.h"
#include "Resource.h"
#include "DynamicPackTool.h"
#include "DynamicPackage.h"
#include "helper/UmPath.h"
#include "DuiLib/Layout/UITreeLayout.h"
#include "DuiLib/Layout/UISplitLayout.h"
#include "Scintilla/include/Scintilla.h"

#include <shlobj.h>
#include <TOM.h>

CDynamicPackTool::CDynamicPackTool() 
{

}

CDynamicPackTool::~CDynamicPackTool()
{

}

void CDynamicPackTool::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CDynamicPackTool::GetWindowClassName() const
{
	return _T("CDynamicPackTool");
}

UILIB_RESOURCETYPE CDynamicPackTool::GetResourceType() const
{
	return UILIB_FILE;
}

LPCTSTR CDynamicPackTool::GetResourceID() const
{
	return _T("");
}

CDuiString CDynamicPackTool::GetZIPFileName() const
{
	return _T("");
}

CDuiString CDynamicPackTool::GetSkinFile()
{
	return _T("DynamicPackTool.xml");
}

CDuiString CDynamicPackTool::GetSkinFolder()
{
	return _T("DynamicPackTool");
}

void CDynamicPackTool::Notify(TNotifyUI& msg)
{
	//MessageBox(NULL, msg.pSender->GetName(), msg.sType,MB_OK);
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_DBCLICK)
		OnDbClick(msg);
	else if (msg.sType == DUI_MSGTYPE_MENU)
		//MessageBox(NULL, msg.pSender->GetName(), msg.sType,MB_OK);
		OnMenuClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE)
		OnItemClick(msg);
	/*else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
	{
		CControlUI* pControl = (CControlUI*)m_PaintManager.FindControl(_T("splid"));
		RECT rcPos = pControl->GetPos();

		rcPos.top -= 100;
		rcPos.bottom -= 100;

		pControl->SetPos(rcPos);

		pControl->GetParent()->Invalidate();
	}*/
	/*else if (msg.sType == DUI_MSGTYPE_VALUECHANGED)
		OnValueChanged(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);*/
}

//LRESULT CDynamicPackTool::OnNotify(WPARAM wParam, LPARAM lParam)
//{
//	if (m_pScintilla)//
//		m_pScintilla->ScintillaNotify(wParam, lParam);

//	return TRUE;
	/*LRESULT lRet = TRUE;
	SCNotification* pSDNotify = (SCNotification*)lParam;  
	LPNMHDR pnmh = (LPNMHDR)lParam;  
	switch(pnmh->code)  
	{
		case SCN_MODIFIED:
				OutputDebugString(_T("修改文件"));
			break;
		case SCN_MARGINCLICK:
				OutputDebugString(_T("确定是Fold页边点击事件"));
			break;
		case SCN_UPDATEUI://界面更新(单击鼠标，按下箭头等)  
				
			break;
		default:
			return FALSE;
	}

	return FALSE;*/
//}

LRESULT CDynamicPackTool::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
		case WM_MENUCLICK:
				OnMenuItem((CControlUI*)lParam);
			break;
		default:
			return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	}
	return FALSE;
}

void CDynamicPackTool::InitWindow()
{
	OutputProcInfo(_T("DynamicPackTool InitWindow.\r\n"));
	SetIcon(IDI_MAINFRAME);

	CTextUI* pTitle = (CTextUI*)m_PaintManager.FindControl(_T("Title"));
	LPCTSTR lpszUserAdmin = _T("");
	if (IsUserAnAdmin() != FALSE)
		lpszUserAdmin = _T("(管理员)");

	CDuiString strProjectTitle = pTitle->GetText();
	strProjectTitle.Append(lpszUserAdmin);
	pTitle->SetText(strProjectTitle);

	m_pSolutionTool = new CSolution();
	if (m_pSolutionTool)
		m_pSolutionTool->LoadSolution(&m_PaintManager, _T("C:\\NSISProject\\Solution.prj"));

	//富文本编辑器设置属性
	CRichEditUI* pScintillaEditor = (CRichEditUI*)m_PaintManager.FindControl(_T("Scintilla"));
	pScintillaEditor->SetFont(3);
	pScintillaEditor->SetMargin(MAKEWORD(1000,1000));	
	pScintillaEditor->SetFocus();
	//pScintillaEditor->SetChildPadding(20);
}

CControlUI* CDynamicPackTool::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	return pControl;
}

void CDynamicPackTool::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
	else if (msg.pSender->GetName() == _T("BtnMin"))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender->GetName() == _T("BtnRestore"))
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else if (msg.pSender->GetName() == _T("BtnMax"))
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
}

void CDynamicPackTool::OnItemClick(TNotifyUI& msg)
{

}

void CDynamicPackTool::OnDbClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("TitleIcon")))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
}

LRESULT CDynamicPackTool::ResponseDefaultKeyEvent(WPARAM wParam)
{
	return FALSE;
}

LRESULT CDynamicPackTool::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = FALSE;
	if (wParam == VK_F7)	//编译当前项目
		lRet = OnBuildRun();
	else if (wParam == VK_F5)
		lRet = OnExecute();

	bHandled = lRet;
	return lRet;
}

LRESULT CDynamicPackTool::OnBuildRun()
{
	LRESULT lRet = FALSE;
	do 
	{
		CDynamicPackage* pDynamicPackage = new CDynamicPackage;

		if (pDynamicPackage == FALSE)
			break;

		if (pDynamicPackage->Init() == FALSE)
			break;

		// 启动编译线程

		lRet = TRUE;
	} while (FALSE);

	return lRet;
}

LRESULT CDynamicPackTool::OnExecute()
{	
	return OnBuildRun();
}

void CDynamicPackTool::OnMenuClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Project"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("Project.xml"));
		pMenu->Init(NULL,strXmlFile, pt, &m_PaintManager);
	}
	else if (msg.pSender->GetName() == _T("Scintilla"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("RichText.xml"));
		pMenu->Init(NULL,strXmlFile, pt, &m_PaintManager);
		/*CMenuUI* pMenuUI = pMenu->GetMenuUI();
		CControlUI* pCopy = pMenuUI->FindSubControl(_T("Delete"));
		pCopy->SetEnabled(false);*/
	}
}

void CDynamicPackTool::OnMenuItem(CControlUI* pControl)
{
	OutputProcInfo(pControl->GetText());
	CDuiString strRootName = pControl->GetManager()->GetRoot()->GetName();
	if (strRootName == _T("ProjectMenu"))
	{
		if (pControl->GetName() == _T("SetStart"))
		{
			m_pSolutionTool->SetProjectActiveStart();
		}
	}
}

void CDynamicPackTool::OutputProcInfo(LPCTSTR lpszDbgInfo)
{
	static CRichEditUI* pScintillaEditor = (CRichEditUI*)m_PaintManager.FindControl(_T("DbgWnd"));
	if (pScintillaEditor != NULL)
	{
		CDuiString strTipInfo = lpszDbgInfo;
		strTipInfo.TrimRight(_T("\r\n"));
		strTipInfo.AppendFormat(_T("\r\n"));
		pScintillaEditor->AppendText(strTipInfo);
	}
}