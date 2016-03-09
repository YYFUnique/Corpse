#pragma once
#include "Group/ToolItemBase.h"

class CCleanProject :
	public CToolItemBase
{
public:
	CCleanProject(void);
	~CCleanProject(void);
	virtual BOOL OnLButtonDbClick();
	virtual BOOL OnRButtonClick();
};
