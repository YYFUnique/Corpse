
// ClipBoardMontorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ClipBoardMontor.h"
#include "ClipBoardMontorDlg.h"

#include "LsCommon/TextTools.h"
#include "LsCommon/FileTools.h"
#include "LsCommon/CommonFunc.h"
#include <gdiplus.h>
#include <shlwapi.h>
//#include <shlobj.h>
#include <Uxtheme.h>
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"uxtheme.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDM_SENDTOFIRST         0x0200
#define IDM_SENDTOLAST          0x02FF

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CClipBoardMontorDlg 对话框

CClipBoardMontorDlg::CClipBoardMontorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClipBoardMontorDlg::IDD, pParent)
{
	m_RunFirst = TRUE;
	m_ImgList.Create(16,16,ILC_COLOR32|ILC_MASK,5,1);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CClipBoardMontorDlg::~CClipBoardMontorDlg()
{
	if (m_ImgList.GetImageCount() != 0)
		m_ImgList.DeleteImageList();
}

void CClipBoardMontorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIP_BOARD_DATA, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CClipBoardMontorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CHANGECBCHAIN()
	ON_WM_DRAWCLIPBOARD()
	ON_WM_DESTROY()
	ON_NOTIFY_EX(NM_DBLCLK,IDC_LIST_CLIP_BOARD_DATA,&CClipBoardMontorDlg::OnDBLClick)
	ON_NOTIFY_EX(NM_RCLICK,IDC_LIST_CLIP_BOARD_DATA,&CClipBoardMontorDlg::OnRClick)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDM_SENDTOFIRST,IDM_SENDTOLAST,&CClipBoardMontorDlg::OnSendTo)
	ON_COMMAND(ID_MENU_COPY, &CClipBoardMontorDlg::OnMenuCopy)
	ON_COMMAND(ID_MENU_DELETE, &CClipBoardMontorDlg::OnMenuDelete)
END_MESSAGE_MAP()


// CClipBoardMontorDlg 消息处理程序

BOOL CClipBoardMontorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TXT));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TXT));

	SetWindowTheme(m_ListCtrl.GetSafeHwnd(),L"Explorer",NULL);
	int nIndex=0;
	m_ListCtrl.InsertColumn(nIndex++,_T("设备类型"),LVCFMT_LEFT,40);
	m_ListCtrl.InsertColumn(nIndex++,_T("主机名称"),LVCFMT_LEFT,70);
	m_ListCtrl.InsertColumn(nIndex++,_T("所属部门"),LVCFMT_RIGHT,145);
	m_ListCtrl.InsertColumn(nIndex++,_T("使用人信息"),LVCFMT_RIGHT,145);
	m_ListCtrl.InsertColumn(nIndex++,_T("IP地址"),LVCFMT_LEFT,390);
	m_ListCtrl.InsertColumn(nIndex++,_T("子网掩码"),LVCFMT_RIGHT,145);
	m_ListCtrl.InsertColumn(nIndex++,_T("MAC地址"),LVCFMT_RIGHT,145);
	m_ListCtrl.InsertColumn(nIndex++,_T("上报机IP"),LVCFMT_LEFT,390);
	m_ListCtrl.InsertColumn(nIndex++,_T("上报时间"),LVCFMT_LEFT,390);
	ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
	
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_SINGLEROW|LVS_EX_DOUBLEBUFFER);
	m_ListCtrl.SetImageList(&m_ImgList,LVSIL_SMALL);
	m_hNextClipboard = SetClipboardViewer();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClipBoardMontorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClipBoardMontorDlg::OnPaint()
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
HCURSOR CClipBoardMontorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClipBoardMontorDlg::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter)
{
	CDialog::OnChangeCbChain(hWndRemove,hWndAfter);
	if (m_hNextClipboard == NULL)
		m_hNextClipboard = hWndAfter;

	if (hWndRemove == m_hNextClipboard)
		m_hNextClipboard = hWndAfter;
	else
		::PostMessage(m_hNextClipboard,WM_CHANGECBCHAIN,
								(WPARAM)hWndRemove, (LPARAM)hWndAfter);
}

void CClipBoardMontorDlg::OnDrawClipboard()
{
	if (m_RunFirst == TRUE)
	{
		m_RunFirst = FALSE;
		return;
	}

	CDialog::OnDrawClipboard();  

	// 如果观看链中在当前程序下面存在下一个程序的话   
	// 就传递一个WM_DRAWCLIPBOARD 消息给它   

	if(m_hNextClipboard)  
		::SendMessage(m_hNextClipboard, WM_DRAWCLIPBOARD, 0, 0); 

	if (GetClipboardOwner()->m_hWnd != m_hWnd)
		ShowClipBoardData();
}

BOOL CClipBoardMontorDlg::ShowClipBoardData()
{
	HGLOBAL hClipboardData = NULL;  
	UINT nFormat[] = {CF_BITMAP,CF_UNICODETEXT,CF_HDROP};
	UINT nFormatType = GetPriorityClipboardFormat(nFormat, sizeof(nFormat)); 

	if (OpenClipboard() == FALSE)
		return FALSE;

	hClipboardData = GetClipboardData(nFormatType);
	if (hClipboardData == NULL)
		return FALSE;

	switch (nFormatType)
	{
		case CF_UNICODETEXT:
			{
				LPCTSTR lpStr = (LPCTSTR)GlobalLock(hClipboardData); 
				GlobalUnlock(hClipboardData); 
				InsertItemToList(lpStr,CLIPBOARDTYPE_Text);
			}
			break;
		case CF_BITMAP:
				//hClipboardData = GetClipboardData(CF_BITMAP);
			break;
		case CF_HDROP:
			{
				HDROP hDrop = (HDROP)hClipboardData;
				int iFiles=DragQueryFile(hDrop,-1,NULL,0);
				TCHAR szFilePath[MAX_PATH];
				for (int i=0;i<iFiles;i++)
				{
					UINT cch=DragQueryFile(hDrop,i,NULL,0);
					DragQueryFile(hDrop,i,szFilePath,_countof(szFilePath));
					
					InsertItemToList(szFilePath,GetFileType(szFilePath));
				}

			}
			break;
	}

	CloseClipboard();

	return TRUE;
}

void CClipBoardMontorDlg::OnDestroy()
{
	ChangeClipboardChain(m_hNextClipboard);
	CDialog::OnDestroy();
}

CString CClipBoardMontorDlg::ConvertClipBoardTypeToString(CLIPBOADRTYPE ClipBoardType)
{
	CString strClipBoardType = _T("未知类型");
	switch(ClipBoardType)
	{
		case CLIPBOARDTYPE_Text:
				strClipBoardType = _T("文本");
			break;
		case CLIPBOARDTYPE_Img:
				strClipBoardType = _T("图像文件");
			break;
		case CLIPBOADRTYPE_File:
				strClipBoardType = _T("文件");
			break;
		case CLIPBOADRTYPE_Execute:
				strClipBoardType = _T("可执行文件");	
			break;
		case CLIPBOADRTYPE_Folder:
				strClipBoardType = _T("文件夹");
			break;
		case CLIPBOADRTYPE_Disk:
				strClipBoardType = _T("磁盘");
			break;
	}

	return strClipBoardType;
}

BOOL CClipBoardMontorDlg::InsertItemToList(LPCTSTR lpszClipData,CLIPBOADRTYPE ClipBoardType)
{
	DWORD dwTotal = m_ListCtrl.GetItemCount();
	DWORD dwIconIndex = (DWORD)ClipBoardType;
	
	if (ClipBoardType == CLIPBOADRTYPE_File)
		dwIconIndex = InsertIconToImageList(lpszClipData,m_ImgList,TRUE);

	m_ListCtrl.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,dwTotal,FormatNumToString(dwTotal+1),0,0,dwIconIndex,ClipBoardType);
	m_ListCtrl.SetItemText(dwTotal,1,lpszClipData);
	m_ListCtrl.SetItemText(dwTotal,2,ConvertClipBoardTypeToString(ClipBoardType));
	m_ListCtrl.SetItemText(dwTotal,3,GetCurrentFormatTime(CURRENT_FORMAT_TIME_Normal));
	return TRUE;
}

BOOL CClipBoardMontorDlg::OnDBLClick(UINT nID,NMHDR* pNMHDR,LRESULT* pLResult)
{
	NMLISTVIEW* pListView = (NMLISTVIEW*)pNMHDR;
	if (pListView->iItem < 0)
		return FALSE;

	CLIPBOADRTYPE ClipBoardType = (CLIPBOADRTYPE)m_ListCtrl.GetItemData(pListView->iItem);
	CString strListViewText = m_ListCtrl.GetItemText(pListView->iItem,1);
	if (ClipBoardType == CLIPBOADRTYPE_File)
	{
		ShellExecute(m_hWnd,_T("open"),strListViewText,NULL,NULL,SW_SHOW);
	}
	else if (ClipBoardType == CLIPBOARDTYPE_Text)
	{
		//某些文本内容也可以打开，如HTTP://10.1.1.5,FTP://10.1.1.5....;
	}

	return TRUE;
}

BOOL CClipBoardMontorDlg::OnRClick(UINT nID,NMHDR* pNMHDR,LRESULT* pLResult)
{
	NMLISTVIEW* pListView = (NMLISTVIEW*)pNMHDR;
	if (pListView->iItem < 0)
		return FALSE;

	m_MenuInfoList.RemoveAll();

	CMenu MenuTabInfo,*pMenuTabInfo;
	MenuTabInfo.LoadMenu(IDR_CLIP_BOARD_MENU);
	pMenuTabInfo = MenuTabInfo.GetSubMenu(MENULIST_ListMenu);

	CLIPBOADRTYPE ClipBoardType = (CLIPBOADRTYPE)m_ListCtrl.GetItemData(pListView->iItem);
	if (ClipBoardType == CLIPBOARDTYPE_Text)
	{
		pMenuTabInfo->DeleteMenu(ID_MENU_OPENFILE,MF_BYCOMMAND);
		pMenuTabInfo->DeleteMenu(0,MF_BYPOSITION);
		//pMenuTabInfo->de
	}
	else if (ClipBoardType == CLIPBOADRTYPE_File)
	{
		HMENU hSubMenu = GetSendToSubMenuHandle(pMenuTabInfo);
		if (hSubMenu)
			GetSystemSendToList(m_hWnd,hSubMenu);
	}

	SetForegroundWindow();

	CPoint pt;
	GetCursorPos(&pt);
	pMenuTabInfo->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);

	return TRUE;
}

void CClipBoardMontorDlg::OnMenuCopy()
{
	// TODO: 在此添加命令处理程序代码
	int nSelectItem = m_ListCtrl.GetSelectionMark();
	if (nSelectItem<0)
		return ;

	CLIPBOADRTYPE ClipBoardType = (CLIPBOADRTYPE)m_ListCtrl.GetItemData(nSelectItem);
	CString strListViewText = m_ListCtrl.GetItemText(nSelectItem,1);
	DWORD dwDataType = CF_UNICODETEXT;

	if (ClipBoardType == CLIPBOADRTYPE_File)
		dwDataType = CF_HDROP;

	CopyDataToClipboard(CF_HDROP,m_hWnd,strListViewText);
}

void CClipBoardMontorDlg::OnMenuDelete()
{
	// TODO: 在此添加命令处理程序代码
	int nSelectItem = m_ListCtrl.GetSelectionMark();
	if (nSelectItem<0)
		return ;

	m_ListCtrl.DeleteItem(nSelectItem);
}

void CClipBoardMontorDlg::OnSendTo(UINT nMenuID)
{
    LPSHELLFOLDER lpShellFolder;
    LPCITEMIDLIST lpItemIdList;
    LPDATAOBJECT lpDataObject;
    LPDROPTARGET lpDropTarget;
    HRESULT hRes;

	UINT nSelect = m_ListCtrl.GetSelectionMark();
	if (nSelect < 0)
		return;

	CString strFileName(m_ListCtrl.GetItemText(nSelect,1));

    hRes = GetUIObjectOfPath(m_hWnd, strFileName, IID_IDataObject,
                                    (LPVOID *)&lpDataObject);

    if (SUCCEEDED(hRes)) 
	{
		//找到我们需要发送的目标
		ULONG_PTR dwItemData = GetMenuItemData(nMenuID);
		if (dwItemData != NULL)
		{
            lpItemIdList = (LPCITEMIDLIST)dwItemData;

			//将目录pItemIdList转换为目标位置
            lpShellFolder = GetSpecialFolder(m_hWnd, CSIDL_SENDTO);
            if (lpShellFolder)
			{
                hRes = lpShellFolder->GetUIObjectOf(m_hWnd, 1, &lpItemIdList,IID_IDropTarget, NULL, (LPVOID *)&lpDropTarget);
                if (SUCCEEDED(hRes))
				{
					//将文件发送到目标
                    DoDrop(lpDataObject, lpDropTarget);
                    lpDropTarget->Release();
                }

                lpShellFolder->Release();
            }
        }
        lpDataObject->Release();
    }
}

BOOL CClipBoardMontorDlg::GetSystemSendToList(HWND hWnd, HMENU hMenu)
{
	CoInitialize(NULL);
    LPSHELLFOLDER lpShellFolder;
    LPITEMIDLIST lpItemIdList;
    LPENUMIDLIST lpEnumIdList;
    HRESULT hRes;
    UINT nMenuStartId = IDM_SENDTOFIRST;

	lpShellFolder = GetSpecialFolder(hWnd, CSIDL_SENDTO);
	if (lpShellFolder)
	{
		hRes = lpShellFolder->EnumObjects(hWnd,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,&lpEnumIdList);

		if (SUCCEEDED(hRes))
		{
			while (lpEnumIdList->Next(1, &lpItemIdList, NULL) == S_OK &&
				nMenuStartId < IDM_SENDTOLAST)
			{
				SHFILEINFO ShFileInfo;
				DWORD_PTR dwRet = SHGetFileInfo((LPCTSTR)lpItemIdList,-1,&ShFileInfo,sizeof(SHFILEINFO),
						SHGFI_ICON|SHGFI_PIDL|SHGFI_DISPLAYNAME|SHGFI_SMALLICON);

					if (AppendMenu(hMenu, MF_ENABLED | MF_STRING,nMenuStartId,ShFileInfo.szDisplayName))
					{
						MENUINFOEX MenuInfo;
						MenuInfo.dwMenuId = nMenuStartId;
						MenuInfo.dwParam = (ULONG_PTR)lpItemIdList;

						m_MenuInfoList.AddTail(MenuInfo);

						if (dwRet != 0 && ShFileInfo.hIcon != NULL)
						{
							HBITMAP hBitmap = ConvertIconToBitmap(ShFileInfo.hIcon);
							DestroyIcon(ShFileInfo.hIcon);
							SetMenuItemBitmaps(hMenu,nMenuStartId,MF_BYCOMMAND,hBitmap,NULL);
						}
						
						nMenuStartId++;
					}
				}
			lpEnumIdList->Release();
			}
		lpShellFolder->Release();
	}

    if (nMenuStartId != IDM_SENDTOFIRST) 
		DeleteMenu(hMenu, ID_MENU_NONE, MF_BYCOMMAND);
    
	CoUninitialize();

	return TRUE;
}

HMENU CClipBoardMontorDlg::GetSendToSubMenuHandle(CMenu* pMainMenu)
{
	MENUITEMINFO MenuItemInfo;

	MenuItemInfo.cbSize = sizeof(MenuItemInfo);
	MenuItemInfo.fMask = MIIM_SUBMENU;

	HMENU hMenu = pMainMenu->GetSafeHmenu();
	GetMenuItemInfo(hMenu, 1, TRUE, &MenuItemInfo);

	return MenuItemInfo.hSubMenu;
}

ULONG_PTR  CClipBoardMontorDlg::GetMenuItemData(UINT nMenuID)
{
	POSITION pos = m_MenuInfoList.GetHeadPosition();
	while(pos)
	{
		MENUINFOEX& MenuInfo = m_MenuInfoList.GetNext(pos);
		if (MenuInfo.dwMenuId == nMenuID)
			return MenuInfo.dwParam;
	}
	return NULL;
}

CLIPBOADRTYPE CClipBoardMontorDlg::GetFileType(LPCTSTR lpszFilePath)
{
	CLIPBOADRTYPE ClipBoardType = CLIPBOADRTYPE_File;
	
	return ClipBoardType;
}

/*CBitmap* CClipBoardMontorDlg::ConvertIconToBitmap(HICON hIcon)
{
	CDC dc;
	CBitmap bmp;
	CClientDC ClientDC(this);
	dc.CreateCompatibleDC(&ClientDC);
	bmp.CreateCompatibleBitmap(&ClientDC, 16, 16);
	CBitmap* pOldBmp = (CBitmap*)dc.SelectObject(&bmp);
	COLORREF ColorRef = GetSysColor(COLOR_3DFACE);
	::DrawIconEx(dc.GetSafeHdc(), 0, 0, hIcon, 16, 16, 0, (HBRUSH)ColorRef, DI_NORMAL);
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
	HBITMAP hBitmap = (HBITMAP)::CopyImage((HANDLE)((HBITMAP)bmp), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	
	return CBitmap::FromHandle(hBitmap);
}*/

HBITMAP CClipBoardMontorDlg::ConvertIconToBitmap(HICON  hIcon)
{ 
	Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);

	HBITMAP hBitmap;
	const Gdiplus::Color colorBackground;
	Gdiplus::Status StatusType =  pBitmap->GetHBITMAP(colorBackground,&hBitmap);
	if (StatusType == Gdiplus::Status::Ok)
		return hBitmap;
	else
		return NULL;
}