#pragma once
#include "ProcessInfo.h"

class CProcessModuleInfo : public CProcessInfo
{
public:
	CProcessModuleInfo();
	//virtual ~CProcessModuleInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
	virtual BOOL OnRButtonDown(UINT nID);
};