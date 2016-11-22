#pragma once

class CVirtualWnd : public CNotifyPump,public INotifyUI
{
public:
	CVirtualWnd();
	virtual ~CVirtualWnd();

public:
	LPCTSTR GetVirtualWnd();
	static void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	virtual void Notify(TNotifyUI& msg);
	/************************************************************************/
	/* 这里已经重写了AddVirtualWnd的实现内容，以实现自由管理                  */
	/************************************************************************/
	virtual bool AddVirtualWnd(CDuiString strName,INotifyUI* pObject);
	virtual bool RemoveVirtualWnd(CDuiString strName);
	DUI_DECLARE_MESSAGE_MAP()
protected:
	static CPaintManagerUI* m_pPaintManager;
	CStdStringPtrMap m_VirtualWndMap;
	CDuiString m_strSubNotifys;
};