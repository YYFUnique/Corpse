#pragma once
#include "DllCore/Utils/ServiceControl.h"

class CCoreDriver : public CBase
{
public:
	CCoreDriver();
	~CCoreDriver();

public:
	/*virtual void Notify(TNotifyUI& msg);*/
	virtual void OnPaint();
protected:
	BOOL GetDriverInfo(CSrvInfoList& SvrInfoList);
protected:

};