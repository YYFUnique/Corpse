
// MainFrm.cpp : CMainFrame ���ʵ��
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
	//ID_SEPARATOR,           // ״̬��ָʾ��

	/*ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,*/
	ID_INDICATOR_TEXT,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	m_wndStatusBar.SetPaneInfo(0,ID_INDICATOR_TEXT,SBPS_NORMAL,600);


	// TODO: �������Ҫ��ͣ������������ɾ��������
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return TRUE;
}

//����״̬��
LRESULT CMainFrame::SetStatusBarValue(WPARAM wParam , LPARAM lParam)
{
	m_wndStatusBar.SetPaneText(0, (TCHAR*)wParam , TRUE);
	return TRUE;
}

void CMainFrame::OnUpdatePane(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
//�������ļ�
void CMainFrame::OpenIniFile()
{
	TCHAR szFilePath[MAX_PATH];
	CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
	pCopyFileView->GetIniFilePath(szFilePath);

	if(PathFileExists(szFilePath) == FALSE)
	{
		MessageBox(_T("�ļ������ڣ���ʧ��"));
		return;
	}

	ShellExecute(NULL , _T("open") , szFilePath , _T("notepad.exe") , NULL , SW_SHOW);
}
//���¸����ļ�
void CMainFrame::CopyFileAgain()
{
	CCopyFileView* pCopyFileView=(CCopyFileView*)GetActiveView();
	pCopyFileView->CopyFileToDestation();
}

//�����Ի���
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
		if (strFileName.FindOneOf(_T(".")) != -1)	//�������'.'
			WritePrivateProfileString(pszIniSection , NULL , NULL , szIniFilePath);		//ɾ�������

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
//	AfxMessageBox(_T("����ɹ�"));
	PostMessage(WM_UPDATE_STSTUS,(WPARAM)(_T("����ɹ�")),0);
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

void CMainFrame::OnClose()
{
	if (m_ModyfySave ==  MODIFY_NOT_SAVE)
	{
		int iRet = AfxMessageBox(_T("�Ƿ񽫸��ı��浽��Ӧ�ļ�?"),MB_ICONQUESTION|MB_YESNOCANCEL);
		if(iRet == IDYES)
			SaveAllToIniFile();
		else if (iRet == IDCANCEL)
			return;
	}
	CWnd::OnClose();
}

// CMainFrame ���

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


// CMainFrame ��Ϣ�������

void CMainFrame::OnMenuOpenPath()
{
	// TODO: �ڴ���������������
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