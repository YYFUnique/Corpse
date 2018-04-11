#include "stdafx.h"
#include "Process.h"

CProcess::CProcess()
{

}

CProcess::~CProcess()
{

}

DUI_BEGIN_MESSAGE_MAP(CProcess, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnPaint)
DUI_END_MESSAGE_MAP()

void CProcess::OnPaint(TNotifyUI& msg)
{

}