#pragma once
#include "DuiLib/Core/UIDlgBuilder.h"

using namespace DuiLib;

class C360Dui : public IDialogBuilderCallback
{	
public:
	C360Dui();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
};