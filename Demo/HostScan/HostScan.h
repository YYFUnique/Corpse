#pragma once
#include "Utils/ThreadPool.h"
#include <commctrl.h>
#include <ShObjIdl.h>
#include "ITaskbarList3.h"

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
	DWORD		dwIpAddress;		//IP��ַ
	ULONG		MacAddr[2];		//����MAC��ַ
	BOOL			bPingOk;				//Pingͨ�в���
	DWORD		HostScanType;
	TCHAR     	szHostName[50];		//������
	UINT			nItemIndex;
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

typedef CThreadPoolMgr<TaskQueue,ResultQueue> CThreadTaskMgr;
typedef CThreadPoolMgr<TaskInfo,ResultInfo> CThreadBlockMgr;
class CHostScan : public WindowImplBase
{
public:
	CHostScan();
	~CHostScan();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT TaskThread(LPVOID lParam);
	static UINT AddTaskThread(LPVOID lParam);
	static UINT BlockHostThread(LPVOID lParam);
	void OnBlockHost();
	void OnClick(TNotifyUI& msg);
	void OnItemMenu(TNotifyUI& msg);
	void OnHeaderClick(TNotifyUI& msg);
	void OnBeginScan(TNotifyUI& msg);
	void OnMenuClick(CControlUI* pMenuControl);
	void OnAddUserName();
	void OnGetTaskResult();
	void OnScanHostFinsh();
	void OnCopyIpAddress();
	void OnCopyMacAddress();
	void OnCopyUserName();
	void OnCreateTaskBarButton();
	BOOL GetAdapterName(LPCTSTR lpszMacAddress,LPTSTR szAdapterName,DWORD dwNameLen);
	BOOL GetHostUserName(LPCTSTR lpszMacAddress,LPTSTR szAdapterName,DWORD dwNameLen);
protected:
	CThreadTaskMgr* m_pThreadTaskMgr;
	CThreadBlockMgr* m_pThreadBlockMgr;
	UINT					  m_uTimerId;
	UINT					  m_uIpCount;
	TCHAR					  m_szMacAddressFilePath[MAX_PATH];
	TCHAR					  m_szUserNameFilePath[MAX_PATH];
	ITaskbarList3*		  m_pTaskbarList;
	static volatile LONG  m_sdwTaskDown;
	HICON					  m_hIcon;
	UINT					 WM_TASKBARBUTTONCREATED;
};

/*#include "ThreadPool.temp"*/