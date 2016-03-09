#pragma once 
#include "ProcessInfo.h"

class CProcessHandleInfo : public CProcessInfo
{
public:
	CProcessHandleInfo();
	//virtual ~CProcessHandleInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
	virtual BOOL OnRButtonDown(UINT nID);
};
