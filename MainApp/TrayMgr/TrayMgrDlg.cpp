
// TrayMgrDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrayMgr.h"
#include "TrayMgrDlg.h"

#include "LsCommon/CommonFunc.h"
#include "LsCommon/ErrorInfo.h"
#include "LsCommon/OsInfo.h"
#include "LsCommon/WndFunc/WndFunc.h"
#include "LsCommon/Class/Process.h"
#include "processdata.h"

#include <uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTrayMgrDlg �Ի���

#define			WM_SHOW_TRAYICON				(WM_USER + 0x100)


CTrayMgrDlg::CTrayMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayMgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Img.DeleteImageList();
	m_Img.Create(16,16,ILC_COLOR32|ILC_MASK,10,10);
}

void CTrayMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRAYICON_LIST, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CTrayMgrDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SHOW_TRAYICON,&CTrayMgrDlg::OnListTrayIcon)
	ON_NOTIFY(NM_RCLICK,IDC_TRAYICON_LIST,&CTrayMgrDlg::OnRClick)
	ON_COMMAND(ID_RIGHT_SHOW_TRAYICON,&CTrayMgrDlg::OnShowTrayIcon)
	ON_COMMAND(ID_RIGHT_LBTN_DOWN,&CTrayMgrDlg::OnLBtnDown)
	ON_COMMAND(ID_RIGHT_LDB_DOWN,&CTrayMgrDlg::OnLDBDown)
	ON_COMMAND(ID_RIGHT_RBTN_DOWN,&CTrayMgrDlg::OnRBtnClick)
	ON_COMMAND(ID_RIGHT_MOVE_UP,&CTrayMgrDlg::OnMoveUp)
	ON_COMMAND(ID_RIGHT_MOVE_DOWN,&CTrayMgrDlg::OnMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CTrayMgrDlg ��Ϣ�������

BOOL CTrayMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetWindowTheme(m_ListCtrl.m_hWnd,L"Explorer",NULL);
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER|LVS_EX_LABELTIP);
	int nHeaderItem = 0;
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("ͼ������"));
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("����ID"),LVCFMT_RIGHT);
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("������"));
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("���ھ��"));
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("���ڱ���"));
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("ͼ��ID"),LVCFMT_RIGHT);
	m_ListCtrl.InsertColumn(nHeaderItem++ , _T("������Ϣ��"));

	m_ListCtrl.SetImageList(&m_Img,LVSIL_SMALL);
	SendMessage(WM_SHOW_TRAYICON,0,0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTrayMgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTrayMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CTrayMgrDlg::OnListTrayIcon(WPARAM wParam , LPARAM lParam)
{
	m_TrayDataInfo.RemoveAll();
	m_ListCtrl.DeleteAllItems();
	m_Img.SetImageCount(0);

	GetTrayAreaIcon();
	if (OsIsVistaOrLater() == TRUE)
		GetOverflowAreaIcon();
	POSITION pos = m_TrayDataInfo.GetHeadPosition();
	while(pos)
	{
		TRAYDATAINFO& TrayData = m_TrayDataInfo.GetNext(pos);
		int nItem = m_ListCtrl.InsertItem(0x7FFFFFFF,TrayData.strTitle,TrayData.hTrayIcon);
		int nSubItem = 1;
		DWORD dwProcessId;
		GetWindowThreadProcessId(TrayData.hWnd,&dwProcessId);
		m_ListCtrl.SetItemText(nItem,nSubItem++,ConvertDwordToString(dwProcessId,FALSE));

		CString strProcessName;
		GetProcessFullPath(dwProcessId,strProcessName);
		m_ListCtrl.SetItemText(nItem,nSubItem++,PathFindFileName(strProcessName));

		m_ListCtrl.SetItemText(nItem,nSubItem++,ConvertDwordToString((DWORD)TrayData.hWnd));
		TCHAR szWinTitle[MAX_PATH];
		::GetWindowText(TrayData.hWnd,szWinTitle,_countof(szWinTitle));
		m_ListCtrl.SetItemText(nItem,nSubItem++,szWinTitle);
		m_ListCtrl.SetItemText(nItem,nSubItem++,ConvertDwordToString(TrayData.uID,FALSE));
		m_ListCtrl.SetItemText(nItem,nSubItem++,ConvertDwordToMessageString(TrayData.uCallbackMessage));
	}

	AdjustListWidth(m_ListCtrl);
	return TRUE;
}

CString CTrayMgrDlg::ConvertDwordToString(DWORD dwNum,BOOL bHex /* = TRUE*/)
{
	CString strNum;
	LPCTSTR lpszFormat = NULL;
	if (bHex)
		lpszFormat = _T("0x%08X");
	else
		lpszFormat = _T("%u");

	strNum.Format(lpszFormat,dwNum);

	return strNum;
}


CString CTrayMgrDlg::ConvertDwordToMessageString(DWORD dwMessageID)
{
	CString strMessage;
	strMessage.Format(_T("WM_USER+%d"),dwMessageID-WM_USER);
	return strMessage;
}


UINT CTrayMgrDlg::SetTrayIcon(HICON hTrayIcon)
{
	return m_Img.Add(hTrayIcon);
}

void CTrayMgrDlg::OnRClick(NMHDR* pNMHDR,LRESULT* pResult)
{
	NMLISTVIEW* pListView = (NMLISTVIEW*)pNMHDR;
	int nItem = pListView->iItem;
	if (nItem<0)
		return;

	CMenu Menu,*pMenu;
	Menu.LoadMenu(IDR_MENU);
	pMenu = Menu.GetSubMenu(0);

	SetForegroundWindow();
	CPoint pt;
	GetCursorPos(&pt);
	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

void CTrayMgrDlg::OnShowTrayIcon()
{
	POSITION pos = m_TrayDataInfo.GetHeadPosition();
	int nSelect = m_ListCtrl.GetSelectionMark();
	int n=0;
	while(pos)
	{
		TRAYDATAINFO& TrayData = m_TrayDataInfo.GetNext(pos);
		if (n==nSelect)
		{
			NOTIFYICONDATA NotifyIconData;
			memset(&NotifyIconData,0,sizeof(NOTIFYICONDATA));
			NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
			NotifyIconData.hWnd = TrayData.hWnd;
			NotifyIconData.uID = TrayData.uID;
			NotifyIconData.uFlags = NIF_STATE;
			NotifyIconData.dwState = TrayData.bShowTray ? NIS_SHAREDICON:NIS_HIDDEN;
			NotifyIconData.dwStateMask = NIS_HIDDEN;
			
			if (Shell_NotifyIcon(NIM_MODIFY, &NotifyIconData) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("�޸�ͼ��״̬ʧ��"));
				MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK);
				BOOL bHideTray = TrayData.bShowTray == FALSE ? TRUE : FALSE;
				if (::SendMessage(m_hWndTray[TrayData.Pos],TB_HIDEBUTTON,TrayData.uBtnID,bHideTray) == FALSE)
					break;
			}
			break;
		}
		n++;
	}

	SendMessage(WM_SHOW_TRAYICON,0,0);
}

void CTrayMgrDlg::OnLBtnDown()
{
	PostMessageToTrayIcon(WM_LBUTTONDOWN);
	PostMessageToTrayIcon(WM_LBUTTONUP);
}

void CTrayMgrDlg::OnLDBDown()
{
	PostMessageToTrayIcon(WM_LBUTTONDBLCLK);
}

void CTrayMgrDlg::OnRBtnClick()
{
	PostMessageToTrayIcon(WM_RBUTTONDOWN);
	PostMessageToTrayIcon(WM_RBUTTONUP);
}

void CTrayMgrDlg::OnMoveUp()
{
	int nSelect = m_ListCtrl.GetSelectionMark();
	PostMoveMessage(nSelect,nSelect-1);
}

void CTrayMgrDlg::OnMoveDown()
{
	int nSelect = m_ListCtrl.GetSelectionMark();
	PostMoveMessage(nSelect,nSelect+1);
}

void CTrayMgrDlg::PostMessageToTrayIcon(LPARAM lParam)
{
	int n=0;
	int nSelect = m_ListCtrl.GetSelectionMark();
	POSITION pos = m_TrayDataInfo.GetHeadPosition();

	while(pos)
	{
		TRAYDATAINFO& TrayData = m_TrayDataInfo.GetNext(pos);
		if (n==nSelect)
		{
			::PostMessage(TrayData.hWnd,TrayData.uCallbackMessage,TrayData.uID,lParam);
			break;
		}
		n++;
	}
}

void CTrayMgrDlg::PostMoveMessage(UINT uOldPos,UINT uNewPos)
{
	POSITION pos = m_TrayDataInfo.GetHeadPosition();
	int n=0;
	while(pos)
	{
		TRAYDATAINFO& TrayData = m_TrayDataInfo.GetNext(pos);
		if (n==uOldPos)
		{
			::SendMessage(m_hWndTray[TrayData.Pos], TB_MOVEBUTTON, uOldPos, uNewPos);
			SendMessage(WM_SHOW_TRAYICON,0,0);
		}
		n++;
	}
}

BOOL CTrayMgrDlg::GetTrayAreaIcon()
{
	m_hWndTray[0] = GetTrayWndHwnd();
	if (::IsWindow(m_hWndTray[0]) == FALSE)
		return FALSE;

	int nItem = ::SendMessage(m_hWndTray[0],TB_BUTTONCOUNT,NULL,NULL);
	return GetIcon(m_hWndTray[0],nItem,Tray);
}

BOOL CTrayMgrDlg::GetOverflowAreaIcon()
{
	m_hWndTray[1] = GetOverflowHwnd();
	if (::IsWindow(m_hWndTray[1]) == FALSE)
		return FALSE;

	int nItem = ::SendMessage(m_hWndTray[1],TB_BUTTONCOUNT,NULL,NULL);
	return GetIcon(m_hWndTray[1],nItem,Overflow);
}

BOOL CTrayMgrDlg::GetIcon(HWND hWnd,int nItemCount,TRAY_AREA TrayArea)
{
	TBBUTTON TbBtn;
	DWORD dwTrayPid;

	GetWindowThreadProcessId(hWnd, &dwTrayPid);

	TRAYDATAFIREWALL pTrayData;
	CProcessData<TBBUTTON> data(dwTrayPid);
	WCHAR szTipTitle[MAX_PATH*4];

	for (int n=0;n<nItemCount;++n)
	{
		::SendMessage(hWnd,TB_GETBUTTON,n,(LPARAM)data.GetData());
		szTipTitle[0] = L'\0';
		data.ReadData(&TbBtn);	
		data.ReadData<TRAYDATAFIREWALL>(&pTrayData,(LPCVOID)TbBtn.dwData);
		data.ReadData<WCHAR,_countof(szTipTitle)>(szTipTitle,(LPCVOID)TbBtn.iString);

		TRAYDATAINFO TrayData;
		TrayData.Pos = TrayArea;
		TrayData.strTitle = CString(szTipTitle);
		TrayData.hWnd = pTrayData.hwnd;
		TrayData.hTrayIcon = SetTrayIcon(pTrayData.hIcon);
		TrayData.uID = pTrayData.uID;
		TrayData.uBtnID = TbBtn.idCommand;
		TrayData.uCallbackMessage = pTrayData.uCallbackMessage;
		TrayData.bShowTray = (TbBtn.fsState & TBSTATE_HIDDEN) == TBSTATE_HIDDEN ? TRUE : FALSE;
		m_TrayDataInfo.AddTail(TrayData);
	}
	return TRUE;
}