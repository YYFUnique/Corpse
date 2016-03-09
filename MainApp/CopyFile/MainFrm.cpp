
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "CopyFile.h"
#include "CopyFileDoc.h"
#include "CopyFileView.h"

#include "MainFrm.h"
#include "AddIniRuleDlg.h"
#include "ToolSettingDlg.h"
#define ID_INDICATOR_TEXT 1
#include "ToolSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TEXT, &CMainFrame::OnUpdatePane)

	ON_MESSAGE(WM_UPDATE_STSTUS,&CMainFrame::SetStatusBarValue)
	ON_COMMAND(ID_TOOL_NOTEPAD_OPEN , &CMainFrame::OpenIniFile)
	ON_COMMAND(ID_TOOL_EDIT_COPY , &CMainFrame::CopyFileAgain)
	ON_COMMAND(ID_TOOL_EDIT_ABOUT , &CMainFrame::OnAbout)
	ON_COMMAND(ID_TOOL_ADD_INI , &CMainFrame::AddIniRule)
	ON_COMMAND(ID_TOOL_SAVE_TOINI , &CMainFrame::SaveAllToIniFile)
	ON_COMMAND(ID_MENU_OPEN_PATH, &CMainFrame::OnMenuOpenPath)
	ON_COMMAND(ID_COPYFILE_SETTING , &CMainFrame::OnCopyFileSetting)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	//ID_SEPARATOR,           // 状态行指示器

	/*ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,*/
	ID_INDICATOR_TEXT,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	ChangeModifyStatus();
}

CMainFrame::~CMainFrame()
{
	ReleaseProcessErrorInfo();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	m_wndStatusBar.SetPaneInfo(0,ID_INDICATOR_TEXT,SBPS_NORMAL,600);


	// TODO: 如果不需要可停靠工具栏，则删除这三行
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return TRUE;
}

//设置状态栏
LRESULT CMainFrame::SetStatusBarValue(WPARAM wParam , LPARAM lParam)
{
	m_wndStatusBar.SetPaneText(0, (TCHAR*)wParam , TRUE);
	return TRUE;
}

void CMainFrame::OnUpdatePane(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
//打开配置文件
void CMainFrame::OpenIniFile()
{
	TCHAR szFilePath[MAX_PATH];
	CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
	pCopyFileView->GetIniFilePath(szFilePath);

	if(PathFileExists(szFilePath) == FALSE)
	{
		MessageBox(_T("文件不存在，打开失败"));
		return;
	}

	ShellExecute(NULL , _T("open") , szFilePath , _T("notepad.exe") , NULL , SW_SHOW);
}
//重新复制文件
void CMainFrame::CopyFileAgain()
{
	CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
	pCopyFileView->CopyFileToDestation();
}

//帮助对话框
void CMainFrame::OnAbout()
{
	CDialog AboutDlg(IDD_ABOUTBOX);

	AboutDlg.DoModal();
}

void CMainFrame::AddIniRule()
{
	CString strTargetFilePath;
	CAddIniRuleDlg AddIniRuleDlg(strTargetFilePath);

	AddIniRuleDlg.DoModal();
	if (strTargetFilePath.IsEmpty() != TRUE)
	{
		CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
		pCopyFileView->ShowUpdate(strTargetFilePath);
		
		pCopyFileView->UpdateFileInfo();
	}
}

void CMainFrame::SaveAllToIniFile()
{
	TCHAR szIniFilePath[MAX_PATH];
	CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
	pCopyFileView->GetIniFilePath(szIniFilePath);

	TCHAR szIniSection[1024];
	GetPrivateProfileString(NULL,NULL,NULL,szIniSection,1024,szIniFilePath);

	TCHAR* pszIniSection = szIniSection;
	while(pszIniSection[0] != NULL)
	{
		CString strFileName(pszIniSection);
		if (strFileName.FindOneOf(_T(".")) != -1)	//如果包含'.'
			WritePrivateProfileString(pszIniSection , NULL , NULL , szIniFilePath);		//删除这个节

		pszIniSection = pszIniSection + _tcslen(pszIniSection)+1;
	}

	CListCtrl& ListCtrl = pCopyFileView->GetListCtrl();
	int nItemTotal = ListCtrl.GetItemCount();
	if(nItemTotal == FALSE)
		return;
	for (int nIndex=0; nIndex < nItemTotal; ++nIndex)
	{
		CString strFileName,strFilePath,strDesFilePath;
		strFileName  = ListCtrl.GetItemText(nIndex , 0);
		strFilePath = ListCtrl.GetItemText(nIndex , 1);
		strDesFilePath = ListCtrl.GetItemText(nIndex , 2);

		WritePrivateProfileString(strFileName , _T("SrcPath") , strFilePath , szIniFilePath);
		WritePrivateProfileString(strFileName , _T("DesPath") , strDesFilePath ,szIniFilePath );
	}
//	AfxMessageBox(_T("保存成功"));
	PostMessage(WM_UPDATE_STSTUS,(WPARAM)(_T("保存成功")),0);
	ChangeModifyStatus(MODIFY_SAVED);
}

void CMainFrame::ChangeModifyStatus(MODIFY_SAVE MDSave/*=NOT_MODIFY*/)
{
	m_ModyfySave = MDSave;
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

void CMainFrame::OnClose()
{
	if (m_ModyfySave ==  MODIFY_NOT_SAVE)
	{
		int iRet = AfxMessageBox(_T("是否将更改保存到对应文件?"),MB_ICONQUESTION|MB_YESNOCANCEL);
		if(iRet == IDYES)
			SaveAllToIniFile();
		else if (iRet == IDCANCEL)
			return;
	}
	CWnd::OnClose();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnMenuOpenPath()
{
	// TODO: 在此添加命令处理程序代码
	//ShellExecute();
}

void CMainFrame::OnCopyFileSetting()
{
	CToolSettingDlg SettingDlg;
	SettingDlg.DoModal();
}

void CMainFrame::GetTargetFilePath(LPTSTR lpszTargetFilePath,DWORD dwSize)
{
	TCHAR szIniFilePath[MAX_PATH];
	CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
	pCopyFileView->GetIniFilePath(szIniFilePath);

	GetPrivateProfileString(_T("BaseInfo"),_T("DesPath"),NULL,lpszTargetFilePath,dwSize,szIniFilePath);
}

BOOL CMainFrame::SetTargetFilePath(LPCTSTR lpszTargetFilePath,LPCTSTR lpszIniFilePath)
{
	return WritePrivateProfileString(_T("BaseInfo"),_T("DesPath"),lpszTargetFilePath,lpszIniFilePath);
}