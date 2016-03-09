#include "StdAfx.h"
#include "ListCtrlInfo.h"

CListHeaderInfo::CListHeaderInfo(const CString& strDescription , DWORD dwPosition/* = LVCFMT_LEFT*/, DWORD dwListWidth/* = 0*/)
{
	strColumnDes = strDescription;
	dwColumnWidth = dwListWidth;
}

CListHeaderInfo::~CListHeaderInfo()
{

}

CListCtrlInfo::CListCtrlInfo(void)
{
	
}

CListCtrlInfo::~CListCtrlInfo(void)
{
	
}

void CListCtrlInfo::AddColumn(LPCTSTR lpszItem)
{
	
}