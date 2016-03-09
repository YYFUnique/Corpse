#include "stdafx.h"
#include "wndtask.h"

DUI_BEGIN_MESSAGE_MAP(CTaskWnd,CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SETFOCUS,OnSetFocus)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_HEADERCLICK,OnHeaderClick)
DUI_END_MESSAGE_MAP()

CTaskWnd::CTaskWnd()
	:m_pPaintManager(NULL)
{

}

void CTaskWnd::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CTaskWnd::OnHeaderClick(TNotifyUI& msg)
{
	MessageBox(m_pPaintManager->GetPaintWindow(),msg.pSender->GetText(),_T("提示"),MB_OK);
}

void CTaskWnd::OnSetFocus(TNotifyUI& msg)
{ 
	MessageBox(m_pPaintManager->GetPaintWindow(),msg.pSender->GetName(),_T("提示"),MB_OK);
}

BOOL CTaskWnd::OnInitDialog()
{
	MessageBox(m_pPaintManager->GetPaintWindow(),_T("OnInitDialog"),_T("提示"),MB_OK);

	return TRUE;
}