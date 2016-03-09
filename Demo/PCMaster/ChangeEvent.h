#pragma once
#include "DuiLib/Utils/observer_impl_base.h"

struct SkinChangedParam
{
	DWORD bkcolor;
	CDuiString bgimage;
};

typedef class ObserverImpl<BOOL, SkinChangedParam> SkinChangedObserver;
typedef class ReceiverImpl<BOOL, SkinChangedParam> SkinChangedReceiver;

