#pragma once
#include "ProcessInfo.h"

class CProcessWinInfo : public CProcessInfo
{
public:
	CProcessWinInfo();
	//virtual ~CProcessWinInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
	virtual BOOL OnRButtonDown(UINT nID);
private:
	BOOL m_bShowChildWnd;
	BOOL m_bShowTitleWnd;
};