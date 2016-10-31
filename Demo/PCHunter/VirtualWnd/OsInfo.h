#pragma once

typedef enum _NODE_TYPE
{
	NODE_TYPE_ROOT	=		0,
	NODE_TYPE_FIRST =		1,
	NODE_TYPE_SECOND = 2,
}NODE_TYPE;

typedef struct _SYSTEM_NODE_INFO 
{
	NODE_TYPE NodeType;
	CDuiString   strTitle;
	LPARAM		 lParam;
}SYSTEM_NODE_INFO,*PSYSTEM_NODE_INFO;

typedef CDuiList <SYSTEM_NODE_INFO, const SYSTEM_NODE_INFO& > CSystemInfoList;

class COsInfo : public CBase
{
public:
	COsInfo();
	~COsInfo();

public:
	void OnPaint();

	BOOL GetSystemInfo(CSystemInfoList& SystemInfoList);
protected:
	void GetOSInfo(CSystemInfoList& SystemInfoList);
	void GetCpuInfo(CSystemInfoList& SystemInfoList);
	void GetHardDiskInfo(CSystemInfoList& SystemInfoList);
protected:
	CSystemInfoList m_SystemInfoList;
};