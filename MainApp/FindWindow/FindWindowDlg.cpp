
// FindWindowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FindWindow.h"
#include "FindWindowDlg.h"
#include "Resource.h"
#include "SystemSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFindWindowDlg 对话框


CFindWindowDlg::CFindWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindWindowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFindWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFindWindowDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_NOTIFYICON , OnNotiyfIconMessage)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_EXIT_MAINFRAME, &CFindWindowDlg::OnExitMainframe)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SHOW_MAINFRAME, &CFindWindowDlg::OnShowMainframe)
	ON_COMMAND(ID_MENU_SYSTEM_SETTING, &CFindWindowDlg::OnMenuSystemSetting)
	ON_BN_CLICKED(IDC_CHECK_SHOW_NUM_WAY, &CFindWindowDlg::OnBnClickedCheckShowNumWay)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CFindWindowDlg 消息处理程序

BOOL CFindWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	NotifyData.cbSize=sizeof(NOTIFYICONDATA);
	NotifyData.hWnd=this->m_hWnd;
	NotifyData.uID=IDR_MAINFRAME;
	NotifyData.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
	NotifyData.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	NotifyData.uCallbackMessage=WM_NOTIFYICON;

	CString strTip(_T("原形毕露 PowerBy Unique"));
	_tcscpy_s(NotifyData.szTip,strTip);
	Shell_NotifyIcon(NIM_ADD,&NotifyData);

	TCHAR szModuleFileName[MAX_PATH];
	GetModuleFileName(NULL , szModuleFileName , sizeof(szModuleFileName));
	PathRenameExtension(szModuleFileName,_T(".ini"));

	TCHAR szShowWindowBuff[MAX_TITLE_LEN];
	GetPrivateProfileString(_T("SystemSet") , 
										_T("ShowWindowOnStartUp") ,
										NULL , 
										szShowWindowBuff , 
										sizeof(szShowWindowBuff) , 
										szModuleFileName);
	
	if(_tcscmp(szShowWindowBuff , _T("Show")) == FALSE)
	{
		ShowWindow(SW_SHOW);
		CenterWindow();
	}

	m_HandlePic.SubclassDlgItem(IDC_STATIC_RELEASE_PIC , this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFindWindowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFindWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFindWindowDlg::LoadStringById(int nStringId , TCHAR* pszBuffer , int iSizeBuff)
{
	LoadString(AfxGetApp()->m_hInstance , nStringId , pszBuffer , iSizeBuff);
}

void CFindWindowDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	
	delete this;
}

LRESULT CFindWindowDlg::OnNotiyfIconMessage( WPARAM wParam, LPARAM lParam )
{
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowMainFrameWindow();
	}
	else if ( lParam == WM_RBUTTONDOWN)
	{
		CMenu Menu;
		Menu.LoadMenu(IDR_MENU_NOTIFYICON);
		CMenu* pMenu = Menu.GetSubMenu(0);
		CPoint Pt;
		GetCursorPos(&Pt);
		SetForegroundWindow();
		pMenu->TrackPopupMenu(TPM_LEFTALIGN,Pt.x,Pt.y,this);
	}
	return TRUE;
}

//显示主窗口界面
void CFindWindowDlg::ShowMainFrameWindow()
{
	//ShowWindow(SW_RESTORE);
	WINDOWPLACEMENT WinPlacement;
	GetWindowPlacement(&WinPlacement);
	switch(WinPlacement.showCmd)
	{
		case SW_SHOWNORMAL:
			ShowWindow(SW_SHOW);
			SetForegroundWindow();	
			break;
		default:
			ShowWindow(SW_RESTORE);
			break;
	}
}
void CFindWindowDlg::OnExitMainframe()
{
	// TODO: 在此添加命令处理程序代码
	PostQuitMessage(0);
}

void CFindWindowDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Shell_NotifyIcon(NIM_DELETE , &NotifyData);
}

void CFindWindowDlg::ShowWindowData(HWND hWndTargetWindow)
{
	if(!IsWindow(hWndTargetWindow))
		return;
	m_OtherWnd = hWndTargetWindow;

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_SHOW_NUM_WAY);
	CString strFormat(pButton->GetCheck()?_T("%08x"):_T("%d"));

	CString strWndHwnd;
	strWndHwnd.Format(strFormat,m_OtherWnd);
	SetDlgItemText(IDC_STATIC_WND_MAIN,strWndHwnd.MakeUpper());

	TCHAR szTitleName[MAX_TITLE_LEN];
	::GetWindowText(m_OtherWnd , szTitleName , MAX_TITLE_LEN);
	SetDlgItemText(IDC_STATIC_WND_TEXT , szTitleName);

	TCHAR szClassName[MAX_CLASS_NAME];
	GetClassName(m_OtherWnd , szClassName , MAX_CLASS_NAME);
	SetDlgItemText(IDC_STATIC_WND_CLASS , szClassName);

	DWORD dwStyle = GetWindowLong(m_OtherWnd , GWL_STYLE);
	CString strStyle;
	strStyle.Format(strFormat,dwStyle);
	SetDlgItemText(IDC_STATIC_WND_STYLE , strStyle.MakeUpper());

	
	CRect rcWindow;
	::GetWindowRect(m_OtherWnd , &rcWindow);
	CString strWindowRect;
	strWindowRect.Format(_T("(%d,%d)-(%d,%d) %d*%d"),
													rcWindow.left , rcWindow.top ,
													rcWindow.right , rcWindow.bottom ,
													rcWindow.right-rcWindow.left ,
													rcWindow.bottom - rcWindow.top
										);
	SetDlgItemText(IDC_STATIC_WND_RECT , strWindowRect);

	DWORD dwProcessID;
	DWORD dwThreadID = GetWindowThreadProcessId(m_OtherWnd , &dwProcessID);

	CString strThreadID;
	strThreadID.Format(strFormat,dwThreadID);
	SetDlgItemText(IDC_STATIC_WND_THREADID , strThreadID.MakeUpper());

	CString strWindowID;
	strWindowID.Format(strFormat,dwProcessID);
	SetDlgItemText(IDC_STATIC_WND_PROCESSID , strWindowID.MakeUpper());
}
void CFindWindowDlg::OnShowMainframe()
{
	// TODO: 在此添加命令处理程序代码
	PostMessage(WM_NOTIFYICON , 0 , WM_LBUTTONDBLCLK);
}

void CFindWindowDlg::OnMenuSystemSetting()
{
	// TODO: 在此添加命令处理程序代码
	CSystemSet SystemDialog;

	SystemDialog.DoModal();
}

void CFindWindowDlg::OnBnClickedCheckShowNumWay()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindowData(m_OtherWnd);
}

void CFindWindowDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
	PostQuitMessage(0);
}
