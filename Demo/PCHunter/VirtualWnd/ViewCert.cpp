#include "StdAfx.h"

CViewCert::CViewCert()
{

}

CViewCert::~CViewCert()
{

}

void CViewCert::OnPaint()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Cert"));
	if (pList == NULL)
		return;
}
