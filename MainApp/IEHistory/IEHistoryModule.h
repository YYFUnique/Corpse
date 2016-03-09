#pragma once


// IEHistoryModule 视图
typedef enum tgaMODULE_TYPE
{
	MODULE_TYPE_Undefined = 0,
	MODULE_TYPE_History,
	MODULE_TYPE_AddressCache,
	MODULE_TYPE_Cookies,
	MODULE_TYPE_IECache,
	MODULE_TYPE_ExplorerCache,
	MODULE_TYPE_RunCache
}MODULE_TYPE;

class CIEHistoryModule : public CTreeView
{
	DECLARE_DYNCREATE(CIEHistoryModule)

protected:
	CIEHistoryModule();           // 动态创建所使用的受保护的构造函数
	virtual ~CIEHistoryModule();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

//override
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelectItem(NMHDR* pNMHDR,LRESULT* pLRESULT);
	afx_msg void OnItemRButton(NMHDR* pNMHDR,LRESULT* pLRESULT);
};
