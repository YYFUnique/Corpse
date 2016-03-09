#ifndef _TASKMARVIEW_H
#define _TASKMARVIEW_H

//#include "ProcessInfoView.h"
#include "LsCommon/Class/Process.h"
#include "LsCommon/Class/SuperGridCtrl.h"

typedef enum _ITEM_STATE
{
	ITEM_STATE_Normal = 0,
	ITEM_STATE_Add,
	ITEM_STATE_Del,
}ITEM_STATE;

typedef struct _ITEM_DATA
{
	DWORD dwProcessId;
	DWORD dwThreadId;
	ITEM_STATE ItemState;
	__int64 ullLastCPUTime;
}ITEM_DATA,*PITEM_DATA;

typedef CList<ITEM_DATA,ITEM_DATA&> CItemDataList;

class CTaskMarView : public CListView
{
protected: // 仅从序列化创建
	CTaskMarView();
	DECLARE_DYNCREATE(CTaskMarView)

// 属性
public:
	CTaskMarDoc* GetDocument() const;
// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	static UINT RefreshProcessListThread(LPVOID lParam);
// 实现
public:
	virtual ~CTaskMarView();
	DWORD GetSelectItem();
	BOOL GetPIDAndPath(CString& strPid, CString& strExePath);
	DWORD GetProcessID();
	BOOL SelectProcessTabInfo(UINT nItem);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL ShowProcessList(CLsSystemProcessList& LsSystemProcessList);
	ULONGLONG GetItemText(int nRow,int nCol);
	void ModifyProcessList(DWORD dwIntTime);
	void SetAddItem(DWORD dwIntTime);
	void SetDelItem();
	void ModifyItemData();
	void DeleteItem(UINT nItem);
	BOOL ListAndItemIsEqual(LS_SYSTEM_PROCESS& LsSystemProcess,UINT nItemIndex);
	BOOL CompareListWithItem(LS_SYSTEM_PROCESS& LsSystemProcess,UINT nItemIndex);
	void ModifyProcessItem(LS_SYSTEM_PROCESS& LsSystemProcess,UINT nItem,DWORD dwIntTime);
	void InsertProcessItem(LS_SYSTEM_PROCESS& LsSystemProcess,ITEM_STATE ItemState = ITEM_STATE_Normal);
// 生成的消息映射函数
private:
	CImageList m_ImgList;
	CLsSystemProcessList m_LsSystemProcessList;
	int m_nLastSortSubItem;
	BOOL m_bExit;
	HANDLE m_hThread;
	CItemDataList m_ItemDataList;
protected:
	void OnRClick(NMHDR* pNMHdr,LPARAM* lResult);
	BOOL OnHeadClick(NMHDR* pNMHDR,LRESULT* lResult);
	BOOL OnSelectChange(NMHDR* pNMHDR,LRESULT* lResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLookupProcess();
	afx_msg void OnLookupFileProperty();
	afx_msg void OnCopyProcessName();
	afx_msg void OnCopyProcessPath();
	afx_msg void OnLookupProcessDetail();
	afx_msg void OnLookupProcessModule();
	afx_msg void OnLookupProcessWindow();
	afx_msg void OnLookupProcessMemery();
	afx_msg void OnLookupProcessHandle();
	afx_msg void OnLookupProcessThread();
	afx_msg void OnLookupProcessServices();
	afx_msg void OnLookupProcessStarttype();
	afx_msg void OnKillProcess();
	afx_msg void OnSuspendProcess();
	afx_msg void OnResumeProcess();
	afx_msg void OnMenuExportProcess();
	afx_msg LRESULT OnShowProcessList(WPARAM wParam,LPARAM lParam);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // TaskMarView.cpp 中的调试版本
inline CTaskMarDoc* CTaskMarView::GetDocument() const
   { return reinterpret_cast<CTaskMarDoc*>(m_pDocument); }
#endif

#endif //_TASKMARVIEW_H