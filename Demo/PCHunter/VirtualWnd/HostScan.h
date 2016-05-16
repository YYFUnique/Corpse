#pragma once

#include "Utils/ThreadPool.h"
#include <commctrl.h>
#include <ShObjIdl.h>
//#include "ITaskbarList3.h"

typedef enum _tagHOST_SCAN_TYPE
{
	HOST_SCAN_TYPE_NONE = 0,			//没有定义扫描方式
	HOST_SCAN_TYPE_ARP = 1,				//通过ARP方式扫描
	HOST_SCAN_TYPE_PING = 2,				//通过PING方式扫描
	HOST_SCAN_TYPE_HOSTNAME = 4,	//通过主机名方式扫描
}HOST_SCAN_TYPE;

typedef struct _tagTaskQueue
{
	DWORD		dwIpAddress;		//IP地址
	DWORD		HostScanType;	//扫描方式
	UINT			nItemIndex;
}TaskQueue,*PTaskQueue;

typedef struct _tagResultQueue
{
	DWORD		dwIpAddress;			//IP地址
	ULONG		MacAddr[2];			//主机MAC地址
	BOOL			bPingOk;					//Ping通行测试
	DWORD		HostScanType;		//扫描方式
	TCHAR     	szHostName[50];		//主机名
	UINT			nItemIndex;				//序号，在进行排序的情况下，可能会出现数据异常
}ResultQueue,*PResultQueue;

typedef struct _tagTaskInfo
{
	ULONG		MacAddrOrg[2];		//发送者的MAC地址
	ULONG		MacAddrDst[2];		//目的主机MAC地址
	ULONG		MacAddrArp[2];		//攻击机模拟的主机MAC地址
	DWORD		dwIpAddress;			//攻击机模拟的主机IP地址
	UINT			nItemIndex;
}TaskInfo,*PTaskInfo;

typedef struct _tagResultInfo
{
	UINT			nItemIndex;
	BOOL			bResult;
}ResultInfo,*PResultInfo;

typedef enum _ARP_TYPE
{
	ARP_TYPE_Other = 1,			//其他类型ARP
	ARP_TYPE_Invalid = 2,			//无效ARP
	ARP_TYPE_Dynamic = 3,		//动态ARP
	ARP_TYPE_Static = 4,			//静态ARP
}ARP_TYPE;

typedef CThreadPoolMgr<TaskQueue,ResultQueue> CThreadTaskMgr;
typedef CThreadPoolMgr<TaskInfo,ResultInfo> CThreadBlockMgr;

class CHostScan : public CBase
{
public:
	CHostScan();
	~CHostScan();

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint();
	void OnClick(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);

	void OnHostScanMenu(CControlUI* pControl);
	void OnRangeMenu(CControlUI* pControl);

	BOOL GetAdapterName(LPCTSTR lpszMacAddress,LPTSTR szAdapterName,DWORD dwNameLen);
	BOOL GetHostUserName(LPCTSTR lpszMacAddress,LPTSTR szAdapterName,DWORD dwNameLen);
protected:
	CThreadTaskMgr* GetThreadMgr();
	CPaintManagerUI* GetPaintManager();

	void StartScan();
	void DeleteAllArp();
	void DeleteDynamicArp();
	void AddStaticArp();
	void OnIpRange(TNotifyUI& msg);
	void OnIpTools(TNotifyUI& msg);

	void OnPingTarget(CListTextElementUI* pItem);
	void OnAddUserName(CListTextElementUI* pItem);
	void OnRemoteDesktop(CListTextElementUI* pItem);

	static UINT TaskThread(LPVOID lParam);
	static void OnTaskResult(LPVOID lParam);
	static void OnTaskFinsh(LPVOID lParam);
	static BOOL PingCmd(DWORD dwIpAddress);
protected:
	CThreadTaskMgr* m_pThreadTaskMgr;
	UINT					  m_uIpCount;
	TCHAR					  m_szMacAddressFilePath[MAX_PATH];
	TCHAR					  m_szUserNameFilePath[MAX_PATH];
};