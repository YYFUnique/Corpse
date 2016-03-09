#pragma once 
#include "ProcessInfo.h"

class CProcessStartInfo : public CProcessInfo
{
public:
	CProcessStartInfo();
	//virtual ~CProcessStartInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
};