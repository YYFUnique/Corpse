#pragma once
#include "../DuiLib/UIDlgBuilder.h"

using namespace DuiLib;

class CControlExtend : public IDialogBuilderCallback
{
public:
	CControlExtend();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
};