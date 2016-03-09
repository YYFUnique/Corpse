
// NetShareMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NetShareMgr.h"
#include "NetShareMgrDlg.h"

#include <lm.h>
#pragma comment(lib,"Netapi32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "LsCommon/ErrorInfo.h"
#include "LsCommon/CommonFunc.h"
// CNetShareMgrDlg 对话框




CNetShareMgrDlg::CNetShareMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetShareMgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetShareMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetShareMgrDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK,IDC_LIST_NETSHARE,&CNetShareMgrDlg::OnRClick)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CNetShareMgrDlg::OnBnClickedBtnRefresh)
	ON_COMMAND(ID_MENU_REFRESH, &CNetShareMgrDlg::OnMenuRefresh)
	ON_COMMAND(ID_MENU_DELETE, &CNetShareMgrDlg::OnMenuDelete)
END_MESSAGE_MAP()


// CNetShareMgrDlg 消息处理程序

BOOL CNetShareMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_NETSHARE);
	int nIndex=0;
	pListCtrl->InsertColumn(nIndex++,_T("网络共享名"));
	pListCtrl->InsertColumn(nIndex++,_T("共享路径"));
	pListCtrl->InsertColumn(nIndex++,_T("描述"));
	pListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	if (ShowNetShare() == FALSE)
	{
		MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK);
		PostQuitMessage(0);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetShareMgrDlg::OnPaint()
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
HCURSOR CNetShareMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CNetShareMgrDlg::EnumLocalNetShare(CLsNetShareInfoArray& NetShareInfoArray)
{
	DWORD dwReadItemCount = 0;
	DWORD dwAllItemCount = 0;

	LPBYTE lpDataBuffer = NULL;
	NET_API_STATUS dwRet = NetShareEnum (NULL , 502, &lpDataBuffer , MAX_PREFERRED_LENGTH , &dwReadItemCount, &dwAllItemCount, NULL);
	if (dwRet != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR , dwRet , _T("枚举当前主机网络共享信息失败"));
		return FALSE;
	}

	PSHARE_INFO_502 pShareInfo = (PSHARE_INFO_502)lpDataBuffer;
	for(DWORD i=0;i<dwReadItemCount;i++,++pShareInfo)
	{
		//由于IPC$在XP系统中无法使用命令删除，故暂时不对该共享目录进行处理
		//if (_wcsicmp(pShareInfo->shi502_netname,L"IPC$") == 0)
		//	continue;

		LS_NET_SHARE_INFO LsNetShareInfo;
		LsNetShareInfo.strNetName = pShareInfo->shi502_netname;
		LsNetShareInfo.dwType = pShareInfo->shi502_type;
		LsNetShareInfo.strRemark = pShareInfo->shi502_remark;
		LsNetShareInfo.dwPermissions = pShareInfo->shi502_permissions;
		LsNetShareInfo.dwMaxUses = pShareInfo->shi502_max_uses;
		LsNetShareInfo.dwCurrentUses = pShareInfo->shi502_current_uses;
		LsNetShareInfo.strFilePath = pShareInfo->shi502_path;
		LsNetShareInfo.strPassword = pShareInfo->shi502_passwd;
		NetShareInfoArray.Add(LsNetShareInfo);
	}
	NetApiBufferFree(lpDataBuffer);

	return TRUE;
}

BOOL CNetShareMgrDlg::ShowNetShare()
{
	CLsNetShareInfoArray LsNetShareInfoArray;
	if (EnumLocalNetShare(LsNetShareInfoArray) != TRUE)
	{
		SetErrorTitle(_T("遍历网络共享失败"));
		return FALSE;
	}

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_NETSHARE);

	pListCtrl->DeleteAllItems();
	for (int i=0;i<LsNetShareInfoArray.GetSize();++i)
	{
		LS_NET_SHARE_INFO& LsNetShareInfo = LsNetShareInfoArray.ElementAt(i);
		int nItem = pListCtrl->InsertItem(0x7FFFFFFF,LsNetShareInfo.strNetName);
		pListCtrl->SetItemText(nItem,1,LsNetShareInfo.strFilePath);
		pListCtrl->SetItemText(nItem,2,LsNetShareInfo.strRemark);
	}

	AdjustListWidth(*pListCtrl);
	return TRUE;
}
void CNetShareMgrDlg::OnBnClickedBtnRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ShowNetShare() == FALSE)
		MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK);
}

void CNetShareMgrDlg::OnRClick(NMHDR* pNMHDR,LRESULT* pResult)
{
	NMLISTVIEW* pLVCD = (NMLISTVIEW*)pNMHDR;

	if (pLVCD->iItem <0)
		return;

	CMenu Menu;
	Menu.LoadMenu(IDR_LIST_MENU);
	CMenu* pMenu;
	pMenu = Menu.GetSubMenu(0);

	SetForegroundWindow();
	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

void CNetShareMgrDlg::OnMenuRefresh()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedBtnRefresh();
}

void CNetShareMgrDlg::OnMenuDelete()
{
	// TODO: 在此添加命令处理程序代码
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_NETSHARE);
	int nSelectItem = pListCtrl->GetSelectionMark();
	if (nSelectItem<0)
		return;

	//CStringW strNetShareName = CStringW(pListCtrl->GetItemText(nSelectItem,0));
	WCHAR wszNetShareName[MAX_PATH];
	wcscpy_s(wszNetShareName , _countof(wszNetShareName) , CStringW(pListCtrl->GetItemText(nSelectItem,0)));
	NET_API_STATUS dwRet = NetShareDel(NULL,wszNetShareName,0);
	if (dwRet != NERR_Success)
	{
		SetErrorInfo(SYSTEM_ERROR,dwRet,_T("删除网络共享文件夹%s失败"),wszNetShareName);
		MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK);
	}
	else
		pListCtrl->DeleteItem(nSelectItem);
}
