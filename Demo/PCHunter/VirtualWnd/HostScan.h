#pragma once

#include "Utils/ThreadPool.h"
#include <commctrl.h>
#include <ShObjIdl.h>
//#include "ITaskbarList3.h"

typedef enum _tagHOST_SCAN_TYPE
{
	HOST_SCAN_TYPE_NONE = 0,			//û�ж���ɨ�跽ʽ
	HOST_SCAN_TYPE_ARP = 1,				//ͨ��ARP��ʽɨ��
	HOST_SCAN_TYPE_PING = 2,				//ͨ��PING��ʽɨ��
	HOST_SCAN_TYPE_HOSTNAME = 4,	//ͨ����������ʽɨ��
}HOST_SCAN_TYPE;

typedef struct _tagTaskQueue
{
	DWORD		dwIpAddress;		//IP��ַ
	DWORD		HostScanType;	//ɨ�跽ʽ
	UINT			nItemIndex;
}TaskQueue,*PTaskQueue;

typedef struct _tagResultQueue
{
	DWORD		dwIpAddress;			//IP��ַ
	ULONG		MacAddr[2];			//����MAC��ַ
	BOOL			bPingOk;					//Pingͨ�в���
	DWORD		HostScanType;		//ɨ�跽ʽ
	TCHAR     	szHostName[50];		//������
	UINT			nItemIndex;				//��ţ��ڽ������������£����ܻ���������쳣
}ResultQueue,*PResultQueue;

typedef struct _tagTaskInfo
{
	ULONG		MacAddrOrg[2];		//�����ߵ�MAC��ַ
	ULONG		MacAddrDst[2];		//Ŀ������MAC��ַ
	ULONG		MacAddrArp[2];		//������ģ�������MAC��ַ
	DWORD		dwIpAddress;			//������ģ�������IP��ַ
	UINT			nItemIndex;
}TaskInfo,*PTaskInfo;

typedef struct _tagResultInfo
{
	UINT			nItemIndex;
	BOOL			bResult;
}ResultInfo,*PResultInfo;

typedef enum _ARP_TYPE
{
	ARP_TYPE_Other = 1,			//��������ARP
	ARP_TYPE_Invalid = 2,			//��ЧARP
	ARP_TYPE_Dynamic = 3,		//��̬ARP
	ARP_TYPE_Static = 4,			//��̬ARP
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