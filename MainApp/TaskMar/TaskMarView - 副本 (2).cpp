
// TaskMarView.cpp : CTaskMarView 类的实现
//

#include "stdafx.h"
#include "TaskMar.h"

#include "TaskMarDoc.h"
#include "TaskMarView.h"
#include "LsCommon/CommonFunc.h"
#include "LsCommon/FuncCallBack.h"
#include "LsCommon/OsInfo.h"
#include "ProcessInfoView.h"
#include "NtDll.h"
#include "LsCommon/Class/ListTreeCtrl.h"

#include <Uxtheme.h>

#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTaskMarView
#define		WM_SHOW_PROCESS_LIST		(WM_USER + 0x100)		//显示进程列表
#define		ID_LIST_TREE_CTRL								0x200
IMPLEMENT_DYNCREATE(CTaskMarView, CView)

BEGIN_MESSAGE_MAP(CTaskMarView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_RCLICK,&CTaskMarView::OnRClick)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK,&CTaskMarView::OnHeadClick)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED,&CTaskMarView::OnSelectChange)
	ON_COMMAND(ID_LOOKUP_PROCESS, &CTaskMarView::OnLookupProcess)
	ON_COMMAND(ID_LOOKUP_FILE_PROPERTY, &CTaskMarView::OnLookupFileProperty)
	ON_COMMAND(ID_COPY_PROCESS_NAME, &CTaskMarView::OnCopyProcessName)
	ON_COMMAND(ID_COPY_PROCESS_PATH, &CTaskMarView::OnCopyProcessPath)
	ON_COMMAND(ID_LOOKUP_PROCESS_DETAIL, &CTaskMarView::OnLookupProcessDetail)
	ON_COMMAND(ID_LOOKUP_PROCESS_MODULE, &CTaskMarView::OnLookupProcessModule)
	ON_COMMAND(ID_LOOKUP_PROCESS_WINDOW, &CTaskMarView::OnLookupProcessWindow)
	ON_COMMAND(ID_LOOKUP_PROCESS_MEMERY, &CTaskMarView::OnLookupProcessMemery)
	ON_COMMAND(ID_LOOKUP_PROCESS_HANDLE, &CTaskMarView::OnLookupProcessHandle)
	ON_COMMAND(ID_LOOKUP_PROCESS_THREAD, &CTaskMarView::OnLookupProcessThread)
	ON_COMMAND(ID_LOOKUP_PROCESS_SERVICES, &CTaskMarView::OnLookupProcessServices)
	ON_COMMAND(ID_LOOKUP_PROCESS_STARTTYPE, &CTaskMarView::OnLookupProcessStarttype)
	ON_COMMAND(ID_KILL_PROCESS, &CTaskMarView::OnKillProcess)
	ON_COMMAND(ID_SUSPEND_PROCESS, &CTaskMarView::OnSuspendProcess)
	ON_COMMAND(ID_RESUME_PROCESS, &CTaskMarView::OnResumeProcess)
	ON_COMMAND(ID_MENU_EXPORT_PROCESS, &CTaskMarView::OnMenuExportProcess)
	ON_MESSAGE(WM_SHOW_PROCESS_LIST,&CTaskMarView::OnShowProcessList)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CTaskMarView::OnNMCustomdraw)
END_MESSAGE_MAP()

// CTaskMarView 构造/析构

CTaskMarView::CTaskMarView()
{
	// TODO: 在此处添加构造代码
	m_hThread = NULL;
	m_bExit = FALSE;
	m_nLastSortSubItem = -1;
	m_ImgList.DeleteImageList();
	m_ImgList.Create(16,16,ILC_COLOR32|ILC_MASK,10,5);
}

CTaskMarView::~CTaskMarView()
{
	m_bExit = TRUE;
	if (WaitForSingleObject(m_hThread,1*2000) == WAIT_TIMEOUT)
	{
		if (TerminateThread(m_hThread,0) == FALSE)
			SetErrorInfo(SYSTEM_ERROR,0,_T("结束循环线程失败"));
	}
	ReleaseProcessErrorInfo();
}

BOOL CTaskMarView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

BOOL CTaskMarView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CView::OnCreate(lpcs) == -1)
		return FALSE;

	return m_ListTreeCtrl.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_LIST_TREE_CTRL);
}

void CTaskMarView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType,cx,cy);
	if (m_ListTreeCtrl.GetSafeHwnd())
		m_ListTreeCtrl.SetWindowPos(0,-1,-1,cx,cy,SWP_SHOWWINDOW);
}

void CTaskMarView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	
	// TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
	//  从而可以用项填充 ListView。
	SetWindowTheme(m_hWnd,L"Explorer",NULL);
	ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
	m_ImgList.Add(LoadIcon(NULL,MAKEINTRESOURCE(IDI_ICON_DEFAULT)));
	
	m_ListTreeCtrl.SetExtendedStyle(LVS_EX_SINGLEROW|LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
	m_ListTreeCtrl.SetImageList(&m_ImgList,LVSIL_SMALL);

	UINT uIndex=0;
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("进程名称"),LVCFMT_LEFT,157);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("进程ID"),LVCFMT_RIGHT,58);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("进程路径"),LVCFMT_LEFT,231);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("用户名"),LVCFMT_RIGHT,130);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("内存使用"),LVCFMT_RIGHT,70);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("CPU使用"),LVCFMT_RIGHT,70);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("线程数"),LVCFMT_RIGHT,50);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("句柄数"),LVCFMT_RIGHT,50);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("会话ID"),LVCFMT_RIGHT,50);
	m_ListTreeCtrl.InsertColumn(uIndex++,_T("文件描述"),LVCFMT_LEFT,250);

	int nTotalCount = m_ListTreeCtrl.GetHeaderCtrl()->GetItemCount();
	for (int nIndex=0;nIndex<nTotalCount;++nIndex)
	{
		HDITEM hditem={0};
		hditem.mask = HDI_LPARAM;
		if (nIndex == 1 || nIndex==5 || nIndex == 6 || nIndex == 7 || nIndex== 8)
			hditem.lParam = NUMBER;
		else if (nIndex == 4)
			hditem.lParam = FILE_SIZE;
		else 
			hditem.lParam = STRING_TEXT;

		m_ListTreeCtrl.GetHeaderCtrl()->SetItem(nIndex,&hditem);
	}

	if (AdjustProcessAuthority(SE_DEBUG_NAME) == FALSE)
	{
		MessageBox(GetThreadErrorInfoString(),_T("提示"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	PostMessage(WM_SHOW_PROCESS_LIST,0,0);
	
	m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RefreshProcessListThread,this,0,NULL);
}


// CTaskMarView 诊断

#ifdef _DEBUG
void CTaskMarView::AssertValid() const
{
	CView::AssertValid();
}

void CTaskMarView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTaskMarDoc* CTaskMarView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTaskMarDoc)));
	return (CTaskMarDoc*)m_pDocument;
}
#endif //_DEBUG


// CTaskMarView 消息处理程序
UINT CTaskMarView::RefreshProcessListThread(LPVOID lParam)
{
	/*CTaskMarView* pTaskMar = (CTaskMarView*)lParam;
	ASSERT(pTaskMar);
	CListCtrl& ListCtrl = pTaskMar->GetListCtrl();

	CLsSystemProcessList LsSystemProcessList;
	DWORD dwLastTickTime = GetTickCount();
	Sleep(1*1000);

	while(TRUE)
	{
		if (pTaskMar->m_bExit == TRUE)
			break;

		//ListCtrl.SetRedraw(FALSE);
		if (EnumSystemProcess(pTaskMar->m_LsSystemProcessList) == FALSE)
			break;
	
		DWORD dwTickTime = GetTickCount();
		DWORD dwIntTime = dwTickTime- dwLastTickTime;

		pTaskMar->ModifyProcessList(dwIntTime);
	
		//ListCtrl.SetRedraw(TRUE);

		dwLastTickTime = dwTickTime;
		
		Sleep(1*1000);
		pTaskMar->ModifyItemData();
	}

	ReleaseThreadErrorInfo();
	return 0;
	*/
	return 0;
}

LRESULT CTaskMarView::OnShowProcessList(WPARAM wParam,LPARAM lParam)
{
	if (EnumSystemProcess(m_LsSystemProcessList) != FALSE)
		ShowProcessList(m_LsSystemProcessList);

	return TRUE;
}

void CTaskMarView::ModifyProcessList(DWORD dwIntTime)
{
	SetAddItem(dwIntTime);
	SetDelItem();
}

void CTaskMarView::SetAddItem(DWORD dwIntTime)
{
	/*CListCtrl& ListCtrl = GetListCtrl();
	POSITION pos = m_LsSystemProcessList.GetHeadPosition();
	while(pos)
	{
		BOOL bFindtheSame = FALSE;
		LS_SYSTEM_PROCESS& LsSystemProcess = m_LsSystemProcessList.GetNext(pos);
		for (int i=0 ; i<ListCtrl.GetItemCount() ; ++i)
		{
			if (ListAndItemIsEqual(LsSystemProcess,i) == TRUE)
			{
				ModifyProcessItem(LsSystemProcess,i,dwIntTime);
				bFindtheSame = TRUE;
				break;
			}
		}

		if (bFindtheSame == FALSE)
			InsertProcessItem(LsSystemProcess,ITEM_STATE_Add);
	}
	*/
}

void CTaskMarView::SetDelItem()
{
	/*CListCtrl& ListCtrl = GetListCtrl();

	for (int i=0; i< ListCtrl.GetItemCount() ; ++i)
	{
		BOOL bFindtheSame = FALSE;
		POSITION pos = m_LsSystemProcessList.GetHeadPosition();
		while(pos)
		{
			LS_SYSTEM_PROCESS& LsSystemProcess = m_LsSystemProcessList.GetNext(pos);
			if (ListAndItemIsEqual(LsSystemProcess,i) == TRUE)
			{
				bFindtheSame = TRUE;
				break;
			}
		}

		if (bFindtheSame == FALSE)
			DeleteItem(i);
	}
	*/
}

BOOL CTaskMarView::ListAndItemIsEqual(LS_SYSTEM_PROCESS& LsSystemProcess,UINT nItemIndex)
{
	BOOL bThesame = GetItemText(nItemIndex,1) == LsSystemProcess.dwProcessId;
	return bThesame;
}

ULONGLONG CTaskMarView::GetItemText(int nRow,int nCol)
{
	/*ULONGLONG ulItem;
	CListCtrl& ListCtrl = GetListCtrl();
	_stscanf_s(ListCtrl.GetItemText(nRow,nCol),_T("%I64u"),&ulItem);
	return ulItem;
	*/
	return 0;
}

void CTaskMarView::InsertProcessItem(LS_SYSTEM_PROCESS& LsSystemProcess,ITEM_STATE ItemState/* = ITEM_STATE_Normal*/)
{
	UINT nIco = GetModuleFileIcon(LsSystemProcess.strProcessPath,&m_ImgList);
	ITEM_DATA ItemData;
	ItemData.dwProcessId = LsSystemProcess.dwProcessId;
	ItemData.ullLastCPUTime = LsSystemProcess.ullProcessUseTime;
	ItemData.ItemState = ItemState;
	ItemData.dwThreadId = 0;//LsSystemProcess.dwThread;

	POSITION Pos = m_ItemDataList.AddTail(ItemData);

	CItemInfo* lp = new CItemInfo();
	lp->SetImage(nIco);
	//add item text
	lp->SetItemText(LsSystemProcess.strProcessName);
	//add subitem text
	CString strPID;
	strPID.Format(_T("%d"),LsSystemProcess.dwProcessId);
	lp->AddSubItemText(strPID);      // 0 zero based subitems...
	lp->AddSubItemText(LsSystemProcess.strProcessPath);// 1
	lp->AddSubItemText(LsSystemProcess.strUserName);  // 2
	lp->AddSubItemText(FormatNumSizeToStringSize(LsSystemProcess.dwMemUse));  // 3 combo goes here
	lp->AddSubItemText(FormatNumToString(0));  // 3 combo goes here
	lp->AddSubItemText(FormatNumToString(LsSystemProcess.dwThread));  // 3 combo goes here
	lp->AddSubItemText(FormatNumToString(LsSystemProcess.dwHandle));  // 3 combo goes here
	lp->AddSubItemText(FormatNumToString(LsSystemProcess.dwConsole));  // 3 combo goes here
	lp->AddSubItemText(LsSystemProcess.strDescrible);  // 3 combo goes here

	//setup a combobox for the last column
	//lp->SetControlType(CONTROLTYPE::combobox, 3/*col # 3 from subitems...really col #4 in view, live with it*/);
	//CStringList list;
	//initalized list for the combobox
	//list.AddTail("Super control :}");
	//list.AddTail("bad written control :{");
	//list.AddTail("said piece of code :{");
	//lp->SetListData(3, &list);
	////Create root item
	CTreeItem * pRoot = m_ListTreeCtrl.InsertRootItem(lp);//previous on N.Y.P.D we call it CreateTreeCtrl(lp)

	/*int nIndex = m_ListTreeCtrl.InsertItem(LVIF_IMAGE|LVIF_TEXT|LVIF_PARAM,0x7FFFFFFF,LsSystemProcess.strProcessName,0,0,nIco,(LPARAM)Pos);

	CString strPID;
	int nSubItem = 0;
	strPID.Format(_T("%d"),LsSystemProcess.dwProcessId);
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,strPID);	
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,LsSystemProcess.strProcessPath);
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,LsSystemProcess.strUserName);
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,FormatNumSizeToStringSize(LsSystemProcess.dwMemUse));
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,FormatNumToString(0));
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,FormatNumToString(LsSystemProcess.dwThread));
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,FormatNumToString(LsSystemProcess.dwHandle));
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,FormatNumToString(LsSystemProcess.dwConsole));
	m_ListTreeCtrl.SetItemText(nIndex,++nSubItem,LsSystemProcess.strDescrible);*/

}

void CTaskMarView::DeleteItem(UINT nItem)
{
	/*CListCtrl& ListCtrl = GetListCtrl();
	POSITION pos = (POSITION)ListCtrl.GetItemData(nItem);
	if (pos)
	{
		ITEM_DATA& ItemData = m_ItemDataList.GetAt(pos);
		ItemData.ItemState = ITEM_STATE_Del;
		ListCtrl.SetItemData(nItem,(DWORD*)pos);
	}	
	*/
}

void CTaskMarView::ModifyItemData()
{
	/*CListCtrl& ListCtrl = GetListCtrl();
	if (IsWindow(ListCtrl.m_hWnd) == FALSE)
		return;

	for (int i=ListCtrl.GetItemCount()-1;i>=0;--i)
	{
		POSITION pos = (POSITION)ListCtrl.GetItemData(i);
		if (pos)
		{
			ITEM_DATA& ItemData = m_ItemDataList.GetAt(pos);
			if (ItemData.ItemState == ITEM_STATE_Del)
			{
				ListCtrl.DeleteItem(i);
				m_ItemDataList.RemoveAt(pos);
			}
			else if (ItemData.ItemState == ITEM_STATE_Add)
				ListCtrl.SetItemData(i,ITEM_STATE_Normal);
		}
	}
	*/
}

void CTaskMarView::ModifyProcessItem(LS_SYSTEM_PROCESS& LsSystemProcess,UINT nItem,DWORD dwIntTime)
{
	/*CListCtrl& ListCtrl = GetListCtrl();
	ListCtrl.SetRedraw(FALSE);

	ULONGLONG ulMemUse = ConvertStringSizeToNum(ListCtrl.GetItemText(nItem,4));
	if (ulMemUse != LsSystemProcess.dwMemUse)
		ListCtrl.SetItemText(nItem,4,FormatNumSizeToStringSize(LsSystemProcess.dwMemUse));
	
	POSITION pos = (POSITION)ListCtrl.GetItemData(nItem);
	if (pos)
	{
		ITEM_DATA& ItemData = m_ItemDataList.GetAt(pos);
		static DWORD dwCPUNum =2;
		DWORD dwCPU = (DWORD)((LsSystemProcess.ullProcessUseTime-ItemData.ullLastCPUTime)/(100*dwIntTime*dwCPUNum));
		ItemData.ullLastCPUTime = LsSystemProcess.ullProcessUseTime;

		if (dwCPU>=100)
			dwCPU = 100;

		if (GetItemText(nItem,5) != dwCPU)
			ListCtrl.SetItemText(nItem,5,FormatNumToString(dwCPU));
	}
		
	if (GetItemText(nItem,6) != LsSystemProcess.dwThread)
		ListCtrl.SetItemText(nItem,6,FormatNumToString(LsSystemProcess.dwThread));

	if (GetItemText(nItem,7) != LsSystemProcess.dwHandle)
		ListCtrl.SetItemText(nItem,7,FormatNumToString(LsSystemProcess.dwHandle));

	ListCtrl.SetRedraw(TRUE);
	*/
}

void CTaskMarView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)    
{
	
	/*NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );   
	*pResult = CDRF_DODEFAULT;  
	CListCtrl& ListCtrl = GetListCtrl();

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )  
	{  
		*pResult = CDRF_NOTIFYITEMDRAW;  
	}  
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )  
	{  
		*pResult = CDRF_NOTIFYSUBITEMDRAW;  
	}  
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )  
	{
		COLORREF clrNewTextColor, clrNewBkColor; 
		int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );  
		
		clrNewTextColor = RGB(0,0,0);
		clrNewBkColor = RGB(255,255,255);

		POSITION pos = (POSITION)ListCtrl.GetItemData(nItem);
		if (pos)
		{
			ITEM_DATA& ItemData = m_ItemDataList.GetAt(pos);

			if (ItemData.ItemState == ITEM_STATE_Add)
				clrNewBkColor = RGB(0,255,0);
			else if (ItemData.ItemState == ITEM_STATE_Del)
				clrNewBkColor = RGB(255,0,0);
		}
	
		pLVCD->clrText = clrNewTextColor;  
		pLVCD->clrTextBk = clrNewBkColor;  

		*pResult = CDRF_DODEFAULT;  
	}  
	*/
}

BOOL CTaskMarView::ShowProcessList(CLsSystemProcessList& LsSystemProcessList)
{
	m_ListTreeCtrl.DeleteAllItems();

	POSITION pos = LsSystemProcessList.GetHeadPosition();
	while(pos)
	{
		LS_SYSTEM_PROCESS& LsSystemProcess = LsSystemProcessList.GetNext(pos);
		if (LsSystemProcess.dwProcessId == 0)
		{
			LsSystemProcess.strProcessName = _T("System Idle Process");
			LsSystemProcess.strUserName = _T("SYSTEM");
		}

		if (LsSystemProcess.dwProcessId == 4)
		{
			LsSystemProcess.strProcessPath = GetSystemDirectoryEx(_T("ntkrnlpa.exe"));
			LsSystemProcess.strUserName = _T("SYSTEM");
		}

		InsertProcessItem(LsSystemProcess);
	}
	return TRUE;
}

BOOL CTaskMarView::OnHeadClick(NMHDR* pNMHDR,LRESULT* lResult)
{
	/*NMLISTVIEW* pLV = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
	CListCtrl& ListCtrl = GetListCtrl();
	int iSubItem = pLV->iSubItem;

	LIST_SORT_ITEM_INFO ListSortItem;

	HDITEM hditem={0};
	hditem.mask = HDI_FORMAT|HDI_LPARAM;
	ListCtrl.GetHeaderCtrl()->GetItem( iSubItem, &hditem );

	if (m_nLastSortSubItem == iSubItem)
		ListSortItem.bSortAscending = (hditem.fmt & HDF_SORTUP) == 0 ? TRUE : FALSE;
	else
		ListSortItem.bSortAscending = TRUE;

	ListSortItem.hListWnd = m_hWnd;
	ListSortItem.nDataType= (LIST_CTRL_COLUMN_FORMAT)hditem.lParam;
	ListSortItem.nSortColumnId = iSubItem;

	ListCtrl.SortItemsEx(&ListCompare , (LPARAM)&ListSortItem);

	m_nLastSortSubItem = iSubItem;
	int nItemCount = ListCtrl.GetHeaderCtrl()->GetItemCount();

	for (int iIndex=0; iIndex<nItemCount; ++iIndex)
	{
		ListCtrl.GetHeaderCtrl()->GetItem( iIndex, &hditem );

		hditem.fmt &= ~(HDF_SORTDOWN|HDF_SORTUP);
		if(iIndex == iSubItem)
			hditem.fmt |= ListSortItem.bSortAscending ? HDF_SORTUP:HDF_SORTDOWN;
		ListCtrl.GetHeaderCtrl()->SetItem( iIndex, &hditem );
	}
*/
	return TRUE;	
}

BOOL CTaskMarView::OnSelectChange(NMHDR* pNMHDR,LRESULT* lResult)
{
	CProcessInfoView* pView = (CProcessInfoView*)(GetDocument()->GetView(RUNTIME_CLASS(CProcessInfoView)));
	
	return pView->UpdateProcessInfo();
}

DWORD CTaskMarView::GetSelectItem()
{
	/*CListCtrl& ListCtrl = GetListCtrl();
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return -1;
	return ListCtrl.GetNextSelectedItem(pos);
	*/
	return 0;
}

BOOL CTaskMarView::GetPIDAndPath(CString& strPid , CString& strExePath)
{
	/*CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return FALSE;
	
	strPid = ListCtrl.GetItemText(dwSelectItem,1);
	strExePath = ListCtrl.GetItemText(dwSelectItem,2);
*/
	return TRUE;
}

DWORD CTaskMarView::GetProcessID()
{
	/*CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return FALSE;

	CString strPid;
	strPid = ListCtrl.GetItemText(dwSelectItem,1);
	DWORD dwPid;
	_stscanf_s(strPid,_T("%u"),&dwPid);

	return dwPid;
	*/
	return 0;
}

void CTaskMarView::OnRClick(NMHDR* pNMHdr,LPARAM* lResult)
{
	NMLISTVIEW* pLVCD = (NMLISTVIEW*)pNMHdr;
	if (pLVCD->iItem < 0 )
		return;

	CMenu RClickMenu,*pRClickMenu;
	RClickMenu.LoadMenu(IDR_MENU_TASKMAR_LIST);
	pRClickMenu = RClickMenu.GetSubMenu(0);

	POSITION pos = m_LsSystemProcessList.GetHeadPosition();
	while(pos)
	{
		m_LsSystemProcessList.GetNext(pos);
		pRClickMenu->DeleteMenu(MF_BYCOMMAND,ID_RESUME_PROCESS);
		pos = NULL;
	}

	SetForegroundWindow();

	CPoint pt;
	GetCursorPos(&pt);
	pRClickMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

//在资源管理器中显示进程对应的文件
void CTaskMarView::OnLookupProcess()
{
	// TODO: 在此添加命令处理程序代码
	/*CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return;

	CString strFilePath = ListCtrl.GetItemText(dwSelectItem,2);
	OpenFolderAndSelectFile(strFilePath);
	*/
}

//使用属性对话框显示进程对应文件的属性
void CTaskMarView::OnLookupFileProperty()
{
	// TODO: 在此添加命令处理程序代码
/*	CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return;

	CString strFilePath = ListCtrl.GetItemText(dwSelectItem,2);
	LookupFileProperty(strFilePath);
	*/
}

void CTaskMarView::OnCopyProcessName()
{
	// TODO: 在此添加命令处理程序代码
/*	CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return;

	CString strProcessName = ListCtrl.GetItemText(dwSelectItem,0);
	CopyDataToClipboard(m_hWnd,strProcessName);
	*/
}

void CTaskMarView::OnCopyProcessPath()
{
	// TODO: 在此添加命令处理程序代码
	/*CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return;

	CString strProcessName = ListCtrl.GetItemText(dwSelectItem,2);
	CopyDataToClipboard(m_hWnd,strProcessName);
	*/
}

void CTaskMarView::OnLookupProcessDetail()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_DETAIL_INFO);
}


void CTaskMarView::OnLookupProcessModule()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_MODULE);
}

void CTaskMarView::OnLookupProcessWindow()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_WINDOWS);
}

void CTaskMarView::OnLookupProcessMemery()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_MEMROY);
}

void CTaskMarView::OnLookupProcessHandle()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_HANDLE);
}

void CTaskMarView::OnLookupProcessThread()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_THREAD);
}

void CTaskMarView::OnLookupProcessServices()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_SERVICE);
}

void CTaskMarView::OnLookupProcessStarttype()
{
	// TODO: 在此添加命令处理程序代码
	SelectProcessTabInfo(PROCESS_STARTTYPE);
}

BOOL CTaskMarView::SelectProcessTabInfo(UINT nItem)
{
	CView* pView = GetDocument()->GetView(RUNTIME_CLASS(CProcessInfoView));
	CProcessInfoView* pProcessInfoView = (CProcessInfoView*)pView;

	pProcessInfoView->SelectTabCtrl(nItem);
	return pProcessInfoView->UpdateProcessInfo();
}

void CTaskMarView::OnKillProcess()
{
	// TODO: 在此添加命令处理程序代码
	/*CListCtrl& ListCtrl = GetListCtrl();	
	DWORD dwSelectItem = GetSelectItem();
	if (dwSelectItem == -1)
		return;

	POSITION pos = (POSITION)ListCtrl.GetItemData(dwSelectItem);
	if (pos)
	{
		ITEM_DATA& ItemData = m_ItemDataList.GetAt(pos);
		if (TerminateProcessByProcessId(ItemData.dwProcessId) == FALSE)
		{
			CString strProcessName = ListCtrl.GetItemText(dwSelectItem,0);
			SetErrorTitle(_T("结束进程 %s[%d] 失败"),strProcessName,ItemData.dwProcessId);
			MessageBox(GetThreadErrorInfoString(),_T("操作失败"),MB_OK|MB_ICONERROR);
		}
		else
		{
			DeleteItem(dwSelectItem);
		}
	}
	*/
}

void CTaskMarView::OnSuspendProcess()
{
	// TODO: 在此添加命令处理程序代码
	
}

void CTaskMarView::OnResumeProcess()
{
	// TODO: 在此添加命令处理程序代码
}

void CTaskMarView::OnMenuExportProcess()
{
	// TODO: 在此添加命令处理程序代码
	/*CFileDialog FileExport(FALSE,_T("txt"),_T("进程列表"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||"));
	if (FileExport.DoModal() == TRUE)
	{
		HANDLE hFile = CreateFile(FileExport.GetOFN().lpstrFile,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开文件%s失败"),FileExport.GetOFN().lpstrFile);
			return;
		}

		HDITEM hdItem;

		TCHAR  szBuffer[MAX_PATH];
		hdItem.mask = HDI_TEXT;
		hdItem.pszText = szBuffer;
		hdItem.cchTextMax = _countof(szBuffer);

		CListCtrl& ListCtrl = GetListCtrl();
		int nTotal = ListCtrl.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i<nTotal;++i)
		{
			ListCtrl.GetHeaderCtrl()->GetItem(i,&hdItem);
			CStringA strListHeader(hdItem.pszText);
			strListHeader += "\t\t";
			if (nTotal - i == 1)
				strListHeader += "\r\n";

			DWORD dwWrite = 0;
			DWORD dwSize = strListHeader.GetLength();
			if (WriteFile(hFile,(LPVOID)(LPCSTR)strListHeader,dwSize,&dwWrite,NULL) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("写入文件内容失败"));
				break;
			}
		}

		int nItemCount = ListCtrl.GetItemCount();
		for (int i=0;i<nItemCount;++i)
		{
			for (int j=0;j<nTotal;++j)
			{
				CStringA strListHeader(ListCtrl.GetItemText(i,j));
				strListHeader += "\t";
				if (nTotal - j ==1 )
					strListHeader += "\r\n";

				DWORD dwSize = strListHeader.GetLength();
				DWORD dwWrite = 0;
				if (WriteFile(hFile,(LPVOID)(LPCSTR)strListHeader,dwSize,&dwWrite,NULL) == FALSE)
				{
					SetErrorInfo(SYSTEM_ERROR,0,_T("写入文件内容失败"));
					break;
				}
			}
		}

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}
	*/
}

void CTaskMarView::OnDraw(CDC* pDC)
{

}