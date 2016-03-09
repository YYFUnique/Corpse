#include "stdafx.h"
#include "Task.h"

DUI_BEGIN_MESSAGE_MAP(CTask, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TABSELECT,OnTabSelect)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_HEADERCLICK,OnHeaderClick)
DUI_END_MESSAGE_MAP()


CTask::CTask()
	: m_pPaintManager(NULL)
{
	/*m_pWndMgr = CWndMgr::GetInstance();*/
}

CTask::~CTask()
{
	
}

void CTask::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CTask::Notify(TNotifyUI& msg)
{
	CNotifyPump::NotifyPump(msg);
}

void CTask::OnFocus(TNotifyUI& msg)
{
	if (msg.pSender->GetName().IsEmpty() == false)
		MessageBox(m_pPaintManager->GetPaintWindow(),msg.pSender->GetName(),_T("提示"),MB_OK);
}

void CTask::OnTabSelect(TNotifyUI& msg)
{

}

void CTask::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetName() == _T("VLayoutTask"))
	{
		CAnimationTabLayoutUI* pTabLayout = (CAnimationTabLayoutUI*)m_pPaintManager->FindControl(_T("TaskWizard"));
		if (pTabLayout != NULL)
		{
			UINT nIndex = _ttoi(msg.pSender->GetUserData());
			pTabLayout->SelectItem(nIndex);
			//m_pWndMgr->SetCurrentObject((OBJECTTYPE)nIndex);
			//CPCObject* pObject = m_pWndMgr->GetObject();
			//if (pObject)
			//	pObject->ShowWindow();
		}
	}
}

void CTask::OnHeaderClick(TNotifyUI& msg)
{
	MessageBox(m_pPaintManager->GetPaintWindow(),_T("OnHeaderClick"),_T("提示"),MB_OK);
}

void CTask::OnClick(TNotifyUI& msg)
{
	//if (msg.pSender == (CButtonUI*)m_pPaintManager->FindControl(_T("btn")))
		MessageBox(m_pPaintManager->GetPaintWindow(),_T("OnClick"),_T("提示"),MB_OK);
}

void CTask::OnItemClick( TNotifyUI &msg )
{
	
}
