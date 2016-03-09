#include "stdafx.h"
#include <atlstr.h>

CVirtualWndMgr::CVirtualWndMgr()
{

}

CVirtualWndMgr::~CVirtualWndMgr()
{
	RemoveVirtualWnd(_T("task"));
	RemoveVirtualWnd(_T("network"));
}

void CVirtualWndMgr::PushObject()
{
	m_SubNotifys.Add(&m_TaskMgr);
	m_SubNotifys.Add(&m_NetMgr);
}

CVirtualWnd* CVirtualWndMgr::FindObjectByName(LPCTSTR lpszVirtualWndName)
{
	for (int n=0;n<m_SubNotifys.GetSize();++n)
	{
		CVirtualWnd* pObject = (CVirtualWnd*)m_SubNotifys.GetAt(n);
		if (pObject && StrStrI(pObject->GetVirtualWnd(),lpszVirtualWndName) != NULL)
			return pObject;
	}

	return NULL;
}
