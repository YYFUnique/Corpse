
// ClientAppView.cpp : CClientAppView 类的实现
//

#include "stdafx.h"
#include "ClientApp.h"

#include "MainFrm.h"
#include "ClientAppDoc.h"
#include "ClientAppView.h"
#include "LsCommon/Json/JsonObject.h"
#include "LsCommon/OsInfo.h"
#include "NetTransport/WinSocket.h"
#include "LsCommon/FuncCallBack.h"
#include "Include/Define.h"

#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//ListView菜单项
#define LIST_VIEW_MENU		0
// CClientAppView

IMPLEMENT_DYNCREATE(CClientAppView, CListView)

BEGIN_MESSAGE_MAP(CClientAppView, CListView)
	//ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_RCLICK,&CClientAppView::OnRClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CClientAppView::OnNMCustomdraw)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, &CClientAppView::OnColumnClk)
	ON_COMMAND_RANGE(ID_SEND_MSG_MENU,ID_DELETE_MENU,&CClientAppView::OnListViewCmd)
END_MESSAGE_MAP()

// CClientAppView 构造/析构

CClientAppView::CClientAppView()
{
	// TODO: 在此处添加构造代码
	m_nLastSortSubItem = -1;
	m_ListParam.RemoveAll();
}

CClientAppView::~CClientAppView()
{

}

BOOL CClientAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CClientAppView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
	//  从而可以用项填充 ListView。
	CListCtrl& ListCtrl = GetListCtrl();
	SetWindowTheme(m_hWnd,L"Explorer",NULL);
	ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
	ListCtrl.SetExtendedStyle(LVS_EX_SINGLEROW|LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER);

	ListCtrl.InsertColumn(0,_T("IP地址"),LVCFMT_LEFT,110);
	ListCtrl.InsertColumn(1,_T("IP归属地"),LVCFMT_LEFT,80);
	ListCtrl.InsertColumn(2,_T("计算机名"),LVCFMT_LEFT,120);
	ListCtrl.InsertColumn(3,_T("操作系统"),LVCFMT_LEFT,130);
	ListCtrl.InsertColumn(4,_T("CPU"),LVCFMT_LEFT,150);
	ListCtrl.InsertColumn(5,_T("可用内存"),LVCFMT_LEFT,60);
	ListCtrl.InsertColumn(6,_T("屏幕分辨率"),LVCFMT_LEFT,75);
	ListCtrl.InsertColumn(7,_T("摄像头"),LVCFMT_LEFT,50);
	ListCtrl.InsertColumn(8,_T("服务版本"),LVCFMT_LEFT,90);
	ListCtrl.InsertColumn(9,_T("备注"),LVCFMT_LEFT,190);

	int iItemCount = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int iIndex=0;iIndex<iItemCount;++iIndex)
	{
		HDITEM hditem={0};
		hditem.mask = HDI_LPARAM;
		hditem.lParam = STRING_TEXT;
		ListCtrl.GetHeaderCtrl()->SetItem(iIndex,&hditem);
	}	
}


// CClientAppView 诊断

#ifdef _DEBUG
void CClientAppView::AssertValid() const
{
	CListView::AssertValid();
}

void CClientAppView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CClientAppDoc* CClientAppView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientAppDoc)));
	return (CClientAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CClientAppView 消息处理程序
void CClientAppView::AdjustColumnWidth()
{
	CListCtrl& ListCtrl = GetListCtrl();
	int nTotal = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for(int i=0; i<nTotal; i++)
		ListCtrl.SetColumnWidth(i , LVSCW_AUTOSIZE_USEHEADER);
}

BOOL CClientAppView::InsertIntoList(DWORD dwConnID,LPCTSTR lpszAddrAndPort,LPCTSTR lpszUserInfo)
{
	CListCtrl& ListCtrl = GetListCtrl();
	CJsonObject Json;
	int nSubItem=0;
	Json.FromString(lpszUserInfo);
	
	CString strUserComputerInfo;
	LIST_PARAM ListParam;
	ListParam.ConStat = ClientOnLine;
	ListParam.dwConnID = dwConnID;
	m_ListParam.AddTail(ListParam);

	int nIndex = ListCtrl.InsertItem(LVIF_TEXT|LVIF_PARAM,0x7FFFFFFF,lpszAddrAndPort,0,0,0,(LPARAM)&m_ListParam.GetTail());

	ListCtrl.SetItemText(nIndex,++nSubItem,_T("未知"));

	strUserComputerInfo = Json.GetValue(_T("ComputerName"));
	ListCtrl.SetItemText(nIndex,++nSubItem,strUserComputerInfo);

	strUserComputerInfo = Json.GetValue(_T("ComputerProduct"));
	ListCtrl.SetItemText(nIndex,++nSubItem,strUserComputerInfo);

	strUserComputerInfo = Json.GetValue(_T("CPUInfo"));
	ListCtrl.SetItemText(nIndex,++nSubItem,strUserComputerInfo);

	int nTotal;
	Json.GetValue(_T("TotalMem"),&nTotal);
	strUserComputerInfo.Format(_T("%dMB"),nTotal);
	ListCtrl.SetItemText(nIndex,++nSubItem,strUserComputerInfo);

	Json.GetValue(_T("ScreenInfo"),&nTotal);
	ListCtrl.SetItemText(nIndex,++nSubItem,GetScreenSize(nTotal));

	Json.GetValue(_T("CameraInfo"),&nTotal);
	ListCtrl.SetItemText(nIndex,++nSubItem,GetCameraDescribeInfo(nTotal));

	return TRUE;
}

int CClientAppView::GetCheckLine(UINT nFlags, CPoint point)
{
	GetListCtrl().SetSelectionMark(-1);
	int i = SendMessage(WM_LBUTTONDOWN , nFlags , MAKELPARAM(point.x,point.y));
	int iItemIndex = GetListCtrl().GetSelectionMark();
	if(iItemIndex ==-1)
		return -1;
	else
		return iItemIndex;
}

BOOL CClientAppView::DeleteItem(DWORD dwConnID)
{
	CListCtrl& ListCtrl = GetListCtrl();
	int nItem = ListCtrl.GetItemCount();
	BOOL bRet = FALSE;
	for(int nIndex=0;nIndex<nItem;nIndex++)
	{
		PLIST_PARAM pListParam = (PLIST_PARAM)ListCtrl.GetItemData(nIndex);
		if (pListParam->dwConnID == dwConnID)
		{
			bRet = ListCtrl.DeleteItem(nIndex);
			break;
		}
	}

	return bRet;
}

void CClientAppView::OnListViewCmd(UINT nID)
{
	CListCtrl& ListCtrl = GetListCtrl();
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	int nItemSelect = ListCtrl.GetSelectionMark();
	DWORD dwID = CORPSE_NONE_MSG;
	CString strData;
	LIST_PARAM* pListParam;
	switch (nID)
	{
		case ID_SEND_MSG_MENU:
			break;
		case ID_EXPLORER_MENU:
			break;
		case ID_DETAIL_INFO_MENU:
			break;
		case ID_REMOTE_DESKTOP_MENU:
			break;
		case ID_DELETE_MENU:
				pListParam = (PLIST_PARAM)ListCtrl.GetItemData(nItemSelect);
				dwID = CORPSE_CLIENT_DELETE_SERVER;
				strData = _T("客户端将服务器从在线列表中删除");
				pListParam->ConStat = ServerDelete;
				ListCtrl.SetItemData(nItemSelect,(LPARAM)pListParam);
			break;
	}
	CMsgHead Msg(dwID,strData);
	pMain->SendData(pListParam->dwConnID,Msg.GetMsgHead(),Msg.GetMsgHeadLen());
}

BOOL CClientAppView::OnColumnClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pLV = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
	CListCtrl& ListCtrl = GetListCtrl();
	int iSubItem = pLV->iSubItem;

	HDITEM hditem = {0};
	hditem.mask = HDI_FORMAT;
	ListCtrl.GetHeaderCtrl()->GetItem( iSubItem, &hditem );

	LIST_SORT_ITEM_INFO ListSortInfo;
	if (m_nLastSortSubItem == iSubItem)
		ListSortInfo.bSortAscending = (hditem.fmt & HDF_SORTUP) == 0 ? TRUE:FALSE;
	else
		ListSortInfo.bSortAscending = TRUE;
	ListSortInfo.nDataType = (LIST_CTRL_COLUMN_FORMAT)hditem.lParam;
	ListSortInfo.nSortColumnId = iSubItem;
	ListSortInfo.hListWnd = m_hWnd;

	m_nLastSortSubItem = iSubItem;
	ListCtrl.SortItemsEx(&ListCompare , (LPARAM)&ListSortInfo);

	int nItemCount = ListCtrl.GetHeaderCtrl()->GetItemCount();

	for (int iIndex=0; iIndex<nItemCount; ++iIndex)
	{
		hditem.mask = HDI_FORMAT;
		hditem.fmt &= ~(HDF_SORTDOWN|HDF_SORTUP);
		if(iIndex == iSubItem)
			hditem.fmt |= ListSortInfo.bSortAscending ? HDF_SORTUP : HDF_SORTDOWN;
		ListCtrl.GetHeaderCtrl()->SetItem( iIndex, &hditem );
	}

	return TRUE;	
}

void CClientAppView::OnNMCustomdraw(NMHDR* pNMHDR,LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	CListCtrl& ListCtrl = GetListCtrl();
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
		*pResult = CDRF_NOTIFYITEMDRAW;
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
		*pResult = CDRF_NOTIFYITEMDRAW;
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{
		COLORREF clrNewTextColor, clrNewBkColor; 
		int   nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec); 
		PLIST_PARAM pListParam = (PLIST_PARAM)ListCtrl.GetItemData(nItem);

		if (pListParam->ConStat == ServerDelete)
		{
			clrNewTextColor = RGB(192,192,192);
			clrNewBkColor = RGB(255,255,255);
		}
		else
		{
			clrNewTextColor = RGB(0,0,0);
			clrNewBkColor = RGB(255,255,255);
		}
		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;

		 *pResult = CDRF_DODEFAULT; 
	}
}

void CClientAppView::OnRClick(NMHDR* pNMHDR,LRESULT* pResult)
{
	NMLISTVIEW* pLVCD = (NMLISTVIEW*)pNMHDR;

	if (pLVCD->iItem <0)
		return;

	CMenu Menu;
	Menu.LoadMenu(IDR_VIEW_MENU);
	CMenu* pMenu;
	pMenu = Menu.GetSubMenu(LIST_VIEW_MENU);

	SetForegroundWindow();
	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}