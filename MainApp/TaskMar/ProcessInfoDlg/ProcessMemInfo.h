#pragma once 
#include "ProcessInfo.h"

class CProcessMemInfo : public CProcessInfo
{
public:
	CProcessMemInfo();
	//virtual ~CProcessMemInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
	virtual BOOL OnRButtonDown(UINT nID);
};