#include "StdAfx.h"
#include "ListViewWnd.h"

CListViewWnd::CListViewWnd(HWND hParent, CEditUI2* pOwner)
{
	m_bAutoClose = FALSE;
	m_pOwner = pOwner;
	// 无焦点子窗口必须使用WS_CHILD，且需要修改父窗口为桌面窗口，否则初始焦点存在异常不能正常弹出子窗口
	Create(hParent, LIST_VIEW_WND, WS_CHILD , WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CListViewWnd::~CListViewWnd()
{

}

void CListViewWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
}

LPCTSTR CListViewWnd::GetWindowClassName() const
{
	return LIST_VIEW_WND;
}

UILIB_RESOURCETYPE CListViewWnd::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CListViewWnd::GetZIPFileName() const
{
	return _T("InjectDll.zip");
}

CDuiString CListViewWnd::GetSkinFile()
{
	return _T("ListView.xml");
}

CDuiString CListViewWnd::GetSkinFolder()
{
	return _T("InjectDll");
}

void CListViewWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemSelect(msg);
}

void CListViewWnd::InitWindow()
{
	// 修改父窗口指向桌面句柄，否则无焦点窗口不能正常显示
	SetParent(m_hWnd, HWND_DESKTOP);

	// 重新计算窗口初始大小，并且应用该大小
	m_PaintManager.SetInitSize(m_pOwner->GetFixedWidth(), 110);
	::SetWindowPos(m_hWnd, NULL, 0, 0, m_pOwner->GetFixedWidth(), 110, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	m_pSearch = (CListUI*)m_PaintManager.FindControl(_T("searchlist"));
}

LRESULT CListViewWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEACTIVATE)
		return MA_NOACTIVATE;
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CListViewWnd::OnItemSelect(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("searchlist"))
	{
		CListContainerElementUI* pListText = (CListContainerElementUI*)m_pSearch->GetItemAt((int)msg.wParam);
		m_pOwner->SetText(pListText->GetSubControlText(_T("searchitem_text")));
		m_pOwner->SetTag(_ttoi(pListText->GetSubControlText(_T("searchitem_id"))));

 		if (m_bAutoClose)
 			ShowWindow(false);
	}
}

void CListViewWnd::Add(const PROCESS_INFO& ProcessInfo)
{
	CListContainerElementUI* pFileItem = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pFileItem = (CListContainerElementUI*)m_DialogBuilder.Create(_T("ListViewItem.xml"), 0, NULL, &m_PaintManager);
	else
		pFileItem = (CListContainerElementUI*)m_DialogBuilder.Create(NULL, &m_PaintManager);

	m_pSearch->Add(pFileItem);
	pFileItem->SetSubControlText(_T("searchitem_text"), ProcessInfo.strProcessName);
	CDuiString strProcessId;
	strProcessId.Format(_T("%d"), ProcessInfo.nProcessId);
	pFileItem->SetSubControlText(_T("searchitem_id"), strProcessId);
}

void CListViewWnd::RemoveAll()
{
	m_pSearch->RemoveAll();
}

void CListViewWnd::ResizeWnd()
{
	CContainerUI* pRoot = (CContainerUI*)m_PaintManager.GetRoot();
	CDuiRect rcWork;
	GetWindowRect(m_hWnd, &rcWork);

	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

	if (m_pSearch->GetCount() == 0)
	{
		ShowWindow(false);
		return;
	}

	ShowWindow(TRUE, FALSE);

	CListContainerElementUI* pListText = (CListContainerElementUI*)m_pSearch->GetItemAt(0);

	szAvailable.cy = m_pSearch->GetCount() * pListText->GetFixedHeight() + 4;

	SIZE szInitSize = m_PaintManager.GetInitSize();

	RECT rcPos = m_pOwner->GetPos();
	HWND hMainWnd = m_pOwner->GetManager()->GetPaintWindow();
	RECT rcShadow = m_pOwner->GetManager()->GetShadowCorner();
	POINT pt = {0, 0};
	ClientToScreen(hMainWnd, &pt);

	if (szAvailable.cy < szInitSize.cy)
		::SetWindowPos(m_hWnd, NULL, rcPos.left + pt.x, rcPos.bottom + pt.y, szAvailable.cx, szAvailable.cy, SWP_NOZORDER|SWP_NOACTIVATE);
	else
		::SetWindowPos(m_hWnd, NULL, rcPos.left + pt.x, rcPos.bottom + pt.y, szAvailable.cx, szInitSize.cy,  SWP_NOZORDER|SWP_NOACTIVATE);
}

LRESULT CListViewWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM , BOOL& bHandled)
{
	bHandled = FALSE;
	m_bAutoClose = FALSE;
	switch(wParam)
	{
		case VK_UP:
				bHandled = TRUE;
				m_pSearch->SelectItem(m_pSearch->GetCurSel() - 1, false);
			break;
		case VK_DOWN:
				bHandled = TRUE;
				m_pSearch->SelectItem(m_pSearch->GetCurSel() + 1, false);
			break;
	}
	m_bAutoClose = TRUE;

	return bHandled == FALSE;
}

LRESULT CListViewWnd::OnMouseWheel(UINT , WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	int zDelta = (int) (short) HIWORD(wParam);
	TEventUI event = { 0 };
	event.Type = UIEVENT_SCROLLWHEEL;
	event.pSender = m_pOwner;
	event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
	event.lParam = lParam;
	event.wKeyState = 0;
	event.dwTimestamp = ::GetTickCount();
	m_pSearch->DoEvent(event);
	return FALSE;
}

LRESULT CListViewWnd::OnPosChanged(UINT , WPARAM , LPARAM , BOOL& bHandle)
{
	bHandle = TRUE;

	RECT rcPos = m_pOwner->GetPos();
	HWND hMainWnd = m_pOwner->GetManager()->GetPaintWindow();
	RECT rcShadow = m_pOwner->GetManager()->GetShadowCorner();
	POINT pt = {0, 0};
	ClientToScreen(hMainWnd, &pt);
	SetWindowPos(m_hWnd, HWND_TOP, pt.x + rcPos.left, pt.y + rcPos.bottom, rcPos.right - rcPos.left, 110, /*SWP_SHOWWINDOW|*/SWP_NOACTIVATE);

	CDuiString strTipInfo;
	strTipInfo.Format(_T("OnPosChanged:len:%d"), rcPos.right - rcPos.left);
	OutputDebugString(strTipInfo);

	return FALSE;
}