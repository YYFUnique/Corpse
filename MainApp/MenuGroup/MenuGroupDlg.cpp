
// MenuGroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuGroup.h"
#include "MenuGroupDlg.h"
#include "ModifyItem.h"
#include "LsCommon/RegClass.h"
#include "LsCommon/TextTools.h"
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuGroupDlg 对话框
#define		INVALID_ERROR_NUMBER				-1

#define		WM_LOAD_MENU_GROUP			WM_USER+0x1000
#define		RIGHT_MENU_SUB_PATH				_T("Directory\\Background\\shell\\LsRightMenu")
#define		RIGHT_MENU_SUB_ITEM_INFO		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CommandStore\\shell")
CMenuGroupDlg::CMenuGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMenuGroupDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ImageList.Create(20,20,ILC_COLOR32|ILC_MASK,10,5);
}

void CMenuGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MENU_GROUP, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CMenuGroupDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_MENU_GROUP,OnRclickList)
	ON_NOTIFY(NM_DBLCLK,IDC_TREE_MENU_GROUP,OnDbClick)
	ON_NOTIFY(TVN_ENDLABELEDIT,IDC_TREE_MENU_GROUP,OnEndlabeledit)
	ON_MESSAGE(WM_LOAD_MENU_GROUP,OnLoadMenuGroup)
	ON_COMMAND(ID_RMENU_ADD,OnAddItem)
	ON_COMMAND(ID_RMENU_EDIT,OnEditItem)
	ON_COMMAND(ID_RMENU_DELETE,OnDelItem)
	ON_COMMAND(ID_RMENU_UP,OnItemUp)
	ON_COMMAND(ID_RMENU_DOWN,OnItemDown)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CREATE, &CMenuGroupDlg::OnBnClickedBtnCreate)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CMenuGroupDlg::OnBnClickedBtnRemove)
END_MESSAGE_MAP()


// CMenuGroupDlg 消息处理程序

BOOL CMenuGroupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_TreeCtrl.SetImageList(&m_ImageList,TVSIL_NORMAL);
	SetWindowTheme(m_TreeCtrl.GetSafeHwnd(),L"Explorer",NULL);
	m_TreeCtrl.SetExtendedStyle(TVS_EX_AUTOHSCROLL, TVS_EX_AUTOHSCROLL);
	PostMessage(WM_LOAD_MENU_GROUP,0,0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMenuGroupDlg::OnPaint()
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
HCURSOR CMenuGroupDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMenuGroupDlg::OnLoadMenuGroup(WPARAM wParam,LPARAM lParam)
{
	CRegClass RightMenuReg(HKEY_CLASSES_ROOT);
	HICON hRootIcon = NULL;
	CString strFileIcon;
	if (RightMenuReg.OpenKey(RIGHT_MENU_SUB_PATH) == TRUE)
		RightMenuReg.ReadKey(_T("icon"),strFileIcon);

	UINT nIcon = INVALID_ERROR_NUMBER;
	if (strFileIcon.IsEmpty() == FALSE)
		nIcon = GetFileIcon(strFileIcon);
	if (nIcon == INVALID_ERROR_NUMBER)
	{
		TCHAR szFilePath[MAX_PATH];
		GetModuleFileName(NULL,szFilePath,_countof(szFilePath));
		strFileIcon = szFilePath;
		nIcon = GetFileIcon(strFileIcon);
	}
	CString strItemName;
	if (RightMenuReg.IsRegOpen())
		RightMenuReg.ReadKey(_T("MUIVerb"),strItemName);
	if (strItemName.IsEmpty() == TRUE)
		strItemName = _T("右键菜单");

	TREEITEMINFO TreeInfo;
	TreeInfo.strIconPath = strFileIcon;
	TreeInfo.strRunCmd = _T("");
	POSITION pos = m_MenuRunCmd.AddTail(TreeInfo);
	HTREEITEM hRoot = m_TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE,strItemName,
							0,0,0,0,(LPARAM)pos,TVI_ROOT,TVI_LAST);
	CString strSubItem;
	if (RightMenuReg.IsRegOpen())
		RightMenuReg.ReadKey(_T("SubCommands"),strSubItem);

	if (strSubItem.IsEmpty() == FALSE)
	{
		CStringArray strSubItemArray;
		AnalyseFormattedTextInfoItem(strSubItem,strSubItemArray,_T(';'));
		RightMenuReg.SetHKEY(HKEY_LOCAL_MACHINE);
		CRegClass RegRunCmd;
		for (int i=0;i<strSubItemArray.GetSize();++i)
		{
			CString strItemRegPath;
			TCHAR szItemRunCmd[MAX_PATH];
			strItemRegPath.Format(_T("%s\\%s\\"),RIGHT_MENU_SUB_ITEM_INFO,strSubItemArray.GetAt(i));
			PathCombine(szItemRunCmd,strItemRegPath,_T("command"));
			if (RightMenuReg.OpenKey(strItemRegPath) == TRUE)
			{	
				TREEITEMINFO TreeItemInfo;
				CString strItemName,strItemIcon,strRunCmd;
				UINT nIcon = 0;
				RightMenuReg.ReadKey(_T(""),strItemName);
				if (RightMenuReg.ReadKey(_T("icon"),strItemIcon) == TRUE)
					nIcon = GetFileIcon(strItemIcon);
				POSITION pos = NULL;
				if (RegRunCmd.OpenKey(szItemRunCmd) && RegRunCmd.ReadKey(_T(""),strRunCmd))
				{
					TreeItemInfo.strIconPath = strItemIcon;
					TreeItemInfo.strRunCmd = strRunCmd;
					TreeItemInfo.strName = strSubItemArray.GetAt(i);
					pos = m_MenuRunCmd.AddTail(TreeItemInfo);
				}
				m_TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM,strItemName,nIcon,nIcon,0,0,(LPARAM)pos,hRoot,TVI_SORT);
			}
		}		
	}

	m_TreeCtrl.Expand(hRoot,TVE_EXPAND);
	return TRUE;
}

UINT CMenuGroupDlg::GetFileIcon(LPCTSTR lpszFilePath)
{
	TCHAR szFilePath[MAX_PATH];
	UINT nIcon = 0;
	CString strFilePath(lpszFilePath);
	strFilePath.Replace(_T("\""),_T(""));
	_stscanf_s(strFilePath,_T("%[^,],%d"),szFilePath,_countof(szFilePath),&nIcon);
	HICON hRootIcon = ExtractIcon(NULL,szFilePath,nIcon);
	if (hRootIcon)
		return m_ImageList.Add(hRootIcon);

	return INVALID_ERROR_NUMBER;
}

void CMenuGroupDlg::OnDbClick(NMHDR* pNMHdr,LPARAM* lResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_TreeCtrl.ScreenToClient(&pt);
	UINT uFlag;
	HTREEITEM hTreeItem = m_TreeCtrl.HitTest(pt,&uFlag);

	if (hTreeItem == NULL)
		return;
	POSITION pos = (POSITION)m_TreeCtrl.GetItemData(hTreeItem);
	if (pos != NULL)
	{
		TREEITEMINFO& TreeItemInfo = m_MenuRunCmd.GetNext(pos);
		TCHAR szFilePath[MAX_PATH],szParam[MAX_PATH];
		_stscanf_s(TreeItemInfo.strRunCmd,_T("%[^ ] %s"),szFilePath,_countof(szFilePath),szParam,_countof(szParam));
		ShellExecute(m_hWnd,_T("open"),szFilePath,szParam,NULL,SW_SHOW);
	}
}

void CMenuGroupDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pTreeView = (NMTREEVIEW*)pNMHDR;
	CPoint pt;
	GetCursorPos(&pt);
	m_TreeCtrl.ScreenToClient(&pt);
	UINT uFlag;
	HTREEITEM hTreeItem = m_TreeCtrl.HitTest(pt,&uFlag);

	if (hTreeItem == NULL)
		return;

	m_TreeCtrl.SelectItem(hTreeItem);
	CMenu Menu,*pRClickMenu;
	Menu.LoadMenu(IDR_MENU_GROUP);
	pRClickMenu = Menu.GetSubMenu(0);
	
	GetCursorPos(&pt);
	HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
	if (hRoot == hTreeItem)
	{
		pRClickMenu->EnableMenuItem(ID_RMENU_EDIT,MF_BYCOMMAND|MF_DISABLED);
		pRClickMenu->EnableMenuItem(ID_RMENU_DELETE,MF_BYCOMMAND|MF_DISABLED);
		pRClickMenu->EnableMenuItem(ID_RMENU_UP,MF_BYCOMMAND|MF_DISABLED);
		pRClickMenu->EnableMenuItem(ID_RMENU_DOWN,MF_BYCOMMAND|MF_DISABLED);
	}
	pRClickMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	SetForegroundWindow();
}

void CMenuGroupDlg::OnAddItem()
{
	HTREEITEM hTreeItem = m_TreeCtrl.GetSelectedItem();
	if (hTreeItem == NULL)
		return;
	CModifyItem ModifyItem(_T("新建菜单"));
	if (ModifyItem.DoModal() == IDOK)
	{	
		TREEITEMINFO TreeItemInfo;
		ModifyItem.GetItemInfo(TreeItemInfo);
		CString strItemName = ModifyItem.GetItemName();
		POSITION pos = m_MenuRunCmd.AddTail(TreeItemInfo);
		HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
		UINT nIcon = GetFileIcon(TreeItemInfo.strIconPath);
		m_TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM,strItemName,nIcon,nIcon,0,0,(LPARAM)pos,hRoot,hTreeItem);
		m_TreeCtrl.Expand(hRoot,TVE_EXPAND);
	}
}

void CMenuGroupDlg::OnEditItem()
{
	HTREEITEM hTreeItem = m_TreeCtrl.GetSelectedItem();
	if (hTreeItem == NULL)
		return;
	CString strItemName = m_TreeCtrl.GetItemText(hTreeItem);
	POSITION pos = (POSITION)m_TreeCtrl.GetItemData(hTreeItem);
	TREEITEMINFO& TreeItemInfo = m_MenuRunCmd.GetAt(pos);
	CModifyItem ModifyItem(strItemName,TreeItemInfo.strIconPath,TreeItemInfo.strRunCmd,TreeItemInfo.strName);
	if (ModifyItem.DoModal() == IDOK)
	{
		TREEITEMINFO TreeItem;
		ModifyItem.GetItemInfo(TreeItem);
		strItemName = ModifyItem.GetItemName();
		m_TreeCtrl.SetItemText(hTreeItem,strItemName);
		UINT nIcon = GetFileIcon(TreeItem.strIconPath);
		pos = m_MenuRunCmd.AddTail(TreeItem);
		m_TreeCtrl.SetItemImage(hTreeItem,nIcon,nIcon);
		m_TreeCtrl.SetItemData(hTreeItem,(DWORD_PTR)pos);
	}
}

void CMenuGroupDlg::OnDelItem()
{
	HTREEITEM hTreeItem = m_TreeCtrl.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		RemoveItemInfo(hTreeItem);
		m_TreeCtrl.DeleteItem(hTreeItem);
	}
}

void CMenuGroupDlg::OnItemUp()
{
	HTREEITEM hTreeItem = m_TreeCtrl.GetSelectedItem();
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hPrevItem = m_TreeCtrl.GetPrevSiblingItem(hTreeItem);
	if (CopyItem(hPrevItem,hRootItem,hTreeItem) != NULL)
		m_TreeCtrl.DeleteItem(hPrevItem);
}

void CMenuGroupDlg::OnItemDown()
{
	HTREEITEM hTreeItem = m_TreeCtrl.GetSelectedItem();
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hNextItem = m_TreeCtrl.GetNextSiblingItem(hTreeItem);
	if (CopyItem(hTreeItem,hRootItem,hNextItem) != NULL)
		m_TreeCtrl.DeleteItem(hTreeItem);
}

void CMenuGroupDlg::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = TRUE;
}

HTREEITEM CMenuGroupDlg::CopyItem(HTREEITEM hItem,HTREEITEM hTreeParent,HTREEITEM hTreeAfter)
{
	TV_INSERTSTRUCT  InsertStruct;
	HTREEITEM        hNewItem;
	CString				   strItemName;

	InsertStruct.item.hItem = hItem;
	InsertStruct.item.mask  = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
    m_TreeCtrl.GetItem(&InsertStruct.item);
	strItemName = m_TreeCtrl.GetItemText(hItem);

	InsertStruct.item.pszText = NULL;

	InsertStruct.item.lParam = m_TreeCtrl.GetItemData(hItem);
	InsertStruct.hParent = hTreeParent;
	InsertStruct.hInsertAfter = hTreeAfter;
	InsertStruct.item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;
	hNewItem = m_TreeCtrl.InsertItem(&InsertStruct);
	m_TreeCtrl.SetItemText(hNewItem,strItemName);
	return hNewItem;
}

void CMenuGroupDlg::OnBnClickedBtnCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CRegClass RightMenuReg(HKEY_CLASSES_ROOT);
	HICON hRootIcon = NULL;
	HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
	POSITION pos = (POSITION)m_TreeCtrl.GetItemData(hRoot);
	if (RightMenuReg.CreateKey(RIGHT_MENU_SUB_PATH) == TRUE)
	{
		TREEITEMINFO& TreeItemInfo = m_MenuRunCmd.GetNext(pos);
		RightMenuReg.WriteKey(_T("icon"),TreeItemInfo.strIconPath);
		RightMenuReg.WriteKey(_T("MUIVerb"),m_TreeCtrl.GetItemText(hRoot));
		RightMenuReg.WriteKey(_T("Position"),_T("top"));
		CString strSubCommand;
		HTREEITEM hChildItem = m_TreeCtrl.GetChildItem(hRoot);
		CRegClass RegItemInfo;
		TCHAR szRegPath[MAX_PATH];
		for (int i=0;hChildItem != NULL;++i)
		{
			POSITION pos = (POSITION)m_TreeCtrl.GetItemData(hChildItem);
			TREEITEMINFO& TreeItemInfo = m_MenuRunCmd.GetNext(pos);
			if (TreeItemInfo.strName.IsEmpty())
				TreeItemInfo.strName.Format(_T("LsRightMenu.%d"),i);
			strSubCommand.AppendFormat(_T("%s;"),TreeItemInfo.strName);
			PathCombine(szRegPath,RIGHT_MENU_SUB_ITEM_INFO,TreeItemInfo.strName);
			if (RegItemInfo.CreateKey(szRegPath) != FALSE)
			{
				RegItemInfo.WriteKey(_T(""),m_TreeCtrl.GetItemText(hChildItem));
				RegItemInfo.WriteKey(_T("icon"),TreeItemInfo.strIconPath);
				PathAppend(szRegPath,_T("command"));
				if (RegItemInfo.CreateKey(szRegPath) != FALSE)
					RegItemInfo.WriteKey(_T(""),TreeItemInfo.strRunCmd);
			}
			hChildItem = m_TreeCtrl.GetNextSiblingItem(hChildItem);
		}
		strSubCommand.TrimRight(_T(";"));
		RightMenuReg.WriteKey(_T("SubCommands"),strSubCommand);
	}

	MessageBox(_T("保存成功"),_T("提示"),MB_OK);
}

void CMenuGroupDlg::OnBnClickedBtnRemove()
{
	// TODO: 在此添加控件通知处理程序代码
	CRegClass RightMenuReg(HKEY_CLASSES_ROOT);
	if (RightMenuReg.OpenKey(_T("Directory\\Background\\shell\\")) == TRUE)
		if (RightMenuReg.DeleteKey(_T("LsRightMenu")) == TRUE)
		{
			HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
			HTREEITEM hChild = m_TreeCtrl.GetChildItem(hRoot);
			while(hChild)
			{
				m_TreeCtrl.DeleteItem(hChild);
				hChild = m_TreeCtrl.GetNextSiblingItem(hChild);
			}
			MessageBox(_T("移除成功"),_T("提示"),MB_OK);
		}
}

void CMenuGroupDlg::RemoveItemInfo(HTREEITEM hTreeItem)
{
	POSITION pos = (POSITION)m_TreeCtrl.GetItemData(hTreeItem);
	if (pos != NULL)
		m_MenuRunCmd.RemoveAt(pos);
}