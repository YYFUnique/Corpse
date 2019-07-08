#include "StdAfx.h"
#include "ISPSInterfaceTest.h"

CISPSInterfaceTest::CISPSInterfaceTest()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

CISPSInterfaceTest::~CISPSInterfaceTest()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

void CISPSInterfaceTest::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

