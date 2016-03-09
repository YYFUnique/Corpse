#pragma once 
#include "ProcessInfo.h"


class CProcessThreadInfo : public CProcessInfo
{
public:
	CProcessThreadInfo();
	//virtual ~CProcessThreadInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
	virtual BOOL OnRButtonDown(UINT nID);
};