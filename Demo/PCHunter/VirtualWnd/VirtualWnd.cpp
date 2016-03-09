#include "stdafx.h"

CPaintManagerUI* CVirtualWnd::m_pPaintManager;

CVirtualWnd::CVirtualWnd()
{

}

CVirtualWnd::~CVirtualWnd()
{

}

DUI_BEGIN_MESSAGE_MAP(CVirtualWnd, CNotifyPump)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_TABSELECT,OnTabSelect)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_HEADERCLICK,OnHeaderClick)
DUI_END_MESSAGE_MAP()

bool CVirtualWnd::AddVirtualWnd(CDuiString strName,INotifyUI* pObject)
{
	if( m_VirtualWndMap.Find(strName) == NULL )
	{
		m_VirtualWndMap.Insert(strName.GetData(),(LPVOID)pObject);
		m_strSubNotifys.AppendFormat(_T("%s,"),(LPCTSTR)strName);
		return true;
	}

	return false;
}

bool CVirtualWnd::RemoveVirtualWnd(CDuiString strName)
{
	if (m_VirtualWndMap.Find(strName) != NULL)
		m_strSubNotifys.Replace(strName,_T(""));

	return true;
}

LPCTSTR CVirtualWnd::GetVirtualWnd()
{
	return m_strSubNotifys;
}

void CVirtualWnd::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
	CBase::SetPaintManager(pPaintMgr);
}

void CVirtualWnd::Notify(TNotifyUI& msg)
{
	INotifyUI* pObject = (INotifyUI*)m_VirtualWndMap.Find(msg.sVirtualWnd,false);
	if (pObject)
		pObject->Notify(msg);
}
