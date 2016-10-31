#pragma once

class CSysInfoMgr : public CVirtualWnd
{
public:
	CSysInfoMgr();
	~CSysInfoMgr();

protected:
	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnSelectChanged(TNotifyUI& msg);

protected:
	COsInfo m_OsInfo;
};