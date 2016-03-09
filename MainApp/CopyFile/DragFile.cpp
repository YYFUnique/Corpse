#include "StdAfx.h"
#include "DragFile.h"
#include "resource.h"
#include "AddIniRuleDlg.h"

BEGIN_MESSAGE_MAP(CDragFile, CListCtrl)
	ON_WM_DROPFILES()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_USER_EDIT_END , &CDragFile::OnEditEnd)

	ON_COMMAND(ID_MENU_EDIT, &CDragFile::OnMenuEdit)
	ON_COMMAND(ID_MENU_INSRET, &CDragFile::OnMenuInsret)
	ON_COMMAND(ID_MENU_DELETE, &CDragFile::OnMenuDelete)
END_MESSAGE_MAP()

CDragFile::CDragFile(void)
{
}

CDragFile::~CDragFile(void)
{
}

BOOL CDragFile::EditLabel(_In_ int nItem , _In_ int nSubItem)
{
	iItem = nItem;
	iSubItem = nSubItem;
	m_EditList.Create(WS_CHILD | ES_LEFT | WS_BORDER,CRect(0,0,0,0),this,IDC_EDIT_BOX);
	CRect RectItem;
	GetSubItemRect(nItem,nSubItem,LVIR_LABEL,RectItem);

	CFont ListFont;
	ListFont.CreateStockObject(DEFAULT_GUI_FONT);
	m_EditList.SetFont(&ListFont);
	ListFont.DeleteObject();

	CString strItem = CListCtrl::GetItemText(nItem,nSubItem);
	int iStrWidth = GetStringWidth(strItem);
	RectItem.top -= 2;    // 防止上边框被遮挡
	

	m_EditList.MoveWindow(&RectItem);
	m_EditList.ShowWindow(SW_SHOW);
	m_EditList.SetWindowText(strItem);
	m_EditList.SetFocus();
	m_EditList.SetSel(MAKEWORD(0,-1)); 

	return TRUE;
}

void CDragFile::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFilePathName[MAX_PATH];
	UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0); //文件的个数
	for ( UINT nIndex=0 ; nIndex< nNumOfFiles; ++nIndex )
	{
		DragQueryFile(hDropInfo, nIndex, szFilePathName, sizeof(szFilePathName)); //得到文件名
		// 把得到的文件名传给父窗口
		LPARAM lParam = (LPARAM)szFilePathName;
		GetParent()->SendMessage(WM_LIST_DROPFILE , 0 , lParam);
	}
	DragFinish(hDropInfo);
}


void CDragFile::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetSelectionMark(-1);
	SendMessage(WM_LBUTTONDOWN , nFlags , MAKELPARAM(point.x,point.y));
	if(GetSelectionMark() == -1 && GetItemCount() != 0)
		return;
	Pt = point;
	CMenu Menu;
	Menu.LoadMenu(IDR_LIST_MENU);
	CMenu* pMenu;
	pMenu = Menu.GetSubMenu(0);
	SetForegroundWindow();
	ClientToScreen(&point);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
}
void CDragFile::OnMenuEdit()
{
	// TODO: 在此添加命令处理程序代码

	LVHITTESTINFO lvHit;    // 保存ListCtrl行列信息的结构
	lvHit.pt = Pt;
	lvHit.flags = LVHT_ONITEMLABEL;
	iItem = CListCtrl::SubItemHitTest(&lvHit);
	if (iItem == -1) 
		return;
	iSubItem = lvHit.iSubItem;

	WPARAM wParam = MAKEWPARAM(iItem , iSubItem);
	GetParent()->SendMessage(WM_ITEM_EDIT , wParam , 0);
}

void CDragFile::OnMenuInsret()
{
	// TODO: 在此添加命令处理程序代码
	LVHITTESTINFO lvHit;    // 保存ListCtrl行列信息的结构
	lvHit.pt = Pt;
	lvHit.flags = LVHT_ONITEMLABEL;
	iItem = CListCtrl::SubItemHitTest(&lvHit);
	if (iItem == -1) 
		return;
	iSubItem = lvHit.iSubItem;
	CListCtrl::InsertItem(iItem,_T(""));
	EditLabel(iItem , iSubItem);
	GetParent()->SendMessage(WM_ITEM_INSRET);
}

void CDragFile::OnMenuDelete()
{
	// TODO: 在此添加命令处理程序代码
	GetParent()->SendMessage(WM_ITEM_DELETE);
}
//编辑框失去焦点
LRESULT CDragFile::OnEditEnd(WPARAM wParam , LPARAM lParam)
{
	CString strEditItemText;
	m_EditList.GetWindowText(strEditItemText);
	//if (strEditItemText.IsEmpty() == FALSE)
	if(GetItemText(iItem,iSubItem) != strEditItemText)
	{
		CListCtrl::SetItemText(iItem,iSubItem,strEditItemText);
		GetParent()->SendMessage(WM_MODIFIED_NOT_SAVE,0,0);
	}
	m_EditList.DestroyWindow();

	return TRUE;
}